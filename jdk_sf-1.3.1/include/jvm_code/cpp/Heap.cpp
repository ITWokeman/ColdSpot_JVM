/**

  Heap(堆区,不包含方法区)
  Heap里面的内容——对象(Object)
  Heap区的垃圾回收算法

**/

#pragma once//防止重复定义
#include "..\headers\Heap.h"
#include "..\headers\Recycler.h"

//堆区对象新建-------------------------------------------------------------------------------------------------------------------

//在堆区创建对象
Object* newObject(MethodAreaClass* c){

	string info[]={charArrayToString((char*)c->name)};
	if(c==NULL){//如果找不到类信息
		printError(0x012,info);
		return NULL;
	}
	
	//判断该类是否可以实例化
	if(IsInterface(c->accessFlag)==1 || IsAbstract(c->accessFlag)==1){//判断是否接口或抽象类
		printError(0x010,info);
		return NULL;
	}

	//可以实例化，就创建类
	Heap* heap = c->loader->heap;
	Object* obj=heap->allocaOfObject(c->instanceSlotCount);//通过堆区为对象分配内存

	obj->age=0;//年龄设置为0
	obj->objClass=c;//对象所属的类
	obj->array=NULL;

	return obj;
}

//新建类对象。注意，类对象不在堆区分配空间
Object* newJClassObject(MethodAreaClass* jClass,MethodAreaClass* myClass){

	//创建一个java.lang.Class对象
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

	//把该对象的objClass设置为myClass
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
	//type		char=0          byte=1		short=2		int=3		float=4		long=5		double=6	引用类型=10
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
		//b1一般是用于字符串的，所以要额外开一个空间，作为字符串结尾
		array->B1Array=(b1*)calloc(len+1,sizeof(b1));
		for(int i=0;i<len;++i){
			array->B1Array[i]=0;
		}
		array->B1Array[len]='\0';//设置字符串结尾
	}
	else if(atype==11 || atype==7){
		array->B8Array=(b8*)calloc(len,sizeof(b8));
		for(int i=0;i<len;++i){
			array->B8Array[i]=0;
		}
	}
	
	arrObj->age=0;//年龄设置为0
	return arrObj;
}

stringPool* getStringPoolFromFrame(Frame* frame){
	Method* m=frame->method;//获取方法
	MethodAreaClass* c=m->Class;//获取方法类
	methodArea* ma=c->loader;//获取方法区
	Heap* heap=ma->heap;//获取堆区
	return heap->getStrPool();//获取字符串池
}
