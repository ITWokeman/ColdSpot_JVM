/**
  RunTimeDataAera�������������������߳�,ջ,ջ֡

**/
#pragma once//��ֹ�ظ�����
#include "Heap.h"
#include "UtilTypes.h"
#include "VMTools.h"
#include "methodArea.h"
#include "registry.h"
#include "ConfigDecoder.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

#define defStackMaxSize	 2<<15		//Ĭ�������ջ��С	2<<15 ռ��Լ0.52MB

//----------------------------------------�ṹ��-----------------------------------

struct Thread;//Ԥ�ȶ���
class ThreadScheduler;

//ջ֡
typedef struct Frame{
	u2 localVarsSize;		//�ֲ��������С
	Slot** localVars;		//�ֲ�������
	u2 operandTop;			//������ջԪ�ظ���
	u2 operandSize;			//������ջ��С
	Slot** opStack;			//������ջ�����飩
	u1* ins;

	Thread* thisThread;		//��ǰ�߳�
	Method* method;			//ջ֡����

	int pc;

	void pushFloat(float f){
		union{
			float fVal;	u4 u4Val;
		}data;
		data.fVal=f;
		pushB4(data.u4Val,4);
	}

	void pushB4(b4 val,int flag){
		opStack[operandTop++]->num=val;
		opStack[operandTop-1]->flag=flag;
	}

	void pushB8(b4 hight,b4 low,int flag){
		pushB4(low,flag);pushB4(hight,flag);
	}

	b8 popB8(){
		return opStack[--operandTop]->num+((opStack[--operandTop]->num)<<32);
	}

	void pushDouble(double d){
		union{
			double dVal;	b4 x[2];
		} data;
		data.dVal=d;

		pushB8(data.x[1],data.x[0],6);
	}

	b4 popInt(){return opStack[--operandTop]->num;}

	float popFloat(){
		union{
			float fVal;	b4 x;	
		} data;
		data.x=opStack[--operandTop]->num;
		return data.fVal;
	}

	double popDouble(){
		union{
			double dVal;	b4 x[2];
		}data;
		
		data.x[1]=popB4();
		data.x[0]=popB4();
		return data.dVal;
	}

	b4 popB4(){
		return opStack[--operandTop]->num;
	}

	b4* popTwoB4(){//val[0]��low,val[1]��hight	�����ٶ�����ԭ�򣬲�����ʹ��
		b4* val=(b4*)calloc(2,sizeof(b4));
		val[0]=opStack[--operandTop]->num;//low;
		opStack[operandTop]->flag=0;
		val[1]=opStack[--operandTop]->num;//hight
		opStack[operandTop]->flag=0;
		return val;
	}

	void pushRef(Object* r){
		opStack[operandTop++]->ref=r;
		opStack[operandTop-1]->flag=10;
	}

	Object* popRef(){
		if(operandTop<=0)return NULL;
		Object* obj=opStack[--operandTop]->ref;
		return obj;
	}

	
	void pushSlot(Slot* slot){
		if(opStack[operandTop]==NULL){
			opStack[operandTop]=(Slot*)malloc(sizeof(Slot));
		}
		opStack[operandTop]->ref=slot->ref;
		opStack[operandTop]->num=slot->num;
		opStack[operandTop++]->flag=slot->flag;
	}

	Slot* popSlot(){
		if(operandTop>0){
			Slot* slot = opStack[--operandTop];
			return slot;
		}
	}
//--------------------------------------------------------------------
	u1 readIns(){
		return ins[pc++];
	}

	u2 readInsTwo(){
		return (ins[pc++]<<8)+ins[pc++];
	}

	b1 readB1(){
		return ins[pc++];
	}

	b2 readB2(){
		int v1=ins[pc++];
		int v2=ins[pc++];
		b2 offset=(v1<<8)+v2;
		return offset;
	}
//--------------------------------------------------------------------

	//�½�ջ֡(���ջ֡���ڵ��߳�,�ֲ��������С,����ջ���ջ��)
	void newFrame(Thread* thread,Method* m){
		thisThread=thread;
		method=m;
		if((int)m->codes > 0){
			ins=m->codes->insArray;
		}

		operandSize=m->codes->maxStack;//������ջ��С
		localVarsSize=m->codes->maxLocals;

		if(localVarsSize<m->argSlotCount){
			localVarsSize=m->argSlotCount+1;
		}

		localVars=(Slot**)calloc(localVarsSize,sizeof(Slot*));//�½��ֲ�������
		for(int i=0;i<localVarsSize;++i){
			localVars[i]=(Slot*)malloc(sizeof(Slot));
			localVars[i]->ref=NULL;
		}
		opStack=(Slot**)calloc(operandSize,sizeof(Slot*));//�½�������ջ
		for(int j=0;j<operandSize;++j){
			opStack[j]=(Slot*)malloc(sizeof(Slot));
			opStack[j]->ref=NULL;
		}
		pc=0;
		operandTop=0;
	}

	Slot* getRefFromTop(int index){		
		if(operandTop-index == 0){
			return opStack[0];
		}
		return opStack[operandTop-index-1];
	}

}Frame;

//�����ջ+���ط���ջ
//VM Stack and Native Method Stack
typedef struct Stack{
	u2 maxSize;				//������������
	u2 top;					//�����ջ�����������ǰ������
	Frame** frameStack;		//�����ջ

	//����ջ�ռ�
	void recFrame(Frame* frame){
		int operandSize=frame->operandSize;
		int localVarsSize=frame->localVarsSize;
		Slot** operand=frame->opStack;
		Slot**  localVars=frame->localVars;
		for(int i1=0;i1<operandSize;++i1){
			free(operand[i1]);
		}
		for(int i=0;i<localVarsSize;++i){
			free(localVars[i]);
		}
		free(frame->opStack);
		free(frame->localVars);
		free(frame);
	}

	//��֡����ջ��
	void pushFrame(Frame *addFrame){
		if(top>=maxSize-1){
			printError(0x014,NULL);
		}else{
			
			if(frameStack[top]!=NULL){
				recFrame(frameStack[top]);
			}
			if(addFrame!=NULL){
				frameStack[top++]=addFrame;
			}
		}
	}

	//��ջ��֡����
	Frame* popFrame(){
		if(frameStack[top-1]==NULL){
			printf("ջ��\n");
			return NULL;
		}else{
			return frameStack[--top];
		}
	}

	//���ص�ǰջ��
	Frame* getStackTop(){
		if(frameStack[top-1]==NULL){
			printf("ջ��\n");
			return NULL;
		}else{
			return frameStack[top-1];
		}
	}

	//����frameStack[top](frameStack[ջ��+1])
	Frame* getTop(){
		return frameStack[top];
	}

}Stack;

//�߳�
typedef struct Thread{

	//�߳���Ϣ��״̬
	int id;								//�߳�id,�ɵ�����������
	u1* name;							//�߳�����

	//�߳��ڴ�
	int pc;//ָ�������
	Stack* stack;//�߳�ջ֡
	float stackUseMB;//ʹ���ڴ��С����λ:MB��
	int stackMaxSize;//ջ���

	//�̵߳�����
	ThreadScheduler* threadScheduler;

	//�̶߳�Ӧ���̶߳���
	Object* threadObject;

	//�½�ջ
	Stack* newStack(u2 stackMaxSize){

		Stack* stack=(Stack*)malloc(sizeof(Stack));
		stack->maxSize=stackMaxSize;
		stack->frameStack=(Frame**)calloc(stack->maxSize,sizeof(Frame*));
		stack->top=0;
		return stack;
	}


	void buildThread(int stackMaxSize){
		stack=newStack(stackMaxSize);		
		stackMaxSize=defStackMaxSize;
		stackUseMB=(float)(stack->maxSize*sizeof(Frame*))/(1024*1024);

		//�����߳�Ĭ������ Thread@TheadId
		//str
	}

	//�½��߳�
	void buildThreadByConfig(ConfigInfo* config){
		int stackMaxSize = stringToInt(config->getConfig("MAX_METHOD_STACK"));
		this->stackMaxSize = stackMaxSize;
		buildThread(stackMaxSize);
	}

}Thread;


