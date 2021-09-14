#pragma once//防止重复定义
#include "UtilTypes.h"

//修饰符识别器 0否 1是

int IsPublic(u2 accessFlag);

int IsPrivate(u2 accessFlag);

int IsProtected(u2 accessFlag);

int IsStatic(u2 accessFlag);

int IsFinal(u2 accessFlag);

int IsSuper(u2 accessFlag);

int IsSynchronized(u2 accessFlag);

int IsVolatile(u2 accessFlag);

int IsBridge(u2 accessFlag);

int IsTransient(u2 accessFlag);

int IsVarages(u2 accessFlag);

int IsNative(u2 accessFlag);

int IsInterface(u2 accessFlag);

int IsAbstract(u2 accessFlag);

int IsStrict(u2 accessFlag);

int IsSynthetic(u2 accessFlag);

int IsAnnotation(u2 accessFlag);

int IsEnum(u2 accessFlag);
