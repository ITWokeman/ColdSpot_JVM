/**
 JVM ���ļ���ȡ��ģ�飨Ѱ�Ҳ���ȡclass�ļ����ݣ�

 ����������Χ���������������ظ����ֽ����ļ�������ҵõ��Ļ���
 ע�⣬zip��jar��ʱ�򲻿����޷�����

 ��ģ�麬�����������,��
 (1)�����������(BootstrapClassLoader)
 (2)��չ�������(ExtensionClassLoader)
 (3)Ӧ���������(ApplicationClassLoader)
 ��˫��ί�ɻ������̼������ļ�


**/
#pragma once//��ֹ�ظ�����
#pragma comment (lib,"lib\\zlib\\zlib.lib")//������������ȡjar��
#include "..\lib\zlib\unzip.h"
#include "UtilTypes.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <io.h>
#include "UtilTypes.h"
#include "VMTools.h"
#include "ConfigDecoder.h"
using namespace std;

//-----------------�����ṩʹ�÷���----------------
FileData* classFileLoader(string findName,bool testing);//���� 
void showPaths(ConfigInfo* config);//��ʾ����·��
void showJavaHome(ConfigInfo* config);//��ʾJAVA_HOME;
void initClassLoader(ConfigInfo* config);//��ʼ���������

