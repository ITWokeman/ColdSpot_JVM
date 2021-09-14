/**
  jvm 字节码执行引擎

**/

#pragma once//防止重复定义
#include "..\headers\Interpreter.h"

/*在每次循环开始，先拿到当前帧，然后根据pc从当前方法中解
码出一条指令。指令执行完毕之后，判断Java虚拟机栈中是否还有
帧。如果没有则退出循环；否则继续 
*/
int opN[300];//指令执行次数
int opT[300];//指令累计耗时
int opM[300];//指令累计占用空间

//获取main方法
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
	thread->stack->pushFrame(frame);	//压入线程

	methodArea* cl = frame->method->Class->loader;


	//打包输入参数。及public static void main(String[] args)的args数组---------------------
	MethodAreaClass* argClass=loadArrayClass(cl,"[Ljava/lang/String;",false);//加载java/lang/String;数组类
	MethodAreaClass* stringClass=loadNonArrayClass(cl,"java/lang/String",false);//加载java/lang/String类

	//创建java/lang/String数组对象
	Object* args=newObject(argClass);
	Array* array=(Array*)malloc(sizeof(Array));
	args->array=array;
	array->len=argc-3;

	array->type=10;
	array->objArray=(Object**)calloc(args->array->len,sizeof(Object*));

	if(array->len>0){
		Object* strObj;
		for(int j=0;j<array->len;++j){

			//创建java/lang/String对象
			strObj=newObject(stringClass);
			strObj->fields[1]->ref=(Object*)malloc(sizeof(Object));
			strObj->fields[1]->ref->array=(Array*)malloc(sizeof(Array));
			strObj->fields[1]->ref->array->B1Array=argv[j+3];

			//添加到java/lang/String数组
			array->objArray[j]=strObj;
		}
	}
	
	args->age=0;//年龄设置为0
	frame->localVars[0]->ref=args;
	frame->localVars[0]->flag=10;

	//-----------------------------------------------------------------
}

//JVM解释器
void interpreter(Thread* mainThread,MethodAreaClass* mainClass,bool logInst,
				 int startMemory,int startTime,char** argv,int argc){

	//获取main方法
	Method* mainMethod = getMainMethod(mainClass);

	//main方法初始化
	//①创建main方法栈帧 
	//②获取main方法的String[] args参数 
	//③将main方法栈帧压入mainThread的方法栈
	mainMethodInit(mainThread,mainMethod,argv,argc);

	//创建指令表
	JInst* instTable=buildInstTable();

	//初始化线程调度器
	ThreadScheduler* threadScheduler = new ThreadScheduler(mainThread,mainClass->loader,logInst);
	mainThread->threadScheduler = threadScheduler;
	

	//运行程序
	int instNum = threadScheduler->startThreadScheduler();

	//运行完毕，打印JVM相关信息
	methodArea* cl = mainMethod->Class->loader;
	Heap* heap = cl->heap;

	printf("\n>>>完成用时:%d ms ",getMS()-startTime);
	printf("执行指令:%d条 ",instNum);
	printf("加载类数:%d个 ",cl->getMethodAreaClassNum());
	printf("\n>>>占用内存:%.2fMB ",((float)(getMemory()-startMemory)/(1024*1024)));
	printf("堆区内存使用率:%.4f%%(%ldB / %dByte) ",heap->getHeapAreaUseRatio(),heap->getHeapAreaUseByte(),heap->getHeapAreaTotalByte());
	printf("\n>>>字符串常量池:%d个字符串 ",cl->heap->getStrPool()->getStrNum());
}




