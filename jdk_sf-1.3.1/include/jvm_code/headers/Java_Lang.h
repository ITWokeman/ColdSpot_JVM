#pragma once//防止重复定义
#include "registry.h"
#include "Thread.h"
#include "Heap.h"
#include "references.h"

//注册java.lang包所有类的本地方法
void regJavaLangAllNativeMethod(Registry* registry);