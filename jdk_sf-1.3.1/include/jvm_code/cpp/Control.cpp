/**
	控制指令集
**/
#pragma once//防止重复定义
#include "..\headers\control.h"

//跳转命令
void Branch(Frame* frame,int offset) {
	frame->pc = frame->thisThread->pc + offset;

}

void _goto(Frame* frame,u1 opNum){
	int offset=frame->readB2();
	Branch(frame,offset);
}

void tableswitch(){
}

void lookupswitch(){
}

void xreturn(Frame* frame,u1 opNum){

	Thread* thread=frame->thisThread;

	Frame* currentFrame=thread->stack->popFrame();//被调用方法

	if(opNum==0xb1){
		return;
	}//如果是return的话,什么也不执行

	Frame* invokerFrame=thread->stack->getStackTop();//调用方法

	/*
	如果是ireturn,freturn,lreturn,dreturn的话，
	把被调用者(currentFrame)返回的数值压入调用者(invokerFrame)的操作数栈
	*/
	if(opNum==0xac || opNum==0xae){//ireturn,freturn
		b4 val=currentFrame->popB4();
//		printf("\n{%s.%s}\n",invokerFrame->method->Class->name,invokerFrame->method->name);
		invokerFrame->pushB4(val,opNum==0xac ? 3 : 4);
	}
	if(opNum==0xad){//lreturn
		b4* val=currentFrame->popTwoB4();
		invokerFrame->pushB8(val[1],val[0],5);
	}
	if(opNum==0xaf){//dreturn
		double val=currentFrame->popDouble();
		invokerFrame->pushDouble(val);
	}
	if(opNum==0xb0){//areturn
		Object* ref=currentFrame->popRef();
		invokerFrame->pushRef(ref);
	}
}

//往译码器里面注册Constants系列指令
void regControlInsts(JInst* insts){

	insts[0xa7].func=_goto;//注册_goto指令
	
	for(int i1=0xac;i1<=0xb1;++i1){							
		insts[i1].func=xreturn;//注册xreturn系列指令
	}
}