#pragma once//��ֹ�ظ�����
#include "UtilTypes.h"
#include "VMTools.h"
#include "ClassFileDecoder.h"
#include "methodArea.h"
#include <iostream>
using namespace std;

class methodArea;
struct FileData;
//���룺.class�ֽ���
//������methodAreClass
MethodAreaClass* byteCodeDecoder(FileData *d,methodArea* loader);
