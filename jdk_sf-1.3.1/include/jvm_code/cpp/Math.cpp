/**
算术与逻辑运算指令集

注意，暂时不做数据越界处理。
两个int相加可能超越int的储存范围，但先不管
类型为t的两个数值相加/减

减法：	bottom -> top	
		[...][c][b][a]
		=b-a
**/
#pragma once//防止重复定义
#include "..\headers\math.h"

void iadd(Frame* frame,u1 opNum){
	b4 v1=frame->popB4();
	b4 v2=frame->popB4();
	frame->pushB4(v1+v2,3);
}

void ladd(Frame* frame,u1 opNum){
	int top=frame->operandTop;
	Slot** s=frame->opStack;
	s[top-3]->num+=s[top-1]->num;
	s[top-4]->num+=s[top-2]->num;
	frame->operandTop-=2;
}

void fadd(Frame* frame,u1 opNum){
	float v[2];
	Slot* s;
	for(int i=0;i<2;++i){
		s=frame->popSlot();
		if(s->flag==4){
			v[i]=B4ToFloat(s->num);
		}else{
			v[i]=s->num;
		}
	}
	frame->pushFloat(v[0]+v[1]);
}

void dadd(Frame* frame,u1 opNum){
	double v[2];
	Slot* s;
	b8 b8Val=0;
	for(int i=0;i<2;++i){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[0]=B8ToDouble(b8Val);}
	else{v[0]=b8Val;}

	b8Val=0;
	for(int j=0;j<2;++j){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[1]=B8ToDouble(b8Val);}
	else{v[1]=b8Val;}

	frame->pushFloat(v[0]+v[1]);
}

void isub(Frame* frame,u1 opNum){
	b4 v2=frame->popB4();
	b4 v1=frame->popB4();
	frame->pushB4(v1-v2,3);
}

void lsub(Frame* frame,u1 opNum){
	int top=frame->operandTop;
	Slot** s=frame->opStack;
	s[top-3]->num-=s[top-1]->num;
	s[top-4]->num-=s[top-2]->num;
	frame->operandTop-=2;
}

void fsub(Frame* frame,u1 opNum){
	float v[2];
	Slot* s;
	for(int i=0;i<2;++i){
		s=frame->popSlot();
		if(s->flag==4){
			v[i]=B4ToFloat(s->num);
		}else{
			v[i]=s->num;
		}
	}
	frame->pushFloat(v[1]-v[0]);
}

void dsub(Frame* frame,u1 opNum){
	double v[2];
	Slot* s;
	b8 b8Val=0;
	for(int i=0;i<2;++i){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[0]=B8ToDouble(b8Val);}
	else{v[0]=b8Val;}

	b8Val=0;
	for(int j=0;j<2;++j){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[1]=B8ToDouble(b8Val);}
	else{v[1]=b8Val;}

	frame->pushFloat(v[1]-v[0]);
}

void imul(Frame* frame,u1 opNum){
	b4 v1=frame->popB4();
	b4 v2=frame->popB4();
	frame->pushB4(v1*v2,3);
}

void lmul(Frame* frame,u1 opNum){

	b8 v2=frame->popB8();
	b8 v1=frame->popB8();
	b8 v3=v1*v2;
	b4* b4s=B8ToTwoB4(v3);

	frame->pushB8(b4s[0],b4s[1],5);
}

void fmul(Frame* frame,u1 opNum){
	float v[2];
	Slot* s;
	for(int i=0;i<2;++i){
		s=frame->popSlot();
		if(s->flag==4){
			v[i]=B4ToFloat(s->num);
		}else{
			v[i]=s->num;
		}
	}
	frame->pushFloat(v[0]*v[1]);
}

void dmul(Frame* frame,u1 opNum){
	double v[2];
	Slot* s;
	b8 b8Val=0;
	for(int j=0;j<2;++j){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[0]=B8ToDouble(b8Val);}
	else{v[0]=b8Val;}

	b8Val=0;
	for(int i=0;i<2;++i){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[1]=B8ToDouble(b8Val);}
	else{v[1]=b8Val;}

	frame->pushDouble(v[0]*v[1]);
}

void idiv(Frame* frame,u1 opNum){
	b4 v2=frame->popB4();
	b4 v1=frame->popB4();
	if(v2==0){
		printf("除0错误!\n");
		return;
	}
	frame->pushB4(v1/v2,3);
}

void ldiv(Frame* frame,u1 opNum){
	b8 v2=frame->popB8();
	b8 v1=frame->popB8();
	if(v2==0){
		printf("除0错误!\n");
		return;
	}
	b8 v3=v1/v2;
	b4* b4s=B8ToTwoB4(v3);

	frame->pushB8(b4s[0],b4s[1],5);
}

void fdiv(Frame* frame,u1 opNum){
	float v[2];
	Slot* s;
	for(int i=0;i<2;++i){
		s=frame->popSlot();
		if(s->flag==4){
			v[i]=B4ToFloat(s->num);
		}else{
			v[i]=s->num;
		}
	}
	if(v[0]==0){
		printf("除0错误!\n");
		return;
	}
	frame->pushFloat(v[1]/v[0]);
}

void ddiv(Frame* frame,u1 opNum){
	double v[2];
	Slot* s;
	b8 b8Val=0;
	for(int i=0;i<2;++i){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[0]=B8ToDouble(b8Val);}
	else{v[0]=b8Val;}

	b8Val=0;
	for(int j=0;j<2;++j){
		s=frame->popSlot();b8Val+=s->num;
	}

	if(s->flag==6){v[1]=B8ToDouble(b8Val);}
	else{v[1]=b8Val;}

	if(v[0]==0){
		printf("除0错误!\n");
		return;
	}

	frame->pushFloat(v[1]/v[0]);
}

//取余数
void trem(Frame* frame,u1 opNum){
	b1* mne=getInstName(opNum);
	int top=frame->operandTop;

	int v2=frame->popB4();
	int v1=frame->popB4();

	if(mne[0]=='i' || mne[0]=='f'){
		if(v2!=0){
			frame->pushB4(v1%v2,mne[0]=='i' ? 3 : 4);
		}else{
			printf("除0异常\n");
		}
	}

	if(mne[0]=='l' || mne[0]=='d'){
		b8 v2=frame->popB8(),v1=frame->popB8();

		union{
			b8 v3;b4 b4s[2];
		}data;

		if(v2!=0){
			data.v3=v1%v2;
			frame->pushB8(data.b4s[0],data.b4s[1],mne[0]=='l' ? 5 : 6);
		}else{
			printf("除0异常\n");
		}
	}
}

//取相反数
void tneg(Frame* frame,u1 opNum){
	frame->opStack[frame->operandTop-1]->num *= -1;
}

//左右移
//算数左右移单纯左右移（ishl,ishr,lshl,lshr）
//逻辑左右移：先转无符号类型，左右移后再转回。即b4->u4->右移->b4。（iushr,lushr）
void tsh(Frame* frame,u1 opNum){

	b4 move=frame->popB4();

	//ishl,ishr,iushr
	if(opNum==0x78 || opNum==0x7a || opNum==0x7c){
		b4 val=frame->popB4();
		move = move & 0x1f;
		switch(opNum){
		case 0x78 : frame->pushB4(val<<move,3);break;
		case 0x7a : frame->pushB4(val>>move,3);break;
		case 0x7c : 
			union{
				b4 b4Val;u4 u4Val;
			}data1;
			data1.b4Val=val;
			u4 m=val>>move;
			data1.u4Val=m;
			frame->pushB4(data1.b4Val,3);
		}
	}else{
		b8 val=frame->popB8();
		
		move = move & 0x3f;
		union{
			b8 v1;b4 b4s[2];
		}data;
		switch(opNum){
		case 0x79 :
			data.v1=val << move; break;
		case 0x7b :
			data.v1=val >> move; break;
		case 0x7d :
				union{
					b8 b8Val;u8 u8Val;
				}data2;
				data2.b8Val=val;
				data2.u8Val=data2.u8Val >> move;//转u8，右移
				data.v1=data2.b8Val;//转回b8
				break;
		}
		frame->pushB8(data.b4s[0],data.b4s[1],5);
	}
}

//逻辑与，或，异或运算（小意思）
//tand tor txor
void tandOrXor(Frame* frame,u1 opNum){
	b1* mne=getInstName(opNum);
	int top=frame->operandTop;

	if(mne[0]=='i' || mne[1]=='f'){
		if(mne[1]=='a'){//与运算 tand
			frame->opStack[top-2]->num=frame->opStack[top-2]->num & frame->opStack[top-1]->num;
		}
		if(mne[1]=='o'){//或运算 tor
			frame->opStack[top-2]->num=frame->opStack[top-2]->num | frame->opStack[top-1]->num;
		}
		if(mne[1]=='x'){//异或运算 txor
			frame->opStack[top-2]->num=frame->opStack[top-2]->num ^ frame->opStack[top-1]->num;
		}

		frame->operandTop--;
	}

	if(mne[0]=='l' || mne[0]=='d'){
		if(mne[1]=='a'){//与运算 tand
			frame->opStack[top-3]->num=frame->opStack[top-3]->num & frame->opStack[top-1]->num;
			frame->opStack[top-4]->num=frame->opStack[top-4]->num & frame->opStack[top-2]->num;
		}
		if(mne[1]=='o'){//或运算 tor
			frame->opStack[top-3]->num=frame->opStack[top-3]->num | frame->opStack[top-1]->num;
			frame->opStack[top-4]->num=frame->opStack[top-4]->num | frame->opStack[top-2]->num;
		}
		if(mne[1]=='x'){//或运算 txor
			frame->opStack[top-3]->num=frame->opStack[top-3]->num ^ frame->opStack[top-1]->num;
			frame->opStack[top-4]->num=frame->opStack[top-4]->num ^ frame->opStack[top-2]->num;
		}
		frame->operandTop-=2;
	}
}

//给局部变量表中的int变量增加常量值
void iinc(Frame* frame,u1 opNum){
	frame->localVars[frame->readIns()]->num+=frame->readB1();
}

//往译码器里面注册Math系列指令
void regMathInsts(JInst* insts){

	insts[0x60].func=iadd;//iadd
	insts[0x61].func=ladd;//ladd
	insts[0x62].func=fadd;//fadd
	insts[0x63].func=dadd;//dadd

	insts[0x64].func=isub;//isub
	insts[0x65].func=lsub;//lsub
	insts[0x66].func=fsub;//fsub
	insts[0x67].func=dsub;//dsub

	insts[0x68].func=imul;//imul
	insts[0x69].func=lmul;//lmul
	insts[0x6a].func=fmul;//fmul
	insts[0x6b].func=dmul;//dmul

	insts[0x6c].func=idiv;//idiv
	insts[0x6d].func=ldiv;//ldiv
	insts[0x6e].func=fdiv;//fdiv
	insts[0x6f].func=ddiv;//ddiv

	for(int i3=0x70;i3<=0x73;++i3){	
		insts[i3].func=trem;//注册trem系列指令
	}
	for(int i4=0x74;i4<=0x77;++i4){	
		insts[i4].func=tneg;//注册tneg系列指令
	}
	for(int i5=0x78;i5<=0x7d;++i5){	
		insts[i5].func=tsh;//注册tsh系列指令
	}
	for(int i6=0x7e;i6<=0x83;++i6){	
		insts[i6].func=tandOrXor;//注册tandOrXor系列指令
	}
	insts[0x84].func=iinc;
}