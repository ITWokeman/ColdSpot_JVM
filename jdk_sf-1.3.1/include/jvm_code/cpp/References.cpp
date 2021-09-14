/**
	引用指令集
**/
#pragma once//防止重复定义
#include "..\headers\references.h"
#include "..\headers\Heap.h"
#include "..\headers\Thread.h"

/*
new操作
举例:Student a=new Student();
1.先通过index索引找到类引用指针
2.然后通过指针找到String类信息。
3.判断一下Student是否能实例化
4.读取Student属性个数n，分配n个Slot存放Student类对象信息。
*/
void invokeMethod(Frame* frame,Method* method);

void _new(Frame* frame,u1 opNum){

	int index=frame->readB2();

	//通过index寻找要创建对象的类
	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;//获取类引用指针
	
	if(cf==NULL){//如果找不到索引
		string info[]={charArrayToString((char*)cf->name)};
		printError(0x011,info);
		return;
	}
	if(cf->Class==NULL){
		cf->Class=resolveClass(cf);//获取类信息
	}

	//如果类没有初始化（到时候想办法把这块移到constClassLoader那里，在加载完成的时候就初始化）
	if(cf->Class->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,cf->Class);
		return;
	}

	Object* obj=newObject(cf->Class);

	Heap* heap=frame->method->Class->loader->heap;//获取堆区
	frame->pushRef(obj);//将对象放入栈帧

}

//给静态变量赋值
void putStatic(Frame* frame,u1 opNum){

	int index=frame->readB2();

	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//获取字段引用
	Field* f=resolveField(fr,false);//获取字段
	MethodAreaClass* fClass=f->Class;

	//如果类没有初始化（到时候想办法把这块移到constClassLoader那里，在加载完成的时候就初始化）
	if(fClass->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,fClass);
		return;
	}

	if(IsStatic(f->accessFlag)==0){
		//报错：java.lang.IncompatibleClassChangeError
		return;
	}
	if(IsFinal(f->accessFlag)==1){
		char* clinit="<clinit>";
		if(Class!=fClass || isEqual(frame->method->name,(u1*)clinit)==0){
			//报错：java.lang.IllegalAccessError
			return;
		}
	}

	u1 des=f->description[0];
	int slotId=f->slotId;
	Slot** sv=fClass->staticVars;
	Slot** os=frame->opStack;

	if(sv[slotId]==NULL){
		sv[slotId]=(Slot*)malloc(sizeof(Slot));
	}

	if(des=='Z' || des=='B' || des=='C' || des=='I' || des=='F'){
		sv[slotId]->num=frame->popB4();
		des=='F' ? sv[slotId]->flag=4 : sv[slotId]->flag=3;
	}
	if(des=='J' || des=='D'){
		if(sv[slotId+1]==NULL){
			sv[slotId+1]=(Slot*)malloc(sizeof(Slot));
		}
		sv[slotId+1]->num=frame->popB4();//低位
		sv[slotId]->num=frame->popB4();//高位
		if(des=='J'){
			sv[slotId]->flag=5;
			sv[slotId+1]->flag=5; 
		}else{
			sv[slotId]->flag=6;
			sv[slotId+1]->flag=6; 
		}
	}
	if(des=='L' || des=='['){
		Slot* slot = sv[slotId];
		slot->ref=frame->popRef();
		slot->flag=10;//10代表引用
	}
}

void getStatic(Frame* frame,u1 opNum){

	int index=frame->readB2();
	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//获取字段引用
	
	Field* f=resolveField(fr,false);//获取字段
	MethodAreaClass* fClass=f->Class;
	
	//如果类没有初始化（到时候想办法把这块移到constClassLoader那里，在加载完成的时候就初始化）
	if(fClass->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,fClass);
		return;
	}

	if(IsStatic(f->accessFlag)==false){
		//报错：java.lang.IncompatibleClassChangeError
		return;
	}

	u1 des=f->description[0];
	int slotId=f->slotId;
	Slot** sv=fClass->staticVars;
	Slot** os=frame->opStack;
	if(des=='Z' || des=='B' || des=='C' || des=='I' || des=='F'){
		if(slotId>fClass->staticSlotCount){
			slotId=fClass->staticSlotCount;
		}
		frame->pushB4(sv[slotId]->num,des=='F' ? 4 : 3);
	}
	if(des=='J' || des=='D'){
		frame->pushB8(sv[slotId+1]->num,sv[slotId]->num,des=='J' ? 5 : 6);
	}
	if((des=='L' || des=='[') && isEqual(f->description,(u1*)"Ljava/io/PrintStream;")==false){
		
		if(sv[slotId]==NULL){
			printError(0x013,NULL);
			return;
		}
		Object* obj = sv[slotId]->ref;
		if((int)obj <= 0 ){
			frame->pushRef(newObject(f->Class));
			return;
		}
		frame->pushRef(obj);
	}
}

void putField(Frame* frame,u1 opNum){

	int index=frame->readB2();
	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//获取字段引用
	Field* f=resolveField(fr,false);//获取字段
	MethodAreaClass* fClass=f->Class;
	
	if(IsStatic(f->accessFlag)==true){
		//panic("java.lang.IncompatibleClassChangeError")
		return;
	}
	if(IsFinal(f->accessFlag)==true){
		char* init="<init>";
		if(Class!=fClass || isEqual(frame->method->name,(u1*)init)==false){
			//报错：java.lang.IllegalAccessError
			return;
		}
	}
	u1 des=f->description[0];
	int slotId=f->slotId;
	Slot** os=frame->opStack;
	Object* ref;

	if(des=='Z' || des=='B' || des=='C' || des=='I' || des=='F'){
		int val=frame->popB4();
		ref=frame->popRef();
		if(ref==NULL){
			printError(0x013,NULL);return;
		}
		if(ref->objClass->instanceSlotCount>slotId){
			ref->fields[slotId]->num=val;
			des=='F' ? ref->fields[slotId]->flag=4 : ref->fields[slotId]->flag=3;
		}
	}	
	if(des=='J' || des=='D'){
		b4 low=frame->popB4();
		b4 hight=frame->popB4();
		ref=frame->popRef();
		if(ref==NULL){
			printError(0x013,NULL);return;
		}

		if(ref->objClass->instanceSlotCount>slotId){

			ref->fields[slotId]->num=hight;
			ref->fields[slotId+1]->num=low;
	
			if(des=='J'){
				ref->fields[slotId]->flag=5;
				ref->fields[slotId]->flag=5; 
			}else{
				ref->fields[slotId]->flag=6;
				ref->fields[slotId]->flag=6; 
			}
		}
	}
	if(des=='L' || des=='['){
		Object* obj=frame->popRef();
		ref=frame->popRef();
		if(ref==NULL){
			printError(0x013,NULL);return;
		}
		if(ref==NULL){
			printf("NULL");
		}
		ref->fields[slotId]->ref=obj;
		ref->fields[slotId]->flag=10;//10代表引用
	}
}

void getField(Frame* frame,u1 opNum){

	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	int index=frame->readB2();

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//获取字段引用
	Field* f=resolveField(fr,false);//获取字段
	MethodAreaClass* fClass=f->Class;

	if(IsStatic(f->accessFlag)==1){
		//panic("java.lang.IncompatibleClassChangeError")
		return;
	}
	Object* ref=frame->popRef();

	if(ref==NULL){
		//java.lang.NullPointerException
		return;
	}

	char des=f->description[0];
	Slot** fields=ref->fields;
	int slotId=f->slotId;
	MethodAreaClass* c = f->Class;
	if(fields == NULL){
		frame->pushRef(NULL);
		return;
	}
	if(des=='Z' || des=='B' || des=='C' || des=='I' || des=='F'){
		int flag=fields[slotId]->flag;
		des=='F' ? flag=4 : flag=3;
		if(fields[slotId]->flag==NULL){
			fields[slotId]->num=0;
			fields[slotId]->flag=flag;
		}
		frame->opStack[frame->operandTop++]->num=fields[slotId]->num;
		frame->opStack[frame->operandTop-1]->flag=flag;
		return;
	}else if(des=='J' || des=='D'){
		frame->pushB8(fields[slotId+1]->num,fields[slotId]->num,des=='J' ? 5 : 6);
	}else if(des=='L' || des=='['){
		frame->pushRef(fields[slotId]->ref);
	}
}

//判断对象是否为other类的实例
bool isInstanceOf(Object* obj,MethodAreaClass* other,u1 opNum){
	MethodAreaClass* self=obj->objClass;
	if(self==other){//如果obj的类就是other，为真
		return true;
	}
	if(IsInterface(other->accessFlag)==0){//当other不是接口时
		for(MethodAreaClass* c=self->superClass;c!=NULL;c=c->superClass){//如果other是obj的(直接或间接的)超类，为真
			if(c==other)return true;
		}
		return false;//否则为假
	}
	if(IsInterface(other->accessFlag)==1){//如果other是接口，判断self是否直接或间接实现了other(self及其超类是否实现other接口)
		for(MethodAreaClass* c=self;c!=NULL;c=c->superClass){
			for(int i=0;i<c->interfacesLen;++i){
				if(c->interfaces[i]==other || isSubInterfaceOf(c->interfaces[i],other)==true){
					return true;
				}
			}
		}
	}
	return false;
}

void instanceof(Frame* frame,u1 opNum){
	
	int index=frame->readB2();

	Object* ref=frame->popRef();
	if(ref==NULL){
		frame->pushB4(0,3);
		return;
	}
	ConstPool* cp=frame->method->Class->cp;
	classRef* cRef=cp->consts[index]->ref->_classRef;
	MethodAreaClass* Class=resolveClass(cRef);

	if(isInstanceOf(ref,Class,0xc1)==true){
		frame->pushB4(1,3);
	}else{
		frame->pushB4(0,3);
	}
}

void checkcast(Frame* frame,u1 opNum){
	int index=frame->readB2();
	Slot* slot=frame->opStack[frame->operandTop];//注意！不是弹出引用！
	if((int)slot<=0 || (int)slot->ref<=0){
		//
		frame->popRef();
		frame->pushB4(0,3);
		return;
	}
	ConstPool* cp=frame->method->Class->cp;
	classRef* cRef=cp->consts[index]->ref->_classRef;
	MethodAreaClass* Class=resolveClass(cRef);
	if(isInstanceOf(slot->ref,Class,0xc1)==false){
		//java.lang.ClassCastException
		return;
	}
	
}

void invokeMethod(Frame* frame,Method* method){//调用方法

	Thread* t=frame->thisThread;
	Frame* newFrame=(Frame*)malloc(sizeof(Frame));//新建栈帧(方法)
	newFrame->newFrame(t,method);
	//参数传递
	if(method->argSlotCount>0){
		for(int i=method->argSlotCount-1;i>=0;--i){
			Slot* slot=frame->popSlot();
			newFrame->localVars[i]->num=slot->num;
			newFrame->localVars[i]->ref=slot->ref;
			newFrame->localVars[i]->flag=slot->flag;
			
		}
	}
	t->stack->pushFrame(newFrame);//推入栈顶
}

void invokeStatic(Frame* frame,u1 opNum){//调用静态方法

	int index=frame->readB2();

	methodRef* mRef=frame->method->Class->cp->consts[index]->ref->_methodRef;
	
	//该方法是否需要被hack。是，执行；否，返回
	if(hackMethod(frame,mRef)==true) return;
	if(mRef->cRef->Class==NULL){
		mRef->cRef->Class=resolveClass(mRef->cRef);
	}

	if(mRef->method==NULL){//如果方法引用为空
		mRef->method=resolveMethod(mRef,false);
	}

	if(IsStatic(mRef->method->accessFlag)==0){//非静态
		//panic("java.lang.IncompatibleClassChangeError")
		return;
	}
	MethodAreaClass* mClass=mRef->method->Class;
	if(mClass->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,mClass);
		return;
	}

	invokeMethod(frame,mRef->method);

}


//调用虚函数（即没有被private、static、native等关键字修饰的函数，或者说就是普通的成员函数）
void invokeVirtual(Frame* frame,u1 opNum){
	
	int index=frame->readB2();
	MethodAreaClass* currentClass=frame->method->Class;
	ConstPool* cp=frame->method->Class->cp;
	methodRef* mRef=cp->consts[index]->ref->_methodRef;

	if(mRef->cRef->Class==NULL){
		mRef->cRef->Class=resolveClass(mRef->cRef);
	}
	MethodAreaClass* resolvedClass=mRef->cRef->Class;

	if(mRef->method==NULL){
		mRef->method=resolveMethod(mRef,false);
	}
	Method* resolvedMethod=mRef->method;

	if(IsStatic(resolvedMethod->accessFlag)==true){
		//panic("java.lang.IncompatibleClassChangeError")
	}

	Slot* slot=frame->getRefFromTop(resolvedMethod->argSlotCount-1);

	if(slot==NULL){
		printError(0x013,NULL);
	}
	Object* ref=slot->ref;

	//该方法是否需要被hack。是，执行；否，返回
	if(hackMethod(frame,mRef)==true) return;

	if( IsProtected(resolvedMethod->accessFlag)==true && isSupClassOf(resolvedMethod->Class,currentClass)==true &&
		isEqual((u1*)getPackageName((char*)resolvedMethod->Class->name),(u1*)getPackageName((char*)currentClass->name))==false &&
		ref->objClass!=currentClass && isSupClassOf(ref->objClass,currentClass)==false){
			//panic("java.lang.IllegalAccessError")
			return;
	}

	//methodToBeInvoked 
	methodRef* MBIRef=(methodRef*)malloc(sizeof(methodRef));
	MBIRef->name=mRef->name;
	MBIRef->descriptor=mRef->descriptor;

	//现在我真的不知道ref->objClass，resolvedClass这两个类他妈哪个能解析出methodToBeInvoked，只好都试一试
	//A a = new B(); ref->objClass为类B,resolvedClass为类A 
	Method* methodToBeInvoked=NULL;

	//不能是接口或抽象类
	if(ref!=NULL && IsInterface(ref->objClass->accessFlag)==false && IsAbstract(ref->objClass->accessFlag)==false){
		methodToBeInvoked=lookupMethod(ref->objClass,MBIRef,false);//ref->objClass
	}
	if(methodToBeInvoked==NULL){
		methodToBeInvoked=lookupMethod(resolvedClass,MBIRef,false);//resolvedClass
	}

	invokeMethod(frame,methodToBeInvoked);
	free(MBIRef);
}

void invokeSpecial(Frame* frame,u1 opNum){//调用父类方法，私有方法，实例构造方法
	
	int index=frame->readB2();

	MethodAreaClass* currentClass=frame->method->Class;
	ConstPool* cp=frame->method->Class->cp;
	methodRef* mRef=cp->consts[index]->ref->_methodRef;

	if(mRef->cRef->Class==NULL){
		mRef->cRef->Class=resolveClass(mRef->cRef);
	}
	MethodAreaClass* resolvedClass=mRef->cRef->Class;

	//该方法是否需要被hack。是，执行；否，返回
	if(hackMethod(frame,mRef)==true) return;

	if(mRef->method==NULL){
		mRef->method=resolveMethod(mRef,false);
	}

	Method* resolvedMethod=mRef->method;

	//构造方法
	if(isEqual(resolvedMethod->name,(u1*)"<init>")==true && resolvedMethod->Class != resolvedClass){
		//panic("java.lang.NoSuchMethodError")
		return;
	}
	if(IsStatic(resolvedMethod->accessFlag)==1){
		//java.lang.IncompatibleClassChangeError
		return;
	}

	Object* ref=frame->getRefFromTop(resolvedMethod->argSlotCount-1)->ref;

	if(ref==NULL){//空指针异常
		//java.lang.NullPointerException
		return;
	}
	if(IsProtected(resolvedMethod->accessFlag)==true &&
	   isSupClassOf(resolvedMethod->Class,currentClass)==true &&
	   isEqual((u1*)getPackageName((char*)resolvedMethod->Class->name),(u1*)getPackageName((char*)currentClass->name))==false &&
	   ref->objClass!=currentClass &&
	   isSupClassOf(ref->objClass,currentClass)==false
	  ){
			//panic("java.lang.IllegalAccessError")
			return;
	}

	//如果调用的中超类中的函数，但不是构造函数，且当前类的ACC_SUPER标志被设置，需要一个额外的过程查找最终要调用的方法；
	//否则前面从方法符号引用中解析出来的方法就是要调用的方法。
	if(IsSuper(currentClass->accessFlag)==1 && isSupClassOf(resolvedClass,currentClass)==true){
		if(isEqual(resolvedMethod->name,(u1*)"<init>")==false){
			resolvedMethod=lookupMethod(currentClass->superClass,mRef,false);
		}
	}

	if(resolvedMethod==NULL || IsAbstract(resolvedMethod->accessFlag)==1){
		//panic("java.lang.AbstractMethodError")		
		return;
	}
	invokeMethod(frame,resolvedMethod);
}

//这个操作数后面跟着index(2字节),slotNum(1字节),other(1字节)
//index是常量池索引,slotNum等价于argSlotCount(由于历史原因留下来),other是Oracle的某些Java虚拟机实现用的，它的值必须是0
void invokeInterface(Frame* frame,u1 opNum){

	int index=frame->readB2();
	int slotNum=frame->readB1();
	int other=frame->readB1();
	
	MethodAreaClass* currentClass=frame->method->Class;
	ConstPool* cp=frame->method->Class->cp;

	//获取interfaceMethod的名称和方法
	interfaceMethodRef* iRef=cp->consts[index]->ref->_interfaceMethodRef;
	int classIndex=iRef->classIndex;
	int nameAndTypeIndex=iRef->nameAndTypeIndex;

	u1* cname=cp->consts[classIndex]->ref->_classRef->name;
	u1* val=cp->consts[nameAndTypeIndex]->val;

	int nameIndex=(val[0]<<8)+val[1];
	int typeIndex=(val[2]<<8)+val[3];

	methodRef* mRef=(methodRef*)malloc(sizeof(methodRef));
	mRef->name=cp->consts[nameIndex]->val;
	mRef->descriptor=cp->consts[typeIndex]->val;
	mRef->cRef=cp->consts[classIndex]->ref->_classRef;

	MethodAreaClass* mClass=resolveClass(mRef->cRef);

//	printf("{%s %s}",mRef->name,mRef->descriptor);
	mClass->mloader->showMethod();
	mRef->method=mClass->mloader->getMethod(mRef);

	Object* ref=frame->getRefFromTop(mRef->method->argSlotCount-1)->ref;

	MethodAreaClass* Class=ref->objClass;
	methodLoader* mloader=Class->mloader;


	if(ref==NULL){
		return;//panic("java.lang.NullPointerException") // todo
	}
	mRef->method=mloader->getMethod(mRef);
	Method* resolvedMethod=mRef->method;

	if(IsStatic(resolvedMethod->accessFlag)==1 && IsPrivate(resolvedMethod->accessFlag)==1){
		//panic("java.lang.IncompatibleClassChangeError")
		return;
	}
	mRef->cRef->Class=resolveClass(mRef->cRef);
	
	if(isImplements(ref->objClass,mRef->cRef->Class)==false){	
		return;//panic("java.lang.IncompatibleClassChangeError")
	}
	
	Method* methodToBeInvoked=lookupMethod(ref->objClass,mRef,false);
	if(methodToBeInvoked==NULL || IsAbstract(methodToBeInvoked->accessFlag)==1){
		return;//	panic("java.lang.AbstractMethodError")
	}

	invokeMethod(frame,methodToBeInvoked);
}

//基本类型数组
void newarray(Frame* frame,u1 opNum){

	int atype=frame->readB1();//读取数组类型
	int len=frame->opStack[--frame->operandTop]->num;//操作数栈弹出int，表示数组长度
	Heap* heap=frame->method->Class->loader->heap;
	Object* array=newPrimitiveTypesArray(frame->method->Class->loader,atype,len);	
	frame->pushRef(array);//将数组放入栈帧
}

//引用类型数组
void anewarray(Frame* frame,u1 opNum){

	int index=frame->readB2();//读取对象类型
	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;//获取类引用指针

	MethodAreaClass* Class=resolveClass(cf);
	string arrayClassName="[L";

	arrayClassName.append((char*)Class->name);
	arrayClassName.append(";");

	Object* array=newObject(loadClassByName(Class->loader,(u1*)arrayClassName.c_str()));
	array->array=(Array*)malloc(sizeof(Array));

	array->array->len=frame->opStack[--frame->operandTop]->num;//操作数栈弹出int，表示数组长度
	if(array->array->len<0){
		//panic("java.lang.NegativeArraySizeException")
		return;
	}

	array->array->type=10;

	array->array->objArray=(Object**)calloc(array->array->len,sizeof(Object*));
	for(int i=0;i<array->array->len;++i){
		array->array->objArray[i]=newObject(Class);
	}
	
	array->age=0;//年龄设置为0

	Heap* heap=frame->method->Class->loader->heap;
	frame->pushRef(array);//将数组放入栈帧
}

Object* newMultiDimensionalArray(int n,int dimensions,b4* counts,u1** arrClassNames,methodArea* loader){

	//创建数组
	int count=counts[n];
	Object* array=newObject(loadClassByName(loader,arrClassNames[dimensions-n-1]));
	
	array->array=(Array*)malloc(sizeof(Array));
	array->array->len=count;
	
	if(dimensions-n>1){
		array->array->type=10;
		Object** objArray=(Object**)calloc(count,sizeof(Object*));
		if(count>0){
			for(int i=0;i<count;++i){
				objArray[i]=newMultiDimensionalArray(n+1,dimensions,counts,arrClassNames,loader);
			}
			array->array->objArray=objArray;
		}
	}else{
		int i=0;
		b1* arrB1;
		b4* arrB4;
		b8* arrB8;
		switch(arrClassNames[dimensions-n-1][1]){
		case 'Z': case 'B':
			array->array->B1Array=(b1*)calloc(count,sizeof(b1));
			arrB1=array->array->B1Array;
			while(i<count){
				arrB1[i++]=0;
			}break;
		case 'S': case 'I' : case 'F' :
			array->array->B4Array=(b4*)calloc(count,sizeof(b4));break;
			arrB4=array->array->B4Array;
			while(i<count){
				arrB4[i++]=0;
			}break;
		case 'J' : case 'D' :
			array->array->B8Array=(b8*)calloc(count,sizeof(b8));break;
			arrB8=array->array->B8Array;
			while(i<count){
				arrB8[i++]=0;
			}break;
		default:
			array->array->type=10;
			array->array->objArray=(Object**)calloc(count,sizeof(Object*));			
		}
	}

	return array;
}

//建立多维数组
void multianewarray(Frame* frame,u1 opNum){
	int index=frame->readB2();//创建对象的类的索引
	int dimensions=frame->readB1();//维度

	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;
	
	u1* multiName=cf->name;//多维数组类名。如果创建三维int型数组,那么multiName=[[[I (I代表int)

	int nameLen=-1;
	while(multiName[++nameLen]!='\0');

	b4* counts=(b4*)calloc(dimensions,sizeof(b4));

	//多维数组的所有类名。比如new int[3][3][3](int型三维数组)。
	//那么应该有[[[I,[[I,[I这三个类名,分别代表3,2,1维int型数组类
	//注意！引用类型最后面要加上逗号！
	u1** arrClassNames=(u1**)calloc(dimensions+1,sizeof(u1*));
	int len;
	for(int i1=dimensions-1;i1>=0;--i1){
		counts[i1]=frame->popB4();
		len=i1+2+nameLen-dimensions;
		if(counts[i1]<0){
			printf("java.lang.NegativeArraySizeException");
		}
		arrClassNames[i1]=(u1*)calloc(len,sizeof(u1));
		int t=dimensions-i1-1;
		for(int i2=0;i2<len;++i2){
			arrClassNames[i1][i2]=multiName[t+i2];
		}
		if(arrClassNames[i1][1]=='L'){
			arrClassNames[i1][len]=';';
			arrClassNames[i1][len+1]='\0';
		}else{
			arrClassNames[i1][len]='\0';
		}
	}

	Object* multArray=newMultiDimensionalArray(0,dimensions,counts,arrClassNames,frame->method->Class->loader);

	frame->pushRef(multArray);

}

//获取数组长度
void arraylength(Frame* frame,u1 opNum){
	Object* ref = frame->popRef();
	int len = 0;
	if(ref!=NULL && ref->array != NULL){
		len=ref->array->len;
	}
	frame->opStack[frame->operandTop++]->num=len;
	frame->opStack[frame->operandTop-1]->flag=3;
}

//异常抛出指令（未完成）
void athrow(Frame* frame,u1 opNum){
	Object* ex=frame->popRef();//弹出异常对象
}

//加锁(未完成)
void monitorEnter(Frame* frame,u1 opNum){
}

//释放锁(未完成)
void monitorExit(Frame* frame,u1 opNum){
}

//往译码器里面注册Refernces系列指令
void regReferncesInsts(JInst* insts){

	insts[0xbb].func=_new;//注册_new指令
	insts[0xbc].func=newarray;//注册newarray
	insts[0xbd].func=anewarray;//注册anewarray
	insts[0xbe].func=arraylength;//注册newarray
	insts[0xb3].func=putStatic;//注册putStatic
	insts[0xb2].func=getStatic;//注册getStatic
	insts[0xb5].func=putField;//注册putField
	insts[0xb4].func=getField;//注册getField
	insts[0xc1].func=instanceof;//注册instanceof
	insts[0xc0].func=checkcast;//注册checkcast
	insts[0xb8].func=invokeStatic;//注册invokeStatic
	insts[0xb6].func=invokeVirtual;
	insts[0xb7].func=invokeSpecial;
	insts[0xb9].func=invokeInterface;
	insts[0xc5].func=multianewarray;
	insts[0xbf].func=athrow;
	insts[0xc2].func=monitorEnter;
	insts[0xc3].func=monitorExit;

}