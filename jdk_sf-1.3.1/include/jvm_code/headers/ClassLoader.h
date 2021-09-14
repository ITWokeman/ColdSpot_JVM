/**
 JVM 类文件读取器模块（寻找并读取class文件内容）

 传入搜索范围，搜索类名。返回该类字节码文件（如果找得到的话）
 注意，zip和jar暂时打不开，无法搜索

 该模块含三种类加载器,即
 (1)启动类加载器(BootstrapClassLoader)
 (2)扩展类加载器(ExtensionClassLoader)
 (3)应用类加载器(ApplicationClassLoader)
 按双亲委派机制流程加载类文件


**/
#pragma once//防止重复定义
#pragma comment (lib,"lib\\zlib\\zlib.lib")//引入第三方库读取jar包
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

//-----------------对外提供使用方法----------------
FileData* classFileLoader(string findName,bool testing);//类名 
void showPaths(ConfigInfo* config);//显示搜索路径
void showJavaHome(ConfigInfo* config);//显示JAVA_HOME;
void initClassLoader(ConfigInfo* config);//初始化类加载器

