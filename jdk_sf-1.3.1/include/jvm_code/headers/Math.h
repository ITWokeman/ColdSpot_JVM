/**
	�������߼�����ָ�
**/

#pragma once//��ֹ�ظ�����
#include"Thread.h"
#include"thread.h"
#include"ClassFileDecoder.h"
#include"InstDecoder.h"
#include<iostream>
using namespace std;

struct JInst;

//ע��Mathϵ��ָ��
void regMathInsts(JInst* insts);