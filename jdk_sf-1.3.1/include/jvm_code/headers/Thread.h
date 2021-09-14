/**
  RunTimeDataAera（运行数据区）——线程,栈,栈帧

**/
#pragma once//防止重复定义
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

#define defStackMaxSize	 2<<15		//默认虚拟机栈大小	2<<15 占用约0.52MB

//----------------------------------------结构体-----------------------------------

struct Thread;//预先定义
class ThreadScheduler;

//栈帧
typedef struct Frame{
	u2 localVarsSize;		//局部变量表大小
	Slot** localVars;		//局部变量表
	u2 operandTop;			//操作数栈元素个数
	u2 operandSize;			//操作数栈大小
	Slot** opStack;			//操作数栈（数组）
	u1* ins;

	Thread* thisThread;		//当前线程
	Method* method;			//栈帧方法

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

	b4* popTwoB4(){//val[0]是low,val[1]是hight	运行速度慢的原因，不建议使用
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

	//新建栈帧(这个栈帧所在的线程,局部变量表大小,操作栈最大栈深)
	void newFrame(Thread* thread,Method* m){
		thisThread=thread;
		method=m;
		if((int)m->codes > 0){
			ins=m->codes->insArray;
		}

		operandSize=m->codes->maxStack;//操作数栈大小
		localVarsSize=m->codes->maxLocals;

		if(localVarsSize<m->argSlotCount){
			localVarsSize=m->argSlotCount+1;
		}

		localVars=(Slot**)calloc(localVarsSize,sizeof(Slot*));//新建局部变量表
		for(int i=0;i<localVarsSize;++i){
			localVars[i]=(Slot*)malloc(sizeof(Slot));
			localVars[i]->ref=NULL;
		}
		opStack=(Slot**)calloc(operandSize,sizeof(Slot*));//新建操作数栈
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

//虚拟机栈+本地方法栈
//VM Stack and Native Method Stack
typedef struct Stack{
	u2 maxSize;				//虚拟机最大容量
	u2 top;					//虚拟机栈顶（虚拟机当前容量）
	Frame** frameStack;		//虚拟机栈

	//回收栈空间
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

	//把帧推入栈顶
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

	//把栈顶帧弹出
	Frame* popFrame(){
		if(frameStack[top-1]==NULL){
			printf("栈空\n");
			return NULL;
		}else{
			return frameStack[--top];
		}
	}

	//返回当前栈顶
	Frame* getStackTop(){
		if(frameStack[top-1]==NULL){
			printf("栈空\n");
			return NULL;
		}else{
			return frameStack[top-1];
		}
	}

	//返回frameStack[top](frameStack[栈顶+1])
	Frame* getTop(){
		return frameStack[top];
	}

}Stack;

//线程
typedef struct Thread{

	//线程信息及状态
	int id;								//线程id,由调度器数设置
	u1* name;							//线程名称

	//线程内存
	int pc;//指令计数器
	Stack* stack;//线程栈帧
	float stackUseMB;//使用内存大小（单位:MB）
	int stackMaxSize;//栈深度

	//线程调度器
	ThreadScheduler* threadScheduler;

	//线程对应的线程对象
	Object* threadObject;

	//新建栈
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

		//设置线程默认名字 Thread@TheadId
		//str
	}

	//新建线程
	void buildThreadByConfig(ConfigInfo* config){
		int stackMaxSize = stringToInt(config->getConfig("MAX_METHOD_STACK"));
		this->stackMaxSize = stackMaxSize;
		buildThread(stackMaxSize);
	}

}Thread;


