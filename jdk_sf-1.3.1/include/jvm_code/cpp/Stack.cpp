
/**
	������ջָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\stack.h"

void pop(Frame* frame,u1 opNum){//����1��4�ֽ����ݣ�int��float��
	//frame->opStack[--frame->operandTop]->flag=0;
	frame->popB4();
}

void pop2(Frame* frame,u1 opNum){//����1��8�ֽ����ݣ�long��double��
	frame->popB8();
}

/*����ջ��Ԫ�أ�Ȼ���ջ
bottom -> top
[...][c][b][a]
             \_
               |
               V
[...][c][b][a][a]
*/
void dup(Frame* frame,u1 opNum){

	Slot* a=frame->popSlot();
	frame->pushSlot(a);
	frame->pushSlot(a);
}

/*����ջ��Ԫ�أ�Ȼ���ջ
bottom -> top
[...][c][b][a]
         \___ \___
             \    |
              V   V
[...][c][b][a][b][a]
*/
void dup2(Frame* frame,u1 opNum){

	Slot* a=frame->popSlot();
	Slot* b=frame->popSlot();

	frame->pushSlot(b);
	frame->pushSlot(a);
	frame->pushSlot(b);
	frame->pushSlot(a);
}

Slot* copySlot(Slot* slot){
	Slot* newSlot=(Slot*)malloc(sizeof(Slot));
	newSlot->ref=slot->ref;
	newSlot->flag=slot->flag;
	newSlot->num=slot->num;
	return newSlot;
}

/*
bottom -> top
[...][c][b][a]
          __/
         |
         V
[...][c][a][b][a]
*/
void dup_x1(Frame* frame,u1 opNum){
	Slot* a=copySlot(frame->popSlot());
	Slot* b=copySlot(frame->popSlot());

	frame->pushSlot(a);
	frame->pushSlot(b);
	frame->pushSlot(a);

	free(a);
	free(b);
}

/*
bottom -> top 
[...][c][b][a]
	   _____/
	  |	
      V
[...][a][c][b][a]
*/
void dup_x2(Frame* frame,u1 opNum){
	Slot* a=copySlot(frame->popSlot());
	Slot* b=copySlot(frame->popSlot());
	Slot* c=copySlot(frame->popSlot());

	frame->pushSlot(a);
	frame->pushSlot(c);
	frame->pushSlot(b);
	frame->pushSlot(a);

	free(a);
	free(b);
	free(c);
}

/*
bottom -> top
[...][c][b][a]
       _/ __/
      |  |
      V  V
[...][b][a][c][b][a]
*/
void dup2_x1(Frame* frame,u1 opNum){
	Slot* a=copySlot(frame->popSlot());
	Slot* b=copySlot(frame->popSlot());
	Slot* c=copySlot(frame->popSlot());

	frame->pushSlot(b);
	frame->pushSlot(a);
	frame->pushSlot(c);
	frame->pushSlot(b);
	frame->pushSlot(a);

	free(a);
	free(b);
	free(c);
}

/*
bottom -> top
[...][d][c][b][a]
		____/ __/
	   |   __/
       V  V
[...][b][a][d][c][b][a]
*/
void dup2_x2(Frame* frame,u1 opNum){
	Slot* a=copySlot(frame->popSlot());
	Slot* b=copySlot(frame->popSlot());
	Slot* c=copySlot(frame->popSlot());
	Slot* d=copySlot(frame->popSlot());

	frame->pushSlot(b);
	frame->pushSlot(a);
	frame->pushSlot(d);
	frame->pushSlot(c);
	frame->pushSlot(b);
	frame->pushSlot(a);

	free(a);
	free(b);
	free(c);
	free(d);
}

/*swapָ�����ջ��Ԫ��
bottom -> top
[...][c][b][a]
          \/
          /\
         V  V
[...][c][a][b]
*/

void _swap(Frame* frame,u1 opNum){

	Slot* a=copySlot(frame->popSlot());
	Slot* b=copySlot(frame->popSlot());

	frame->pushSlot(a);
	frame->pushSlot(b);

	free(a);
	free(b);
}


//������������ע��Stackϵ��ָ��
void regStackInsts(JInst* insts){
	
	insts[0x57].func=pop;//ע��popָ��
	insts[0x58].func=pop2;//ע��pop2ָ��
	insts[0x59].func=dup;//ע��dupָ��

	insts[0x5c].func=dup2;//ע��dup2ָ��
	insts[0x5f].func=_swap;//ע��swapָ��

	insts[0x5a].func=dup_x1;//ע��dup_x1ָ��
	insts[0x5b].func=dup_x2;//ע��dup_x2ָ��
	insts[0x5d].func=dup2_x1;//ע��dup2_x1ָ��
	insts[0x5e].func=dup2_x2;//ע��dup2_x2ָ��
}