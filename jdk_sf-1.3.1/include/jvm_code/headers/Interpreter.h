/**
  jvm 解释器

**/
#pragma once//防止重复定义
#include "Thread.h"
#include "UtilTypes.h"
#include "ClassFileDecoder.h"
#include "AccessFlagDecoder.h"
#include "Recycler.h"
#include "JVMThreadScheduler.h"
#include "InstDecoder.h"
#include <iostream>
#include <windows.h>

using namespace std;

//解释器(运行主方法)
void interpreter(Thread* mainThread,MethodAreaClass* mainClass,bool logInst,int startMemory,int startTime,char** argv,int argc);

//获取主方法
Method* getMainMethod(MethodAreaClass* c);

