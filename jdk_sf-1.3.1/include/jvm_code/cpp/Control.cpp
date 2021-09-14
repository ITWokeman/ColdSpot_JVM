/**
	����ָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\control.h"

//��ת����
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

	Frame* currentFrame=thread->stack->popFrame();//�����÷���

	if(opNum==0xb1){
		return;
	}//�����return�Ļ�,ʲôҲ��ִ��

	Frame* invokerFrame=thread->stack->getStackTop();//���÷���

	/*
	�����ireturn,freturn,lreturn,dreturn�Ļ���
	�ѱ�������(currentFrame)���ص���ֵѹ�������(invokerFrame)�Ĳ�����ջ
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

//������������ע��Constantsϵ��ָ��
void regControlInsts(JInst* insts){

	insts[0xa7].func=_goto;//ע��_gotoָ��
	
	for(int i1=0xac;i1<=0xb1;++i1){							
		insts[i1].func=xreturn;//ע��xreturnϵ��ָ��
	}
}