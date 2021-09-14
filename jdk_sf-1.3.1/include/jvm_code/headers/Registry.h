/**
���ط�������ģ��

���ط��������������Ա�д�ģ�����ɺʹ�������صĻ�������
�÷�����ʵ���ɷ�java����ʵ�֣�����C�������������java�����У��ܶ������ı�����Զ�����һ���ƣ�
������C�����У��������extern "C"��֪C����������ȥ����һ��C�ĺ�����

**/
#pragma once//��ֹ�ظ�����
#include <map>
#include <string>
#include "java_lang.h"
#include "VMTools.h"

struct Frame;

typedef struct nativeMethod{
	void (*func)(Frame* frame);
}nativeMethod;

void regJavaLangAllNativeMethod(Registry *registry);

void regSunMiscAllNativeMethod(Registry* registry);

void regSunReflectAllNativeMethod(Registry* registry);

void regJavaUtilAllNativeMethod(Registry* registry);

void regJavaSecurityAllNativeMethod(Registry* registry);

void regSaltedfishIOAllNativeMethod(Registry* registry);

//���ط���ע���
class Registry{
private:
	map<string,nativeMethod*> nativeMethodMap;

public:

	//ע�����б��ط���
	Registry(){
		//ע��java.lang��������ı��ط���
		regJavaLangAllNativeMethod(this);
		//ע��java.io��������ı��ط���
		//regJavaIOAllNativeMethod(this);
		//ע��sun.misc��������ı��ط���
		regSunMiscAllNativeMethod(this);
		//ע��sun.reflect��������ı��ط���
		regSunReflectAllNativeMethod(this);
		//ע��java.util��������ı��ط���
		regJavaUtilAllNativeMethod(this);
		//ע��java.security��������ı��ط���
		regJavaSecurityAllNativeMethod(this);
		//ע��salted.util��������ı��ط���
		regSaltedfishIOAllNativeMethod(this);
	}

	void regNativeMethod(string nativeMethodName,nativeMethod* nm){
		nativeMethodMap[nativeMethodName]=nm;
	}

	//ִ�б��ط���
	void executeNativeMethod(string className,string methodName,string descriptor,Frame* frame){
		//�����void registerNatives()��������ô����ִ���ˡ���Ϊ�Ѿ�ע������Ӧ�ı��ط�����
		regJavaLangAllNativeMethod(this);
		if(isEqual((u1*)methodName.c_str(),(u1*)"registerNatives")==true && isEqual((u1*)descriptor.c_str(),(u1*)"()V")==true){
			return;
		}
		string key=className + "~" + methodName + "~" +descriptor;
		if(nativeMethodMap[key]==NULL){
			string info[]={key};
			printError(0x016,info);
		}
		(*nativeMethodMap[key]->func)(frame);
	}

};