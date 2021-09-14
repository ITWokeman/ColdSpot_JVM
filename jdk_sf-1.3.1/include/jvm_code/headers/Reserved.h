/**
	本地方法指令集
**/
#pragma once//防止重复定义
#include"Thread.h"
#include"registry.h"
#include"InstDecoder.h"
#include<iostream>
using namespace std;

struct JInst;

void regReservedInsts(JInst* insts);