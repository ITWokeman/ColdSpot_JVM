/**
	���ط���ָ�
**/
#include "..\headers\reserved.h"

//���ط�������
void invokenative(Frame* frame,u1 opNum){
	Method* method=frame->method;

	Heap* heap=method->Class->loader->heap;

	string className,methodName,descriptor;
		
	className  += (char*)method->Class->name;//����
	methodName += (char*)method->name;//������
	descriptor += (char*)method->description;//��������

	MethodAreaClass* Class=frame->method->Class;
	methodArea* loader=Class->loader;
	Registry* nmm=loader->nativeMethodMap;
	nmm->executeNativeMethod(className,methodName,descriptor,frame);

}

//������������ע��Reservedϵ��ָ��
void regReservedInsts(JInst* insts){
	insts[0xfe].func=invokenative;//ע��invokenativeָ��
}
