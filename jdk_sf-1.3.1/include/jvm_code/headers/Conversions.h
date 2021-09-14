/**
	强制转化类型
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

//注册Conversions系列指令
void regConversInsts(JInst* insts);