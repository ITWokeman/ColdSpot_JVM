/**
  methodArea  方法区
  将类信息结构（classInfo）进一步解析后，存放到方法区（methodArea）
  同时负责一切有关MethodAreaClass的事宜
**/
#pragma once//防止重复定义
#include "ClassFileDecoder.h"
#include "ClassLoadActuator.h"
#include "AccessFlagDecoder.h"
#include "VMTools.h"
#include "UtilTypes.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Heap;
struct Field;
struct Frame;
struct Method;
struct methodArea;
struct fieldLoader;
struct methodLoader;
struct ConstPool;
struct fieldRef;
struct methodRef;
struct methodRef;
class Registry;
struct Thread;

int isEqual(u1* str1,u1* str2);
string charArrayToString(char* chas);//字符数组转字符串
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest);
Object* newJClassObject(MethodAreaClass* jClass,MethodAreaClass* myClass);
void loadPrimitiveClasses(methodArea* loader,MethodAreaClass* jlClass);
int getMemory();
b1* getClassKind(u2 accessFlag);

//类引用
typedef struct classRef{
	ConstPool* cp;//类所在常量
	u1* name;//类名
	MethodAreaClass* Class;//类
}classRef;

//方法引用
typedef struct memberRef{
	ConstPool* cp;
	int classIndex;
	int nameAndTypeIndex;
}memberRef;

//接口引用
typedef struct interfaceMethodRef{
	int classIndex;
	int nameAndTypeIndex;
}interfaceMethodRef;

typedef struct Ref{
	classRef* _classRef;//类引用
	methodRef* _methodRef;//方法引用
	fieldRef* _fieldRef;//字段引用
	interfaceMethodRef* _interfaceMethodRef;//接口引用
}Ref;

//MethodAreaClass常量
typedef struct Constant{
	char type;//数据类型
	u1* val;//基本数据类型（int，float，double，string）
	Ref* ref;
};

//运行时常量池
typedef struct ConstPool{
	MethodAreaClass* Class;
	Constant** consts;
	int cpSize;
}ConstPool;


//要放到方法区内的类 Method Area Class
typedef struct MethodAreaClass{

	u2 accessFlag;				//访问标志
	u1* name;
	u1* superClassName;
	int interfacesLen;//接口列表数量
	u1** interfacesNames;//接口名称表
	ConstPool* cp;
	int fieldsLen;//字段表长度
	fieldLoader* floader;//字段加载器(能快速获取字段信息的字段表)
	int methodsLen;
	methodLoader* mloader;//方法加载器
	methodArea* loader;//方法区（类加载器）
	MethodAreaClass* superClass;
	MethodAreaClass** interfaces;//接口类信息
	u2	instanceSlotCount;
	u2 staticSlotCount;//静态字段数量
	Slot** staticVars;//静态字段表
	bool InitStarted;//是否初始化
	Object* jClass;//类对象

	//显示继承树
	void showExtendsTree(MethodAreaClass* Class){
		printf("%s ",Class->name);
		if(Class->superClass != NULL){
			printf("-> ");
			showExtendsTree(Class->superClass);
		}
	}

	void showInterfacesNames(MethodAreaClass* Class){
		for(int i=0;i<Class->interfacesLen;++i){
			printf("%s ",Class->interfacesNames[i]);
		}
	}


}MethodAreaClass;



//方法引用字段
typedef struct methodRef{
	//memberRef------------------------------
	//symRef
	ConstPool* cp;
	classRef* cRef;

	u1* name;
	u1* descriptor;
	//---------------------------------------

	Method* method;

}methodRef;

typedef struct Method{
	u2 accessFlag;
	u1* name;
	u1* description;//方法描述(含:参数列表,方法返回类型)
	MethodAreaClass* Class;

	ExceptionInfo** exceptions;//异常表
	Code* codes;//内含maxStack,maxLocals,指令集数组

	int argSlotCount;
	int desNum;//方法参数列表形参个数
	string* des;//方法形式参数列表

}Method;

//字段引用
typedef struct fieldRef{

	//memberRef------------------------------
	//symRef
	ConstPool* cp;
	u1* className;
	classRef* cRef;

	u1* name;
	u1* descriptor;
	//---------------------------------------

	Field* field;

}fieldRef;


//字段(对象)
typedef struct Field{
	u2 accessFlag;
	u1* name;
	u1* description;//描述(字段的类型)
	MethodAreaClass* Class;//字段所在类
	u2 constValueIndex;
	u2 slotId;//操作数栈索引(该字段在类字段表中的位置)
}Field;

/**
字段加载器。根据字段名和字段描述快速查找字段内容
查找方法：
1.根据 [字段名] 在fieldMap里面找到相应id
2.根据id在fieldVectors找到对应的vector容器。再遍历容器匹配 [属性] 找到对应的字段

比如:字段名为hash,描述为I
1.根据字段名"hash"在fieldMap找到id=3(所有名为"hash"的字段都存放在fieldVectors[3]中
2.遍历fieldVectors[3],
**/
class fieldLoader{
	private:
		map<string,Field*> fieldMap;
		map<string,Field*>::iterator mit;//方法区迭代器
		MethodAreaClass* Class;
		b1* classKind;
	public:

		fieldLoader(MethodAreaClass* fClass){
			Class=fClass;
			classKind = getClassKind(fClass->accessFlag);
		}

		//在本类范围内寻找字段
		Field* findFieldInThis(string name){
			return fieldMap[name];
		}

		Field* findFieldInThisOrSuper(string name,MethodAreaClass* fieldClass){
			Field* field = fieldClass->floader->findFieldInThis(name);
			if(field != NULL){
				return field;
			}else if(fieldClass->superClass != NULL){
				return findFieldInThisOrSuper(name,fieldClass->superClass);
			}
			return NULL;
		}

		void showField(){
			if(fieldMap.begin()==fieldMap.end()){
				printf("\n[%s(%s)]字段表为空!\n",Class->name,classKind);return;
			}
			printf("\n[%s(%s)]字段表如下:\n",Class->name,classKind);
			Field* field;
			int index=0;
			for(mit = fieldMap.begin();mit!=fieldMap.end();mit++){
				string zd = mit->first;
				field=mit->second;

				if(field==NULL){
					fieldMap[zd]=findFieldInThisOrSuper(mit->first,Class);
					field = mit->second;
				}
				bool isStatic = IsStatic(field->accessFlag);
				string staticOrNot = "实例";
				if(isStatic == true){
					staticOrNot = "静态";
				}
				
				printf("--字段 %d (%s) : 名称 %s	描述 %s	",index++,staticOrNot.c_str(),zd.c_str(),field->description);
				printf("%s字段表索引 %d\n",staticOrNot.c_str(),field->slotId);
			}
		}

		//添加field
		void addField(Field* field){
			string fname=(char*)field->name;
			fieldMap[fname]=field;
		}

		Field* getFieldByName(string name){
			Field* field = fieldMap[name];
			if(field==NULL){
				fieldMap[name]=findFieldInThisOrSuper(name,Class);
			}
			
			return fieldMap[name];
		}

		//获取field(在本类及父类寻找)
		Field* getField(fieldRef* fRef){
			string fname=(char*)fRef->name;
			return getFieldByName(fname);
		}
		
		map<string,Field*> getFieldMap(){
			return fieldMap;
		}
};

/**
方法加载器。与字段加载器类似
**/
class methodLoader{
	private:
		map<string,Method*> methodMap;
		map<string,Method*>::iterator mit;//方法区迭代器
		MethodAreaClass* mClass;
		methodRef* cMRef;//clinit方法引用
		b1* classKind;
	public:
		methodLoader(MethodAreaClass* Class){
			mClass=Class;
			cMRef=(methodRef*)malloc(sizeof(methodRef));
			cMRef->name=(u1*)"<clinit>";
			cMRef->descriptor=(u1*)"()V";
			classKind = getClassKind(Class->accessFlag);
		}

		void showMethod(){
			if(methodMap.begin()==methodMap.end()){
				printf("\n[%s(%s)]方法表为空!\n",mClass->name,classKind);return;
			}
			
			printf("\n[%s(%s)]方法表如下(不含父类方法):\n",mClass->name,classKind);
			Method* method;
			int index=0;
			
			for(mit = methodMap.begin();mit!=methodMap.end();mit++){
				string zd = mit->first;
				if(mit->second == NULL){
					mit->second = findMethodInThisOrSuper(mClass->superClass,mit->first);
				}
				method = mit->second;
		 		bool isStatic = IsStatic(method->accessFlag);
				printf("--方法 %d (%s) : 方法签名 %s	",index++,isStatic == true ? "静态":"实例",zd.c_str());
				int desNum=method->desNum;
				printf("	返回类型 %s\n",method->des[desNum-1].c_str());
			}
		}

		//添加method
		void addMethod(Method* method){
			string nameAndDes=charArrayToString((char*)method->name)+"~"+charArrayToString((char*)method->description);
			methodMap[nameAndDes]=method;
		}

		Method* findMethodInThis(MethodAreaClass* Class,string key){
			return methodMap[key];
		}

		Method* findMethodInThisOrSuper(MethodAreaClass* Class,string key){
	
			Method* method = Class->mloader->findMethodInThis(Class,key);
			if(method == NULL && Class->superClass != NULL){
				return findMethodInThisOrSuper(Class->superClass,key);
			}
			return method;
		}

		//获取方法
		Method* getMethodByNameAndDes(string mname,string mdes){
			string key=mname+"~"+mdes;
			Method* method = methodMap[key];
			if(method == NULL && mClass->superClass != NULL){
				return findMethodInThisOrSuper(mClass->superClass,key);
			}
			return methodMap[key];
		}

		//获取方法
		Method* getMethod(methodRef* mRef){
			string mname=(char*)mRef->name;
			string mdes=(char*)mRef->descriptor;
			return getMethodByNameAndDes(mname,mdes);
		}
		
		Method* getClinitMethod(){
			return getMethod(cMRef);	
		}

		Method* getMethodInThisClass(MethodAreaClass* Class){

		}

};

//方法区(类加载器)
class methodArea{
		
	private:
		map<string,MethodAreaClass*> classMap;
		map<string,MethodAreaClass*>::iterator mit;//方法区迭代器
		MethodAreaClass* jlClass;//java/lang/Class类
		int MethodAreaClassNum;//方法区类数量
	public:
		Heap* heap;//方法区所属的堆区
		Registry* nativeMethodMap;//本地方法注册表

		methodArea(Heap* heap){
			this->heap = heap;
			MethodAreaClassNum=0;

			//---------------------loadBasicClasses-------------------
			//加载java/lang/Class类
			jlClass=NULL;
			jlClass=loadNonArrayClass(this,"java/lang/Class",false);

			//给java/lang/Class类和基本类型加载java/lang/Class对象jClass(类关联类对象)
			for(mit = classMap.begin();mit!=classMap.end();mit++){
				mit->second->jClass=newJClassObject(jlClass,mit->second);
			}

			//-------------------loadPrimitiveClasses------------------
			loadPrimitiveClasses(this,jlClass);
		}

		//添加类
		void loadClass(MethodAreaClass* Class){
			
			if(jlClass!=NULL && Class){
				Class->jClass=newJClassObject(jlClass,Class);

			}
			string name=charArrayToString((char*)Class->name);
			if(classMap[name]==NULL){
				classMap[name]=Class;
				MethodAreaClassNum++;
			}
		}

		//获取类(找不到类,返回空;不用报错)
		MethodAreaClass* getLoadClass(string cname){
			if(classMap[cname]==0){
				//假如找不到key为"Object"的元素，map会自动添加键值对<"Object",NULL>进去。但要防止这种情况，所以要删掉!
				classMap.erase(cname);
				return NULL;
			}
			return classMap[cname];
		}

		void showMethodArea(){
			if(classMap.begin()==classMap.end()){
				printf("\n方法区为空!\n\n");return;
			}
			printf("\n方法区有如下类信息:\n\n");
			for(mit = classMap.begin();mit!=classMap.end();mit++){
				string zd = mit->first;
				printf("%s(JClass@%d)\n",zd.c_str(),mit->second->jClass);
			}
		}

		int getMethodAreaClassNum(){
			return MethodAreaClassNum;
		}

		map<string,MethodAreaClass*> getClassMap(){
			return classMap;
		};
};


//模块可用方法-------------------------------------------------------------------------------------------

//传入：索引指针
//传出：类/接口信息
MethodAreaClass* resolveClass(classRef* cRef);

//传入：字段索引,是否显示测试信息
//传出：引用字段
Field* resolveField(fieldRef* fRef,bool istest);

//传入：方法索引,是否显示测试信息
//传出：引用方法
Method* resolveMethod(methodRef* mRef,bool istest);

void resolveMethodRef(methodRef* mRef,bool istest);

//传入：类c,类other
//传出：类other是否为类c的超类
bool isSupClassOf(MethodAreaClass* c,MethodAreaClass* other);

//传入：查找类,方法索引,是否显示测试信息
//传出：引用方法
Method* lookupMethod(MethodAreaClass* c,methodRef* mRef,bool istest);

//判断c是否实现接口iface
bool isImplements(MethodAreaClass* c,MethodAreaClass* iface);

bool isSubInterfaceOf(MethodAreaClass* c,MethodAreaClass* iface);

//通过类名加载类
MethodAreaClass* loadClassByName(methodArea* loader,u1* name);