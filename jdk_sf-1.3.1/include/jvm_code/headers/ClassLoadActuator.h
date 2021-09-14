/**

  �����ִ����

  ���ع��̣��������֤��׼������������ʼ����ʹ�á�ж��

  ��ģ�鸺����շ�������������Ϣ�������������ع��̵�ǰ5��
  ί��.class���ļ���������.class�ļ��������ͽ������룬Ȼ�������֤��׼���������ͳ�ʼ��
  ��󷵻�MethodAreaClass(��������)��������
  ������֤�׶α�ʡȥ
  ��ĳ�ʼ��Ҫ�ȵ��������ָ���ִ��

  ����A�࣬A��ĸ��࣬����ĸ���Ҳ�ᱻ�ݹ�������ݹ�����Ĺ���������׶κ�ʼ����֤�׶�ǰ��ɡ�
**/

#pragma once//��ֹ�ظ�����
#include "methodArea.h"
#include "Frame.h"
#include "UtilTypes.h"
#include "AccessFlagDecoder.h"
#include "ConfigDecoder.h"
#include <iostream>
using namespace std;

struct Frame;
struct Thread;
class ConfigInfo;


//���룺��������������������Ƿ�Ϊ����ģʽ
//ִ�У����ظ��࣬���ุ�࣬����ʵ�ֽӿ�
//�����������ص���
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest);

//����������
MethodAreaClass* loadArrayClass(methodArea* loader,string cn,bool isTest);

//���ػ�������
void loadPrimitiveClasses(methodArea* loader);

//��ʼ����
void initClass(Frame* frame,Thread* thread,MethodAreaClass* fClass);
