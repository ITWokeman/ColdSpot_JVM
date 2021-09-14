/**
  methodArea  ������
  ������Ϣ�ṹ��classInfo����һ�������󣬴�ŵ���������methodArea��
  ͬʱ����һ���й�MethodAreaClass������
**/
#pragma once//��ֹ�ظ�����
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
string charArrayToString(char* chas);//�ַ�����ת�ַ���
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest);
Object* newJClassObject(MethodAreaClass* jClass,MethodAreaClass* myClass);
void loadPrimitiveClasses(methodArea* loader,MethodAreaClass* jlClass);
int getMemory();
b1* getClassKind(u2 accessFlag);

//������
typedef struct classRef{
	ConstPool* cp;//�����ڳ���
	u1* name;//����
	MethodAreaClass* Class;//��
}classRef;

//��������
typedef struct memberRef{
	ConstPool* cp;
	int classIndex;
	int nameAndTypeIndex;
}memberRef;

//�ӿ�����
typedef struct interfaceMethodRef{
	int classIndex;
	int nameAndTypeIndex;
}interfaceMethodRef;

typedef struct Ref{
	classRef* _classRef;//������
	methodRef* _methodRef;//��������
	fieldRef* _fieldRef;//�ֶ�����
	interfaceMethodRef* _interfaceMethodRef;//�ӿ�����
}Ref;

//MethodAreaClass����
typedef struct Constant{
	char type;//��������
	u1* val;//�����������ͣ�int��float��double��string��
	Ref* ref;
};

//����ʱ������
typedef struct ConstPool{
	MethodAreaClass* Class;
	Constant** consts;
	int cpSize;
}ConstPool;


//Ҫ�ŵ��������ڵ��� Method Area Class
typedef struct MethodAreaClass{

	u2 accessFlag;				//���ʱ�־
	u1* name;
	u1* superClassName;
	int interfacesLen;//�ӿ��б�����
	u1** interfacesNames;//�ӿ����Ʊ�
	ConstPool* cp;
	int fieldsLen;//�ֶα���
	fieldLoader* floader;//�ֶμ�����(�ܿ��ٻ�ȡ�ֶ���Ϣ���ֶα�)
	int methodsLen;
	methodLoader* mloader;//����������
	methodArea* loader;//�����������������
	MethodAreaClass* superClass;
	MethodAreaClass** interfaces;//�ӿ�����Ϣ
	u2	instanceSlotCount;
	u2 staticSlotCount;//��̬�ֶ�����
	Slot** staticVars;//��̬�ֶα�
	bool InitStarted;//�Ƿ��ʼ��
	Object* jClass;//�����

	//��ʾ�̳���
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



//���������ֶ�
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
	u1* description;//��������(��:�����б�,������������)
	MethodAreaClass* Class;

	ExceptionInfo** exceptions;//�쳣��
	Code* codes;//�ں�maxStack,maxLocals,ָ�����

	int argSlotCount;
	int desNum;//���������б��βθ���
	string* des;//������ʽ�����б�

}Method;

//�ֶ�����
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


//�ֶ�(����)
typedef struct Field{
	u2 accessFlag;
	u1* name;
	u1* description;//����(�ֶε�����)
	MethodAreaClass* Class;//�ֶ�������
	u2 constValueIndex;
	u2 slotId;//������ջ����(���ֶ������ֶα��е�λ��)
}Field;

/**
�ֶμ������������ֶ������ֶ��������ٲ����ֶ�����
���ҷ�����
1.���� [�ֶ���] ��fieldMap�����ҵ���Ӧid
2.����id��fieldVectors�ҵ���Ӧ��vector�������ٱ�������ƥ�� [����] �ҵ���Ӧ���ֶ�

����:�ֶ���Ϊhash,����ΪI
1.�����ֶ���"hash"��fieldMap�ҵ�id=3(������Ϊ"hash"���ֶζ������fieldVectors[3]��
2.����fieldVectors[3],
**/
class fieldLoader{
	private:
		map<string,Field*> fieldMap;
		map<string,Field*>::iterator mit;//������������
		MethodAreaClass* Class;
		b1* classKind;
	public:

		fieldLoader(MethodAreaClass* fClass){
			Class=fClass;
			classKind = getClassKind(fClass->accessFlag);
		}

		//�ڱ��෶Χ��Ѱ���ֶ�
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
				printf("\n[%s(%s)]�ֶα�Ϊ��!\n",Class->name,classKind);return;
			}
			printf("\n[%s(%s)]�ֶα�����:\n",Class->name,classKind);
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
				string staticOrNot = "ʵ��";
				if(isStatic == true){
					staticOrNot = "��̬";
				}
				
				printf("--�ֶ� %d (%s) : ���� %s	���� %s	",index++,staticOrNot.c_str(),zd.c_str(),field->description);
				printf("%s�ֶα����� %d\n",staticOrNot.c_str(),field->slotId);
			}
		}

		//���field
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

		//��ȡfield(�ڱ��༰����Ѱ��)
		Field* getField(fieldRef* fRef){
			string fname=(char*)fRef->name;
			return getFieldByName(fname);
		}
		
		map<string,Field*> getFieldMap(){
			return fieldMap;
		}
};

/**
���������������ֶμ���������
**/
class methodLoader{
	private:
		map<string,Method*> methodMap;
		map<string,Method*>::iterator mit;//������������
		MethodAreaClass* mClass;
		methodRef* cMRef;//clinit��������
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
				printf("\n[%s(%s)]������Ϊ��!\n",mClass->name,classKind);return;
			}
			
			printf("\n[%s(%s)]����������(�������෽��):\n",mClass->name,classKind);
			Method* method;
			int index=0;
			
			for(mit = methodMap.begin();mit!=methodMap.end();mit++){
				string zd = mit->first;
				if(mit->second == NULL){
					mit->second = findMethodInThisOrSuper(mClass->superClass,mit->first);
				}
				method = mit->second;
		 		bool isStatic = IsStatic(method->accessFlag);
				printf("--���� %d (%s) : ����ǩ�� %s	",index++,isStatic == true ? "��̬":"ʵ��",zd.c_str());
				int desNum=method->desNum;
				printf("	�������� %s\n",method->des[desNum-1].c_str());
			}
		}

		//���method
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

		//��ȡ����
		Method* getMethodByNameAndDes(string mname,string mdes){
			string key=mname+"~"+mdes;
			Method* method = methodMap[key];
			if(method == NULL && mClass->superClass != NULL){
				return findMethodInThisOrSuper(mClass->superClass,key);
			}
			return methodMap[key];
		}

		//��ȡ����
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

//������(�������)
class methodArea{
		
	private:
		map<string,MethodAreaClass*> classMap;
		map<string,MethodAreaClass*>::iterator mit;//������������
		MethodAreaClass* jlClass;//java/lang/Class��
		int MethodAreaClassNum;//������������
	public:
		Heap* heap;//�����������Ķ���
		Registry* nativeMethodMap;//���ط���ע���

		methodArea(Heap* heap){
			this->heap = heap;
			MethodAreaClassNum=0;

			//---------------------loadBasicClasses-------------------
			//����java/lang/Class��
			jlClass=NULL;
			jlClass=loadNonArrayClass(this,"java/lang/Class",false);

			//��java/lang/Class��ͻ������ͼ���java/lang/Class����jClass(����������)
			for(mit = classMap.begin();mit!=classMap.end();mit++){
				mit->second->jClass=newJClassObject(jlClass,mit->second);
			}

			//-------------------loadPrimitiveClasses------------------
			loadPrimitiveClasses(this,jlClass);
		}

		//�����
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

		//��ȡ��(�Ҳ�����,���ؿ�;���ñ���)
		MethodAreaClass* getLoadClass(string cname){
			if(classMap[cname]==0){
				//�����Ҳ���keyΪ"Object"��Ԫ�أ�map���Զ���Ӽ�ֵ��<"Object",NULL>��ȥ����Ҫ��ֹ�������������Ҫɾ��!
				classMap.erase(cname);
				return NULL;
			}
			return classMap[cname];
		}

		void showMethodArea(){
			if(classMap.begin()==classMap.end()){
				printf("\n������Ϊ��!\n\n");return;
			}
			printf("\n����������������Ϣ:\n\n");
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


//ģ����÷���-------------------------------------------------------------------------------------------

//���룺����ָ��
//��������/�ӿ���Ϣ
MethodAreaClass* resolveClass(classRef* cRef);

//���룺�ֶ�����,�Ƿ���ʾ������Ϣ
//�����������ֶ�
Field* resolveField(fieldRef* fRef,bool istest);

//���룺��������,�Ƿ���ʾ������Ϣ
//���������÷���
Method* resolveMethod(methodRef* mRef,bool istest);

void resolveMethodRef(methodRef* mRef,bool istest);

//���룺��c,��other
//��������other�Ƿ�Ϊ��c�ĳ���
bool isSupClassOf(MethodAreaClass* c,MethodAreaClass* other);

//���룺������,��������,�Ƿ���ʾ������Ϣ
//���������÷���
Method* lookupMethod(MethodAreaClass* c,methodRef* mRef,bool istest);

//�ж�c�Ƿ�ʵ�ֽӿ�iface
bool isImplements(MethodAreaClass* c,MethodAreaClass* iface);

bool isSubInterfaceOf(MethodAreaClass* c,MethodAreaClass* iface);

//ͨ������������
MethodAreaClass* loadClassByName(methodArea* loader,u1* name);