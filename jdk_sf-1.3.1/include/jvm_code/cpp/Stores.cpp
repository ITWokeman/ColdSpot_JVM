/**
	操作数栈指令集
**/
#pragma once//防止重复定义
#include "..\headers\stores.h"

//已优化
void tstore(Frame* frame,u1 opNum){

	int index=frame->readB1();

	if(opNum==0x36 || opNum==0x38){
		frame->localVars[index]->num=frame->opStack[--frame->operandTop]->num;
		opNum==0x36 ? frame->localVars[index]->flag=3 : frame->localVars[index]->flag=4;
	}
	else if(opNum==0x37 || opNum==0x39){

		frame->localVars[index+1]->num=frame->opStack[--frame->operandTop]->num;//low
		frame->localVars[index]->num=frame->opStack[--frame->operandTop]->num;//hight

		opNum==0x37 ? frame->localVars[index]->flag=5 , frame->localVars[index+1]->flag=5
			: frame->localVars[index]->flag=6 , frame->localVars[index+1]->flag=6;
	}
	else if(opNum==0x3a){
		frame->localVars[index]->ref=frame->opStack[--frame->operandTop]->ref;
		frame->localVars[index]->flag=10;
	}
}
 
//已优化
void tstore_n(Frame* frame,u1 opNum){

	int index;

	if(opNum>=0x3b && opNum<=0x3e){//istore_x
		index=opNum-0x3b;
		frame->localVars[index]->num=frame->opStack[--frame->operandTop]->num;
		frame->localVars[index]->flag=3;
	}
	else if(opNum>=0x3f && opNum<=0x42){//lstore_x
		index=opNum-0x3f;

		frame->localVars[index+1]->num=frame->opStack[--frame->operandTop]->num;//low
		frame->localVars[index]->num=frame->opStack[--frame->operandTop]->num;//hight
		
		frame->localVars[index]->flag=5; 
		frame->localVars[index+1]->flag=5;
	}
	else if(opNum>=0x43 && opNum<=0x46){//fstore_x
		index=opNum-0x43;
		frame->localVars[index]->num=frame->popB4();
		frame->localVars[index]->flag=4;
	}
	else if(opNum>=0x47 && opNum<=0x4a){//dstore_x
		index=opNum-0x47;

		frame->localVars[index+1]->num=frame->opStack[--frame->operandTop]->num;//low
		frame->localVars[index]->num=frame->opStack[--frame->operandTop]->num;//hight

		frame->localVars[index]->flag=6; 
		frame->localVars[index+1]->flag=6;
	}
	else if(opNum>=0x4b && opNum<=0x4e){//astore系列
		index=opNum-0x4b;
		if(frame->operandTop>0){
			frame->localVars[index]->flag=10;
			frame->localVars[index]->ref=frame->opStack[--frame->operandTop]->ref;
		}
	}
	else if((opNum>=0x4f && opNum<=0x55) && opNum!=0x53){//xastore系列
		b4 num=frame->opStack[--frame->operandTop]->num;
		int index=frame->opStack[--frame->operandTop]->num;

		Object* arr=frame->opStack[--frame->operandTop]->ref;
		if(arr==NULL){
		}
		if(index<0){
		}

		if(opNum==0x4f || opNum==0x51){//iastore fastore 
			arr->array->B4Array[index]=num;
		}else if(opNum==0x55){//castore
			arr->array->B1Array[index]=num;
		}
	}
	else if(opNum==0x53){//aastore
		Object* ref=frame->opStack[--frame->operandTop]->ref;
		index=frame->opStack[--frame->operandTop]->num;
		Object* arrRef=frame->opStack[--frame->operandTop]->ref;
		if(arrRef==NULL){
			return;//panic("java.lang.NullPointerException")
		}
		
		if(index<0 || index>=arrRef->array->len){
			return;//panic("ArrayIndexOutOfBoundsException")
		}
		
		arrRef->array->objArray[index]=ref;
	}

}

//往译码器里面注册Store系列指令
void regStoreInsts(JInst* insts){
	
	for(int i1=0x36;i1<=0x3a;++i1){	
		insts[i1].func=tstore;//注册tstore系列指令
	}
	for(int i2=0x3b;i2<=0x55;++i2){	
		insts[i2].func=tstore_n;//注册tstore_n系列指令
	}
}