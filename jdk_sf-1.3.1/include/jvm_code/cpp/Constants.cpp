/**
	常量指令集
**/
#pragma once//防止重复定义
#include "..\headers\constants.h"


void nop(Frame* frame,u1 opNum){

}

//tconst_x系列------------------------------------------------------------------------------------------------------------

void tconst_x(Frame* frame,u1 opNum){

	if(opNum==0x01){
		frame->pushRef(NULL);
	}
	else if(opNum==0x02){
		frame->pushB4(-1,3);
	}
	else if(opNum>=0x03 && opNum<=0x08){//iconst_x
		frame->pushB4(opNum-0x03,3);
	}

	else if(opNum==0x09 || opNum==0x0a){//lconst_x
		frame->pushB8(0,opNum-0x09,5);
	}
	else if(opNum>=0x0b && opNum<=0x0d){//fconst_x
		frame->pushFloat((float)opNum-0x0b);
	}
	else if(opNum>=0x0e || opNum==0x0f){//dconst_x
		frame->pushDouble((double)opNum-0x0e);
	}
	
}

//------------------------------------------------------------------------------------------------------------------------
void bipush(Frame* frame,u1 opNum){
	b4 val=frame->readB1();
	frame->pushB4(val,3);
}

void sipush(Frame* frame,u1 opNum){
	b4 val=frame->readB2();
	frame->pushB4(val,3);
}

//常量池常量加载
void ldc(Frame* frame,u1 opNum){//ldc
	int index=(u1)frame->readB1();
	ConstPool* cp=frame->method->Class->cp;
	Constant* con=cp->consts[index];
	switch(con->type){
	case 3: case 4://Integer、Float
		frame->pushB4(toB4(con->val),con->type);
		break;
	case 5: case 6://Long、Double
		frame->pushB8(toB4(con->val),toB4(cp->consts[index+1]->val),con->type);break;
	case 7:
		{//类引用
		//获取类对象(每个类都有一个可用的类对象)并推入操作数栈
		con->ref->_classRef->Class=resolveClass(con->ref->_classRef);
		frame->pushRef(con->ref->_classRef->Class->jClass);
		break;
	}
	case 8://java/lang/String
		//比如String a = "12345"
		//先通过ldc指令加载运行时常量池的字面量"12345",然后在字符串常量池创建字符串对象"12345"
		int index=(con->val[0]<<8)+con->val[1];
		stringPool* sp=frame->method->Class->loader->heap->getStrPool();
		Object* internedStr=sp->JString((b1*)cp->consts[index]->val);
		frame->pushRef(internedStr);
		break;
	}
}

void ldcn_w(Frame* frame,u1 opNum){//ldc_w,ldc2_w
	int index=frame->readB2();
	ConstPool* cp=frame->method->Class->cp;
	Constant* con=cp->consts[index];
	if(con->type==3 || con->type==4){
		frame->pushB4(toB4(con->val),con->type);
	}
	else if(con->type==5 || con->type==6){
		frame->pushB8(toB4(con->val),toB4(cp->consts[index+1]->val),con->type);
	}
	else if(con->type==8){//第8章实现

	}
	else if(con->type==7){//第9章实现
	}

}

//往译码器里面注册Constants系列指令
void regConstInsts(JInst* insts){

	insts[0].func=nop;//注册nop指令
	
	for(int i1=0x01;i1<=0x0f;++i1){							
		insts[i1].func=tconst_x;//注册tconst_x 系列指令
	}

	insts[0x10].func=bipush;//注册bipush指令
	insts[0x11].func=sipush;//注册sipush指令
	insts[0x12].func=ldc;
	insts[0x13].func=ldcn_w;
	insts[0x14].func=ldcn_w;
}