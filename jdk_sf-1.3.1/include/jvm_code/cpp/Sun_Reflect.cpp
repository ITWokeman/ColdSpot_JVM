/**
sun.reflect下类的本地方法
**/
#pragma once//防止重复定义
#include "../headers/sun_reflect.h"

//-------------------------------Reflection类的本地方法-----------------------------------

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


//-----------------------注册sun.reflect包所有类的本地方法--------------------------------

void regSunReflectAllNativeMethod(Registry* registry){

	string CKeys[]={//sun.misc所有类本地方法的名称
		"sun/reflect/Reflection~getCallerClass~()Ljava/lang/Class;"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//sun.reflect所有类本地方法的数量

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//注册sun.reflect类所有本地方法
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//注册本地方法
	}

	//-----------------本地方法.func = 执行本地方法的相应函数-------------------------
	NMS[0]->func=getCallerClass;

}