/**
 ��������ģ��
**/
#pragma once//��ֹ�ظ�����
#pragma comment (lib,"lib\\clib\\Psapi.lib")
#include"UtilTypes.h"
#include"ClassLoader.h"
#include"windows.h"
#include"methodArea.h"
#include "..\lib\clib\Psapi.h"
#include<iostream>
#include <time.h>
using namespace std;

struct ConstPool;
struct Frame;

//-------------��������----------------
void color(int b);//��ɫ
bool suffix(string path,string end);//�ж�path�Ƿ��к�׺end
char* getPackageName(char* name);//ͨ����ȫ�ƻ�ȡ�������� "java/lang/Object" ��ȡ���� "java/lang"
char* getClassName(char* name);//ͨ����ȫ�ƻ�ȡ�������� "java/lang/Object" ��ȡ���� "Object";
int isEqual(u1* str1,u1* str2);
float u4ToFloat(u4 val);
float toFloat(u1* chars);
double u8ToDouble(u8 chars);
double toDouble(u1* chars);
_int64 toLong(u1* chars);
_int32 toInt(u1* chars);
b4 toB4(u1* chars);
b4* eightB1ToTwoB4(u1* chars);
b8 toB8(u1* chars);
float B4ToFloat(b4 val);
double B8ToDouble(b8 val);
b4* B8ToTwoB4(b8 val);
string charArrayToString(char* chars);
clock_t getMS();
void printError(int errorNum,string info[]);
int stringToInt(string val);
bool isNumber(string val);
//-------------------------------------

//-------------���Է���----------------
void printStr(string str);//�ַ������
void dataOutput(FileData *d,int startIndex);//�ֽ�����Ϣ��ʽ����ӡ
void showConstPool(ConstPool* cp);//��ʾ����ʱ������
long getNS();//�����ȡ(�����Ż�jvm,���������������߳�)
int getUS();//΢���ȡ
int getMemory();//��ȡʹ�õ��ڴ�(�����Ż�jvm)
void showSlot(Slot* slot);
void showLoop(Frame* frame,int pc,int opNum,int num);//���Թ��ߣ���ʾָ��ִ�й���
b1* getClassKind(u2 accessFlag);//�������
//-------------------------------------