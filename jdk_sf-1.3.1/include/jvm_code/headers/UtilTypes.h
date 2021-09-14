/**
  通用类型，常量信息表
**/
#pragma once//防止重复定义

#include <string>
#include <iostream>
#include <vector>
using namespace std;


//基础信息集合--------------------------------------------------------------------------------------------------------------

typedef unsigned char u1;//1字节无符号
typedef unsigned int u2;//2字节无符号
typedef unsigned long u4;//4字节无符号
typedef unsigned _int64 u8;//8字节无符号

typedef char  b1;
typedef short b2;
typedef int b4;//4字节有符号
typedef __int64 b8;

typedef unsigned char* classReader;//比特数组
typedef unsigned char* ConstantInfo;//常量内容
typedef ConstantInfo* ConstantPool;//常量池

typedef struct AttrInfo AttrInfo;
typedef struct Attributes Attributes;

typedef struct Slot Slot;
struct Object;


//.class文件字节码信息------------------------------------------------------------------------------------------------------

//存放从.class文件里读取的字节数据,并提供若干方法供外部读取数据（8位，16位，32位，64位）以便解码
struct FileData{
	string name;//.class文件名
	unsigned char *data;//存放数据数组（8位）
	int start;//开始读取数据的索引，用于数据处理
	int size;//大小
	//[0,start)为已经读取过的数据
	//[start,size)为未读取数据
	FileData(unsigned char *d,int s){
		data=d;size=s;
		start=0;
	}

	//读取1个8位数据：（unsigned char）1字节
	unsigned char readU1(){
		if(size>start){
			return data[start++];
		}
		return 0;
	}

		//读取1个16位数据：（unsigned int）2字节
	unsigned int readU2(){
		unsigned int read=0;
		int usize;
		size-start>2 ? usize=2 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}

	//读取1个32位数据：（unsigned long）4字节
	unsigned long readU4(){
		unsigned int read=0;
		int usize;
		size-start>4 ? usize=4 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}
		

	//读取1个64位数据：（unsigned __int64）8字节
	unsigned _int64 readU8(){
		unsigned _int64 read=0;
		int usize;
		size-start>8 ? usize=8 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}
		
		//读取若干位8位数据
	unsigned char* readBytes(unsigned int n){
		unsigned char* s=(unsigned char*)calloc(n+1,sizeof(unsigned char));
		for(int i=0;i<n;++i){
			s[i]=readU1();
		}
		s[n]='\0';//一定要加上这玩意，代表结束！！不然会一直找下去，找到'\0'为止
		return s;
	}

	//读取若干16位数据
	unsigned int* readU2s(int n){
		unsigned int* s=(unsigned int*)calloc(n,sizeof(unsigned int));
		for(int i=0;i<n;++i){
			s[i]=readU2();
		}
		return s;
	}
		
};

//Attribute_info类型集--------------------------------------------------------------
//即属性信息集合

//局部变量信息
typedef struct LocalVarInfo{
	u2 start_pc;			//局部变量生命周期开始的字节码偏移量
	u2 length;				//局部变量作用覆盖范围
	u2 nameIndex;			//局部变量名称
	u2 descIndex;			//局部变量描述符
	u2 index;				//局部变量在栈帧局部变量表的位置(如果是long/double,占两位)
}LocalVarInfo;

//局部变量表
typedef struct LocalVarTable{
	u2 attrNameIndex;
	u4 attrLen;
	u2 localVarTableLen;
	LocalVarInfo** localVars;
}LocalVarTable;

//弃用属性
typedef struct Deprecate{
	u2 nameIndex;
	u4 attrLen;
}Deprecate;

//合成属性
typedef struct Synthetic{
	u2 nameIndex;
	u4 attrLen;
}Synthetic;

//源文件
typedef struct SourceFile{
	u2 nameIndex;
	u4 attrLen;
	u2 sourceFileIndex;
}SourceFile;

//异常表属性
typedef struct Exceptions{
	u2 nameIndex;
	u4 attrLen;
	u2 exceptionsNum;
	u2* exceptionIndexTable;
}Exceptions;

//ConstantValue属性。存放静态变量
//存放在字段表中
typedef struct ConstantValue{
	u2 attNameIndex;
	u4 attrLen;
	u2 constantValueIndex;
}ConstantValue;

typedef struct ExceptionInfo{
	u2 start_pc;
	u2 end_pc;
	u2 handler_pc;
	u2 catch_type;
}ExceptionInfo;

typedef struct LineNumInfo{
	u2 start_pc;//字节码行号
	u2 lineNum;//java源码行号
}LineNumInfo;

//行号信息
typedef struct LineNumTable{
	u2 attrNameIndex;
	u4 attrLen;
	u2 lineNumTableLen;
	LineNumInfo** lineNums;
}LineNumTableInfo;


//其他不明属性（RuntimeVisableAnntations,Signature等）
typedef struct Other{
	u2 attrNameIndex;
	u4 len;
	u1* chars;
}Other;

//code属性。方法体的代码。存放java代码编译成的字节码指令
typedef struct Code{
	u2 attrNameIndex;						//属性类型名称索引
	u4 codeLen;								//code长度
	u2 maxStack;							//操作栈最大深度
	u2 maxLocals;							//局部变量储存需要的空间
	u4 insLen;								//字节码指令数组长。java虚拟机规范中一个方法不能超过65535条指令
	u1* insArray;							//字节码指令数组
	u2 exceptionInfoNum;					//异常信息表大小
	ExceptionInfo** excs;					//异常信息(不是Exceptions!);
	u2 attrCount;							//code属性的属性个数

	u1* surplus;							//剩余字节码
	u2 Nums[3];								//Nums[0]:localVarTables长度;Nums[1]:lineNumInfos长度;Num[3]:others长度
	LocalVarTable** localVarTables;
	LineNumTableInfo** lineNumInfos;
	Other** others;

}Code;

//属性，字段表----------------------------------------------------------------------

//字段表
typedef struct MemberInfo{
	u2 accessFlag;						//字段修饰符
	u2 nameIndex;						//字段名索引	
	u2 desIndex;						//字段描述索引
	u2 attrCount;						//属性个数

	//属性集合-----------------------------------------------
	u2 Nums[9];//属性个数。Nums[0]~Nums[8]依次代表Codes,Exceptions,Deprecate,Synthetic....Other属性的个数
	Code** codes;						//Codes属性集合（长度=Nums[0]）
	Exceptions** excps;
	Deprecate** depres;
	Synthetic** synths;
	SourceFile** sourFiles;
	ConstantValue** constantVals;
	LineNumTableInfo** lineNumInfos;
	LocalVarTable** localVarTables;
	Other** others;

}MemberInfo;


//运行时数据区--------------------------------------------------------------------------------------------------------------

//变量（基本类型，引用类型通用）
typedef struct Slot{
	char flag;			//标记	0代表没有存储数据，3代表int，4代表float，5代表long。。。
	b4 num;				//int,byte,short等小于等于32位的直接存，double，long这些64位的就拆分成两个Slot。Slot[id]放高位，Slot[id+1]放低位
	Object* ref;		//存放引用类型
}Slot;

typedef struct Array{
	//数组(如果对象是数组)
	int len;//数组长度
	char type;//数组元素类型//0:char 1:byte 2:short 3:int 4:float 5:long 6:double 10:引用类型
	b1* B1Array;
	b4* B4Array;
	b8* B8Array;
	Object** objArray;
}Array;

struct MethodAreaClass;

typedef struct Object{

	//-----------------------------对象头------------------------------------

	//MarkWord(标记字段)
	char age;								//对象年龄(GC幸存次数)
	bool alive;								//是否存活。GC标志
	int hashCode;							//哈希码
//	int lockStatus;							//锁状态
//	int threadId;							//偏向线程id

	//Klass Pointer(类型指针)
	MethodAreaClass* objClass;				//类指针,该对象属于哪一类

	//---------------------------实例数据------------------------------------
	Slot** fields;							//该对象的字段(属性)表
	Array* array;

	//---------------------------对齐填充------------------------------------

}Object;