/**
	����ָ��
**/

#pragma once//��ֹ�ظ�����
#include "..\headers\extended.h"
#include "..\headers\control.h"

//�����Ƿ�Ϊ�ս�����ת
//ifnull:�����������Ϊ�գ���ת��
//ifnonnull:����������ͷǿգ���ת
void ifNullOrNonNull(Frame* frame,u1 opNum){
	Object* obj=frame->popRef();
	int offset=frame->readB2();
	bool skip=false;
	if(opNum==0xc6 && obj==NULL){skip=true;}
	if(opNum==0xc7 && obj!=NULL){skip=true;}

	if(skip==true){
		//��ת
		Branch(frame,offset);
	}

}

//��_goto�������ǣ������ֽ����2�ֽڱ�Ϊ4�ֽ�
void _goto_w(Frame* frame,u1 opNum){
	int Offset=frame->readInsTwo();
}

//������������ע��extendedϵ��ָ��
void regExtendedInsts(JInst* insts){

	insts[0xc6].func=ifNullOrNonNull;
	insts[0xc7].func=ifNullOrNonNull;
	insts[0xc8].func=_goto_w;//ע��_goto_wָ��

}