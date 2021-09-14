/**
	比较指令集
**/
#pragma once//防止重复定义
#include "..\headers\comparisons.h"

//t类型数据比较
void lcmp(Frame* frame,u1 opNum){
	b8 v2=frame->popB8();
	b8 v1=frame->popB8();
	if(v1>v2){frame->pushB4(1,3);}
	else if(v1==v2){frame->pushB4(0,3);}
	else if(v1<v2){frame->pushB4(-1,3);}
}

//
void fcmpx(Frame* frame,u1 opNum){
	float v2=frame->popFloat();
	float v1=frame->popFloat();

	if(v1>v2){
		frame->pushB4(1,3);
	}else if(v1==v2){
		frame->pushB4(0,3);
	}else if(v1<v2){
		frame->pushB4(-1,3);
	}else{//NaN的情况，对于fcmpg,比较结果是1；而fcmpl，比较结果是-1
		if(opNum==0x95){
			frame->pushB4(1,3);
		}else{
			frame->pushB4(-1,3);
		}
	}
}

//同上
void dcmpx(Frame* frame,u1 opNum){
	double v2=frame->popDouble();
	double v1=frame->popDouble();

	if(v2!=NULL && v1!=NULL){
		if(v1>v2){
			frame->pushB4(1,3);
		}else if(v1==v2){
			frame->pushB4(0,3);
		}else if(v1<v2){
			frame->pushB4(-1,3);
		}
	}else{//NaN的情况，对于dcmpg,比较结果是1；而dcmpl，比较结果是-1
		if(opNum==0x97){
			frame->pushB4(-1,3);
		}else{
			frame->pushB4(1,3);
		}
	}
}

//跳转条件:
//ifeq:x==0(0x99) ifne:x!=0(0x9a) iflt:x<0(0x9b) 
//ifge:x<=0(0x9c) ifgt:x>0(0x9d)  ifle:x>=0(0x9e)
//x为操作数栈顶元素(b4)
void if_cond(Frame* frame,u1 opNum){
	int x=frame->popB4();
	int offset=frame->readB2();

	if(opNum==0x99 && x==0){
		Branch(frame,offset);//跳转
	}else if(opNum==0x9a && x!=0){
		Branch(frame,offset);//跳转
	}else if(opNum==0x9b && x<0){
		Branch(frame,offset);//跳转
	}else if(opNum==0x9c && x>=0){
		Branch(frame,offset);//跳转
	}else if(opNum==0x9d && x>0){
		Branch(frame,offset);//跳转
	}else if(opNum==0x9e && x<=0){
		Branch(frame,offset);//跳转
	}
}

//弹出两个b4类型数据进行比较。跳转条件同上
void if_icmp_cond(Frame* frame,u1 opNum){

	int v2=frame->popB4();
	int v1=frame->popB4();
	int offset=frame->readB2();

	switch(opNum){
	case 0x9f :
		if(v1==v2){Branch(frame,offset);}break;
	case 0xa0 :
		if(v1!=v2){Branch(frame,offset);}break;
	case 0xa1 :
		if(v1<v2){Branch(frame,offset);}break;
	case 0xa2 :
		if(v1>=v2){Branch(frame,offset);}break;
	case 0xa3 :
		if(v1>v2){Branch(frame,offset);}break;
	case 0xa4 :
		if(v1<=v2){Branch(frame,offset);}break;
	}
}

//弹出两个引用类型，根据是否相同进行跳转
void if_acmp_cond(Frame* frame,u1 opNum){
	Object* a2=frame->popRef();
	Object* a1=frame->popRef();
	
	//跳转
	int offset=frame->readB2();
	bool skip=false;
	if(opNum==0xa5 && a1==a2){
		Branch(frame,offset);
	}else if(opNum==0xa6 && a1!=a2){
		Branch(frame,offset);
	}
}

//往译码器里面注册comparisons系列指令
void regComparInsts(JInst* insts){

	insts[0x94].func=lcmp;//注册lcmp指令
	insts[0x95].func=fcmpx;//注册fcmpx系列指令
	insts[0x96].func=fcmpx;//注册fcmpx系列指令
	insts[0x97].func=dcmpx;//注册dcmpx系列指令
	insts[0x98].func=dcmpx;//注册dcmpx系列指令

	for(int i1=0x99;i1<=0x9e;++i1){							
		insts[i1].func=if_cond;//注册if_cond系列指令
	}
	for(int i2=0x9f;i2<=0xa4;++i2){							
		insts[i2].func=if_icmp_cond;//注册if_icmp_cond系列指令
	}

	insts[0xa5].func=if_acmp_cond;//注册if_acmp_cond指令
	insts[0xa6].func=if_acmp_cond;//注册if_acmp_cond指令

}