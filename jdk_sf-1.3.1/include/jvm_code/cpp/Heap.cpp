/**

  Heap(����,������������)
  Heap��������ݡ�������(Object)
  Heap�������������㷨

**/

#pragma once//��ֹ�ظ�����
#include "..\headers\Heap.h"
#include "..\headers\Recycler.h"

//���������½�-------------------------------------------------------------------------------------------------------------------

//�ڶ�����������
Object* newObject(MethodAreaClass* c){

	string info[]={charArrayToString((char*)c->name)};
	if(c==NULL){//����Ҳ�������Ϣ
		printError(0x012,info);
		return NULL;
	}
	
	//�жϸ����Ƿ����ʵ����
	if(IsInterface(c->accessFlag)==1 || IsAbstract(c->accessFlag)==1){//�ж��Ƿ�ӿڻ������
		printError(0x010,info);
		return NULL;
	}

	//����ʵ�������ʹ�����
	Heap* heap = c->loader->heap;
	Object* obj=heap->allocaOfObject(c->instanceSlotCount);//ͨ������Ϊ��������ڴ�

	obj->age=0;//��������Ϊ0
	obj->objClass=c;//������������
	obj->array=NULL;

	return obj;
}

//�½������ע�⣬������ڶ�������ռ�
Object* newJClassObject(MethodAreaClass* jClass,MethodAreaClass* myClass){

	//����һ��java.lang.Class����
	Object* JCO = (Object*)malloc(sizeof(Object));
	int instanceSlotCount = jClass->instanceSlotCount;
	JCO->fields = (Slot**)calloc(instanceSlotCount,sizeof(Slot*));
	JCO->objClass=jClass;
	JCO->age = 0;
	JCO->alive = true;
	JCO->array =NULL;
	for(int i=0;i<instanceSlotCount;++i){
		JCO->fields[i] = (Slot*)malloc(sizeof(Slot));
		JCO->fields[i]->flag = 0;
		JCO->fields[i]->num = 0;
		JCO->fields[i]->ref = NULL;
	}

	//�Ѹö����objClass����ΪmyClass
	JCO->objClass=myClass;
	
	return JCO;
}

Object* newPrimitiveTypesArray(methodArea* loader,int atype,int len){

	Heap* heap = loader->heap;

	Object* arrObj=heap->allocaOfObject(0);
	arrObj->array=(Array*)malloc(sizeof(Array));
	Array* array=arrObj->array;
	array->len=len;
	if(array->len<0){
		return NULL;//panic("java.lang.NegativeArraySizeException")
	}

	//atype:	AT_BOOLEAN=4	AT_CHAR=5	AT_FLOAT=6	AT_DOUBLE=7	AT_BYTE=8	AT_SHORT=9	AT_INT=10	AT_LONG=11
	//type		char=0          byte=1		short=2		int=3		float=4		long=5		double=6	��������=10
	string atypeName[12]={"","","","","[boolean","[char","[float","[double","[byte","[short","[int","[long"};

	arrObj->objClass=loadArrayClass(loader,atypeName[atype],false);
	switch(atype){
	case 4 : array->type=0;break;
	case 5 : array->type=0;break;
	case 6 : array->type=4;break;
	case 7 : array->type=6;break;
	case 8 : array->type=1;break;
	case 9 : array->type=2;break;
	case 10: array->type=3;break;
	case 11: array->type=5;break;
	}

	//int,float,short
	if(atype==10 || atype==6 || atype==9){
		array->B4Array=(b4*)calloc(len,sizeof(b4));
		for(int i=0;i<len;++i){
			array->B4Array[i]=0;
		}
	}

	if(atype==4 || atype==5 || atype==8){
		//b1һ���������ַ����ģ�����Ҫ���⿪һ���ռ䣬��Ϊ�ַ�����β
		array->B1Array=(b1*)calloc(len+1,sizeof(b1));
		for(int i=0;i<len;++i){
			array->B1Array[i]=0;
		}
		array->B1Array[len]='\0';//�����ַ�����β
	}
	else if(atype==11 || atype==7){
		array->B8Array=(b8*)calloc(len,sizeof(b8));
		for(int i=0;i<len;++i){
			array->B8Array[i]=0;
		}
	}
	
	arrObj->age=0;//��������Ϊ0
	return arrObj;
}

stringPool* getStringPoolFromFrame(Frame* frame){
	Method* m=frame->method;//��ȡ����
	MethodAreaClass* c=m->Class;//��ȡ������
	methodArea* ma=c->loader;//��ȡ������
	Heap* heap=ma->heap;//��ȡ����
	return heap->getStrPool();//��ȡ�ַ�����
}
