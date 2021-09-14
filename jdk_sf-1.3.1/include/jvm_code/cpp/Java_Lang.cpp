/**
java.lang下类的本地方法
**/
#pragma once//防止重复定义
#include "../headers/java_lang.h"

//--------------------------------Class类的本地方法---------------------------------------
//static native Class<?> getPrimitiveClass(String name);方法
//(Ljava/lang/String;)Ljava/lang/Class;
void getPrimitiveClass(Frame* frame) {

	Object* nameObj=frame->localVars[0]->ref;
	
	Method* m=frame->method;//获取方法
	MethodAreaClass* c=m->Class;//获取方法类
	methodArea* ma=c->loader;//获取方法区
	Heap* heap=ma->heap;//获取堆区
	stringPool* sPool=heap->getStrPool();//获取字符串池

	b1* name=sPool->getJStrValue(nameObj);
	string str=(char*)name;
	MethodAreaClass* Class=ma->getLoadClass(str);
	Object* JClass=Class->jClass;
	frame->pushRef(JClass);
}

//public native boolean isArray();
//()Z
void isArray(Frame* frame){
	Object* This=frame->localVars[0]->ref;//获取this引用
	if(This->array!=NULL){
		frame->pushB4(1,3);
	}else{
		frame->pushB4(0,3);
	}
}

//public final native Class<?> getSuperclass();方法
//()Ljava/lang/Class;
void getSuperclass(Frame* frame){
	Object* This=frame->localVars[0]->ref;//获取this引用
	MethodAreaClass* superClass=This->objClass->superClass;
	if(superClass!=NULL){
		frame->pushRef(superClass->jClass);
	}else{//如果没有父类的话
		//创建一个空对象
		Object* nullObj=(Object*)malloc(sizeof(Object));
		nullObj->objClass=(MethodAreaClass*)malloc(sizeof(MethodAreaClass));
		nullObj->objClass->name=(u1*)"null";

		//创建空对象字段表
		nullObj->fields=(Slot**)calloc(10,sizeof(Slot*));
		
		//创建name字段
		Slot* nullClassName=(Slot*)malloc(sizeof(Slot));
		nullClassName->ref=NULL;
		nullObj->fields[9]=nullClassName;

		frame->pushRef(nullObj);
	}
}

//public native Class<?> getComponentType();
//()Ljava/lang/Class;
void getComponentType(Frame* frame) {
	Object* This=frame->localVars[0]->ref;//获取this引用
	MethodAreaClass* objClass=This->objClass;
	Object* JClass=objClass->jClass;
	frame->pushRef(JClass);
}

//public native boolean isPrimitive();
//()Z
void isPrimitive(Frame* frame) {
	Object* This=frame->localVars[0]->ref;//获取this引用
	string primitiveName[9]={"char","void","boolean","short","int","long","byte","float","double"};

	string name=charArrayToString((char*)This->objClass->name);
	for(int i=0;i<9;++i){
		if(strcmp(name.c_str(),primitiveName[i].c_str())==true){
			frame->pushB4(1,3);
			return;
		}
	}
	frame->pushB4(0,3);
}

//private native String getName0();方法
//()Ljava/lang/String;
void getName0(Frame* frame) {
	Object* This=frame->localVars[0]->ref;//获取this引用
	MethodAreaClass* Class=This->objClass;
	methodArea* ma=frame->method->Class->loader;
	Heap* heap=ma->heap;
	stringPool* sPool=heap->getStrPool();
	Object* nameObj=sPool->JString((b1*)Class->name);
	frame->pushRef(nameObj);
}

//private static native boolean desiredAssertionStatus0(Class<?> clazz);方法
//(Ljava/lang/Class;)Z
void desiredAssertionStatus0(Frame* frame) {
	frame->pushB4(0,3);
}

void forName0(Frame* frame) {

	Object* jName = frame->localVars[0]->ref;
	b1 initialize = frame->localVars[1]->num;
	//jLoader := vars.GetRef(2)

	b1* goName = frame->method->Class->loader->heap->getStrPool()->getJStrValue(jName);

	methodArea* loader = frame->method->Class->loader;

	MethodAreaClass* Class = loadNonArrayClass(loader,charArrayToString(goName),false);

	Object* jClass = Class->jClass;

	if(initialize==true && Class->InitStarted==false){
		// undo forName0
		Thread* thread = frame->thisThread;
		frame->pc = thread->pc;
		// init class
		initClass(frame,thread,Class);
	}else{
		frame->pushRef(jClass);
	}
}

int isSuperClass(MethodAreaClass* Class1,MethodAreaClass* Class2){
	if(Class1 == Class2){
		return 1;
	}
	if(Class2->superClass != NULL){
		return isSuperClass(Class1,Class2->superClass);
	}
	return 0;
}


void isAssignableFrom(Frame* frame){
	//Class1与Class2相等，或者Class1是Class2的父类
	MethodAreaClass* Class1 = frame->localVars[0]->ref->objClass;
	MethodAreaClass* Class2 = frame->localVars[1]->ref->objClass;

	if(Class1 == Class2){
		frame->pushB4(1,3);
		return;
	}
	frame->pushB4(isSuperClass(Class1,Class2),3);

}

void getDeclaredFields0(Frame* frame){
	MethodAreaClass* Class = loadArrayClass(frame->method->Class->loader,"java/lang/reflect/Field",false);
	frame->pushRef(newObject(Class));
}

//------------------------------- Object类的本地方法--------------------------------------

//public final native Class<?> getClass();方法
//()Ljava/lang/Class;
void getClass(Frame* frame){
	Object* This=frame->localVars[0]->ref;//获取this引用
	Object* classObj=This->objClass->jClass;
	frame->pushRef(classObj);
}

//public native int hashCode();方法
//()V
void hashCode(Frame* frame){
	Object* This=frame->localVars[0]->ref;//获取this引用
	b4 hashcode=(b4)This;//直接取地址作为hash值
	frame->pushB4(hashcode,3);
}

//protected native Object clone() throws CloneNotSupportedException;
//()Ljava/lang/Object;
void clone(Frame* frame) {

	Object* This=frame->localVars[0]->ref;//获取this引用

	MethodAreaClass* cloneable=loadNonArrayClass(This->objClass->loader,"java/lang/Cloneable",false);

	if(isImplements(This->objClass,cloneable)==false){
		//panic("java.lang.CloneNotSupportedException")
	}
	Object* cloneObj=newObject(This->objClass);

	//下面进行浅拷贝
	/**
	1.浅拷贝:对于基本数据类型进行值传递，对于引用类型拷贝其引用地址（即地址还是指向原来对象）
	2.深拷贝:对于基本数据类型进行值传递，对于引用类型创建一个新的对象并复制其内容。
	可以看出深拷贝有层数问题（因为引用类型数据内容也有引用类型数据！），而且深拷贝会出现循环引用的问题，需要小心！
	**/
	cloneObj->array = This->array;//拷贝其引用地址
	cloneObj->fields = This->fields;//拷贝其引用地址

	frame->pushRef(cloneObj);
}

//-------------------------------System类的本地方法---------------------------------------

//public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length);方法
//(Ljava/lang/Object;ILjava/lang/Object;II)V
void arraycopy(Frame* frame){

	Slot** vars=frame->localVars;
	
	Object* src=vars[0]->ref;//获取源数组
	u1 srcPos=vars[1]->num;//源数组拷贝元素的起始位置
	
	Object* dest=vars[2]->ref;//获取目标数组
	u1 destPos=vars[3]->num;//拷贝到目标数组的起始位置

	u1 length=vars[4]->num;//拷贝元素个数

	if(src==NULL || dest==NULL){
		printError(0x013,NULL);
		return;
	}

	if(src->array->len<0 || dest->array->len<0 || (src->array->type!=dest->array->type)){//len＜0，说明不是数组。要确保两个数组类型一样
		printError(0x015,NULL);
	}
	if(srcPos<0 || destPos<0 || length<0){
		printError(0x015,NULL);
	}

	//ArrayCopy
	int atype=src->array->type;
	if(atype==10){
		Object** newDst=dest->array->objArray;
		Object** objArray=src->array->objArray;
		for(int i=0;i<length;++i){
			newDst[i+destPos]=objArray[i+srcPos];
		}
		//newDst[length+destPos]=NULL;
	}
	else if(atype>2 && atype<5){
		b4* newDst=dest->array->B4Array;
		b4* b4Array=src->array->B4Array;
		for(int i=0;i<length;++i){
			newDst[i+destPos]=b4Array[i+srcPos];
		}
		//newDst[length+destPos]='\0';
	}
	else if(atype<3){
		b1* newDst=dest->array->B1Array;
		b1* b1Array=src->array->B1Array;
	//	printf("(src: dest:%s destPos:%d srcPos:%d len:%d)\n",b1Array,newDst,destPos,srcPos,length);
		int index=destPos;
		for(int i=0;i<length;++i){
			newDst[i+destPos]=b1Array[i+srcPos];
		}
		//newDst[srcPos+length+(destPos-srcPos)]='\0';
	}else{
		b8* newDst=dest->array->B8Array;
		b8* b8Array=src->array->B8Array;
		for(int i=0;i<length;++i){
			newDst[i+destPos]=b8Array[i+srcPos];
		}
		newDst[length+destPos]='\0';
	}
}

void identityHashCode(Frame* frame){
	hashCode(frame);
}

//-------------------------------Float类的本地方法----------------------------------------
//public static native int floatToRawIntBits(float value);
//(F)I
void floatToRawIntBits(Frame* frame) {
	b4 value=frame->localVars[0]->num;
	frame->pushB4(value,3);//push Int
}

//public static native float intBitsToFloat(int bits);
//(I)F
void intBitsToFloat(Frame* frame) {
	b4 value=frame->localVars[0]->num;
	frame->pushB4(value,4);//push Float
}

//-------------------------------Double类的本地方法---------------------------------------
//public static native long doubleToRawLongBits(double value);
//(D)J
void doubleToRawLongBits(Frame* frame) {
	b8 hight=frame->localVars[0]->num;
	b8 low=frame->localVars[1]->num;
	frame->pushB8(hight,low,5);//push Long
}

//public static native double longBitsToDouble(long bits);
//(J)D
void longBitsToDouble(Frame* frame) {
	b8 hight=frame->localVars[0]->num;
	b8 low=frame->localVars[1]->num;
	frame->pushB8(hight,low,6);//push Double
}

//-------------------------------String类的本地方法---------------------------------------
//public native String intern();
//()Ljava/lang/String;
void intern(Frame* frame) {
	Object* This=frame->localVars[0]->ref;
	methodArea* ma=frame->method->Class->loader;
	Heap* heap=ma->heap;
	stringPool* sPool=heap->getStrPool();
	b1* strVal=sPool->getJStrValue(This);
	Object* interned=sPool->JString(strVal);
	frame->pushRef(interned);
}

//------------------------------Throwable类的本地方法-------------------------------------

//private native Throwable fillInStackTrace(int dummy);
//(I)Ljava/lang/Throwable;
void fillInStackTrace(Frame* frame) {//未完成！！！
	Object* This=frame->localVars[0]->ref;
	frame->pushRef(This);
}

//--------------------------------Thread类的本地方法--------------------------------------
//public static native Thread currentThread();
//()Ljava/lang/Thread;
void currentThread(Frame* frame){
	//jThread := frame.Thread().JThread()
	methodArea* classLoader=frame->method->Class->loader;

	Object* jThread=frame->thisThread->threadObject;
	MethodAreaClass* threadClass = jThread->objClass;

	MethodAreaClass* threadGroupClass = loadNonArrayClass(classLoader,"java/lang/ThreadGroup",false);
	Object* jGroup = newObject(threadClass);

	//将jThread的group字段赋值为jGroup
	int groupId = threadClass->floader->getFieldByName("group")->slotId;
	jThread->fields[groupId]->ref=jGroup;
	jThread->fields[groupId]->flag=10;

	//将jThread的priority字段赋值为1
	int priorityId = threadClass->floader->getFieldByName("priority")->slotId;
	jThread->fields[priorityId]->num=1;
	jThread->fields[priorityId]->flag=3;

	frame->pushRef(jThread);
}

//-------------------------------Runtime类的本地方法--------------------------------------

//public native int availableProcessors();
//()I
void availableProcessors(Frame* frame) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    int numCPU = si.dwNumberOfProcessors;//获取逻辑cpu数
	frame->pushB4(numCPU,3);
}

//-----------------------------------------------------------------------------------------
void getStackAccessControlContext(Frame* frame){
	MethodAreaClass* Class = frame->method->Class;
	frame->pushRef(newObject(loadNonArrayClass(Class->loader,"java/security/AccessControlContext",false)));
}

void getInheritedAccessControlContext(Frame* frame){
	MethodAreaClass* Class = frame->method->Class;
	frame->pushRef(newObject(loadNonArrayClass(Class->loader,"java/security/AccessControlContext",false)));
}

//------------------------注册java.lang包所有类的本地方法---------------------------------
void regJavaLangAllNativeMethod(Registry* registry){

	string CKeys[]={//java.lang所有类本地方法的名称
		"java/lang/Class~getPrimitiveClass~(Ljava/lang/String;)Ljava/lang/Class;",
		"java/lang/Class~getName0~()Ljava/lang/String;",
		"java/lang/Class~desiredAssertionStatus0~(Ljava/lang/Class;)Z",
		"java/lang/Object~getClass~()Ljava/lang/Class;",
		"java/lang/System~arraycopy~(Ljava/lang/Object;ILjava/lang/Object;II)V",
		"java/lang/Float~floatToRawIntBits~(F)I",
		"java/lang/Float~intBitsToFloat~(I)F",
		"java/lang/Double~doubleToRawLongBits~(D)J",
		"java/lang/Double~longBitsToDouble~(J)D",
		"java/lang/String~intern~()Ljava/lang/String;",
		"java/lang/Class~isArray~()Z",
		"java/lang/Class~isPrimitive~()Z",
		"java/lang/Class~getComponentType~()Ljava/lang/Class;",
		"java/lang/Throwable~fillInStackTrace~(I)Ljava/lang/Throwable;",
		"java/lang/Object~hashCode~()I",
		"java/lang/Thread~currentThread~()Ljava/lang/Thread;",
		"java/lang/Class~forName0~(Ljava/lang/String;ZLjava/lang/ClassLoader;Ljava/lang/Class;)Ljava/lang/Class;",
		"java/lang/Runtime~availableProcessors~()I",
		"java/lang/Object~clone~()Ljava/lang/Object;",
		"java/lang/Class~getSuperclass~()Ljava/lang/Class;",
		"java/security/AccessController~getStackAccessControlContext~()Ljava/security/AccessControlContext;",
		"java/security/AccessController~getInheritedAccessControlContext~()Ljava/security/AccessControlContext;",
		"java/lang/Class~isAssignableFrom~(Ljava/lang/Class;)Z",
		"java/lang/Class~getDeclaredFields0~(Z)[Ljava/lang/reflect/Field;",
		"java/lang/System~identityHashCode~(Ljava/lang/Object;)I"
	};

	int nativeMethodNum=sizeof(CKeys)/sizeof(CKeys[0]);//java.lang所有类本地方法的数量

	//nativeMethods
	nativeMethod** NMS=(nativeMethod**)calloc(nativeMethodNum,sizeof(nativeMethod*));

	//注册java.lang包下类所有本地方法
	for(int i1=0;i1<nativeMethodNum;++i1){
		NMS[i1]=(nativeMethod*)malloc(sizeof(nativeMethod));
		registry->regNativeMethod(CKeys[i1],NMS[i1]);//注册本地方法
	}
	//-----------------本地方法.func = 执行本地方法的相应函数-------------------------
	NMS[0]->func=getPrimitiveClass;
	NMS[1]->func=getName0;
	NMS[2]->func=desiredAssertionStatus0;
	NMS[3]->func=getClass;
	NMS[4]->func=arraycopy;
	NMS[5]->func=floatToRawIntBits;
	NMS[6]->func=intBitsToFloat;
	NMS[7]->func=doubleToRawLongBits;
	NMS[8]->func=longBitsToDouble;
	NMS[9]->func=intern;
	NMS[10]->func=isArray;
	NMS[11]->func=isPrimitive;
	NMS[12]->func=getComponentType;
	NMS[13]->func=fillInStackTrace;
	NMS[14]->func=hashCode;
	NMS[15]->func=currentThread;
	NMS[16]->func=forName0;
	NMS[17]->func=availableProcessors;
	NMS[18]->func=clone;
	NMS[19]->func=getSuperclass;
	NMS[20]->func=getStackAccessControlContext;
	NMS[21]->func=getInheritedAccessControlContext;
	NMS[22]->func=isAssignableFrom;
	NMS[23]->func=getDeclaredFields0;
	NMS[24]->func=identityHashCode;
}
