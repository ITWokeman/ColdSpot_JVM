/**
	本地方法指令集
**/
#include "..\headers\reserved.h"

//本地方法调用
void invokenative(Frame* frame,u1 opNum){
	Method* method=frame->method;

	Heap* heap=method->Class->loader->heap;

	string className,methodName,descriptor;
		
	className  += (char*)method->Class->name;//类名
	methodName += (char*)method->name;//方法名
	descriptor += (char*)method->description;//方法描述

	MethodAreaClass* Class=frame->method->Class;
	methodArea* loader=Class->loader;
	Registry* nmm=loader->nativeMethodMap;
	nmm->executeNativeMethod(className,methodName,descriptor,frame);

}

//往译码器里面注册Reserved系列指令
void regReservedInsts(JInst* insts){
	insts[0xfe].func=invokenative;//注册invokenative指令
}
