/**
 开发工具模块
**/
#pragma once//防止重复定义
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

//-------------辅助方法----------------
void color(int b);//颜色
bool suffix(string path,string end);//判断path是否有后缀end
char* getPackageName(char* name);//通过类全称获取包名。如 "java/lang/Object" 获取包名 "java/lang"
char* getClassName(char* name);//通过类全称获取类名。如 "java/lang/Object" 获取类名 "Object";
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

//-------------调试方法----------------
void printStr(string str);//字符串输出
void dataOutput(FileData *d,int startIndex);//字节码信息格式化打印
void showConstPool(ConstPool* cp);//显示运行时常量池
long getNS();//纳秒获取(用于优化jvm,生成随机数与调度线程)
int getUS();//微秒获取
int getMemory();//获取使用的内存(用于优化jvm)
void showSlot(Slot* slot);
void showLoop(Frame* frame,int pc,int opNum,int num);//测试工具：显示指令执行过程
b1* getClassKind(u2 accessFlag);//类的类型
//-------------------------------------