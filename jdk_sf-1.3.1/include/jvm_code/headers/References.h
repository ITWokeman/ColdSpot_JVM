/**
	����ָ�
**/
#pragma once//��ֹ�ظ�����
#include"Thread.h"
#include"ClassFileDecoder.h"
#include"InstDecoder.h"
#include "..\headers\UTF8CNDecoder.h"
#include "HackMethod.h"
#include<iostream>
using namespace std;

struct JInst;

void invokeMethod(Frame* frame,Method* method);

//ע��Referencesϵ��ָ��
void regReferncesInsts(JInst* insts);