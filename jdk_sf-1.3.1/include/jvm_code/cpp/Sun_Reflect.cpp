/**
sun.reflect����ı��ط���
**/
#pragma once//��ֹ�ظ�����
#include "../headers/sun_reflect.h"

//-------------------------------Reflection��ı��ط���-----------------------------------

//public static native Class<?> getCallerClass();
//()Ljava/lang/Class;
void getCallerClass(Frame* frame) {
	// frameStack :
	// top is NULL
	// top-1 is sun/reflect/Reflection
	// top-2 is the caller of getCallerClass()
	// top-3 is the caller of method
	int top=frame->thisThread->stack->top;
	Frame* callerframe=frame->thisThread->stack->frameStack[top-3];// todo
	Object* callerClass=callerframe->method->Class->jClass;
	frame->pushRef(callerClass);
}


//-----------------------ע��sun.reflect��������ı��ط���--------------------------------

void regSunReflectAllNativeMethod(Registry* registry){

	string CKeys[]={//sun.misc�����౾�ط���������
		"sun/reflect/Reflection~getCallerClass~()Ljava/lang/Class;"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//sun.reflect�����౾�ط���������

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//ע��sun.reflect�����б��ط���
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//ע�᱾�ط���
	}

	//-----------------���ط���.func = ִ�б��ط�������Ӧ����-------------------------
	NMS[0]->func=getCallerClass;

}