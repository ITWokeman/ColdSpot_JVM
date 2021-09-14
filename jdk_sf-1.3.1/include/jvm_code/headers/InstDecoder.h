/***
 JVM������ģ��
 
***/
#pragma once//��ֹ�ظ�����
#include "Thread.h"
#include "methodArea.h"
#include "InstDecoder.h"
#include "VMTools.h"
#include "UtilTypes.h"
#include "constants.h"
#include "loads.h"
#include "stores.h"
#include "stack.h"
#include "math.h"
#include "conversions.h"
#include "comparisons.h"
#include "control.h"
#include "references.h"
#include "extended.h"
#include "reserved.h"
#include <iostream>
#include <windows.h>
using namespace std;

//JVMָ��
struct JInst{
	//ָ������
	u1* name;			
	//ָ���Ӧ�ĺ����ĵ�ַ��ͨ�����ֱ�ӵ���ָ���Ӧ�ĺ�����
	void (*func)(Frame* frame,u1 opNum);		//ջ֡��ָ��
};

char* getInstName(u1 index);//���룺������	��������Ƿ�

JInst* buildInstTable();//����ָ���

