/**
Hack������
��hack����һЩ����ķ�������Scanner��Ĺ��췽����System���print���������̵߳�����صķ�����
��Щ�������������δ����/���������޸���bug�������Ѳ���Hack�������
**/
#pragma once//��ֹ�ظ�����
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
