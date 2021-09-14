
#pragma once//防止重复定义
#include "UtilTypes.h"
#include "Heap.h"
#include "UtilTypes.h"
#include "ClassFileDecoder.h"
#include "ClassLoadActuator.h"
#include <exception>  
#include <string>
#include <iostream>
#include <vector>
using namespace std;

/**
Serial回收器——单线程，复制算法。client模式新生代使用
将新生代分为内存等大的A,B两区。
当A区内存满时，将A区存活的对象复制到B区。同时清除A区内存。
回收完后下次使用B区内存

存活算法：可达算法
GC ROOT的对象有：
1、虚拟机栈中引用的对象（本地变量表，操作数栈）
2、方法区中静态属性引用的对象（类静态变量）
3、方法区中常量引用的对象(final static?)

回收算法:广度优先遍历
1.把所有的GCRoot放入容器,设置alive为true
2.遍历容器内的对象。将对象的非alive=true引用放入容器(放入后alive设为true)。
3.重复步骤2,直到容器遍历完
**/

class Serial{

	private:

		Heap* heap;
		Stack* stack;//线程栈帧
		stringPool* strPool;
		methodArea* ma;//方法区

		bool testMode;

		vector<Slot*> GCRoots;//GCRoot对象


		//寻找GC根节点
		void findGCRoot(){

			GCRoots.clear();
			vector<Object*> objs = heap->getYoungObjHeader();//获取内存对象地址
			
			//将虚拟机栈中引用的对象（本地变量表，操作数栈）作为GC根节点
			int top = stack->top;
			Frame** frameStack = stack->frameStack;
			for(int i=0;i<top;++i){
				Frame* frame=frameStack[i];
				int localVarsSize = frame->localVarsSize;
				Slot** localVars = frame->localVars;
				for(int j=0;j<localVarsSize;++j){
					if(localVars[j]->ref != NULL){
						localVars[j]->ref->alive=true;
						GCRoots.push_back(localVars[j]);
					}
				}
				int operandTop = frame->operandTop;
				Slot** opStack = frame->opStack;
				for(int k=0;k<operandTop;++k){
					if(opStack[k]->ref != NULL){
						opStack[k]->ref->alive=true;
						GCRoots.push_back(opStack[k]);
					}
				}
			}

			//将方法区的静态变量和类对象作为GC根节点
			map<string,MethodAreaClass*> classMap = ma->getClassMap();
			map<string,MethodAreaClass*>::iterator mit;//方法区迭代器
			for(mit = classMap.begin();mit!=classMap.end();mit++){
				MethodAreaClass* Class = mit->second;
				int staticSlotCount = Class->staticSlotCount;
				Slot** slot = Class->staticVars;
				for(int c=0;c<staticSlotCount;++c){
					if((int)slot[c]->ref>0 ){
						slot[c]->ref->alive=true;
						GCRoots.push_back(slot[c]);
					}
				}
			}
		}

		vector<Object*> aliveObjs;

		//新旧地址映射
		//比如对象A引用对象B(地址123456)。但是复制算法将B复制到新地址78901112去了
		//为了确保对象A能正确指向对象B的新地址(78901112)，要在新旧地址间做映射
		map<Object*,Object*> oldAddressToNew;//key:旧地址 value:新地址
		map<Object*,Object*> newAddressToOld;//key:旧地址 value:新地址

		//寻找幸存节点，复制到新内存区并保存新旧地址映射
		void findAliveByDFS(Object* obj){

			MethodAreaClass* objClass = obj->objClass;

			//将本对象拷贝到备用区
			//由于内存已经交换了
			//现在new Object的时候其实是在交换内存前的备用区开辟空间
			
			//先判断一下该对象是否本来就在堆区分配内存
			//不然，不在堆区分配内存的对象也来挤占空间，会挤爆备用区的！
			
			Object* copyObj = obj;
			if(heap->inUsingArea(obj)==true){
				Heap* heap = objClass->loader->heap;
				if(obj->age < heap->getOldAge()){
					copyObj = newObject(objClass);	
				}else{
					copyObj = heap->toOldArea(obj);
				}
				aliveObjs.push_back(copyObj);
			}

	//		printf("[old:%d new:%d] ",obj,copyObj);

			//拷贝Object的基本信息即可。
			copyObj->age = obj->age+1;
			copyObj->objClass = objClass;
			copyObj->alive = true;
			copyObj->array = obj->array;
				
			int fieldsLen = objClass->fieldsLen;

			//建立新旧地址映射
			oldAddressToNew[obj] = copyObj;
			newAddressToOld[copyObj] = obj;

			Slot** oldFields = obj->fields;
			Slot** newFields = copyObj->fields;

			if(oldFields!=NULL){

				map<string,Field*> fieldMap = objClass->floader->getFieldMap();
				map<string,Field*>::iterator fit;//方法区迭代器

				for(fit=fieldMap.begin();fit!=fieldMap.end();fit++){
					printf("\n--字段%s",fit->first.c_str());
					if(fit->second==NULL){//Field为空，说明是父类的。去父类寻找Field
						Field* field = objClass->floader->findFieldInThisOrSuper(fit->first,objClass->superClass);
						fit->second = field;
						
					}
				
					if( IsStatic(fit->second->accessFlag) == false){
						printf("dfwe");
						int slotId = fit->second->slotId;
						Slot* slot = oldFields[slotId];
						printf("%d",slot);
						if((int)slot!=0xfdfdfdfd && slot!=NULL){
							newFields[slotId]->flag = slot->flag;
							newFields[slotId]->num = slot->num;

							if(slot->ref != NULL){
								if(slot->ref->alive == false){
									findAliveByDFS(slot->ref);
								}else{
									if(isEqual((u1*)"Ljava/lang/String;",fit->second->description)==true){
										newFields[slotId]->ref = slot->ref;
										printf("{str:%s %s %d}",fit->second->name,fit->second->description,slot->ref);
									}else if(isEqual((u1*)"value",fit->second->name)==true){
										findAliveByDFS(slot->ref);
									}	
								}
							}
						}
					}
					printf("完成");
				}
			}
		}

		//根据新旧地址映射恢复对象引用关系
		//比如对象A引用对象B(地址123456)。但是复制算法将B复制到新地址78901112去了
		//等A,B节点都复制到新地址去时，要让A对象指向B对象新地址
		void recObjectReference(){

			map<Object*,Object*>::iterator fit;//地址映射迭代器
			int indexOfObj = 0,indexOfField = 0;
			for(fit=newAddressToOld.begin();fit!=newAddressToOld.end();fit++){//迭代新旧地址映射表
				Object* newAddress = fit->first;
				Object* oldAddress = fit->second;
				newAddress->alive = false;
				Slot** newFields = newAddress->fields;
				Slot** oldFields = oldAddress->fields;
				
				if(oldFields!=NULL){
					printf("%d:恢复%s对象%d(原%d)的字段引用关系:",indexOfObj++,newAddress->objClass->name,newAddress,oldAddress);
					map<string,Field*> fieldMap = newAddress->objClass->floader->getFieldMap();
					map<string,Field*>::iterator fit;//方法区迭代器
					indexOfField = 0;
					for(fit=fieldMap.begin();fit!=fieldMap.end();fit++){
						int slotId = fit->second->slotId;
						Slot* slot = oldFields[slotId];
						printf("\n--(%d).恢复%s字段",indexOfField++,fit->first.c_str());
					//	printf("{%d}",slot);
						//printf("{%d}",slot->ref);
						if((int)slot!=0xfdfdfdfd && slot!=NULL && slot->ref!=NULL){
							
							/**
							class Object{
								Object ref;
							}
							        新地址	新地址ref	旧地址	旧地址ref  
							对象 A   2000     空         1000     1100
							对象 B   2100                1100

							由上可以看出Object类对象A引用了Obj对象B。
							当对象A,B都被复制到新内存的时候,要怎么恢复A,B间的引用关系呢?
							1).首先由A的新地址映射到A的旧地址。找到旧地址ref,即B的旧地址
							2).根据B的旧地址,找到到B的新地址
							3).将A的新地址ref赋值为B的新地址,即2100
							**/

							Object* oldSlotRef = slot->ref;
							Object* newSlotRef = oldAddressToNew[oldSlotRef];
							newFields[slotId]->ref = newSlotRef;
							printf("(原%d 现%d)",oldSlotRef,newSlotRef);
							if(slot->ref->array!=NULL){
								Object* arrayObj = newFields[slotId]->ref;
								if(arrayObj == NULL){
									arrayObj = (Object*)malloc(sizeof(Object));
								}
								arrayObj->array = slot->ref->array;
								newFields[slotId]->ref = arrayObj;
							}
						}
					}
					printf("\n完成\n");
				}else{
					printf("%d:%s对象%d(原%d)无字段\n",indexOfObj++,oldAddress->objClass->name,newAddress,oldAddress);
				}
			}
			
		}


	public:

		Serial(Heap* heap,Thread* thread){
			this->heap = heap;
			this->stack = thread->stack;
			ma = heap->getMethodArea();
			strPool = heap->getStrPool();
			testMode = false;
		}

		void setTest(){
			testMode = true;
		}

		void showObjs(vector<Object*> objs,int flag){
			for(int i=0;i<objs.size();++i){
				Object* address = objs[i];
				if(flag == 0){
					printf("\n%d:[%s类 对象%d] 年龄:%d",i,address->objClass->name,address,address->age);
					if(isEqual(address->objClass->name,(u1*)"java/lang/String") == true){
						printf(" 内容:");
						b1* strVal=address->objClass->loader->heap->getStrPool()->getJStrValue(address);
						if(strVal!=NULL){
							printfUTF8(strVal);
						}
					}
				}else{
					Object* oldAddress = newAddressToOld[address];
					printf("\n%d:[%s类 对象%d 复制到 %d 年龄:%d]",i,address->objClass->name,oldAddress,address,address->age);
				}
			}
		}

		//基本完成，但仍有点小问题
		//1.当堆区空间较小的时候，对象数据会丢失
		//如果引用对象是String的话，甚至会回收失败
		//2.由于字符串常量池的原因，引用字符串常量池的对象会丢失数据。
		//比如String a="12345",回收垃圾后a的值不见了
		void minorGC(){
			
			HeapMemory* usingArea = heap->getUsingArea();

			aliveObjs.clear();
			oldAddressToNew.clear();
			newAddressToOld.clear();

			if(testMode==true){
				printf("\n回收前\n--新生代(%d 大小%dB 已用%dB)",usingArea->heapArea,usingArea->maxHeapSize*4,
					(int)usingArea->canUseAddress-(int)usingArea->heapArea);
				showObjs(heap->getYoungObjHeader(),0);

				printf("\n--老年区");
				showObjs(heap->getOldObjHeader(),0);

				printf("\n--字符串池");
				strPool->showStrPool();
			}

			heap->usingAreaChange();//更换正在使用的内存
			usingArea = heap->getUsingArea();
			usingArea->canUseAddress = (int)usingArea->heapArea;

			//步骤0.寻找GCRoot
			findGCRoot();


			if(testMode==true){
				printf("\n\n--根节点：\n");
				for(int i=0;i<GCRoots.size();++i){
					Object* ref = GCRoots[i]->ref;
					printf("%d:[%s类 对象%d]",i,ref->objClass->name,ref);
					if(isEqual(ref->objClass->name,(u1*)"java/lang/String") == true){
						printf(" 内容:");
						b1* strVal=ref->objClass->loader->heap->getStrPool()->getJStrValue(ref);
						printfUTF8(strVal);
					}
					printf("\n");
				}
				printf("---\n");
			}


			//步骤1.寻找幸存节点，并复制到新的内存
			//别忘了，对Slot更新其ref地址
			for(int i=0;i<GCRoots.size();++i){
				Object* oldObj = GCRoots[i]->ref;
				printf("遍历根节点:%d",oldObj);
				findAliveByDFS(oldObj);
				GCRoots[i]->ref = oldAddressToNew[oldObj];
				printf("完成。根节点%d -> %d\n",oldObj,GCRoots[i]->ref);
			}
			printf("\n复制完成,恢复对象间引用\n");
			

			//步骤2.根据新旧地址映射，恢复对象间的引用关系
			//顺便把对象存活设置为false
			recObjectReference();

			if(testMode==true){
				heap->setYoungObjHeader(aliveObjs);
				printf("\n回收后\n--新生代(%d 大小%dB 已用%dB)",usingArea->heapArea,
					usingArea->maxHeapSize*4,(int)usingArea->canUseAddress-(int)usingArea->heapArea);
				showObjs(heap->getYoungObjHeader(),1);
				
				printf("\n--老年区");
				showObjs(heap->getOldObjHeader(),1);
				
				printf("\n--字符串池");
				strPool->showStrPool();

				printf("\n回收完成\n\n");
			}
		}

};

void minorGC(Serial* serial);