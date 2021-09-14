/**
saltedfish.util����ı��ط���
**/
#pragma once//��ֹ�ظ�����
#include "../headers/saltedfish_io.h"

bool checkPath(b1* filePathVals){
	int index=0;
	while(filePathVals[index]!='\0'){
		if(filePathVals[index++]<0){
			printf("\n�޷�ʶ��·��:%s\n",filePathVals);
			return false;
		}
	}
	return true;
}

//--------------------------------SFFile��ı��ط���--------------------------------------
//private native char[] readFileBytes(String filepath);
//(Ljava/lang/String;)[C
//��ȡ�ļ�����
void readFileBytes(Frame* frame){

	Object* filepath = frame->localVars[1]->ref;//readFileBytes�����Ĳ���,filepath����
	stringPool* sPool = getStringPoolFromFrame(frame);
	b1* filePathVals=sPool->getJStrValue(frame->localVars[1]->ref);//��ȡfilepath����

	if(checkPath(filePathVals)==false){
		return;
	}

	
	ifstream infile; 
    infile.open(filePathVals);   //���ļ����������ļ��������� 

    string lineBytes,fileBytes;
    while(getline(infile,lineBytes)){
		fileBytes.append(lineBytes+"\n");
    }
	infile.close();             //�ر��ļ������� 
	
	int fileBytesSize = fileBytes.size();

	Object* charArray=newPrimitiveTypesArray(frame->method->Class->loader,5,fileBytesSize);

	b1* fileChars = (b1*)fileBytes.data();
	b1* chars = charArray->array->B1Array;

	for(int i=0;i<fileBytesSize;++i){
		chars[i] = (b1)fileChars[i];
	}

	frame->pushRef(charArray);
	
}

//private boolean writeFileBytes(String filepath,char[] bytes);
//(Ljava/lang/String;[CI)Z
//��ȡ�ļ�����
void writeFileBytes(Frame* frame){
	Object* filepath = frame->localVars[1]->ref;//readFileBytes�����Ĳ���,filepath����
	stringPool* sPool = getStringPoolFromFrame(frame);
	b1* filePathVals=sPool->getJStrValue(frame->localVars[1]->ref);//��ȡfilepath����
	
	if(checkPath(filePathVals)==false){
		frame->pushB4(0,3);
		return;
	}

	b1* chars = frame->localVars[2]->ref->array->B1Array;//��ȡҪд��ı�������
	int len = frame->localVars[3]->num;

	FILE *fpWrite=fopen(filePathVals,"w");

	if(fpWrite==NULL){

		int index=0;
		while(filePathVals[index++]!='\0');
		while(filePathVals[index--]!='\\');
		filePathVals[index+1]='\0';

		printf("\n�Ҳ���·��[%s]!\n",filePathVals);
		frame->pushB4(0,3);
		return;
	}
	int index=0;
	while(index<len){
		//printf("%d ",(b1)chars[index++]);
		fprintf(fpWrite,"%c",chars[index++]);
	}
	printf("\nlen:%d\n",len);
	fclose(fpWrite);

	frame->pushB4(1,3);
}


//----------------------ע��saltedfish.oi��������ı��ط���------------------------------
void regSaltedfishIOAllNativeMethod(Registry* registry){

	string CKeys[]={//java.lang�����౾�ط���������
		"saltedfish/io/SFFile~readFileBytes~(Ljava/lang/String;)[C",
		"saltedfish/io/SFFile~writeFileBytes~(Ljava/lang/String;[CI)Z"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//java.lang�����౾�ط���������

	//nativeMethods
	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	//ע��java.lang���������б��ط���
	for(int i1=0;i1<nativeMethodNum;++i1){
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//ע�᱾�ط���
	}
	//-----------------���ط���.func = ִ�б��ط�������Ӧ����-------------------------
	NMS[0]->func=readFileBytes;
	NMS[1]->func=writeFileBytes;
}