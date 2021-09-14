
#pragma once//��ֹ�ظ�����
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
Serial�������������̣߳������㷨��clientģʽ������ʹ��
����������Ϊ�ڴ�ȴ��A,B������
��A���ڴ���ʱ����A�����Ķ����Ƶ�B����ͬʱ���A���ڴ档
��������´�ʹ��B���ڴ�

����㷨���ɴ��㷨
GC ROOT�Ķ����У�
1�������ջ�����õĶ��󣨱��ر�����������ջ��
2���������о�̬�������õĶ����ྲ̬������
3���������г������õĶ���(final static?)

�����㷨:������ȱ���
1.�����е�GCRoot��������,����aliveΪtrue
2.���������ڵĶ��󡣽�����ķ�alive=true���÷�������(�����alive��Ϊtrue)��
3.�ظ�����2,ֱ������������
**/

class Serial{

	private:

		Heap* heap;
		Stack* stack;//�߳�ջ֡
		stringPool* strPool;
		methodArea* ma;//������

		bool testMode;

		vector<Slot*> GCRoots;//GCRoot����


		//Ѱ��GC���ڵ�
		void findGCRoot(){

			GCRoots.clear();
			vector<Object*> objs = heap->getYoungObjHeader();//��ȡ�ڴ�����ַ
			
			//�������ջ�����õĶ��󣨱��ر�����������ջ����ΪGC���ڵ�
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

			//���������ľ�̬�������������ΪGC���ڵ�
			map<string,MethodAreaClass*> classMap = ma->getClassMap();
			map<string,MethodAreaClass*>::iterator mit;//������������
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

		//�¾ɵ�ַӳ��
		//�������A���ö���B(��ַ123456)�����Ǹ����㷨��B���Ƶ��µ�ַ78901112ȥ��
		//Ϊ��ȷ������A����ȷָ�����B���µ�ַ(78901112)��Ҫ���¾ɵ�ַ����ӳ��
		map<Object*,Object*> oldAddressToNew;//key:�ɵ�ַ value:�µ�ַ
		map<Object*,Object*> newAddressToOld;//key:�ɵ�ַ value:�µ�ַ

		//Ѱ���Ҵ�ڵ㣬���Ƶ����ڴ����������¾ɵ�ַӳ��
		void findAliveByDFS(Object* obj){

			MethodAreaClass* objClass = obj->objClass;

			//�������󿽱���������
			//�����ڴ��Ѿ�������
			//����new Object��ʱ����ʵ���ڽ����ڴ�ǰ�ı��������ٿռ�
			
			//���ж�һ�¸ö����Ƿ������ڶ��������ڴ�
			//��Ȼ�����ڶ��������ڴ�Ķ���Ҳ����ռ�ռ䣬�ἷ���������ģ�
			
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

			//����Object�Ļ�����Ϣ���ɡ�
			copyObj->age = obj->age+1;
			copyObj->objClass = objClass;
			copyObj->alive = true;
			copyObj->array = obj->array;
				
			int fieldsLen = objClass->fieldsLen;

			//�����¾ɵ�ַӳ��
			oldAddressToNew[obj] = copyObj;
			newAddressToOld[copyObj] = obj;

			Slot** oldFields = obj->fields;
			Slot** newFields = copyObj->fields;

			if(oldFields!=NULL){

				map<string,Field*> fieldMap = objClass->floader->getFieldMap();
				map<string,Field*>::iterator fit;//������������

				for(fit=fieldMap.begin();fit!=fieldMap.end();fit++){
					printf("\n--�ֶ�%s",fit->first.c_str());
					if(fit->second==NULL){//FieldΪ�գ�˵���Ǹ���ġ�ȥ����Ѱ��Field
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
					printf("���");
				}
			}
		}

		//�����¾ɵ�ַӳ��ָ��������ù�ϵ
		//�������A���ö���B(��ַ123456)�����Ǹ����㷨��B���Ƶ��µ�ַ78901112ȥ��
		//��A,B�ڵ㶼���Ƶ��µ�ַȥʱ��Ҫ��A����ָ��B�����µ�ַ
		void recObjectReference(){

			map<Object*,Object*>::iterator fit;//��ַӳ�������
			int indexOfObj = 0,indexOfField = 0;
			for(fit=newAddressToOld.begin();fit!=newAddressToOld.end();fit++){//�����¾ɵ�ַӳ���
				Object* newAddress = fit->first;
				Object* oldAddress = fit->second;
				newAddress->alive = false;
				Slot** newFields = newAddress->fields;
				Slot** oldFields = oldAddress->fields;
				
				if(oldFields!=NULL){
					printf("%d:�ָ�%s����%d(ԭ%d)���ֶ����ù�ϵ:",indexOfObj++,newAddress->objClass->name,newAddress,oldAddress);
					map<string,Field*> fieldMap = newAddress->objClass->floader->getFieldMap();
					map<string,Field*>::iterator fit;//������������
					indexOfField = 0;
					for(fit=fieldMap.begin();fit!=fieldMap.end();fit++){
						int slotId = fit->second->slotId;
						Slot* slot = oldFields[slotId];
						printf("\n--(%d).�ָ�%s�ֶ�",indexOfField++,fit->first.c_str());
					//	printf("{%d}",slot);
						//printf("{%d}",slot->ref);
						if((int)slot!=0xfdfdfdfd && slot!=NULL && slot->ref!=NULL){
							
							/**
							class Object{
								Object ref;
							}
							        �µ�ַ	�µ�ַref	�ɵ�ַ	�ɵ�ַref  
							���� A   2000     ��         1000     1100
							���� B   2100                1100

							���Ͽ��Կ���Object�����A������Obj����B��
							������A,B�������Ƶ����ڴ��ʱ��,Ҫ��ô�ָ�A,B������ù�ϵ��?
							1).������A���µ�ַӳ�䵽A�ľɵ�ַ���ҵ��ɵ�ַref,��B�ľɵ�ַ
							2).����B�ľɵ�ַ,�ҵ���B���µ�ַ
							3).��A���µ�ַref��ֵΪB���µ�ַ,��2100
							**/

							Object* oldSlotRef = slot->ref;
							Object* newSlotRef = oldAddressToNew[oldSlotRef];
							newFields[slotId]->ref = newSlotRef;
							printf("(ԭ%d ��%d)",oldSlotRef,newSlotRef);
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
					printf("\n���\n");
				}else{
					printf("%d:%s����%d(ԭ%d)���ֶ�\n",indexOfObj++,oldAddress->objClass->name,newAddress,oldAddress);
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
					printf("\n%d:[%s�� ����%d] ����:%d",i,address->objClass->name,address,address->age);
					if(isEqual(address->objClass->name,(u1*)"java/lang/String") == true){
						printf(" ����:");
						b1* strVal=address->objClass->loader->heap->getStrPool()->getJStrValue(address);
						if(strVal!=NULL){
							printfUTF8(strVal);
						}
					}
				}else{
					Object* oldAddress = newAddressToOld[address];
					printf("\n%d:[%s�� ����%d ���Ƶ� %d ����:%d]",i,address->objClass->name,oldAddress,address,address->age);
				}
			}
		}

		//������ɣ������е�С����
		//1.�������ռ��С��ʱ�򣬶������ݻᶪʧ
		//������ö�����String�Ļ������������ʧ��
		//2.�����ַ��������ص�ԭ�������ַ��������صĶ���ᶪʧ���ݡ�
		//����String a="12345",����������a��ֵ������
		void minorGC(){
			
			HeapMemory* usingArea = heap->getUsingArea();

			aliveObjs.clear();
			oldAddressToNew.clear();
			newAddressToOld.clear();

			if(testMode==true){
				printf("\n����ǰ\n--������(%d ��С%dB ����%dB)",usingArea->heapArea,usingArea->maxHeapSize*4,
					(int)usingArea->canUseAddress-(int)usingArea->heapArea);
				showObjs(heap->getYoungObjHeader(),0);

				printf("\n--������");
				showObjs(heap->getOldObjHeader(),0);

				printf("\n--�ַ�����");
				strPool->showStrPool();
			}

			heap->usingAreaChange();//��������ʹ�õ��ڴ�
			usingArea = heap->getUsingArea();
			usingArea->canUseAddress = (int)usingArea->heapArea;

			//����0.Ѱ��GCRoot
			findGCRoot();


			if(testMode==true){
				printf("\n\n--���ڵ㣺\n");
				for(int i=0;i<GCRoots.size();++i){
					Object* ref = GCRoots[i]->ref;
					printf("%d:[%s�� ����%d]",i,ref->objClass->name,ref);
					if(isEqual(ref->objClass->name,(u1*)"java/lang/String") == true){
						printf(" ����:");
						b1* strVal=ref->objClass->loader->heap->getStrPool()->getJStrValue(ref);
						printfUTF8(strVal);
					}
					printf("\n");
				}
				printf("---\n");
			}


			//����1.Ѱ���Ҵ�ڵ㣬�����Ƶ��µ��ڴ�
			//�����ˣ���Slot������ref��ַ
			for(int i=0;i<GCRoots.size();++i){
				Object* oldObj = GCRoots[i]->ref;
				printf("�������ڵ�:%d",oldObj);
				findAliveByDFS(oldObj);
				GCRoots[i]->ref = oldAddressToNew[oldObj];
				printf("��ɡ����ڵ�%d -> %d\n",oldObj,GCRoots[i]->ref);
			}
			printf("\n�������,�ָ����������\n");
			

			//����2.�����¾ɵ�ַӳ�䣬�ָ����������ù�ϵ
			//˳��Ѷ���������Ϊfalse
			recObjectReference();

			if(testMode==true){
				heap->setYoungObjHeader(aliveObjs);
				printf("\n���պ�\n--������(%d ��С%dB ����%dB)",usingArea->heapArea,
					usingArea->maxHeapSize*4,(int)usingArea->canUseAddress-(int)usingArea->heapArea);
				showObjs(heap->getYoungObjHeader(),1);
				
				printf("\n--������");
				showObjs(heap->getOldObjHeader(),1);
				
				printf("\n--�ַ�����");
				strPool->showStrPool();

				printf("\n�������\n\n");
			}
		}

};

void minorGC(Serial* serial);