/**
	控制指令集
**/
#pragma once//防止重复定义
#include "Thread.h"
#include "methodArea.h"
#include "InstDecoder.h"
#include "VMTools.h"
#include <iostream>
#include <windows.h>
using namespace std;

struct JInst;

void Branch(Frame* frame,int offset);

//注册Control系列指令
void regControlInsts(JInst* insts);
