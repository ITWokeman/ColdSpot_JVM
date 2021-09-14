/**
	延伸指令
**/

#pragma once//防止重复定义
#include "..\headers\extended.h"
#include "..\headers\control.h"

//根据是否为空进行跳转
//ifnull:如果引用类型为空，跳转；
//ifnonnull:如果引用类型非空，跳转
void ifNullOrNonNull(Frame* frame,u1 opNum){
	Object* obj=frame->popRef();
	int offset=frame->readB2();
	bool skip=false;
	if(opNum==0xc6 && obj==NULL){skip=true;}
	if(opNum==0xc7 && obj!=NULL){skip=true;}

	if(skip==true){
		//跳转
		Branch(frame,offset);
	}

}

//与_goto的区别是，索引字节码从2字节变为4字节
void _goto_w(Frame* frame,u1 opNum){
	int Offset=frame->readInsTwo();
}

//往译码器里面注册extended系列指令
void regExtendedInsts(JInst* insts){

	insts[0xc6].func=ifNullOrNonNull;
	insts[0xc7].func=ifNullOrNonNull;
	insts[0xc8].func=_goto_w;//注册_goto_w指令

}