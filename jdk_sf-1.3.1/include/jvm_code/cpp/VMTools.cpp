/**
 JVM �������߰�
 
 �����ߣ�����ٷ�˹

**/
#pragma once
#include "..\headers\UtilTypes.h"
#include "..\headers\UtilTypes.h"
#include "..\headers\Heap.h"
#include "..\headers\InstDecoder.h"
#include "..\headers\UTF8CNDecoder.h"


//����̨����------------------------------------------------------------------------------------

void color(int b){
    HANDLE hConsole = GetStdHandle((STD_OUTPUT_HANDLE)) ;
    SetConsoleTextAttribute(hConsole, b) ;//��ɫ����
}//��ɫ����ģ�� 

//�ַ���������--------------------------------------------------------------------------------

//��׺�ж�
bool suffix(string path,string end){
	int pl=path.length()-1,el=end.length()-1;
	for(int i=el;i>=0;--i){
		if(path[pl-i]!=end[el-i]){return false;}
	}
	return true;
}


//�ж��ַ������
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

//��ȡ��İ���
char* getPackageName(char* name){

	int len=0;
	while(name[len++]);

	while(name[len--]!='/' && len>0);
	if(len==0)return "\0";//û�а��������ؿ��ַ���

	char* p=(char*)calloc(++len+1,sizeof(char));
	for(int i=0;i<len;p[i]=name[i++]);
	p[len]='\0';

	return p;
}

//��ȡ����
char* getClassName(char* name){
	int start=0,end;

	while(name[start++]);
	end=start;

	while(name[start--]!='/' && start>0);
	if(start==0)return name;//û�а�������������

	char* p=(char*)calloc(end,sizeof(char));
	for(int i=start+2;i<end-1;p[i-start-2]=name[i++]);
	p[end]='\0';
	return p;
}

//����ת������----------------------------------------------------------------------------------

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


//��8��b1����ת��������b4
//b4s[0]��low b4s[1]��high
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

//����
void printError(int errorNum,string info[]){
	color(4);
	printf("\n-----------------------------------------------------------------------------------------");

	switch(errorNum){

	case 0x000:
		printf("\n[����0x000]�Ҳ���$JAVA_HOME·��%s\n����C:\\my_jvm_config\\CONFIG.INI��Ϊ$JAVA_HOME����������ȷ·��!\n",info[0].c_str());
		break;
	//δ�ҵ�.class�ļ�
	case 0x001:
		printf("\n[����0x001]�Ҳ���%s�ļ�!\n��ȷ�����������Ƿ���ȷ\n",info[0].c_str());
		printf("\n�� %s\\config\\CONFIG.INI����ȷ����·��(ʹ��txt�༭)\n",info[1].c_str());
		break;

	//�����ؽ����쳣
	case 0x003:printf("\n[����0x003]%s.class�����ؽ����쳣!\n",info[0].c_str());break;
	
	//������޷�ʶ��ָ��
	case 0x004:printf("\n[����0x004]�޷�ʶ�� \" %s \" ָ��!\n",info[0].c_str());break;

	//����Ȩ�޴���
	case 0x005:printf("\n[����0x005]����Ȩ�޴����޷����� \" %s \" ��\n",info[0].c_str());break;

	case 0x006:printf("\n[����0x006]�Ҳ��������� public static void main(String[] arg)! \n");break;

	case 0x008:printf("\n[����0x008]�Ҳ������� %s!\n",info[0].c_str());break;

	case 0x009:printf("\n[����0x009]Ȩ�޲���,�޷����ʷ��� %s!\n",info[0].c_str());break;

	case 0x010:printf("\n[����0x010]%s�ǽӿڻ������,�޷�ʵ����!\n",info[0].c_str());break;

	case 0x011:printf("\n[����0x011]�Ҳ���%s��(�ӿ�)������ָ��!\n",info[0].c_str());break;

	case 0x012:printf("\n[����0x012]�޷�ͨ��%s������ָ�뵽%s����Ϣ!\n",info[0].c_str());break;

	case 0x013:printf("\n[����0x013]��ָ���쳣!\n");break;

	case 0x014:printf("\n[����0x014]�����ջ���!\n");break;

	case 0x015:printf("\n[����0x015]���鿽���쳣!\n");break;

	case 0x016:printf("\n[����0x016]�Ҳ������ط���%s!\n",info[0].c_str());break;

	case 0x017:printf("\n[����0x017]����Խ��! ���鳤��:%s   Խ������:%s\n",info[0].c_str(),info[1].c_str());break;

	case 0x018:printf("\n[����0x018]���ڴ����!\n");break;

	case 0x019:printf("\n[����0x019]int�������������! ����ԭ��:%s\n",info[0].c_str());break;

	default:break;
	}
	printf("-----------------------------------------------------------------------------------------\n");
	color(7);
	exit(1);
}



//���Թ���--------------------------------------------------------------------------------------

//��ȡstring���ݣ��������
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

#define lineByteCodeNum 32//ÿ���ֽ�����

//��ʽ�����.class�ļ�������
//���ڽ��뿪��
void dataOutput(FileData *d,int startIndex){
	printf("\n\n %s�ļ�����:\n\n",d->name.c_str());
//	color(1);printf(" ħ��   ");
//	color(2);printf("�ΰ汾   ");
//	color(3);printf("���汾   ");
//	color(4);printf("������   ");
//	color(5);printf("������   ");
//	color(6);printf("�ӿڳ�   ");
//	color(8);printf("�ֶα�   ");
//	color(10);printf("������\n");
//	color(7);
	printf(" +-----+-------------------------------------------------------------------------------------------------+\n");
	printf(" |��\\��| 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 |\n");
	printf(" +-----+-------------------------------------------------------------------------------------------------+");
	int i;
	for(int j=startIndex;j<d->size;++j){
		i=j-startIndex;
		
		//�����ͷ
		if(i%lineByteCodeNum==0){
			int k=i/lineByteCodeNum;
			if(k<10) printf("\n |0000%d| ",k);
			else if(k<100) printf("\n |000%d| ",k);
			else if(k<1000) printf("\n |00%d| ",k);
			else if(k<10000) printf("\n |0%d| ",k);
			else if(k<100000) printf("\n |%d| ",k);
		}
		
		//ȷ�����������ɫ����ʾ����
//		if(j<4){
//			color(1);//ħ��
//		}else if(j<6){
//			color(2);//�ΰ汾
//		}else if(j<8){
//			color(3);//���汾
//		}else if(j<79){
//			color(4);
//		}

		//����ֽ���
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
//    cout<<"�ڴ�ʹ��:"<<pmc.WorkingSetSize/1000 <<"K/"<<pmc.PeakWorkingSetSize/1000<<"K + "<<pmc.PagefileUsage/1000 <<"K/"<<pmc.PeakPagefileUsage/1000 <<"K"<<endl;

	return (int)pmc.WorkingSetSize;//PeakWorkingSetSize���ڴ�ʹ�÷�ֵ
}

b1* getClassKind(u2 accessFlag){
	if(IsInterface(accessFlag) == true){
		return (b1*)"�ӿ�";
	}else if(IsAbstract(accessFlag) == false){
		return (b1*)"��";
	}
	return (b1*)"������";
	
}

//����ʱ������������ʾ
void showConstPool(ConstPool* cp){

	printf("\n[%s(%s)]����ʱ������(��С%d) ��Ϣ����:\n",cp->Class->name,getClassKind(cp->Class->accessFlag),cp->cpSize);
	Constant* consts;
	for(int i=1;i<cp->cpSize;++i){
		consts=cp->consts[i];
		if(consts == NULL){
			printf("--���� %d : δ֪����,��!\n",i);
			continue;
		}
		switch(consts->type){
		case 1://utf8
			printf("--���� %d (Utf8): ���� %s\n",i,decodeUTF8((b1*)consts->val));
			break;
		case 3://Integer
			printf("--���� %d (Integer): %d\n",i,toB4(consts->val));break;
		case 4://Float
			printf("--���� %d (Float): %f\n",i,toFloat(consts->val));break;
		case 5://Long
			printf("--���� %d (Long): %ld\n",i,toLong(consts->val));break;
		case 6://Double
			printf("--���� %d (Double): %ld\n",i,toDouble(consts->val));break;
		case 7://Class
			printf("--���� %d (Class): ���� %s\n",i,consts->ref->_classRef->name);break;
		case 8://String
			printf("--���� %d (String): ���ó���%d����\n",i,(consts->val[0]<<8) + consts->val[1]);break;
		case 9://Field
			printf("--���� %d (Field): ������ %s	�ֶ��� %s	���� %s\n",i,consts->ref->_fieldRef->cRef->name,consts->ref->_fieldRef->name,consts->ref->_fieldRef->descriptor);
			break;
		case 10://Method
			printf("--���� %d (Method): ������ %s	������ %s	���� %s\n",i,consts->ref->_methodRef->cRef->name,consts->ref->_methodRef->name,consts->ref->_methodRef->descriptor);
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

			printf("--���� %d (InterfaceMethod): ���ڽӿ�%s ������ %s ���� %s\n",i,cname,cp->consts[nameIndex]->val,cp->consts[typeIndex]->val);
			break;
		}
		case 12://NameAndType
			u1* val=consts->val;
			printf("--���� %d (NameAndType): �������� %d �������� %d\n",i,(val[0]<<8)+val[1],(val[2]<<8)+val[3]);
			break;
		}
	}
}


long getNS(){//�����ȡ(�����Ż�jvm)
	LARGE_INTEGER nFreq,t1;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	return ((t1.QuadPart%1000000000)*1000000000)/(long)nFreq.QuadPart;
}

int getUS(){//΢���ȡ(�����Ż�jvm)
	LARGE_INTEGER nFreq,t1;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	return ((t1.QuadPart%1000000000)*1000000)/(int)nFreq.QuadPart;
}


clock_t getMS(){//�����ȡ(�����Ż�jvm)
	return clock();
}

void showSlot(Slot* slot){
	if(slot->flag==3 || slot->flag==5){
		printf("%d ",slot->num);
	}else if(slot->flag==4 || slot->flag==6){
		printf("%f ",u4ToFloat(slot->num));
	}else if(slot->flag==2){
		printf("%d ",slot->ref);
	}else if(slot->flag==10){//��������
		if(slot->ref!=NULL){
			if(slot->ref->objClass->name[0]=='['){//˵��������
				printf("%s����(����)[@%d] ",slot->ref->objClass->name,slot->ref);
				if(isEqual((u1*)"[char",slot->ref->objClass->name)==true){//���char��������
					color(6);
					if(slot->ref->array!=NULL ){
						printf("{����:%s} ",decodeUTF8(slot->ref->array->B1Array));
					}else{
						printf("{����Ϊ��!} ");
					}
					color(7);
				}
			}else{
				printf("��%s����[@%d]",slot->ref->objClass->name,slot->ref);
				if(isEqual((u1*)"java/lang/String",slot->ref->objClass->name)==true){//���java/lang/String����
					color(6);
					b1* strVal=slot->ref->objClass->loader->heap->getStrPool()->getJStrValue(slot->ref);
					if(strVal!=NULL){
						printf("{����:%s(@%d)} ",decodeUTF8(strVal),strVal);
					}else{
						printf("{����Ϊ��!} ",strVal);
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

//���Թ��ߣ���ʾָ��ִ�й���
//��ʾ�ֲ�������������ջ
void showLoop(Frame* frame,int pc,int opNum,int num){

	Slot** lv=frame->localVars;//���ر�����
	Slot** os=frame->opStack;//������ջ
	Method* method=frame->method;

	printf(" LocalVaris:{ ");
	for(int i=0;i<frame->localVarsSize;++i){
		showSlot(lv[i]);
	}
	printf("}\n OperaStack:{ ");

	for(int j=0;j<frame->operandTop;++j){
		showSlot(os[j]);
	}
	printf(" } �K\n");
	color(4);
	Thread* thread=frame->thisThread;
	
	printf(" #%d PC %d:%x(ָ��:%s) %d�̵߳�%d��ջ֡(%s .%s%s����(ջ��:%d)) �K\n",num,pc,opNum,
		getInstName(opNum),thread->id,thread->stack->top,method->Class->name,method->name,method->description,
		method->codes->maxStack);
	color(7);
}


