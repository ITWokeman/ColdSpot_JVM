/**
java.util����ı��ط���
**/
#pragma once//��ֹ�ظ�����
#include "../headers/java_util.h"

//-------------------------------AtomicLong��ı��ط���-----------------------------------

void vmSupportsCS8(Frame* frame) {
	frame->pushB4(0,3);//push false
}

//-----------------------ע��java.util��������ı��ط���----------------------------------

void regJavaUtilAllNativeMethod(Registry* registry){

	string CKeys[]={//java.uitl�����౾�ط���������
		"java/util/concurrent/atomic/AtomicLong~VMSupportsCS8~()Z"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//sun.reflect�����౾�ط���������

	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	for(int i1=0;i1<nativeMethodNum;++i1){//ע��sun.reflect�����б��ط���
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//ע�᱾�ط���
	}

	//-----------------���ط���.func = ִ�б��ط�������Ӧ����-------------------------
	NMS[0]->func=vmSupportsCS8;

}