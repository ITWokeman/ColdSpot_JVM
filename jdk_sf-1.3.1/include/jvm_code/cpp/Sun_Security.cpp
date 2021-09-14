/**
java.security����ı��ط���
**/
#pragma once//��ֹ�ظ�����
#include "../headers/sun_security.h"

//----------------------------AccessController��ı��ط���--------------------------------

void doPrivileged(Frame* frame) {

	Object* action=frame->localVars[0]->ref;
	frame->pushRef(action);

	Method* method = action->objClass->mloader->getMethodByNameAndDes("run","()Ljava/lang/Object;");// todo
	invokeMethod(frame,method);
}

//---------------------ע��java.security��������ı��ط���--------------------------------
void regJavaSecurityAllNativeMethod(Registry* registry){

	string CKeys[]={//java.security�����౾�ط���������
		"java/security/AccessController~doPrivileged~(Ljava/security/PrivilegedAction;)Ljava/lang/Object;"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//java.security�����౾�ط���������

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//ע��java.security�����б��ط���
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//ע�᱾�ط���
	}

	//-----------------���ط���.func = ִ�б��ط�������Ӧ����-------------------------
	NMS[0]->func=doPrivileged;

}