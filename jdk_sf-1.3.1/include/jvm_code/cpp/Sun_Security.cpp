/**
java.security下类的本地方法
**/
#pragma once//防止重复定义
#include "../headers/sun_security.h"

//----------------------------AccessController类的本地方法--------------------------------

void doPrivileged(Frame* frame) {

	Object* action=frame->localVars[0]->ref;
	frame->pushRef(action);

	Method* method = action->objClass->mloader->getMethodByNameAndDes("run","()Ljava/lang/Object;");// todo
	invokeMethod(frame,method);
}

//---------------------注册java.security包所有类的本地方法--------------------------------
void regJavaSecurityAllNativeMethod(Registry* registry){

	string CKeys[]={//java.security所有类本地方法的名称
		"java/security/AccessController~doPrivileged~(Ljava/security/PrivilegedAction;)Ljava/lang/Object;"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//java.security所有类本地方法的数量

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//注册java.security类所有本地方法
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//注册本地方法
	}

	//-----------------本地方法.func = 执行本地方法的相应函数-------------------------
	NMS[0]->func=doPrivileged;

}