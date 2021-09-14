/**

 JVM��������й���/���&����

**/
#include "..\headers\UtilTypes.h"
#include "..\headers\Interpreter.h"
#include "..\headers\Heap.h"
#include "..\headers\Recycler.h"
#include "..\headers\UTF8CNDecoder.h"
#include "..\headers\ConfigDecoder.h"
#include <vector>
#include <string>
#include <windows.h>
#include <iostream>
using namespace std; 

char info[10][1000];

string pic[]={
"                                        .:'.                                      ",
"                                       ;|!:;|`                                    ",
"                                .;:    ;!;;;;!%%'                                 ",
"                               ;;.  ';!!;;;|:.:||!`                               ",
"                             `;'      .!$!:!;..'!!!;`                             ",
"                            ';.         `:;|;` .';;:;:                            ",
"                           ::               '` `!|@@@||`                          ",
"                          ::               .. `:;@$@@;.';.                        ",
"                         :'                .'.  `` .:!:'':.                       ",
"                        ''                 .:`    ':''::;:'.                      ",
"                       ':                  .'.      ';':;;:`                      ",
"                      ':                   `'`.     `:::;;;:'                     ",
"                     `!:                   .``.      :;':;;;;`                    ",
"                     !;       .'`          .'`.      `;:;!;;::'                   ",
"                    ;!.       `:'           ``        :;!!;;;''`                  ",
"                   '!'          `!;                   ;|;;;;;:'`.                 ",
"                  `|!.             '|:             '!'`;!;;;;;''`                 ",
"                  !%'                 .;!|%%%%%|;:`    :|;;;;;;```                ",
"                 :$;                             '!;.  `!!;;;;;``:.               ",
"                .||`                          `;!||;`   ;|;;:''. :%;.             ",
"                '|;                            `;||;`   :|;;:`   :%!|!`           ",
"                :|`                             '||!`   `!!;;:.  `|%|!!!'         ",
"                !!.                             .!%!`   .;!!!;:` `!%|||!'         ",
"               `|:                            _#|:$|`    ;|;!;:'..:!||%:          "};

void priPic(){
	printf("\n");
	for(int i=0;i<24;++i){
		printf("%s\n",pic[i].c_str());
	}
	printf("\n");
}

void help(){
	color(4);
	printf("\n>>>>>ȷ����cmd��ִ��! ���л���:Windows����ϵͳ");
	color(7);
	printf("\n\n���ٷ�˹�����ҵ��˾(Saltedfish Software Industry Company)\nColdSpot Java����� \n\n����ָ��˵��");
	printf("\n  {path}	: ����·��\n  -inst		: ����ָ��\n  [input]	: �������\n");
	printf("  <arg>		: Java�����ⲿ��������public static void main������String[] args��������\n\n");
	printf("����ָ������\n  �鿴����ָ��					: {jvm.exe·��} -help\n");
	printf("  ����JVM					: {jvm.exe·��} -info\n");
	printf("  ִ��x.class������				: {jvm.exe·��} -run [x.classȫ��] <����1> <����2> ... <����n>\n");
	printf("  ִ��x.class������(��ʾ����)			: {jvm.exe·��} -showLoop [x.classȫ��] <����1> <����2> ... <����n>\n");
	printf("  ��ʾx.class��Ϣ(�����أ��ֶα�������)	: {jvm.exe·��} -showClassInfo [x.classȫ��]\n");
	printf("  ��ʾx.class������				: {jvm.exe·��} -showConstantPool [x.classȫ��]\n");
	printf("  ��ʾx.class�ֶα�				: {jvm.exe·��} -showFields [x.classȫ��]\n");
	printf("  ��ʾx.class������				: {jvm.exe·��} -showMethods [x.classȫ��]\n");
	printf("  ��ʾx.class�ļ��ֽ���				: {jvm.exe·��} -showByteCode [x.classȫ��]\n");
	printf("  ��ʾJAVA_HOME(��JDKĿ¼)			: {jvm.exe·��} -showJavaHome\n");
	printf("  �����������		: {jvm.exe·��} -testClassLoader\n\n");
	printf("  ��ʾJVM���õ�.class�ļ�����·��		: {jvm.exe·��} -showClassPath\n\n");
	printf("  ��������������				: {jvm.exe·��} -GCTest [x.classȫ��]\n\n");
	printf("ʾ��\n  \"D:SF_JVM 1.1.0.3\\bin\\SF_JVM.exe\" -run test/args/ArgsTest arg1 arg2\n");
	printf("  ���������arg1��arg2��test/args/ArgsTest��������������");
	printf("\n\n>>>���س����˳�");
	scanf("%c");
}

//Java�������Ϣ
void JVMInfo(){
	priPic();
	printf("\n1.��Ʒ����\n");
	printf("  ���Java�����	ColdSpot Java Virtual Machine\n\n");
	printf("2.��������\n");
	printf("  ���ٷ�˹�����ҵ��˾	Saltedfish Software Industry Company	{ ��Ȩ����(C) 2019 }\n\n");
	printf("3.�汾 v1.1.3\n");
	printf("  (1).Java 1.0��׼\n  (2).��֧��UTF-8�������������3500���������ּ����ı��\n  (3).ʵ��serial����������");
	printf("\n  (4).JVMִ���ٶ�δ�Ż�,260����ۼ�/��\n  (5).�쳣��������δ���\n\n");
	printf("4.���л���\n  Windows����ϵͳ\n\n");
}

int main(int argc,char **argv){

	int startMemory=getMemory();
	int startTime=getMS();
	ConfigInfo* config = ConfigDecoder();
	initClassLoader(config);

	if(argc<2){//�������
		help();
	}else{

		if(strcmp(argv[1],"-testClassLoader")==0){
			classFileLoader(argv[2],true);
			return 0;
		}

		//�������߳�
		Thread* mainThread=(Thread*)malloc(sizeof(Thread));	
		mainThread->buildThreadByConfig(config);
		mainThread->name = (u1*)"main";


		//��������������������
		Heap* heap=new Heap(config);
		methodArea *cl=heap->getMethodArea();
		Serial* serial = new Serial(heap,mainThread);
		heap->setGCer(serial);

		if(strcmp(argv[1],"-run")==0){//���г���
			MethodAreaClass* mainClass=loadNonArrayClass(cl,argv[2],false);
			interpreter(mainThread,mainClass,false,startMemory,startTime,argv,argc);
		}else if(strcmp(argv[1],"-showLoop")==0){//���г�����ʾ���裩
			MethodAreaClass* mainClass=loadNonArrayClass(cl,argv[2],false);
			interpreter(mainThread,mainClass,true,startMemory,startTime,argv,argc);
		}else if(strcmp(argv[1],"-showConstantPool")==0){//��ʾ������ʱ������
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			showConstPool(Class->cp);
		}else if(strcmp(argv[1],"-showClassInfo")==0){
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			printf("\n[%s]�ļ̳���:\n",Class->name);
			Class->showExtendsTree(Class);		//��ʾ�̳���

			printf("\n\n[%s]ʵ�ֵĽӿ�:\n",Class->name);
			Class->showInterfacesNames(Class);
			printf("\n");

			showConstPool(Class->cp);				
			Class->floader->showField();			//��ʾ�ֶα�
			Class->mloader->showMethod();			//��ʾ������
		}else if(strcmp(argv[1],"-showFields")==0){//��ʾ���ֶα�
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			Class->floader->showField();
		}else if(strcmp(argv[1],"-showMethods")==0){
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			Class->mloader->showMethod();
		}else if(strcmp(argv[1],"-info")==0){
			JVMInfo();
		}else if(strcmp(argv[1],"-showJavaHome")==0){
			showJavaHome(config);
		}else if(strcmp(argv[1],"-showByteCode")==0){
			FileData* fd=classFileLoader(argv[2],false);
			dataOutput(fd,0);
		}else if(strcmp(argv[1],"-showClassPath")==0){
			showPaths(config);
		}else if(strcmp(argv[1],"-loadClass")==0){
			loadNonArrayClass(cl,argv[2],true);
		}else if(strcmp(argv[1],"-GCTest")==0 || strcmp(argv[1],"-GCTests")==0){

			int size;
			printf("��������ֽڴ�С:");
			scanf("%d",&size);
			heap->reSetHeap(size);

			//��������������������
			methodArea *cl=heap->getMethodArea();
			Serial* serial = new Serial(heap,mainThread);
			heap->setGCer(serial);
			serial->setTest();

			MethodAreaClass* mainClass=loadNonArrayClass(cl,argv[2],false);
			if(strcmp(argv[1],"-GCTest")==0){
				interpreter(mainThread,mainClass,false,startMemory,startTime,argv,argc);
			}else{
				interpreter(mainThread,mainClass,true,startMemory,startTime,argv,argc);
			}
		}else{
			help();
		}
	}
	return 0;
}

