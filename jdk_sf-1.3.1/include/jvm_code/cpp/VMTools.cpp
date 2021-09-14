/**
 JVM 辅助工具包
 
 开发者：扬·索顿费斯

**/
#pragma once
#include "..\headers\UtilTypes.h"
#include "..\headers\UtilTypes.h"
#include "..\headers\Heap.h"
#include "..\headers\InstDecoder.h"
#include "..\headers\UTF8CNDecoder.h"


//控制台工具------------------------------------------------------------------------------------

void color(int b){
    HANDLE hConsole = GetStdHandle((STD_OUTPUT_HANDLE)) ;
    SetConsoleTextAttribute(hConsole, b) ;//颜色控制
}//颜色控制模块 

//字符串处理工具--------------------------------------------------------------------------------

//后缀判断
bool suffix(string path,string end){
	int pl=path.length()-1,el=end.length()-1;
	for(int i=el;i>=0;--i){
		if(path[pl-i]!=end[el-i]){return false;}
	}
	return true;
}


//判断字符串相等
int isEqual(u1* str1,u1* str2){
	int len1=0;
	int len2=0;

	while(str1[len1++]);
	while(str2[len2++]);

	if(len1 != len2){return 0;}
	for(int i=0;i<len1-1;++i){
		if(str1[i]!=str2[i]){return 0;}
	}
	return 1;
}

//获取类的包名
char* getPackageName(char* name){

	int len=0;
	while(name[len++]);

	while(name[len--]!='/' && len>0);
	if(len==0)return "\0";//没有包名，返回空字符串

	char* p=(char*)calloc(++len+1,sizeof(char));
	for(int i=0;i<len;p[i]=name[i++]);
	p[len]='\0';

	return p;
}

//获取类名
char* getClassName(char* name){
	int start=0,end;

	while(name[start++]);
	end=start;

	while(name[start--]!='/' && start>0);
	if(start==0)return name;//没有包名，返回类名

	char* p=(char*)calloc(end,sizeof(char));
	for(int i=start+2;i<end-1;p[i-start-2]=name[i++]);
	p[end]='\0';
	return p;
}

//类型转换工具----------------------------------------------------------------------------------

b4 toB4(u1* chars){
	union{
		b4 b4Val;
		u1 x[4];
	} data;
	for(int i=0;i<4;++i){
		data.x[i]=chars[3-i];
	}
	return data.b4Val;
}


b2 toB2(u1* chars){
	union{
		b2 b2Val;
		u1 x[4];
	} data;
	for(int i=0;i<2;++i){
		data.x[i]=chars[1-i];
	}
	return data.b2Val;
}


//将8个b1数据转化成两个b4
//b4s[0]是low b4s[1]是high
b4* eightB1ToTwoB4(u1* chars){
	union {
		b4 b4s[2];
		u1 x[8];
	} data;
	for(int i=0;i<8;++i){
		data.x[7-i]=chars[i];
	}
	return data.b4s;
}

_int32 toInt(u1* chars){
	union{
		_int32 intVal;
		u1 x[4];
	} data;
	for(int i=0;i<4;++i){
		data.x[i]=chars[3-i];
	}
	return data.intVal;
}

float toFloat(u1* chars){
	union {
		float floatVar;
		u1 x[4];
	} data;
	for(int i=0;i<4;++i){
		data.x[i]=chars[3-i];
	}
	return data.floatVar;
}

float u4ToFloat(u4 val){
	union {
		float floatVar;
		u4 x;
	} data;
	data.x=val;
	return data.floatVar;
}

double toDouble(u1* chars){
	union {
		double doubleVar;
		u1 x[8];
	} data;
	for(int i=0;i<8;++i){
		data.x[7-i]=chars[i];
	}
	return data.doubleVar;
}

double u8ToDouble(u8 chars){
	union {
		double doubleVar;
		u8 x;
	} data;
	data.x=chars;
	return data.doubleVar;
}

_int64 toLong(u1* chars){
	union {
		_int64 longVal;
		u1 x[8];
	} data;
	for(int i=0;i<8;++i){
		data.x[7-i]=chars[i];
	}
	return data.longVal;
}

b8 toB8(u1* chars){
	union {
		b8 longVal;
		u1 x[8];
	} data;
	for(int i=0;i<8;++i){
		data.x[7-i]=chars[i];
	}
	return data.longVal;
}
float B4ToFloat(b4 val){
	union{
		float fVal;	b4 x;	
	} data;
	data.x=val;
	return data.fVal;
}

double B8ToDouble(b8 val){
	union{
		double dVal;	b8 x;	
	} data;
	data.x=val;
	return data.dVal;
}

b4* B8ToTwoB4(b8 val){
	union{
		b8 b8Val;b4 twoB4[2];
	}data;
	data.b8Val=val;
	return data.twoB4;
}

int stringToInt(string val){
	int digit=1,num=0;
	for(int i=val.size()-1;i>=0;--i){
		num+=digit*(val.c_str()[i]-'0');
		digit*=10;
	}
	return num;
}

bool isNumber(string val){
	char* c = (char*)val.c_str();
	for(int i=val.size()-1;i>=0;--i){
		if(c[i]>'9' || c[i]<'0') return false;
	}
	return true;
}

//报错
void printError(int errorNum,string info[]){
	color(4);
	printf("\n-----------------------------------------------------------------------------------------");

	switch(errorNum){

	case 0x000:
		printf("\n[错误0x000]找不到$JAVA_HOME路径%s\n请在C:\\my_jvm_config\\CONFIG.INI下为$JAVA_HOME参数配置正确路径!\n",info[0].c_str());
		break;
	//未找到.class文件
	case 0x001:
		printf("\n[错误0x001]找不到%s文件!\n请确定输入名称是否正确\n",info[0].c_str());
		printf("\n或到 %s\\config\\CONFIG.INI下正确配置路径(使用txt编辑)\n",info[1].c_str());
		break;

	//常量池解析异常
	case 0x003:printf("\n[错误0x003]%s.class常量池解析异常!\n",info[0].c_str());break;
	
	//虚拟机无法识别指令
	case 0x004:printf("\n[错误0x004]无法识别 \" %s \" 指令!\n",info[0].c_str());break;

	//访问权限错误
	case 0x005:printf("\n[错误0x005]访问权限错误：无法访问 \" %s \" 类\n",info[0].c_str());break;

	case 0x006:printf("\n[错误0x006]找不到主方法 public static void main(String[] arg)! \n");break;

	case 0x008:printf("\n[错误0x008]找不到方法 %s!\n",info[0].c_str());break;

	case 0x009:printf("\n[错误0x009]权限不足,无法访问方法 %s!\n",info[0].c_str());break;

	case 0x010:printf("\n[错误0x010]%s是接口或抽象类,无法实例化!\n",info[0].c_str());break;

	case 0x011:printf("\n[错误0x011]找不到%s类(接口)的引用指针!\n",info[0].c_str());break;

	case 0x012:printf("\n[错误0x012]无法通过%s类引用指针到%s类信息!\n",info[0].c_str());break;

	case 0x013:printf("\n[错误0x013]空指针异常!\n");break;

	case 0x014:printf("\n[错误0x014]虚拟机栈溢出!\n");break;

	case 0x015:printf("\n[错误0x015]数组拷贝异常!\n");break;

	case 0x016:printf("\n[错误0x016]找不到本地方法%s!\n",info[0].c_str());break;

	case 0x017:printf("\n[错误0x017]数组越界! 数组长度:%s   越界索引:%s\n",info[0].c_str(),info[1].c_str());break;

	case 0x018:printf("\n[错误0x018]堆内存溢出!\n");break;

	case 0x019:printf("\n[错误0x019]int型数据输入错误! 错误原因:%s\n",info[0].c_str());break;

	default:break;
	}
	printf("-----------------------------------------------------------------------------------------\n");
	color(7);
	exit(1);
}



//测试工具--------------------------------------------------------------------------------------

//读取string内容，方便测试
void printStr(string str){
	for(int i=0;i<str.length();++i){
		printf("%c",str[i]);
	}
}

u1* readChar(int size,FileData *d){
	printf("[ ");
	u1* chars=(u1*)calloc(size+1,sizeof(u1));
	for(int i=0;i<size;++i){
		chars[i]=d->readU1();
		printf("%d ",chars[i]);
	}
	chars[size]='\0';
	printf(" ]");
	return chars;
}

#define lineByteCodeNum 32//每行字节码数

//格式化输出.class文件的内容
//便于解码开发
void dataOutput(FileData *d,int startIndex){
	printf("\n\n %s文件内容:\n\n",d->name.c_str());
//	color(1);printf(" 魔数   ");
//	color(2);printf("次版本   ");
//	color(3);printf("主版本   ");
//	color(4);printf("常量池   ");
//	color(5);printf("类属性   ");
//	color(6);printf("接口池   ");
//	color(8);printf("字段表   ");
//	color(10);printf("方法表\n");
//	color(7);
	printf(" +-----+-------------------------------------------------------------------------------------------------+\n");
	printf(" |行\\列| 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 |\n");
	printf(" +-----+-------------------------------------------------------------------------------------------------+");
	int i;
	for(int j=startIndex;j<d->size;++j){
		i=j-startIndex;
		
		//输出行头
		if(i%lineByteCodeNum==0){
			int k=i/lineByteCodeNum;
			if(k<10) printf("\n |0000%d| ",k);
			else if(k<100) printf("\n |000%d| ",k);
			else if(k<1000) printf("\n |00%d| ",k);
			else if(k<10000) printf("\n |0%d| ",k);
			else if(k<100000) printf("\n |%d| ",k);
		}
		
		//确定输出何种颜色，以示区分
//		if(j<4){
//			color(1);//魔数
//		}else if(j<6){
//			color(2);//次版本
//		}else if(j<8){
//			color(3);//主版本
//		}else if(j<79){
//			color(4);
//		}

		//输出字节码
		u1 h=d->data[j];
		if(h<16){printf("0%x ",h);}
		else{printf("%x ",h);}


		color(7);
		if(i%lineByteCodeNum==lineByteCodeNum-1){printf("|");}
	}
	while((i++)%lineByteCodeNum!=lineByteCodeNum-1){
		printf("   ");
	}

	printf("|\n");
	printf(" +-----+-------------------------------------------------------------------------------------------------+\n\n");
}

string charArrayToString(char* chars){
	int len=0;
	string str;

	while(chars[len++]);
	for(int i=0;i<len-1;++i){
		str+=chars[i];
	}

	return str;
}

int getMemory(){
    HANDLE handle=GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle,&pmc,sizeof(pmc));
//    cout<<"内存使用:"<<pmc.WorkingSetSize/1000 <<"K/"<<pmc.PeakWorkingSetSize/1000<<"K + "<<pmc.PagefileUsage/1000 <<"K/"<<pmc.PeakPagefileUsage/1000 <<"K"<<endl;

	return (int)pmc.WorkingSetSize;//PeakWorkingSetSize是内存使用峰值
}

b1* getClassKind(u2 accessFlag){
	if(IsInterface(accessFlag) == true){
		return (b1*)"接口";
	}else if(IsAbstract(accessFlag) == false){
		return (b1*)"类";
	}
	return (b1*)"抽象类";
	
}

//运行时常量池内容显示
void showConstPool(ConstPool* cp){

	printf("\n[%s(%s)]运行时常量池(大小%d) 信息如下:\n",cp->Class->name,getClassKind(cp->Class->accessFlag),cp->cpSize);
	Constant* consts;
	for(int i=1;i<cp->cpSize;++i){
		consts=cp->consts[i];
		if(consts == NULL){
			printf("--常量 %d : 未知错误,空!\n",i);
			continue;
		}
		switch(consts->type){
		case 1://utf8
			printf("--常量 %d (Utf8): 内容 %s\n",i,decodeUTF8((b1*)consts->val));
			break;
		case 3://Integer
			printf("--常量 %d (Integer): %d\n",i,toB4(consts->val));break;
		case 4://Float
			printf("--常量 %d (Float): %f\n",i,toFloat(consts->val));break;
		case 5://Long
			printf("--常量 %d (Long): %ld\n",i,toLong(consts->val));break;
		case 6://Double
			printf("--常量 %d (Double): %ld\n",i,toDouble(consts->val));break;
		case 7://Class
			printf("--常量 %d (Class): 类名 %s\n",i,consts->ref->_classRef->name);break;
		case 8://String
			printf("--常量 %d (String): 引用常量%d内容\n",i,(consts->val[0]<<8) + consts->val[1]);break;
		case 9://Field
			printf("--常量 %d (Field): 属于类 %s	字段名 %s	描述 %s\n",i,consts->ref->_fieldRef->cRef->name,consts->ref->_fieldRef->name,consts->ref->_fieldRef->descriptor);
			break;
		case 10://Method
			printf("--常量 %d (Method): 属于类 %s	方法名 %s	描述 %s\n",i,consts->ref->_methodRef->cRef->name,consts->ref->_methodRef->name,consts->ref->_methodRef->descriptor);
			break;
		case 11://InterfaceMethod
		{
			interfaceMethodRef* iRef=consts->ref->_interfaceMethodRef;
			int classIndex=iRef->classIndex;
			int nameAndTypeIndex=iRef->nameAndTypeIndex;

			u1* cname=cp->consts[classIndex]->ref->_classRef->name;
			u1* val=cp->consts[nameAndTypeIndex]->val;

			int nameIndex=(val[0]<<8)+val[1];
			int typeIndex=(val[2]<<8)+val[3];

			printf("--常量 %d (InterfaceMethod): 属于接口%s 方法名 %s 描述 %s\n",i,cname,cp->consts[nameIndex]->val,cp->consts[typeIndex]->val);
			break;
		}
		case 12://NameAndType
			u1* val=consts->val;
			printf("--常量 %d (NameAndType): 名称索引 %d 类型索引 %d\n",i,(val[0]<<8)+val[1],(val[2]<<8)+val[3]);
			break;
		}
	}
}


long getNS(){//纳秒获取(用于优化jvm)
	LARGE_INTEGER nFreq,t1;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	return ((t1.QuadPart%1000000000)*1000000000)/(long)nFreq.QuadPart;
}

int getUS(){//微秒获取(用于优化jvm)
	LARGE_INTEGER nFreq,t1;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	return ((t1.QuadPart%1000000000)*1000000)/(int)nFreq.QuadPart;
}


clock_t getMS(){//毫秒获取(用于优化jvm)
	return clock();
}

void showSlot(Slot* slot){
	if(slot->flag==3 || slot->flag==5){
		printf("%d ",slot->num);
	}else if(slot->flag==4 || slot->flag==6){
		printf("%f ",u4ToFloat(slot->num));
	}else if(slot->flag==2){
		printf("%d ",slot->ref);
	}else if(slot->flag==10){//对象，数组
		if(slot->ref!=NULL){
			if(slot->ref->objClass->name[0]=='['){//说明是数组
				printf("%s对象(数组)[@%d] ",slot->ref->objClass->name,slot->ref);
				if(isEqual((u1*)"[char",slot->ref->objClass->name)==true){//输出char数组内容
					color(6);
					if(slot->ref->array!=NULL ){
						printf("{内容:%s} ",decodeUTF8(slot->ref->array->B1Array));
					}else{
						printf("{内容为空!} ");
					}
					color(7);
				}
			}else{
				printf("类%s对象[@%d]",slot->ref->objClass->name,slot->ref);
				if(isEqual((u1*)"java/lang/String",slot->ref->objClass->name)==true){//输出java/lang/String内容
					color(6);
					b1* strVal=slot->ref->objClass->loader->heap->getStrPool()->getJStrValue(slot->ref);
					if(strVal!=NULL){
						printf("{内容:%s(@%d)} ",decodeUTF8(strVal),strVal);
					}else{
						printf("{内容为空!} ",strVal);
					}
					color(7);
				}else{
					printf(" ");
				}
			}
		}else{
			printf("NULL ");
		}
	}else{printf("NULL ");}
}

//测试工具：显示指令执行过程
//显示局部变量表，操作数栈
void showLoop(Frame* frame,int pc,int opNum,int num){

	Slot** lv=frame->localVars;//本地变量表
	Slot** os=frame->opStack;//操作数栈
	Method* method=frame->method;

	printf(" LocalVaris:{ ");
	for(int i=0;i<frame->localVarsSize;++i){
		showSlot(lv[i]);
	}
	printf("}\n OperaStack:{ ");

	for(int j=0;j<frame->operandTop;++j){
		showSlot(os[j]);
	}
	printf(" } ↘\n");
	color(4);
	Thread* thread=frame->thisThread;
	
	printf(" #%d PC %d:%x(指令:%s) %d线程第%d个栈帧(%s .%s%s方法(栈深:%d)) ↘\n",num,pc,opNum,
		getInstName(opNum),thread->id,thread->stack->top,method->Class->name,method->name,method->description,
		method->codes->maxStack);
	color(7);
}


