/**
	����ָ�
**/

#pragma once//��ֹ�ظ�����
#include "..\headers\loads.h"

//��ʽ��tload n��nΪ2�ֽڴ洢������
//������Ϊt���ֲ��������±�Ϊn�ı���ѹ�����ջ
//���Ż�
void tload(Frame* frame,u1 opNum){

	int index=frame->readB1();

	if(opNum==0x15 || opNum==0x17){//iload,fload
		b4 val=frame->localVars[index]->num;
		frame->localVars[index]->flag=0;
		frame->opStack[frame->operandTop++]->num=val;
		frame->opStack[frame->operandTop-1]->flag = (opNum==0x15 ? 3 : 4);
	}
	else if(opNum==0x16 || opNum==0x18){//lload,dload
		b4 b4s0=frame->localVars[index]->num,b4s1=0;
		if(frame->localVars[index+1]->flag!=0){
			b4s1=frame->localVars[index+1]->num;
		}
	//	b4s[1]��low��b4s[0]��hight
		int f = (opNum==0x16 ? 5 : 6);
		
		frame->opStack[frame->operandTop++]->num=b4s0;
		frame->opStack[frame->operandTop-1]->flag=f;
		frame->opStack[frame->operandTop++]->num=b4s1;
		frame->opStack[frame->operandTop-1]->flag=f;
	}
	else if(opNum==0x19){//aload
		frame->opStack[frame->operandTop]->ref=frame->localVars[index]->ref;
		frame->opStack[frame->operandTop++]->flag=10;
	}
}

void xaload(Frame* frame,u1 opNum){
	if(opNum>=0x2e && opNum<=0x35){//xaload

		int index=frame->opStack[--frame->operandTop]->num;
		Object* arr=frame->opStack[--frame->operandTop]->ref;

		if(arr==NULL){
		}
		if(index<0){
		}

		if(opNum==0x2e || opNum==0x30){//iaload faload 

			frame->opStack[frame->operandTop++]->num=arr->array->B4Array[index];
		}
		else if(opNum==0x2f){//laload
			b4* twoB4 = B8ToTwoB4(arr->array->B8Array[index]);
			frame->opStack[frame->operandTop++]->num=twoB4[1];
			frame->opStack[frame->operandTop++]->num=twoB4[0];
		}
		else if(opNum==0x32){//aaload
			if(index>=arr->array->len){
				char ch[10];
				sprintf(ch, "%d", arr->array->len);
				string str1(ch, ch + strlen(ch));

				sprintf(ch, "%d", index);
				string str2(ch, ch + strlen(ch));
				string info[]={str1,str2};
				printError(0x17,info);
			}
			frame->opStack[frame->operandTop++]->ref=arr->array->objArray[index];
		}
		else if(opNum==0x34){//caload
			frame->opStack[frame->operandTop++]->num=arr->array->B1Array[index];
		}
		frame->opStack[frame->operandTop-1]->flag=arr->array->type;
	}

}

//��ʽ��tload_x��
//������Ϊt���ֲ��������±�Ϊx�ı���ѹ�����ջ
//���Ż�,����
void tload_x(Frame* frame,u1 opNum){
		
	int index;

	if(opNum>=0x1a && opNum<=0x1d){//iload_x
		index=opNum-0x1a;
		frame->opStack[frame->operandTop++]->num=frame->localVars[index]->num;
		frame->opStack[frame->operandTop-1]->flag=3;
	}

	else if(opNum>=0x1e && opNum<=0x21){//lload_x
		index=opNum-0x1e;
		b4 b4s0=frame->localVars[index]->num,b4s1=0;
		if(frame->localVars[index+1]->flag!=0){
			b4s1=frame->localVars[index+1]->num;
		}
		//b4s[1]��low��b4s[0]��hight

		frame->opStack[frame->operandTop++]->num=b4s0;
		frame->opStack[frame->operandTop-1]->flag=5;
		frame->opStack[frame->operandTop++]->num=b4s1;
		frame->opStack[frame->operandTop-1]->flag=5;

	}
	else if(opNum>=0x22 && opNum<=0x25){//fload_x
		index=opNum-0x22;
		b4 val=frame->localVars[index]->num;
		frame->opStack[frame->operandTop++]->num=val;
		frame->opStack[frame->operandTop-1]->flag=4;
	}
	else if(opNum>=0x26 && opNum<=0x29){//dload_x
		index=opNum-0x26;
		b4 b4s0=frame->localVars[index]->num,b4s1;
		if(frame->localVars[index+1]->flag!=0){
			b4s1=frame->localVars[index+1]->num;
		}
		//b4s[0]��low��b4s[1]��hight
		
		frame->opStack[frame->operandTop++]->num=b4s0;
		frame->opStack[frame->operandTop-1]->flag=6;
		frame->opStack[frame->operandTop++]->num=b4s1;
		frame->opStack[frame->operandTop-1]->flag=6;
	}
	else if(opNum>=0x2a && opNum<=0x2d){//aload_x
		index=opNum-0x2a;
		Object* ref=frame->localVars[index]->ref;

		//��Ҫ�ж�ref�Ƿ�գ���ʱ�����Ҫload�յģ�
		frame->opStack[frame->operandTop]->ref=ref;
		frame->opStack[frame->operandTop++]->flag=10;

	}
}

//������������ע��Loadsϵ��ָ��
void regLoadInsts(JInst* insts){
	
	for(int i1=0x15;i1<=0x19;++i1){	
		insts[i1].func=tload;//ע��tloadϵ��ָ��
	}

	for(int i2=0x1a;i2<=0x2d;++i2){	
		insts[i2].func=tload_x;//ע��tload_xϵ��ָ��
	}
	for(int i3=0x2e;i3<=0x35;++i3){	
		insts[i3].func=xaload;//ע��xaloadϵ��ָ��
	}
}

