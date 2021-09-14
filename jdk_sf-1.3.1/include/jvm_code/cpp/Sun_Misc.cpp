/**
sun.misc����ı��ط���
**/
#pragma once//��ֹ�ظ�����
#include "../headers/sun_misc.h"

//----------------------------------VM��ı��ط���----------------------------------------

//private static native void initialize(); 
//()V
void initialize(Frame* frame) { // hack: just make VM.savedProps nonempty

	MethodAreaClass* vmClass=frame->method->Class;
	fieldLoader* floader=vmClass->floader;

	Field* savedPropsField=floader->getFieldByName("savedProps");
	Object* savedProps=vmClass->staticVars[savedPropsField->slotId]->ref;

	Heap* heap=vmClass->loader->heap;
	stringPool* sPool=heap->getStrPool();

	Object* key=sPool->JString((b1*)"foo");
	Object* val=sPool->JString((b1*)"bar");

	frame->pushRef(savedProps);
	frame->pushRef(key);
	frame->pushRef(val);

	MethodAreaClass* propsClass=loadNonArrayClass(vmClass->loader,"java/util/Properties",false);
	methodLoader* mLoader=propsClass->mloader;
	Method* setPropMethod=mLoader->getMethodByNameAndDes("setProperty","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
	
	invokeMethod(frame,setPropMethod);
}

//--------------------------------Unsafe��ı��ط���--------------------------------------

//public native int arrayBaseOffset(Class<?> type);
//(Ljava/lang/Class;)I
void arrayBaseOffset(Frame* frame) {
	frame->pushB4(0,3);
}

//public native int arrayIndexScale(Class<?> type);
//(Ljava/lang/Class;)I
void arrayIndexScale(Frame* frame) {
	frame->pushB4(1,3);
}

//public native int addressSize();
//()I
void addressSize(Frame* frame) {
	frame->pushB4(8,3);// todo unsafe.Sizeof(int)
}

void objectFieldOffset(Frame* frame){
	frame->pushB8(0,0,5);
}

void compareAndSwapObject(Frame* frame){

    /**
	*�����Ⱦ͸��·���true���������ȾͲ����·���false��
    * compareAndSwapObject(Object var1, long var2, Object var3, Object var4)
    * var1 �����Ķ���
    * var2 �����Ķ�������
    * var3 var2��var3�Ƚϣ���ȲŸ���
    * var4 ����ֵ
    */

	/*
	Slot** localVars = frame->localVars;
	Slot* var1 = localVars[1];
	b8 var2 = (localVars[2]->num << 8) + localVars[3]->num;
	Slot* var3 = localVars[4];
	Slot* var4 = localVars[5];

	if((var1 != NULL && var3 != NULL && var1->ref->fields!=NULL 
		&& var1->ref->fields[var2]->ref == var3->ref) || (var1 == NULL && var3 == NULL)){
		var1 = var4;
		frame->pushB4(1,3);
		return;
	}*/
	frame->pushB4(0,3);

}

//-------------------------ע��sun.misc��������ı��ط���---------------------------------
void regSunMiscAllNativeMethod(Registry* registry){

	string CKeys[]={//sun.misc�����౾�ط���������
		"sun/misc/VM~initialize~()V",
		"sun/misc/Unsafe~arrayBaseOffset~(Ljava/lang/Class;)I",
		"sun/misc/Unsafe~arrayIndexScale~(Ljava/lang/Class;)I",
		"sun/misc/Unsafe~addressSize~()I",
		"sun/misc/Unsafe~objectFieldOffset~(Ljava/lang/reflect/Field;)J",
		"sun/misc/Unsafe~compareAndSwapObject~(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//sun.mis�����౾�ط���������

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//ע��sun.mis���б��ط���
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//ע�᱾�ط���
	}

	//-----------------���ط���.func = ִ�б��ط�������Ӧ����-------------------------
	NMS[0]->func=initialize;
	NMS[1]->func=arrayBaseOffset;
	NMS[2]->func=arrayIndexScale;
	NMS[3]->func=addressSize;
	NMS[4]->func=objectFieldOffset;
	NMS[5]->func=compareAndSwapObject;
}