/**
java.util下类的本地方法
**/
#pragma once//防止重复定义
#include "../headers/java_util.h"

//-------------------------------AtomicLong类的本地方法-----------------------------------

void vmSupportsCS8(Frame* frame) {
	frame->pushB4(0,3);//push false
}

//-----------------------注册java.util包所有类的本地方法----------------------------------

void regJavaUtilAllNativeMethod(Registry* registry){

	string CKeys[]={//java.uitl所有类本地方法的名称
		"java/util/concurrent/atomic/AtomicLong~VMSupportsCS8~()Z"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//sun.reflect所有类本地方法的数量

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//注册sun.reflect类所有本地方法
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//注册本地方法
	}

	//-----------------本地方法.func = 执行本地方法的相应函数-------------------------
	NMS[0]->func=vmSupportsCS8;

}