/**
saltedfish.util下类的本地方法
**/
#pragma once//防止重复定义
#include "../headers/saltedfish_io.h"

bool checkPath(b1* filePathVals){
	int index=0;
	while(filePathVals[index]!='\0'){
		if(filePathVals[index++]<0){
			printf("\n无法识别路径:%s\n",filePathVals);
			return false;
		}
	}
	return true;
}

//--------------------------------SFFile类的本地方法--------------------------------------
//private native char[] readFileBytes(String filepath);
//(Ljava/lang/String;)[C
//读取文件内容
void readFileBytes(Frame* frame){

	Object* filepath = frame->localVars[1]->ref;//readFileBytes方法的参数,filepath对象
	stringPool* sPool = getStringPoolFromFrame(frame);
	b1* filePathVals=sPool->getJStrValue(frame->localVars[1]->ref);//获取filepath内容

	if(checkPath(filePathVals)==false){
		return;
	}

	
	ifstream infile; 
    infile.open(filePathVals);   //将文件流对象与文件连接起来 

    string lineBytes,fileBytes;
    while(getline(infile,lineBytes)){
		fileBytes.append(lineBytes+"\n");
    }
	infile.close();             //关闭文件输入流 
	
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
//读取文件内容
void writeFileBytes(Frame* frame){
	Object* filepath = frame->localVars[1]->ref;//readFileBytes方法的参数,filepath对象
	stringPool* sPool = getStringPoolFromFrame(frame);
	b1* filePathVals=sPool->getJStrValue(frame->localVars[1]->ref);//获取filepath内容
	
	if(checkPath(filePathVals)==false){
		frame->pushB4(0,3);
		return;
	}

	b1* chars = frame->localVars[2]->ref->array->B1Array;//获取要写入的比特数组
	int len = frame->localVars[3]->num;

	FILE *fpWrite=fopen(filePathVals,"w");

	if(fpWrite==NULL){

		int index=0;
		while(filePathVals[index++]!='\0');
		while(filePathVals[index--]!='\\');
		filePathVals[index+1]='\0';

		printf("\n找不到路径[%s]!\n",filePathVals);
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


//----------------------注册saltedfish.oi包所有类的本地方法------------------------------
void regSaltedfishIOAllNativeMethod(Registry* registry){

	string CKeys[]={//java.lang所有类本地方法的名称
		"saltedfish/io/SFFile~readFileBytes~(Ljava/lang/String;)[C",
		"saltedfish/io/SFFile~writeFileBytes~(Ljava/lang/String;[CI)Z"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//java.lang所有类本地方法的数量

	//nativeMethods
	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	//注册java.lang包下类所有本地方法
	for(int i1=0;i1<nativeMethodNum;++i1){
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//注册本地方法
	}
	//-----------------本地方法.func = 执行本地方法的相应函数-------------------------
	NMS[0]->func=readFileBytes;
	NMS[1]->func=writeFileBytes;
}