/**
	�Ƚ�ָ�
**/
#pragma once//��ֹ�ظ�����
#include "Thread.h"
#include "methodArea.h"
#include "InstDecoder.h"
#include "VMTools.h"
#include "control.h"
#include <iostream>
#include <windows.h>
using namespace std;

struct JInst;

//ע��Comparisonsϵ��ָ��
void regComparInsts(JInst* insts);
