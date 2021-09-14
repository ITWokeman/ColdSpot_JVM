/***
 JVM译码器模块
 
***/
#pragma once//防止重复定义
#include "Thread.h"
#include "methodArea.h"
#include "InstDecoder.h"
#include "VMTools.h"
#include "UtilTypes.h"
#include "constants.h"
#include "loads.h"
#include "stores.h"
#include "stack.h"
#include "math.h"
#include "conversions.h"
#include "comparisons.h"
#include "control.h"
#include "references.h"
#include "extended.h"
#include "reserved.h"
#include <iostream>
#include <windows.h>
using namespace std;

//JVM指令
struct JInst{
	//指令名称
	u1* name;			
	//指令对应的函数的地址。通过这个直接调用指令对应的函数！
	void (*func)(Frame* frame,u1 opNum);		//栈帧，指令
};

char* getInstName(u1 index);//输入：操作数	输出：助记符

JInst* buildInstTable();//创建指令表

