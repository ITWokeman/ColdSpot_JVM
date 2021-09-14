/**
	�Ƚ�ָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\comparisons.h"

//t�������ݱȽ�
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
	}else{//NaN�����������fcmpg,�ȽϽ����1����fcmpl���ȽϽ����-1
		if(opNum==0x95){
			frame->pushB4(1,3);
		}else{
			frame->pushB4(-1,3);
		}
	}
}

//ͬ��
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
	}else{//NaN�����������dcmpg,�ȽϽ����1����dcmpl���ȽϽ����-1
		if(opNum==0x97){
			frame->pushB4(-1,3);
		}else{
			frame->pushB4(1,3);
		}
	}
}

//��ת����:
//ifeq:x==0(0x99) ifne:x!=0(0x9a) iflt:x<0(0x9b) 
//ifge:x<=0(0x9c) ifgt:x>0(0x9d)  ifle:x>=0(0x9e)
//xΪ������ջ��Ԫ��(b4)
void if_cond(Frame* frame,u1 opNum){
	int x=frame->popB4();
	int offset=frame->readB2();

	if(opNum==0x99 && x==0){
		Branch(frame,offset);//��ת
	}else if(opNum==0x9a && x!=0){
		Branch(frame,offset);//��ת
	}else if(opNum==0x9b && x<0){
		Branch(frame,offset);//��ת
	}else if(opNum==0x9c && x>=0){
		Branch(frame,offset);//��ת
	}else if(opNum==0x9d && x>0){
		Branch(frame,offset);//��ת
	}else if(opNum==0x9e && x<=0){
		Branch(frame,offset);//��ת
	}
}

//��������b4�������ݽ��бȽϡ���ת����ͬ��
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

//���������������ͣ������Ƿ���ͬ������ת
void if_acmp_cond(Frame* frame,u1 opNum){
	Object* a2=frame->popRef();
	Object* a1=frame->popRef();
	
	//��ת
	int offset=frame->readB2();
	bool skip=false;
	if(opNum==0xa5 && a1==a2){
		Branch(frame,offset);
	}else if(opNum==0xa6 && a1!=a2){
		Branch(frame,offset);
	}
}

//������������ע��comparisonsϵ��ָ��
void regComparInsts(JInst* insts){

	insts[0x94].func=lcmp;//ע��lcmpָ��
	insts[0x95].func=fcmpx;//ע��fcmpxϵ��ָ��
	insts[0x96].func=fcmpx;//ע��fcmpxϵ��ָ��
	insts[0x97].func=dcmpx;//ע��dcmpxϵ��ָ��
	insts[0x98].func=dcmpx;//ע��dcmpxϵ��ָ��

	for(int i1=0x99;i1<=0x9e;++i1){							
		insts[i1].func=if_cond;//ע��if_condϵ��ָ��
	}
	for(int i2=0x9f;i2<=0xa4;++i2){							
		insts[i2].func=if_icmp_cond;//ע��if_icmp_condϵ��ָ��
	}

	insts[0xa5].func=if_acmp_cond;//ע��if_acmp_condָ��
	insts[0xa6].func=if_acmp_cond;//ע��if_acmp_condָ��

}