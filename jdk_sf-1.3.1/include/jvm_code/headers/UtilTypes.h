/**
  ͨ�����ͣ�������Ϣ��
**/
#pragma once//��ֹ�ظ�����

#include <string>
#include <iostream>
#include <vector>
using namespace std;


//������Ϣ����--------------------------------------------------------------------------------------------------------------

typedef unsigned char u1;//1�ֽ��޷���
typedef unsigned int u2;//2�ֽ��޷���
typedef unsigned long u4;//4�ֽ��޷���
typedef unsigned _int64 u8;//8�ֽ��޷���

typedef char  b1;
typedef short b2;
typedef int b4;//4�ֽ��з���
typedef __int64 b8;

typedef unsigned char* classReader;//��������
typedef unsigned char* ConstantInfo;//��������
typedef ConstantInfo* ConstantPool;//������

typedef struct AttrInfo AttrInfo;
typedef struct Attributes Attributes;

typedef struct Slot Slot;
struct Object;


//.class�ļ��ֽ�����Ϣ------------------------------------------------------------------------------------------------------

//��Ŵ�.class�ļ����ȡ���ֽ�����,���ṩ���ɷ������ⲿ��ȡ���ݣ�8λ��16λ��32λ��64λ���Ա����
struct FileData{
	string name;//.class�ļ���
	unsigned char *data;//����������飨8λ��
	int start;//��ʼ��ȡ���ݵ��������������ݴ���
	int size;//��С
	//[0,start)Ϊ�Ѿ���ȡ��������
	//[start,size)Ϊδ��ȡ����
	FileData(unsigned char *d,int s){
		data=d;size=s;
		start=0;
	}

	//��ȡ1��8λ���ݣ���unsigned char��1�ֽ�
	unsigned char readU1(){
		if(size>start){
			return data[start++];
		}
		return 0;
	}

		//��ȡ1��16λ���ݣ���unsigned int��2�ֽ�
	unsigned int readU2(){
		unsigned int read=0;
		int usize;
		size-start>2 ? usize=2 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}

	//��ȡ1��32λ���ݣ���unsigned long��4�ֽ�
	unsigned long readU4(){
		unsigned int read=0;
		int usize;
		size-start>4 ? usize=4 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}
		

	//��ȡ1��64λ���ݣ���unsigned __int64��8�ֽ�
	unsigned _int64 readU8(){
		unsigned _int64 read=0;
		int usize;
		size-start>8 ? usize=8 : usize=size-start;
		for(int i=usize-1;i>=0;--i){
			read+=data[start++]<<(8*i);
		}
		return read;
	}
		
		//��ȡ����λ8λ����
	unsigned char* readBytes(unsigned int n){
		unsigned char* s=(unsigned char*)calloc(n+1,sizeof(unsigned char));
		for(int i=0;i<n;++i){
			s[i]=readU1();
		}
		s[n]='\0';//һ��Ҫ���������⣬�������������Ȼ��һֱ����ȥ���ҵ�'\0'Ϊֹ
		return s;
	}

	//��ȡ����16λ����
	unsigned int* readU2s(int n){
		unsigned int* s=(unsigned int*)calloc(n,sizeof(unsigned int));
		for(int i=0;i<n;++i){
			s[i]=readU2();
		}
		return s;
	}
		
};

//Attribute_info���ͼ�--------------------------------------------------------------
//��������Ϣ����

//�ֲ�������Ϣ
typedef struct LocalVarInfo{
	u2 start_pc;			//�ֲ������������ڿ�ʼ���ֽ���ƫ����
	u2 length;				//�ֲ��������ø��Ƿ�Χ
	u2 nameIndex;			//�ֲ���������
	u2 descIndex;			//�ֲ�����������
	u2 index;				//�ֲ�������ջ֡�ֲ��������λ��(�����long/double,ռ��λ)
}LocalVarInfo;

//�ֲ�������
typedef struct LocalVarTable{
	u2 attrNameIndex;
	u4 attrLen;
	u2 localVarTableLen;
	LocalVarInfo** localVars;
}LocalVarTable;

//��������
typedef struct Deprecate{
	u2 nameIndex;
	u4 attrLen;
}Deprecate;

//�ϳ�����
typedef struct Synthetic{
	u2 nameIndex;
	u4 attrLen;
}Synthetic;

//Դ�ļ�
typedef struct SourceFile{
	u2 nameIndex;
	u4 attrLen;
	u2 sourceFileIndex;
}SourceFile;

//�쳣������
typedef struct Exceptions{
	u2 nameIndex;
	u4 attrLen;
	u2 exceptionsNum;
	u2* exceptionIndexTable;
}Exceptions;

//ConstantValue���ԡ���ž�̬����
//������ֶα���
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
	u2 start_pc;//�ֽ����к�
	u2 lineNum;//javaԴ���к�
}LineNumInfo;

//�к���Ϣ
typedef struct LineNumTable{
	u2 attrNameIndex;
	u4 attrLen;
	u2 lineNumTableLen;
	LineNumInfo** lineNums;
}LineNumTableInfo;


//�����������ԣ�RuntimeVisableAnntations,Signature�ȣ�
typedef struct Other{
	u2 attrNameIndex;
	u4 len;
	u1* chars;
}Other;

//code���ԡ�������Ĵ��롣���java�������ɵ��ֽ���ָ��
typedef struct Code{
	u2 attrNameIndex;						//����������������
	u4 codeLen;								//code����
	u2 maxStack;							//����ջ������
	u2 maxLocals;							//�ֲ�����������Ҫ�Ŀռ�
	u4 insLen;								//�ֽ���ָ�����鳤��java������淶��һ���������ܳ���65535��ָ��
	u1* insArray;							//�ֽ���ָ������
	u2 exceptionInfoNum;					//�쳣��Ϣ���С
	ExceptionInfo** excs;					//�쳣��Ϣ(����Exceptions!);
	u2 attrCount;							//code���Ե����Ը���

	u1* surplus;							//ʣ���ֽ���
	u2 Nums[3];								//Nums[0]:localVarTables����;Nums[1]:lineNumInfos����;Num[3]:others����
	LocalVarTable** localVarTables;
	LineNumTableInfo** lineNumInfos;
	Other** others;

}Code;

//���ԣ��ֶα�----------------------------------------------------------------------

//�ֶα�
typedef struct MemberInfo{
	u2 accessFlag;						//�ֶ����η�
	u2 nameIndex;						//�ֶ�������	
	u2 desIndex;						//�ֶ���������
	u2 attrCount;						//���Ը���

	//���Լ���-----------------------------------------------
	u2 Nums[9];//���Ը�����Nums[0]~Nums[8]���δ���Codes,Exceptions,Deprecate,Synthetic....Other���Եĸ���
	Code** codes;						//Codes���Լ��ϣ�����=Nums[0]��
	Exceptions** excps;
	Deprecate** depres;
	Synthetic** synths;
	SourceFile** sourFiles;
	ConstantValue** constantVals;
	LineNumTableInfo** lineNumInfos;
	LocalVarTable** localVarTables;
	Other** others;

}MemberInfo;


//����ʱ������--------------------------------------------------------------------------------------------------------------

//�������������ͣ���������ͨ�ã�
typedef struct Slot{
	char flag;			//���	0����û�д洢���ݣ�3����int��4����float��5����long������
	b4 num;				//int,byte,short��С�ڵ���32λ��ֱ�Ӵ棬double��long��Щ64λ�ľͲ�ֳ�����Slot��Slot[id]�Ÿ�λ��Slot[id+1]�ŵ�λ
	Object* ref;		//�����������
}Slot;

typedef struct Array{
	//����(�������������)
	int len;//���鳤��
	char type;//����Ԫ������//0:char 1:byte 2:short 3:int 4:float 5:long 6:double 10:��������
	b1* B1Array;
	b4* B4Array;
	b8* B8Array;
	Object** objArray;
}Array;

struct MethodAreaClass;

typedef struct Object{

	//-----------------------------����ͷ------------------------------------

	//MarkWord(����ֶ�)
	char age;								//��������(GC�Ҵ����)
	bool alive;								//�Ƿ��GC��־
	int hashCode;							//��ϣ��
//	int lockStatus;							//��״̬
//	int threadId;							//ƫ���߳�id

	//Klass Pointer(����ָ��)
	MethodAreaClass* objClass;				//��ָ��,�ö���������һ��

	//---------------------------ʵ������------------------------------------
	Slot** fields;							//�ö�����ֶ�(����)��
	Array* array;

	//---------------------------�������------------------------------------

}Object;