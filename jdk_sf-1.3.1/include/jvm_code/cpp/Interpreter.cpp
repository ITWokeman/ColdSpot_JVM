/**
  jvm �ֽ���ִ������

**/

#pragma once//��ֹ�ظ�����
#include "..\headers\Interpreter.h"

/*��ÿ��ѭ����ʼ�����õ���ǰ֡��Ȼ�����pc�ӵ�ǰ�����н�
���һ��ָ�ָ��ִ�����֮���ж�Java�����ջ���Ƿ���
֡�����û�����˳�ѭ����������� 
*/
int opN[300];//ָ��ִ�д���
int opT[300];//ָ���ۼƺ�ʱ
int opM[300];//ָ���ۼ�ռ�ÿռ�

//��ȡmain����
Method* getMainMethod(MethodAreaClass* mainClass){

	methodLoader* mLoader=mainClass->mloader;
	
	Method* mainMethod=mLoader->getMethodByNameAndDes("main","([Ljava/lang/String;)V");
	if(mainMethod==NULL || IsStatic(mainMethod->accessFlag)==false){
		printError(0x006,NULL);
	}
	Code* code = mainMethod->codes;
	code->maxStack = code->maxStack+1;
	return mainMethod;
}

void mainMethodInit(Thread* thread,Method* mainMethod,char** argv,int argc){

	Frame* frame=(Frame*)malloc(sizeof(Frame));
	frame->newFrame(thread,mainMethod);
	thread->stack->pushFrame(frame);	//ѹ���߳�

	methodArea* cl = frame->method->Class->loader;


	//��������������public static void main(String[] args)��args����---------------------
	MethodAreaClass* argClass=loadArrayClass(cl,"[Ljava/lang/String;",false);//����java/lang/String;������
	MethodAreaClass* stringClass=loadNonArrayClass(cl,"java/lang/String",false);//����java/lang/String��

	//����java/lang/String�������
	Object* args=newObject(argClass);
	Array* array=(Array*)malloc(sizeof(Array));
	args->array=array;
	array->len=argc-3;

	array->type=10;
	array->objArray=(Object**)calloc(args->array->len,sizeof(Object*));

	if(array->len>0){
		Object* strObj;
		for(int j=0;j<array->len;++j){

			//����java/lang/String����
			strObj=newObject(stringClass);
			strObj->fields[1]->ref=(Object*)malloc(sizeof(Object));
			strObj->fields[1]->ref->array=(Array*)malloc(sizeof(Array));
			strObj->fields[1]->ref->array->B1Array=argv[j+3];

			//��ӵ�java/lang/String����
			array->objArray[j]=strObj;
		}
	}
	
	args->age=0;//��������Ϊ0
	frame->localVars[0]->ref=args;
	frame->localVars[0]->flag=10;

	//-----------------------------------------------------------------
}

//JVM������
void interpreter(Thread* mainThread,MethodAreaClass* mainClass,bool logInst,
				 int startMemory,int startTime,char** argv,int argc){

	//��ȡmain����
	Method* mainMethod = getMainMethod(mainClass);

	//main������ʼ��
	//�ٴ���main����ջ֡ 
	//�ڻ�ȡmain������String[] args���� 
	//�۽�main����ջ֡ѹ��mainThread�ķ���ջ
	mainMethodInit(mainThread,mainMethod,argv,argc);

	//����ָ���
	JInst* instTable=buildInstTable();

	//��ʼ���̵߳�����
	ThreadScheduler* threadScheduler = new ThreadScheduler(mainThread,mainClass->loader,logInst);
	mainThread->threadScheduler = threadScheduler;
	

	//���г���
	int instNum = threadScheduler->startThreadScheduler();

	//������ϣ���ӡJVM�����Ϣ
	methodArea* cl = mainMethod->Class->loader;
	Heap* heap = cl->heap;

	printf("\n>>>�����ʱ:%d ms ",getMS()-startTime);
	printf("ִ��ָ��:%d�� ",instNum);
	printf("��������:%d�� ",cl->getMethodAreaClassNum());
	printf("\n>>>ռ���ڴ�:%.2fMB ",((float)(getMemory()-startMemory)/(1024*1024)));
	printf("�����ڴ�ʹ����:%.4f%%(%ldB / %dByte) ",heap->getHeapAreaUseRatio(),heap->getHeapAreaUseByte(),heap->getHeapAreaTotalByte());
	printf("\n>>>�ַ���������:%d���ַ��� ",cl->heap->getStrPool()->getStrNum());
}




