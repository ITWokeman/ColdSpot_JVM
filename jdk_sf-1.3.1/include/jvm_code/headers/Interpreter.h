/**
  jvm ������

**/
#pragma once//��ֹ�ظ�����
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

//������(����������)
void interpreter(Thread* mainThread,MethodAreaClass* mainClass,bool logInst,int startMemory,int startTime,char** argv,int argc);

//��ȡ������
Method* getMainMethod(MethodAreaClass* c);

