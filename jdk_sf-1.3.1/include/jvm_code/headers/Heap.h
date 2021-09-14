/**

  Heap(����,��������)

**/
#pragma once//��ֹ�ظ�����
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

//jvm�ַ����أ�����ַ����ĳ����أ�
class stringPool{
	private:
		map<string,Object*> internedStrings;
		MethodAreaClass* JStrClass;
		methodArea* MTA;
		u2 valSlotId;//java/lang/String��value�ֶε�����
		map<string,Object*>::iterator mit;//stringPool������
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

		//�����ַ�������ע�⣡�ַ�����ǧ�����ڶ�������ռ�
		Object* JString(b1* chars){

			string goStr=(char*)chars;
			if(goStr.size() == 0) return NULL;
			if(internedStrings[goStr]!=NULL){
				return internedStrings[goStr];
			}

			//����jChars����(ǧ�����ڶ��������ڴ�)
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


			//����jStr����(ǧ�����ڶ��������ڴ�)
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
			if(jStr==NULL || jStr->fields[valSlotId]->ref==NULL){//����ַ�������Ϊ�ջ�value�ֶ�Ϊ��
				return NULL;
			}
			return jStr->fields[valSlotId]->ref->array->B1Array;
		}

		Object* getJStrValueFieldRef(Object* jStr){
			if(jStr==NULL || jStr->fields[valSlotId]->ref==NULL){//����ַ�������Ϊ�ջ�value�ֶ�Ϊ��
				return NULL;
			}
			return jStr->fields[valSlotId]->ref;
		}
	
		void showStrPool(){
			if(internedStrings.begin()==internedStrings.end()){
				printf("\n�ַ����ؿ�!\n\n");return;
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

//�����ռ�
class HeapMemory{

	public:
		b4* heapArea;//������ʼ��ַ
		int canUseAddress;//�����ڴ���ʼ��ַ
		int maxHeapSize;//������С(��λ��4Byte)
		int endAddress;//����������ַ
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
		����
		Ӧ�ڶ������������
		1.��ͨ���� 1)Object�ṹ������� 2)Object�ṹ��fields���õ�Slot����(���ö����õ�������Slot����)
		2.������� 1)Object�ṹ������� 2)Object�ṹ��array���õ�Array���� 3)Array���ݵ�b1,b4��b8����

		һ���������
		���´���Ӧ�����㷨���з��䡣�ռ䲻��ʱ����GC
	
		����
		1.һ��JVM�ڴ����
		����԰�� �� TO,From�� �� ������
		  8      :   1:1      :   20

		2.��JVM�ڴ����
		TO,From�� �� ������
		  1:1     :    4
		**/
		HeapMemory* ToArea;//To��
		HeapMemory* FormArea;//From��
		HeapMemory* usingArea;//����������ʹ�õ��ڴ���
		vector<Object*> youngObjHeader;//����������ͷ��ַ

		int oldAge;//��������������������(һ��Ϊ15)
		HeapMemory* OldArea;//������
		vector<Object*> oldObjHeader;//���������ͷ��ַ

		int maxHeapSize;//�ڴ����ܴ�С����λ:4�ֽ�

		//������
		methodArea* MA;	
								
		//�ַ���������(String Pool)
		//JVMΪ���������ܺͼ����ڴ濪���������ַ������ظ������������ַ���ʱ�������ַ����ؼ���Ƿ�����ͬ���ݵ��ַ�������û�У��Ŵ����µ�
		stringPool* SP;	
		Serial* serial;

	public:

		Heap(ConfigInfo* config){

			//���ö�����С(��λ:�ֽ�)
			//maxHeapSize��λ��4�ֽ�
			maxHeapSize = stringToInt(config->getConfig("HEAP_MAX_MEMORY"))/4;
			if(maxHeapSize<=64){//��С64*4=256B
				maxHeapSize=64;
			}

			//��������������
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

			MA=new methodArea(this);	//�½�������
			MA->nativeMethodMap=new Registry();
			SP=new stringPool(MA);  //�½��ַ�����
			
		}

		methodArea* getMethodArea(){
			return MA;
		}

		stringPool* getStrPool(){
			return SP;
		}
	
		//����������ڴ�
		Object* allocaOfObject(int sizeOfSlot){

			int canUseAddress = usingArea->canUseAddress;
			int* heapArea = usingArea->heapArea;
			int endAddress = usingArea->endAddress;

			int objSize =  sizeof(Object) + sizeOfSlot*sizeof(Slot);
			
			while(canUseAddress+objSize > endAddress){
				//������������
				minorGC(serial);
				canUseAddress = usingArea->canUseAddress;
			}


			Object* obj = new((int*)canUseAddress) Object;
			usingArea->canUseAddress += sizeof(Object);

			if(sizeOfSlot>0){//Ϊ����������Դ�ſռ䡣����һ��������3�����ԣ��ͷ���3��Slot���ڴ����������
				obj->fields=(Slot**)calloc(sizeOfSlot,sizeof(Slot*));
				Slot** fields=obj->fields;
				for(int i=0;i<sizeOfSlot;++i){
					fields[i]=new((int*)usingArea->canUseAddress) Slot;
					usingArea->canUseAddress += sizeof(Slot);
					fields[i]->ref=NULL;//����Ҫ!
					fields[i]->flag=0;
					fields[i]->num=0;
				}

			}
			youngObjHeader.push_back(obj);
			return obj;
		}


		//ȥ������
		Object* toOldArea(Object* oldObj){

			int canUseAddress = OldArea->canUseAddress;
			int* heapArea = OldArea->heapArea;
			int endAddress = OldArea->endAddress;

			MethodAreaClass* objClass = oldObj->objClass;
			int sizeOfSlot = objClass->instanceSlotCount;

			int objSize =  sizeof(Object) + sizeOfSlot*sizeof(Slot);
			bool isGC=false;
			if(canUseAddress+objSize > endAddress){
				//�����������ա���ʼfull gc
				printError(0x018,NULL);
			}

			Object* obj = new((int*)canUseAddress) Object;
			OldArea->canUseAddress += sizeof(Object);

			if(sizeOfSlot>0){//Ϊ����������Դ�ſռ䡣����һ��������3�����ԣ��ͷ���3��Slot���ڴ����������
				obj->fields=(Slot**)calloc(sizeOfSlot,sizeof(Slot*));
				Slot** fields=obj->fields;

				for(int i=0;i<sizeOfSlot;++i){
					fields[i]=new((int*)OldArea->canUseAddress) Slot;
					OldArea->canUseAddress += sizeof(Slot);
					fields[i]->ref=NULL;//����Ҫ!
					fields[i]->flag=0;
					fields[i]->num=0;
				}

			}
			oldObjHeader.push_back(obj);
			return obj;
		}

		long getHeapAreaUseByte(){
			//������ʹ���ڴ�
			long YoungAreaUsing = (long)usingArea->canUseAddress-(long)usingArea->heapArea;

			//������ʹ���ڴ�
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

		//�ı�����������ʹ�õ��ڴ���
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

		//�ж϶����Ƿ���������
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

