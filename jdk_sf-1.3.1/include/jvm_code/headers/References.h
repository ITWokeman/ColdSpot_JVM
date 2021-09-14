/**
	引用指令集
**/
#pragma once//防止重复定义
#include"Thread.h"
#include"ClassFileDecoder.h"
#include"InstDecoder.h"
#include "..\headers\UTF8CNDecoder.h"
#include "HackMethod.h"
#include<iostream>
using namespace std;

struct JInst;

void invokeMethod(Frame* frame,Method* method);

//注册References系列指令
void regReferncesInsts(JInst* insts);