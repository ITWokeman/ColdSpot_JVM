/**
本地方法管理模块

本地方法是由其它语言编写的，编译成和处理器相关的机器代码
该方法的实现由非java语言实现，比如C。这个特征并非java所特有，很多其它的编程语言都有这一机制，
比如在C＋＋中，你可以用extern "C"告知C＋＋编译器去调用一个C的函数。

**/
#pragma once//防止重复定义
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

//本地方法注册表
class Registry{
private:
	map<string,nativeMethod*> nativeMethodMap;

public:

	//注册所有本地方法
	Registry(){
		//注册java.lang包所有类的本地方法
		regJavaLangAllNativeMethod(this);
		//注册java.io包所有类的本地方法
		//regJavaIOAllNativeMethod(this);
		//注册sun.misc包所有类的本地方法
		regSunMiscAllNativeMethod(this);
		//注册sun.reflect包所有类的本地方法
		regSunReflectAllNativeMethod(this);
		//注册java.util包所有类的本地方法
		regJavaUtilAllNativeMethod(this);
		//注册java.security包所有类的本地方法
		regJavaSecurityAllNativeMethod(this);
		//注册salted.util包所有类的本地方法
		regSaltedfishIOAllNativeMethod(this);
	}

	void regNativeMethod(string nativeMethodName,nativeMethod* nm){
		nativeMethodMap[nativeMethodName]=nm;
	}

	//执行本地方法
	void executeNativeMethod(string className,string methodName,string descriptor,Frame* frame){
		//如果是void registerNatives()方法，那么不用执行了。因为已经注册了相应的本地方法了
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