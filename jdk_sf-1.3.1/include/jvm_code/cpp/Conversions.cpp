/**
	����ת��ָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\conversions.h"

//ǿ��ת�����ͣ�ûʲô�õģ���Ϊint��float����b4��long��double����b8��
void conversions(Frame* frame,u1 opNum){
	b1* mne=getInstName(opNum);
	int top=frame->operandTop;

	union{
		b8 b8Val;	b4 b4Val;	b2 b2Val;	u2 u2Val;	b1 b1Val;
	}data1;
	union{
		b8 b8Val;	b4 b4Val;	b2 b2Val;	u2 u2Val;	b1 b1Val;	b4 b4s[2];
	}data2;

	int t1=mne[0],t2=mne[2];
	if(t1=='i' || t1=='f'){//i2x �� l2x����int��floatת���������͡�
		//ע��int��float֮�䲻���໥ת������Ϊ������b4�洢��
		if(t2=='l' || t2=='d'){//int��floatתlong��double
			b4 val=frame->popB4();
			frame->pushB8(val,0,t2=='l' ? 5 : 6);
		}

		if(t2=='b'){
			char val=frame->popB4();
			frame->pushB4(val,3);
		}

		if(t2=='c'){//i2c:b4->u2->b4
			data1.b4Val=frame->popB4();
			data2.u2Val=data1.u2Val;//תu2
			frame->pushB4(data2.b4Val,3);//תb4
		}

		if(t2=='s'){//i2s:b4->b2->b4
			//����ת
		}
	}

	if(t1=='l' || t1=='d'){//ͬ��
		b8 val=frame->popB8();
		if(t2=='i'){
			data1.b8Val=val;
			frame->pushB4(data1.b4Val,3);
		}else if(t2=='f'){
			frame->pushFloat(val);
		}

	}
}

//������������ע��Conversionsϵ��ָ��
void regConversInsts(JInst* insts){

	for(int i1=0x85;i1<=0x93;++i1){
		insts[i1].func=conversions;//ע��conversionsָ��
	}

}