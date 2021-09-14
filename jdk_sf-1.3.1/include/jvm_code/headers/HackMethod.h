/**
Hack方法库
用hack处理一些特殊的方法比如Scanner类的构造方法、System类的print方法，多线程调度相关的方法等
这些方法由于虚拟机未完善/出了难以修复的bug，不得已采用Hack调用输出
**/
#pragma once//防止重复定义
#include "Thread.h"
#include "ClassFileDecoder.h"
#include "InstDecoder.h"
#include "UTF8CNDecoder.h"
#include "methodArea.h"
#include <map>
#include <string>
#include <iostream>
using namespace std;

bool hackMethod(Frame* frame,methodRef* mRef);//
