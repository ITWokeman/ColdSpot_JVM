/**

 JVM命令控制行工具/入口&测试

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
	printf("\n>>>>>确保在cmd下执行! 运行环境:Windows操作系统");
	color(7);
	printf("\n\n索顿菲斯软件工业公司(Saltedfish Software Industry Company)\nColdSpot Java虚拟机 \n\n控制指令说明");
	printf("\n  {path}	: 程序路径\n  -inst		: 操作指令\n  [input]	: 输入参数\n");
	printf("  <arg>		: Java程序外部参数。即public static void main方法的String[] args数组内容\n\n");
	printf("控制指令内容\n  查看控制指令					: {jvm.exe路径} -help\n");
	printf("  关于JVM					: {jvm.exe路径} -info\n");
	printf("  执行x.class主方法				: {jvm.exe路径} -run [x.class全称] <参数1> <参数2> ... <参数n>\n");
	printf("  执行x.class主方法(显示步骤)			: {jvm.exe路径} -showLoop [x.class全称] <参数1> <参数2> ... <参数n>\n");
	printf("  显示x.class信息(常量池，字段表，方法表)	: {jvm.exe路径} -showClassInfo [x.class全称]\n");
	printf("  显示x.class常量池				: {jvm.exe路径} -showConstantPool [x.class全称]\n");
	printf("  显示x.class字段表				: {jvm.exe路径} -showFields [x.class全称]\n");
	printf("  显示x.class方法表				: {jvm.exe路径} -showMethods [x.class全称]\n");
	printf("  显示x.class文件字节码				: {jvm.exe路径} -showByteCode [x.class全称]\n");
	printf("  显示JAVA_HOME(即JDK目录)			: {jvm.exe路径} -showJavaHome\n");
	printf("  测试类加载器		: {jvm.exe路径} -testClassLoader\n\n");
	printf("  显示JVM配置的.class文件搜索路径		: {jvm.exe路径} -showClassPath\n\n");
	printf("  垃圾回收器测试				: {jvm.exe路径} -GCTest [x.class全称]\n\n");
	printf("示例\n  \"D:SF_JVM 1.1.0.3\\bin\\SF_JVM.exe\" -run test/args/ArgsTest arg1 arg2\n");
	printf("  即传入参数arg1、arg2给test/args/ArgsTest类主方法并运行");
	printf("\n\n>>>按回车键退出");
	scanf("%c");
}

//Java虚拟机信息
void JVMInfo(){
	priPic();
	printf("\n1.产品名称\n");
	printf("  冷点Java虚拟机	ColdSpot Java Virtual Machine\n\n");
	printf("2.生产厂商\n");
	printf("  索顿菲斯软件工业公司	Saltedfish Software Industry Company	{ 版权所有(C) 2019 }\n\n");
	printf("3.版本 v1.1.3\n");
	printf("  (1).Java 1.0标准\n  (2).仅支持UTF-8编码下输入输出3500个常见汉字及中文标点\n  (3).实现serial垃圾回收器");
	printf("\n  (4).JVM执行速度未优化,260万次累加/秒\n  (5).异常处理部分尚未完成\n\n");
	printf("4.运行环境\n  Windows操作系统\n\n");
}

int main(int argc,char **argv){

	int startMemory=getMemory();
	int startTime=getMS();
	ConfigInfo* config = ConfigDecoder();
	initClassLoader(config);

	if(argc<2){//输出帮助
		help();
	}else{

		if(strcmp(argv[1],"-testClassLoader")==0){
			classFileLoader(argv[2],true);
			return 0;
		}

		//创建主线程
		Thread* mainThread=(Thread*)malloc(sizeof(Thread));	
		mainThread->buildThreadByConfig(config);
		mainThread->name = (u1*)"main";


		//创建堆区和垃圾回收器
		Heap* heap=new Heap(config);
		methodArea *cl=heap->getMethodArea();
		Serial* serial = new Serial(heap,mainThread);
		heap->setGCer(serial);

		if(strcmp(argv[1],"-run")==0){//运行程序
			MethodAreaClass* mainClass=loadNonArrayClass(cl,argv[2],false);
			interpreter(mainThread,mainClass,false,startMemory,startTime,argv,argc);
		}else if(strcmp(argv[1],"-showLoop")==0){//运行程序（显示步骤）
			MethodAreaClass* mainClass=loadNonArrayClass(cl,argv[2],false);
			interpreter(mainThread,mainClass,true,startMemory,startTime,argv,argc);
		}else if(strcmp(argv[1],"-showConstantPool")==0){//显示类运行时常量池
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			showConstPool(Class->cp);
		}else if(strcmp(argv[1],"-showClassInfo")==0){
			MethodAreaClass* Class=loadNonArrayClass(cl,argv[2],false);
			printf("\n[%s]的继承树:\n",Class->name);
			Class->showExtendsTree(Class);		//显示继承树

			printf("\n\n[%s]实现的接口:\n",Class->name);
			Class->showInterfacesNames(Class);
			printf("\n");

			showConstPool(Class->cp);				
			Class->floader->showField();			//显示字段表
			Class->mloader->showMethod();			//显示方法表
		}else if(strcmp(argv[1],"-showFields")==0){//显示类字段表
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
			printf("输入堆区字节大小:");
			scanf("%d",&size);
			heap->reSetHeap(size);

			//创建堆区和垃圾回收器
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

