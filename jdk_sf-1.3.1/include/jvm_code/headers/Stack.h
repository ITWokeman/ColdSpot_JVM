
/**
	操作数栈指令集
**/
#pragma once//防止重复定义
#include"Thread.h"
#include"thread.h"
#include"ClassFileDecoder.h"
#include"InstDecoder.h"
#include<iostream>
using namespace std;

struct JInst;

//注册Stack系列指令
void regStackInsts(JInst* insts);
