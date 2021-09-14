
#include "..\headers\ClassLoadActuator.h"
#include "..\headers\methodArea.h"
#include "..\headers\Heap.h"

bool test=false;//是否测试模式

void initClass(Frame* frame,Thread* thread,MethodAreaClass* fClass);

MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn);

void initSuperClass(Frame* frame,Thread* thread,MethodAreaClass* fClass) {
	if(IsInterface(fClass->accessFlag)==false){
		MethodAreaClass* superClass=fClass->superClass;
		if( superClass != NULL){
			if(superClass->InitStarted==false){
				initClass(frame,thread,superClass);
			}
		}
	}
}

void initClass(Frame* frame,Thread* thread,MethodAreaClass* fClass){
	fClass->InitStarted=true;
	Method* clinit=fClass->mloader->getClinitMethod();
	if(clinit != NULL){
		if(clinit->Class == fClass){
			// exec <clinit>
			//该方法是否需要被hack。是，执行；否，返回
			methodRef* mRef = (methodRef*)malloc(sizeof(methodRef));
			mRef->name = clinit->name;
			mRef->cRef = (classRef*)malloc(sizeof(classRef));
			mRef->cRef->name = clinit->Class->name;
			if(hackMethod(frame,mRef)==false){ 
				invokeMethod(frame,clinit);
			}
			
		}
	}
	//初始化父类
	initSuperClass(frame,thread,fClass);
}

//实例字段数计算
void calcInstanceFieldSlotIds(MethodAreaClass* Class){

	int slotId=0;	

	//如果有父类的话
	if(IsInterface(Class->accessFlag)==false && Class->superClass!=NULL){
		slotId=Class->superClass->instanceSlotCount;
		slotId < 0 ? slotId = 0 : slotId=slotId;
	}

	map<string,Field*>::iterator mit;//方法区迭代器

	map<string,Field*> fieldMap=Class->floader->getFieldMap();

	for(mit = fieldMap.begin();mit!=fieldMap.end();mit++){
		Field* field = mit->second;
		if(IsStatic(field->accessFlag)==0){
			field->slotId=slotId++;
			if(field->description[0]=='D' || field->description[0]=='J'){
				++slotId;
			}
		}
	}

	Class->instanceSlotCount=slotId;
}

//静态字段数计算
void calcStaticFieldSlotIds(MethodAreaClass* Class){

	int slotId=0;
	char* obj="java/lang/Object";

	if(Class->fieldsLen<=0)return;//字段区大小为0

	map<string,Field*>::iterator mit;//方法区迭代器

	map<string,Field*> fieldMap=Class->floader->getFieldMap();

	for(mit = fieldMap.begin();mit!=fieldMap.end();mit++){
		Field* field = mit->second;
		if(IsStatic(field->accessFlag)==1){
			field->slotId=slotId++;
			if(field->description[0]=='D' || field->description[0]=='J'){
				++slotId;
			}
		}
	}

	Class->staticSlotCount=slotId;

}

//初始化静态final变量(final static)
void initStaticFinalVar(MethodAreaClass* Class,Field* field){
	
	Slot** vars=Class->staticVars;
	ConstPool* cp=Class->cp;
	u2 cpIndex=field->constValueIndex;
	u2 slotId=field->slotId;

	if(field->constValueIndex > 0){
		char des=field->description[0];
		if(des=='Z' || des=='B' || des=='C' || des=='S' || des=='I'){//加载int
			u1* var=cp->consts[field->constValueIndex]->val;
			vars[slotId]->num=toB4(var);
			vars[slotId]->flag=3;
		}
		if(des=='J'){//加载long
			b4* val=eightB1ToTwoB4(cp->consts[cpIndex]->val);
			vars[slotId]->num=val[0];
			vars[slotId+1]=(Slot*)malloc(sizeof(Slot));
			vars[slotId+1]->num=val[1];
		}
		if(des=='F'){//加载float
			b4 val = toB4(cp->consts[cpIndex]->val);
			vars[slotId]->num=val;
		}
		if(des=='D'){//加载double
			b4* val=eightB1ToTwoB4(cp->consts[cpIndex]->val);
			vars[slotId]->num=val[0];
			vars[slotId+1]=(Slot*)malloc(sizeof(Slot));
			vars[slotId+1]->num=val[1];
		}
		if(des=='L'){
			if(isEqual((u1*)"Ljava/lang/String;",(u1*)field->description)){//"Ljava/lang/String;"
				b1* val = (b1*)cp->consts[field->constValueIndex]->val;
				stringPool* strPool = Class->loader->heap->getStrPool();
				vars[slotId]->flag = 10;
				vars[slotId]->ref = strPool->JString(val);
			}
			return;
		}
	}

}

//分配并初始化静态变量
void allocAndInitStaticVars(MethodAreaClass* Class){

	if(Class==NULL)return;

	if(Class->fieldsLen<=0)return;//字段区大小为0

	Class->staticVars=(Slot**)calloc(Class->staticSlotCount,sizeof(Slot*));

	for(int i=0;i<Class->staticSlotCount;++i){
		Class->staticVars[i] = (Slot*)malloc(sizeof(Slot));
		Class->staticVars[i]->flag = 0;
		Class->staticVars[i]->num = 0;
		Class->staticVars[i]->ref = NULL;
	}

	map<string,Field*>::iterator mit;//方法区迭代器

	map<string,Field*> fieldMap=Class->floader->getFieldMap();

	for(mit = fieldMap.begin();mit!=fieldMap.end();mit++){
		Field* field = mit->second;
		u2 accessFlag=field->accessFlag;
		if(IsStatic(accessFlag)==true && IsFinal(accessFlag)==true){
			initStaticFinalVar(Class,field);
		}
		
	}
	
}

//准备
void prepare(MethodAreaClass* Class){
	Class->instanceSlotCount=0;
	Class->staticSlotCount=0;
	calcInstanceFieldSlotIds(Class);//实例字段数计算
	calcStaticFieldSlotIds(Class);////静态字段数计算
	allocAndInitStaticVars(Class);//分配并初始化静态变量
}

//验证
void verify(MethodAreaClass* mac){
	//todo
}

//加载超类
void resolveSuperClass(MethodAreaClass* mac){
	char* obj="java/lang/Object";
	if(isEqual((u1*)obj,mac->name)==0){
		string macSuper=charArrayToString((char*)mac->superClassName);
		if(test==true){
			printf("\n[连接%s类父类]...\n",mac->name);
		}
		mac->superClass=loadNonArrayClass(mac->loader,macSuper,test);
	}
}


//解析符号引用。符号引用换为直接引用
//其实这里是附和jvm类加载机制的。
//其实使用懒加载，即当用到该符号时再解析比较好
//因为这样可以按需解析，不用解析一堆可能用不到的引用，能提升效率
void symbolResolve(ConstPool* cp){

	if(cp == NULL) return;

	methodArea* cloader = cp->Class->loader;
	string cname;

	/**
	符号引用转换为直接引用:
	将classRef、fieldRef、methodRef这些符号引用解析为直接引用
	比如对于类引用classRef* a，解析a引用的类A，然后令a->Class = A

	使用部分解析:
	只有符号引用的类在方法区时才解析该符号引用
	比如方法引用methodRef* b，它所属的类为B，如果B不在方法区里面，就不解析方法引用B
	为啥？
	因为如果解析b的话，势必会解析类B、然后就会加载类B、解析类B的符号引用。
	如果类B的某个符号引用的类C不在方法区的话，又会触发类C的解析，形成套娃
	加载了一堆可能用不到的东西，降低jvm的运行效率，占用jvm的内存！
	**/
	

	for(int i=0;i<cp->cpSize;++i){
		Constant* constant = cp->consts[i];
		if(constant != NULL){
			switch(constant->type){
			case 7://Class
				cname = (char*)constant->ref->_classRef->name;
				if( cloader->getLoadClass(cname) != NULL){
					constant->ref->_classRef->Class = resolveClass(constant->ref->_classRef);
				}
				break;
			case 9://Field
				cname = (char*)constant->ref->_fieldRef->cRef->name;
				if(cloader->getLoadClass(cname) != NULL){
					constant->ref->_fieldRef->field = resolveField(constant->ref->_fieldRef,false);
				}
				break;
			case 10://Method
				cname = (char*)constant->ref->_methodRef->cRef->name;
				if(cloader->getLoadClass(cname) != NULL){
					constant->ref->_methodRef->method = resolveMethod(constant->ref->_methodRef,false);
				}	
			}
		}
	}
}

//加载实现接口
void resolveInterfaces(MethodAreaClass* mac){

	int len=mac->interfacesLen;
	if(len>0){
		mac->interfaces=(MethodAreaClass**)calloc(len,sizeof(MethodAreaClass*));
		for(int i=0;i<len;++i){
			string interfaceName=charArrayToString((char*)mac->interfacesNames[i]);
			mac->interfaces[i]=loadNonArrayClass(mac->loader,interfaceName,test);
			if(test==true){
				printf("[加载接口:%s",interfaceName.c_str());
				if(mac->interfaces[i]==NULL){
					printf("失败!]");
				}else{
					printf("成功!]");
				}
			}
		}
	}

}

void resolveInterfaceAndSuper(MethodAreaClass* Class){
	if(IsInterface(Class->accessFlag)==false){
		resolveSuperClass(Class);//加载mac父类
	}
	resolveInterfaces(Class);//加载mac实现接口
	Class->loader->loadClass(Class);//将mac加入到类加载器
}

//加载普通类
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest){
	MethodAreaClass* mac=loader->getLoadClass(cn);
	
	if(mac!=NULL){
		if(isTest==true){
			printf("\n[通过类加载器在方法区找到 %s 的类信息!]\n",cn.c_str());
		}
		return mac;
	}else{
		if(isTest==true){
			printf("\n[类加载器无法在方法区找到 %s 的类信息!]\n",cn.c_str());
		}
	}


	//1.加载----------------------------------------------------------------------------------------

	//委托.CLASS类文件加载器加载.class文件
	FileData* fd=classFileLoader(cn,isTest);
	if(fd==NULL)return NULL;
	//委托.CLASS文件解析器将.class文件字节码内容转换成类结构体
	mac=byteCodeDecoder(fd,loader);
	
	if(isTest==true){
		printf("\n[通过 %s.class 文件加载类信息成功!]\n",mac->name);
	}

	//1.5加载父类和实现接口-------------------------------------------------------------------------
	resolveInterfaceAndSuper(mac);

	//2.验证----------------------------------------------------------------------------------------
	verify(mac);

	//3.准备----------------------------------------------------------------------------------------
	prepare(mac);

	//4.解析----------------------------------------------------------------------------------------
	symbolResolve(mac->cp);
	
	return mac;
}

//加载数组类
MethodAreaClass* loadArrayClass(methodArea* loader,string cn,bool isTest){

	//必须拷贝！不然字符串cn在函数执行完后被回收，里面的内容也没有了！
	u1* cname=(u1*)calloc(cn.length()+1,sizeof(u1));
	for(int i=0;i<cn.length();++i){
		cname[i]=cn.c_str()[i];
	}
	cname[cn.length()]='\0';

	MethodAreaClass* arrayClass=(MethodAreaClass*)malloc(sizeof(MethodAreaClass));
	arrayClass->accessFlag=0x0001;//ACC_PUBLIC
	arrayClass->name=cname;
	arrayClass->loader=loader;
	arrayClass->instanceSlotCount=0;
	arrayClass->InitStarted=true;
	arrayClass->superClass=loader->getLoadClass("java/lang/Object");
	arrayClass->interfacesLen=2;
	arrayClass->interfaces=(MethodAreaClass**)calloc(2,sizeof(MethodAreaClass*));
	arrayClass->interfaces[0]=loadNonArrayClass(loader,"java/lang/Cloneable",false);
	arrayClass->interfaces[1]=loadNonArrayClass(loader,"java/io/Serializable",false);
	arrayClass->mloader=new methodLoader(arrayClass);
	arrayClass->floader=new fieldLoader(arrayClass);
	loader->loadClass(arrayClass);
	return arrayClass;
}


//加载基本所有类（基本类型类没有父类）
void loadPrimitiveClasses(methodArea* loader,MethodAreaClass* jlClass){

	//加载基本类型类(char void boolean short int long byte float double) 没有超类，没有接口
	string pClassName[9]={"char","void","boolean","short","int","long","byte","float","double"};
	int len;
	for(int i=0;i<9;++i){
		MethodAreaClass* pClass=(MethodAreaClass*)malloc(sizeof(MethodAreaClass));
		//设置基本类型类名字
		len=pClassName[i].length();
		u1* pcn=(u1*)pClassName[i].c_str();
		pClass->name=(u1*)calloc(len+1,sizeof(u1));
		for(int j=0;j<len;++j){
			pClass->name[j]=pcn[j];
		}		
		pClass->name[len]='\0';
		pClass->accessFlag=0x0001;	//ACC_PUBLIC
		pClass->loader=loader;
		pClass->instanceSlotCount=0;
		pClass->InitStarted=true;
		pClass->superClass=NULL;
		pClass->interfacesLen=0;
		pClass->interfaces=NULL;
		pClass->mloader=new methodLoader(pClass);
		pClass->floader=new fieldLoader(pClass);
		loader->loadClass(pClass);
	}
}

