/**

  Heap(堆区,含方法区)

**/
#pragma once//防止重复定义
#include "UtilTypes.h"
#include "methodArea.h"
#include "Thread.h"
#include "UtilTypes.h"
#include "registry.h"
#include "UTF8CNDecoder.h"
#include "ConfigDecoder.h"
#include <string>
#include <vector>
using namespace std;

struct Slot;
struct Frame;
struct Heap;
struct Method;
class stringPool;
class Serial;

Object* newObject(MethodAreaClass* c);
Object* newJClassObject(MethodAreaClass* jClass);
Object* newPrimitiveTypesArray(methodArea* loader,int atype,int len);
stringPool* getStringPoolFromFrame(Frame* frame);
void minorGC(Serial* serial);

//jvm字符串池（存放字符串的常量池）
class stringPool{
	private:
		map<string,Object*> internedStrings;
		MethodAreaClass* JStrClass;
		methodArea* MTA;
		u2 valSlotId;//java/lang/String的value字段的索引
		map<string,Object*>::iterator mit;//stringPool迭代器
		int stringNum;

	public:

		stringPool(methodArea* mta){
			JStrClass=loadNonArrayClass(mta,"java/lang/String",0);
			fieldRef* valueField=(fieldRef*)malloc(sizeof(fieldRef));
			valueField->descriptor=(u1*)"[C";
			valueField->name=(u1*)"value";
			Field* field=JStrClass->floader->getField(valueField);
			valSlotId=field->slotId;
			MTA=mta;
			stringNum=0;
		}

		//创建字符常量。注意！字符常量千万不能在堆区分配空间
		Object* JString(b1* chars){

			string goStr=(char*)chars;
			if(goStr.size() == 0) return NULL;
			if(internedStrings[goStr]!=NULL){
				return internedStrings[goStr];
			}

			//创建jChars数组(千万不能在堆区分配内存)
			Object* jChars = (Object*)malloc(sizeof(Object));
			jChars->age = 0;
			jChars->alive = true;
			jChars->objClass = loadArrayClass(MTA,"[char",false);
			jChars->array = (Array*)malloc(sizeof(Array));
			b1* val = (b1*)calloc(goStr.length()+1,sizeof(b1));
			for(int i=0;i<goStr.length();++i){
				val[i] = chars[i];
			}
			val[goStr.length()]='\0';
			jChars->array->B1Array=val;
			jChars->array->type=0;
			jChars->fields=NULL;
			jChars->array->len=goStr.length();


			//创建jStr对象(千万不能在堆区分配内存)
			Object* jStr = (Object*)malloc(sizeof(Object));
			jStr->age = 0;
			jStr->alive = true;
			jStr->array = NULL;
			jStr->objClass = JStrClass;

			int instanceSlotCount = JStrClass->instanceSlotCount;
			Slot** fields = (Slot**)calloc(instanceSlotCount,sizeof(Slot*));
			for(int j=0;j<instanceSlotCount;++j){
				fields[j]=(Slot*)malloc(sizeof(Slot));
				fields[j]->flag = 0;
				fields[j]->num = 0;
				fields[j]->ref = NULL;
			}
			jStr->fields = fields;
			jStr->fields[valSlotId]->ref=jChars;

			internedStrings[goStr]=jStr;
			stringNum++;

			return jStr;
		}

		b1* getJStrValue(Object* jStr){
			if(jStr==NULL || jStr->fields[valSlotId]->ref==NULL){//如果字符串对象为空或value字段为空
				return NULL;
			}
			return jStr->fields[valSlotId]->ref->array->B1Array;
		}

		Object* getJStrValueFieldRef(Object* jStr){
			if(jStr==NULL || jStr->fields[valSlotId]->ref==NULL){//如果字符串对象为空或value字段为空
				return NULL;
			}
			return jStr->fields[valSlotId]->ref;
		}
	
		void showStrPool(){
			if(internedStrings.begin()==internedStrings.end()){
				printf("\n字符串池空!\n\n");return;
			}
			for(mit = internedStrings.begin();mit!=internedStrings.end();mit++){
				printf("\n%s(%d) ",decodeUTF8((b1*)mit->first.c_str()),mit->second);
			}
		}

		int getStrNum(){
			return stringNum;
		}

		map<string,Object*> getInternedStrings(){
			return internedStrings;
		}
};

//堆区空间
class HeapMemory{

	public:
		b4* heapArea;//堆区起始地址
		int canUseAddress;//可用内存起始地址
		int maxHeapSize;//堆区大小(单位：4Byte)
		int endAddress;//堆区结束地址
		HeapMemory(int maxHeapSize){
			this->maxHeapSize=maxHeapSize;
			heapArea = (b4*)calloc(maxHeapSize,sizeof(b4));
			canUseAddress=(int)heapArea;
			endAddress = (int)maxHeapSize*4 + (int)heapArea;
		}
};

class Heap{

	private:

		/**
		堆区
		应在堆区分配的数据
		1.普通对象 1)Object结构体的内容 2)Object结构体fields引用的Slot数据(即该对象用到的所有Slot数据)
		2.数组对象 1)Object结构体的内容 2)Object结构体array引用的Array数据 3)Array内容的b1,b4或b8数组

		一、分配规则
		按下次适应分配算法进行分配。空间不够时触发GC
	
		二、
		1.一般JVM内存分配
		伊甸园区 → TO,From区 → 老年区
		  8      :   1:1      :   20

		2.本JVM内存分配
		TO,From区 → 老年区
		  1:1     :    4
		**/
		HeapMemory* ToArea;//To区
		HeapMemory* FormArea;//From区
		HeapMemory* usingArea;//新生代正在使用的内存区
		vector<Object*> youngObjHeader;//新生代对象头地址

		int oldAge;//进入老年区的年龄条件(一般为15)
		HeapMemory* OldArea;//老年区
		vector<Object*> oldObjHeader;//老年代对象头地址

		int maxHeapSize;//内存区总大小。单位:4字节

		//方法区
		methodArea* MA;	
								
		//字符串常量池(String Pool)
		//JVM为了提升性能和减少内存开销，避免字符串的重复创建，创建字符串时都先在字符串池检查是否有相同内容的字符串，若没有，才创建新的
		stringPool* SP;	
		Serial* serial;

	public:

		Heap(ConfigInfo* config){

			//配置堆区大小(单位:字节)
			//maxHeapSize单位是4字节
			maxHeapSize = stringToInt(config->getConfig("HEAP_MAX_MEMORY"))/4;
			if(maxHeapSize<=64){//最小64*4=256B
				maxHeapSize=64;
			}

			//配置老年区年龄
			oldAge = stringToInt(config->getConfig("HEAP_OLD_AREA_AGE"));

			init();
		}

		void reSetHeap(int size){
			maxHeapSize = size;
			init();
		}

		init(){
			ToArea = new HeapMemory(maxHeapSize/6);
			FormArea = new HeapMemory(maxHeapSize/6);
			OldArea = new HeapMemory(maxHeapSize/6*4);
			usingArea = ToArea;

			MA=new methodArea(this);	//新建方法区
			MA->nativeMethodMap=new Registry();
			SP=new stringPool(MA);  //新建字符串池
			
		}

		methodArea* getMethodArea(){
			return MA;
		}

		stringPool* getStrPool(){
			return SP;
		}
	
		//给对象分配内存
		Object* allocaOfObject(int sizeOfSlot){

			int canUseAddress = usingArea->canUseAddress;
			int* heapArea = usingArea->heapArea;
			int endAddress = usingArea->endAddress;

			int objSize =  sizeof(Object) + sizeOfSlot*sizeof(Slot);
			
			while(canUseAddress+objSize > endAddress){
				//启动垃圾回收
				minorGC(serial);
				canUseAddress = usingArea->canUseAddress;
			}


			Object* obj = new((int*)canUseAddress) Object;
			usingArea->canUseAddress += sizeof(Object);

			if(sizeOfSlot>0){//为对象分配属性存放空间。比如一个对象有3个属性，就分配3个Slot用于存放它的属性
				obj->fields=(Slot**)calloc(sizeOfSlot,sizeof(Slot*));
				Slot** fields=obj->fields;
				for(int i=0;i<sizeOfSlot;++i){
					fields[i]=new((int*)usingArea->canUseAddress) Slot;
					usingArea->canUseAddress += sizeof(Slot);
					fields[i]->ref=NULL;//很重要!
					fields[i]->flag=0;
					fields[i]->num=0;
				}

			}
			youngObjHeader.push_back(obj);
			return obj;
		}


		//去老年区
		Object* toOldArea(Object* oldObj){

			int canUseAddress = OldArea->canUseAddress;
			int* heapArea = OldArea->heapArea;
			int endAddress = OldArea->endAddress;

			MethodAreaClass* objClass = oldObj->objClass;
			int sizeOfSlot = objClass->instanceSlotCount;

			int objSize =  sizeof(Object) + sizeOfSlot*sizeof(Slot);
			bool isGC=false;
			if(canUseAddress+objSize > endAddress){
				//启动垃圾回收。开始full gc
				printError(0x018,NULL);
			}

			Object* obj = new((int*)canUseAddress) Object;
			OldArea->canUseAddress += sizeof(Object);

			if(sizeOfSlot>0){//为对象分配属性存放空间。比如一个对象有3个属性，就分配3个Slot用于存放它的属性
				obj->fields=(Slot**)calloc(sizeOfSlot,sizeof(Slot*));
				Slot** fields=obj->fields;

				for(int i=0;i<sizeOfSlot;++i){
					fields[i]=new((int*)OldArea->canUseAddress) Slot;
					OldArea->canUseAddress += sizeof(Slot);
					fields[i]->ref=NULL;//很重要!
					fields[i]->flag=0;
					fields[i]->num=0;
				}

			}
			oldObjHeader.push_back(obj);
			return obj;
		}

		long getHeapAreaUseByte(){
			//新生代使用内存
			long YoungAreaUsing = (long)usingArea->canUseAddress-(long)usingArea->heapArea;

			//老年区使用内存
			long OldAreaUsing = (long)OldArea->canUseAddress-(long)OldArea->heapArea;
			return YoungAreaUsing+OldAreaUsing;
		}

		int getHeapAreaTotalByte(){
			return maxHeapSize*4;
		}

		float getHeapAreaUseRatio(){
			return (float)((float)getHeapAreaUseByte()/((float)maxHeapSize*4))*100;
		}

		HeapMemory* getToArea(){
			return ToArea;
		}

		HeapMemory* getFormArea(){
			return FormArea;
		}

		HeapMemory* getOldArea(){
			return OldArea;
		}

		vector<Object*> getYoungObjHeader(){
			return youngObjHeader;
		}

		vector<Object*> getOldObjHeader(){
			return oldObjHeader;
		}

		void setYoungObjHeader(vector<Object*> objHeader){
			youngObjHeader.swap(vector<Object*>());
			youngObjHeader = objHeader;
		}

		void setOldObjHeader(vector<Object*> objHeader){
			oldObjHeader.swap(vector<Object*>());
			oldObjHeader = objHeader;
		}

		void setGCer(Serial* serial){
			this->serial = serial;
		}

		Serial* getGCer(){
			return serial;
		}

		//改变新生代正在使用的内存区
		void usingAreaChange(){
			usingArea->canUseAddress = (int)usingArea->heapArea;
			if(usingArea == ToArea){
				usingArea = FormArea;
			}else{
				usingArea = ToArea;
			}
		}

		HeapMemory* getUsingArea(){
			return usingArea;
		}

		HeapMemory* getBackUpArea(){
			if(usingArea == ToArea){
				return FormArea;
			}
			return ToArea;
		}

		//判断对象是否在新生代
		bool inUsingArea(Object* obj){
			int startAddress = (int)ToArea->heapArea;
			int endAddress = ToArea->endAddress;
			if((int)obj>=startAddress && (int)obj<=endAddress){
				return true;
			}
			
			startAddress = (int)FormArea->heapArea;
			endAddress = FormArea->endAddress;
			if((int)obj>=startAddress && (int)obj<=endAddress){
				return true;
			}

			return false;
		}

		int getOldAge(){
			return oldAge;
		}
};

