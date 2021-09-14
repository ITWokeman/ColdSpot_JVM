/**
	����ָ�
**/
#pragma once//��ֹ�ظ�����
#include "..\headers\references.h"
#include "..\headers\Heap.h"
#include "..\headers\Thread.h"

/*
new����
����:Student a=new Student();
1.��ͨ��index�����ҵ�������ָ��
2.Ȼ��ͨ��ָ���ҵ�String����Ϣ��
3.�ж�һ��Student�Ƿ���ʵ����
4.��ȡStudent���Ը���n������n��Slot���Student�������Ϣ��
*/
void invokeMethod(Frame* frame,Method* method);

void _new(Frame* frame,u1 opNum){

	int index=frame->readB2();

	//ͨ��indexѰ��Ҫ�����������
	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;//��ȡ������ָ��
	
	if(cf==NULL){//����Ҳ�������
		string info[]={charArrayToString((char*)cf->name)};
		printError(0x011,info);
		return;
	}
	if(cf->Class==NULL){
		cf->Class=resolveClass(cf);//��ȡ����Ϣ
	}

	//�����û�г�ʼ������ʱ����취������Ƶ�constClassLoader����ڼ�����ɵ�ʱ��ͳ�ʼ����
	if(cf->Class->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,cf->Class);
		return;
	}

	Object* obj=newObject(cf->Class);

	Heap* heap=frame->method->Class->loader->heap;//��ȡ����
	frame->pushRef(obj);//���������ջ֡

}

//����̬������ֵ
void putStatic(Frame* frame,u1 opNum){

	int index=frame->readB2();

	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//��ȡ�ֶ�����
	Field* f=resolveField(fr,false);//��ȡ�ֶ�
	MethodAreaClass* fClass=f->Class;

	//�����û�г�ʼ������ʱ����취������Ƶ�constClassLoader����ڼ�����ɵ�ʱ��ͳ�ʼ����
	if(fClass->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,fClass);
		return;
	}

	if(IsStatic(f->accessFlag)==0){
		//����java.lang.IncompatibleClassChangeError
		return;
	}
	if(IsFinal(f->accessFlag)==1){
		char* clinit="<clinit>";
		if(Class!=fClass || isEqual(frame->method->name,(u1*)clinit)==0){
			//����java.lang.IllegalAccessError
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
		sv[slotId+1]->num=frame->popB4();//��λ
		sv[slotId]->num=frame->popB4();//��λ
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
		slot->flag=10;//10��������
	}
}

void getStatic(Frame* frame,u1 opNum){

	int index=frame->readB2();
	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//��ȡ�ֶ�����
	
	Field* f=resolveField(fr,false);//��ȡ�ֶ�
	MethodAreaClass* fClass=f->Class;
	
	//�����û�г�ʼ������ʱ����취������Ƶ�constClassLoader����ڼ�����ɵ�ʱ��ͳ�ʼ����
	if(fClass->InitStarted==false){
		frame->pc=frame->thisThread->pc;
		initClass(frame,frame->thisThread,fClass);
		return;
	}

	if(IsStatic(f->accessFlag)==false){
		//����java.lang.IncompatibleClassChangeError
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

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//��ȡ�ֶ�����
	Field* f=resolveField(fr,false);//��ȡ�ֶ�
	MethodAreaClass* fClass=f->Class;
	
	if(IsStatic(f->accessFlag)==true){
		//panic("java.lang.IncompatibleClassChangeError")
		return;
	}
	if(IsFinal(f->accessFlag)==true){
		char* init="<init>";
		if(Class!=fClass || isEqual(frame->method->name,(u1*)init)==false){
			//����java.lang.IllegalAccessError
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
		ref->fields[slotId]->flag=10;//10��������
	}
}

void getField(Frame* frame,u1 opNum){

	MethodAreaClass* Class=frame->method->Class;
	ConstPool* cp=Class->cp;

	int index=frame->readB2();

	fieldRef* fr=cp->consts[index]->ref->_fieldRef;//��ȡ�ֶ�����
	Field* f=resolveField(fr,false);//��ȡ�ֶ�
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

//�ж϶����Ƿ�Ϊother���ʵ��
bool isInstanceOf(Object* obj,MethodAreaClass* other,u1 opNum){
	MethodAreaClass* self=obj->objClass;
	if(self==other){//���obj�������other��Ϊ��
		return true;
	}
	if(IsInterface(other->accessFlag)==0){//��other���ǽӿ�ʱ
		for(MethodAreaClass* c=self->superClass;c!=NULL;c=c->superClass){//���other��obj��(ֱ�ӻ��ӵ�)���࣬Ϊ��
			if(c==other)return true;
		}
		return false;//����Ϊ��
	}
	if(IsInterface(other->accessFlag)==1){//���other�ǽӿڣ��ж�self�Ƿ�ֱ�ӻ���ʵ����other(self���䳬���Ƿ�ʵ��other�ӿ�)
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
	Slot* slot=frame->opStack[frame->operandTop];//ע�⣡���ǵ������ã�
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

void invokeMethod(Frame* frame,Method* method){//���÷���

	Thread* t=frame->thisThread;
	Frame* newFrame=(Frame*)malloc(sizeof(Frame));//�½�ջ֡(����)
	newFrame->newFrame(t,method);
	//��������
	if(method->argSlotCount>0){
		for(int i=method->argSlotCount-1;i>=0;--i){
			Slot* slot=frame->popSlot();
			newFrame->localVars[i]->num=slot->num;
			newFrame->localVars[i]->ref=slot->ref;
			newFrame->localVars[i]->flag=slot->flag;
			
		}
	}
	t->stack->pushFrame(newFrame);//����ջ��
}

void invokeStatic(Frame* frame,u1 opNum){//���þ�̬����

	int index=frame->readB2();

	methodRef* mRef=frame->method->Class->cp->consts[index]->ref->_methodRef;
	
	//�÷����Ƿ���Ҫ��hack���ǣ�ִ�У��񣬷���
	if(hackMethod(frame,mRef)==true) return;
	if(mRef->cRef->Class==NULL){
		mRef->cRef->Class=resolveClass(mRef->cRef);
	}

	if(mRef->method==NULL){//�����������Ϊ��
		mRef->method=resolveMethod(mRef,false);
	}

	if(IsStatic(mRef->method->accessFlag)==0){//�Ǿ�̬
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


//�����麯������û�б�private��static��native�ȹؼ������εĺ���������˵������ͨ�ĳ�Ա������
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

	//�÷����Ƿ���Ҫ��hack���ǣ�ִ�У��񣬷���
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

	//��������Ĳ�֪��ref->objClass��resolvedClass�������������ĸ��ܽ�����methodToBeInvoked��ֻ�ö���һ��
	//A a = new B(); ref->objClassΪ��B,resolvedClassΪ��A 
	Method* methodToBeInvoked=NULL;

	//�����ǽӿڻ������
	if(ref!=NULL && IsInterface(ref->objClass->accessFlag)==false && IsAbstract(ref->objClass->accessFlag)==false){
		methodToBeInvoked=lookupMethod(ref->objClass,MBIRef,false);//ref->objClass
	}
	if(methodToBeInvoked==NULL){
		methodToBeInvoked=lookupMethod(resolvedClass,MBIRef,false);//resolvedClass
	}

	invokeMethod(frame,methodToBeInvoked);
	free(MBIRef);
}

void invokeSpecial(Frame* frame,u1 opNum){//���ø��෽����˽�з�����ʵ�����췽��
	
	int index=frame->readB2();

	MethodAreaClass* currentClass=frame->method->Class;
	ConstPool* cp=frame->method->Class->cp;
	methodRef* mRef=cp->consts[index]->ref->_methodRef;

	if(mRef->cRef->Class==NULL){
		mRef->cRef->Class=resolveClass(mRef->cRef);
	}
	MethodAreaClass* resolvedClass=mRef->cRef->Class;

	//�÷����Ƿ���Ҫ��hack���ǣ�ִ�У��񣬷���
	if(hackMethod(frame,mRef)==true) return;

	if(mRef->method==NULL){
		mRef->method=resolveMethod(mRef,false);
	}

	Method* resolvedMethod=mRef->method;

	//���췽��
	if(isEqual(resolvedMethod->name,(u1*)"<init>")==true && resolvedMethod->Class != resolvedClass){
		//panic("java.lang.NoSuchMethodError")
		return;
	}
	if(IsStatic(resolvedMethod->accessFlag)==1){
		//java.lang.IncompatibleClassChangeError
		return;
	}

	Object* ref=frame->getRefFromTop(resolvedMethod->argSlotCount-1)->ref;

	if(ref==NULL){//��ָ���쳣
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

	//������õ��г����еĺ����������ǹ��캯�����ҵ�ǰ���ACC_SUPER��־�����ã���Ҫһ������Ĺ��̲�������Ҫ���õķ�����
	//����ǰ��ӷ������������н��������ķ�������Ҫ���õķ�����
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

//����������������index(2�ֽ�),slotNum(1�ֽ�),other(1�ֽ�)
//index�ǳ���������,slotNum�ȼ���argSlotCount(������ʷԭ��������),other��Oracle��ĳЩJava�����ʵ���õģ�����ֵ������0
void invokeInterface(Frame* frame,u1 opNum){

	int index=frame->readB2();
	int slotNum=frame->readB1();
	int other=frame->readB1();
	
	MethodAreaClass* currentClass=frame->method->Class;
	ConstPool* cp=frame->method->Class->cp;

	//��ȡinterfaceMethod�����ƺͷ���
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

//������������
void newarray(Frame* frame,u1 opNum){

	int atype=frame->readB1();//��ȡ��������
	int len=frame->opStack[--frame->operandTop]->num;//������ջ����int����ʾ���鳤��
	Heap* heap=frame->method->Class->loader->heap;
	Object* array=newPrimitiveTypesArray(frame->method->Class->loader,atype,len);	
	frame->pushRef(array);//���������ջ֡
}

//������������
void anewarray(Frame* frame,u1 opNum){

	int index=frame->readB2();//��ȡ��������
	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;//��ȡ������ָ��

	MethodAreaClass* Class=resolveClass(cf);
	string arrayClassName="[L";

	arrayClassName.append((char*)Class->name);
	arrayClassName.append(";");

	Object* array=newObject(loadClassByName(Class->loader,(u1*)arrayClassName.c_str()));
	array->array=(Array*)malloc(sizeof(Array));

	array->array->len=frame->opStack[--frame->operandTop]->num;//������ջ����int����ʾ���鳤��
	if(array->array->len<0){
		//panic("java.lang.NegativeArraySizeException")
		return;
	}

	array->array->type=10;

	array->array->objArray=(Object**)calloc(array->array->len,sizeof(Object*));
	for(int i=0;i<array->array->len;++i){
		array->array->objArray[i]=newObject(Class);
	}
	
	array->age=0;//��������Ϊ0

	Heap* heap=frame->method->Class->loader->heap;
	frame->pushRef(array);//���������ջ֡
}

Object* newMultiDimensionalArray(int n,int dimensions,b4* counts,u1** arrClassNames,methodArea* loader){

	//��������
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

//������ά����
void multianewarray(Frame* frame,u1 opNum){
	int index=frame->readB2();//����������������
	int dimensions=frame->readB1();//ά��

	classRef* cf=frame->method->Class->cp->consts[index]->ref->_classRef;
	
	u1* multiName=cf->name;//��ά�������������������άint������,��ômultiName=[[[I (I����int)

	int nameLen=-1;
	while(multiName[++nameLen]!='\0');

	b4* counts=(b4*)calloc(dimensions,sizeof(b4));

	//��ά�������������������new int[3][3][3](int����ά����)��
	//��ôӦ����[[[I,[[I,[I����������,�ֱ����3,2,1άint��������
	//ע�⣡�������������Ҫ���϶��ţ�
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

//��ȡ���鳤��
void arraylength(Frame* frame,u1 opNum){
	Object* ref = frame->popRef();
	int len = 0;
	if(ref!=NULL && ref->array != NULL){
		len=ref->array->len;
	}
	frame->opStack[frame->operandTop++]->num=len;
	frame->opStack[frame->operandTop-1]->flag=3;
}

//�쳣�׳�ָ�δ��ɣ�
void athrow(Frame* frame,u1 opNum){
	Object* ex=frame->popRef();//�����쳣����
}

//����(δ���)
void monitorEnter(Frame* frame,u1 opNum){
}

//�ͷ���(δ���)
void monitorExit(Frame* frame,u1 opNum){
}

//������������ע��Referncesϵ��ָ��
void regReferncesInsts(JInst* insts){

	insts[0xbb].func=_new;//ע��_newָ��
	insts[0xbc].func=newarray;//ע��newarray
	insts[0xbd].func=anewarray;//ע��anewarray
	insts[0xbe].func=arraylength;//ע��newarray
	insts[0xb3].func=putStatic;//ע��putStatic
	insts[0xb2].func=getStatic;//ע��getStatic
	insts[0xb5].func=putField;//ע��putField
	insts[0xb4].func=getField;//ע��getField
	insts[0xc1].func=instanceof;//ע��instanceof
	insts[0xc0].func=checkcast;//ע��checkcast
	insts[0xb8].func=invokeStatic;//ע��invokeStatic
	insts[0xb6].func=invokeVirtual;
	insts[0xb7].func=invokeSpecial;
	insts[0xb9].func=invokeInterface;
	insts[0xc5].func=multianewarray;
	insts[0xbf].func=athrow;
	insts[0xc2].func=monitorEnter;
	insts[0xc3].func=monitorExit;

}