
#pragma once//防止重复定义
#include "Heap.h"
#include "registry.h"
#include "Thread.h"

//注册sun.misc包所有类的本地方法
void regSunReflectAllNativeMethod(Registry* registry);