/**

  类加载执行器

  加载过程：载入→验证→准备→解析→初始化→使用→卸载

  本模块负责接收方法区加载类信息的请求，完成类加载过程的前5项
  委托.class类文件加载器和.class文件解析器和进行载入，然后进行验证、准备、解析和初始化
  最后返回MethodAreaClass(方法区类)给方法区
  其中验证阶段被省去
  类的初始化要等到触发相关指令才执行

  加载A类，A类的父类，父类的父类也会被递归解析。递归解析的工作在载入阶段后开始，验证阶段前完成。
**/

#pragma once//防止重复定义
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


//传入：类加载器，加载类名，是否为测试模式
//执行：加载该类，该类父类，该类实现接口
//传出：被加载的类
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest);

//加载数组类
MethodAreaClass* loadArrayClass(methodArea* loader,string cn,bool isTest);

//加载基本类型
void loadPrimitiveClasses(methodArea* loader);

//初始化类
void initClass(Frame* frame,Thread* thread,MethodAreaClass* fClass);
