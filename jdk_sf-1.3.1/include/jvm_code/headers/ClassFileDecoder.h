#pragma once//防止重复定义
#include "UtilTypes.h"
#include "VMTools.h"
#include "ClassFileDecoder.h"
#include "methodArea.h"
#include <iostream>
using namespace std;

class methodArea;
struct FileData;
//传入：.class字节码
//传出：methodAreClass
MethodAreaClass* byteCodeDecoder(FileData *d,methodArea* loader);
