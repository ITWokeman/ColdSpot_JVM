/**
	����ָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\constants.h"


void nop(Frame* frame,u1 opNum){

}

//tconst_xϵ��------------------------------------------------------------------------------------------------------------

void tconst_x(Frame* frame,u1 opNum){

	if(opNum==0x01){
		frame->pushRef(NULL);
	}
	else if(opNum==0x02){
		frame->pushB4(-1,3);
	}
	else if(opNum>=0x03 && opNum<=0x08){//iconst_x
		frame->pushB4(opNum-0x03,3);
	}

	else if(opNum==0x09 || opNum==0x0a){//lconst_x
		frame->pushB8(0,opNum-0x09,5);
	}
	else if(opNum>=0x0b && opNum<=0x0d){//fconst_x
		frame->pushFloat((float)opNum-0x0b);
	}
	else if(opNum>=0x0e || opNum==0x0f){//dconst_x
		frame->pushDouble((double)opNum-0x0e);
	}
	
}

//------------------------------------------------------------------------------------------------------------------------
void bipush(Frame* frame,u1 opNum){
	b4 val=frame->readB1();
	frame->pushB4(val,3);
}

void sipush(Frame* frame,u1 opNum){
	b4 val=frame->readB2();
	frame->pushB4(val,3);
}

//�����س�������
void ldc(Frame* frame,u1 opNum){//ldc
	int index=(u1)frame->readB1();
	ConstPool* cp=frame->method->Class->cp;
	Constant* con=cp->consts[index];
	switch(con->type){
	case 3: case 4://Integer��Float
		frame->pushB4(toB4(con->val),con->type);
		break;
	case 5: case 6://Long��Double
		frame->pushB8(toB4(con->val),toB4(cp->consts[index+1]->val),con->type);break;
	case 7:
		{//������
		//��ȡ�����(ÿ���඼��һ�����õ������)�����������ջ
		con->ref->_classRef->Class=resolveClass(con->ref->_classRef);
		frame->pushRef(con->ref->_classRef->Class->jClass);
		break;
	}
	case 8://java/lang/String
		//����String a = "12345"
		//��ͨ��ldcָ���������ʱ�����ص�������"12345",Ȼ�����ַ��������ش����ַ�������"12345"
		int index=(con->val[0]<<8)+con->val[1];
		stringPool* sp=frame->method->Class->loader->heap->getStrPool();
		Object* internedStr=sp->JString((b1*)cp->consts[index]->val);
		frame->pushRef(internedStr);
		break;
	}
}

void ldcn_w(Frame* frame,u1 opNum){//ldc_w,ldc2_w
	int index=frame->readB2();
	ConstPool* cp=frame->method->Class->cp;
	Constant* con=cp->consts[index];
	if(con->type==3 || con->type==4){
		frame->pushB4(toB4(con->val),con->type);
	}
	else if(con->type==5 || con->type==6){
		frame->pushB8(toB4(con->val),toB4(cp->consts[index+1]->val),con->type);
	}
	else if(con->type==8){//��8��ʵ��

	}
	else if(con->type==7){//��9��ʵ��
	}

}

//������������ע��Constantsϵ��ָ��
void regConstInsts(JInst* insts){

	insts[0].func=nop;//ע��nopָ��
	
	for(int i1=0x01;i1<=0x0f;++i1){							
		insts[i1].func=tconst_x;//ע��tconst_x ϵ��ָ��
	}

	insts[0x10].func=bipush;//ע��bipushָ��
	insts[0x11].func=sipush;//ע��sipushָ��
	insts[0x12].func=ldc;
	insts[0x13].func=ldcn_w;
	insts[0x14].func=ldcn_w;
}