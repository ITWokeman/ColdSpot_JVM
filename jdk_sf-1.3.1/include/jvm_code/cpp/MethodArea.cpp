#include "..\headers\methodArea.h"

//判断other是否为c超类
bool isSupClassOf(MethodAreaClass* c,MethodAreaClass* other){
	MethodAreaClass* f=c->superClass;
	while(f!=NULL){
		if(f==other){
			return true;
		}
		f=f->superClass;
	}
	return false;
}

// self extends iface
bool isSubInterfaceOf(MethodAreaClass* c,MethodAreaClass* iface){
	for(int i=0;i<c->interfacesLen;++i){
		if(c->interfaces[i]==iface || isSubInterfaceOf(c->interfaces[i],iface)==true){
			return true;
		}
	}
	return false;
}

//判断c是否实现接口iface
bool isImplements(MethodAreaClass* c,MethodAreaClass* iface){
	while(c->superClass!=NULL){
		for(int i=0;i<c->interfacesLen;++i){
			if(c->interfaces[i]==iface || isSubInterfaceOf(c->interfaces[i],iface)==true){
				return true;
			}
		}
		c=c->superClass;
	}
	return false;
}

//类引用查找-------------------------------------------------------------------------------------------------------------------------

//如果d类想访问c类：
//c类权限为public，允许；
//c类和d类在同一个包下运行，允许
bool isAccessibleTo(MethodAreaClass* d,MethodAreaClass* c){//d类想访问c类

	//检查c类权限
	if(IsPublic(c->accessFlag)==1){return true;}

	//检查c，d包名
	char* dName=getPackageName((char*)d->name);//d的包名
	char* cName=getPackageName((char*)c->name);//c的包名
	if(isEqual((u1*)dName,(u1*)cName)==true){return true;}

	return false;
}

//通过类名来加载类
MethodAreaClass* loadClassByName(methodArea* loader,u1* name){
	string className=charArrayToString((char*)name);
	if(name[0]=='['){//加载数组类
		return loadArrayClass(loader,className,false);
	}else{//加载普通类
		string className=charArrayToString((char*)name);
		return loadNonArrayClass(loader,className,false);
	}
}

//通过类引用找到类
MethodAreaClass* resolveClass(classRef* cRef){
	MethodAreaClass* d=cRef->cp->Class;
	string name=charArrayToString((char*)cRef->name);
	MethodAreaClass* c=d->loader->getLoadClass(name);
	if(c==NULL){
		return loadClassByName(d->loader,cRef->name);
	}else{
		//如果无权访问
		if(isAccessibleTo(d,c)==false){
			string info[]={name};
			printError(0x005,info);
		}
	}
	return c;
}

//字段引用查找-----------------------------------------------------------------------------------------------------------------------

Field* lookupField(MethodAreaClass* c,fieldRef* fRef,bool istest){
	Field* field=c->floader->getField(fRef);//先在类c下寻找字段
	if(field!=NULL){
		if(istest==true){
			printf("[在%s类下找到字段%s!]\n",c->name,field->name);
		}
		return field;
	}
	for(int i=0;i<c->interfacesLen;++i){//找不到，遍历类c的接口
		field=c->interfaces[i]->floader->getField(fRef);
		if(field!=NULL){
			if(istest==true){
				printf("[在%s接口下找到字段%s!]\n",c->name,field->name);
			}
			return field;
		}
	}	
	//如果有父类的话
	if(c->superClass!=NULL){
		return lookupField(c->superClass,fRef,istest);
	}
	return NULL;

}

void resolveFieldRef(fieldRef* fRef,bool istest){
	
	MethodAreaClass* d=fRef->cp->Class;//要访问fRef的类d
	MethodAreaClass* c=resolveClass(fRef->cRef);//fRef所在的类
	Field* field=lookupField(c,fRef,istest);
	string info[]={charArrayToString((char*)fRef->name)};
	if(field==NULL){
		printError(0x008,info);
	}
	if(isAccessibleTo(d,c)==false){
		printError(0x009,info);
	}
	fRef->field=field;

}

Field* resolveField(fieldRef* fRef,bool istest){

	if(fRef->field==NULL){//如果引用为空
		resolveFieldRef(fRef,istest);
	}
	return fRef->field;
}

//方法引用查找-----------------------------------------------------------------------------------------------------------------------

Method* lookupMethod(MethodAreaClass* c,methodRef* mRef,bool istest){
	Method* method;

	method=c->mloader->getMethod(mRef);//先在类c下寻找字段

	if(method!=NULL){
		if(istest==true){
			printf("[在%s类下找到方法%s!]\n",c->name,method->name);
		}
		return method;
	}
	
	for(int i=0;i<c->interfacesLen;++i){//找不到，遍历类c的接口
		if(c->interfaces[i]!=NULL){
			method=c->interfaces[i]->mloader->getMethod(mRef);
		}
		if(method!=NULL){
			if(istest==true){
				printf("[在%s接口下找到方法%s!]\n",c->name,mRef->name);
			}
			return method;
		}
	}
	//如果有父类的话
	if(c->superClass!=NULL){
		return lookupMethod(c->superClass,mRef,istest);
	}
	return NULL;
}

void resolveMethodRef(methodRef* mRef,bool istest){

	MethodAreaClass* c=mRef->cRef->Class;
	if(mRef->cRef->Class==NULL){
		c=resolveClass(mRef->cRef);//mRef所在的类
	}
	if(IsInterface(c->accessFlag)==true){
		//panic("java.lang.IncompatibleClassChangeError")
	}
	Method* method=lookupMethod(c,mRef,istest);

	//暂时不区分方法是接口的还是类的了。没必要
	string info[]={charArrayToString((char*)mRef->name)};
	if(method==NULL){
		printError(0x008,info);return;
	}

	if(isEqual(mRef->name,(u1*)"main")==false){//不是主方法
		MethodAreaClass* d=mRef->cp->Class;//要访问mRef的类d
		if(isAccessibleTo(d,c)==false){
			printError(0x009,info);return;
		}
	}
	mRef->method=method;
}

Method* resolveMethod(methodRef* mRef,bool istest){
	if(mRef->method==NULL){//如果引用为空
		resolveMethodRef(mRef,istest);
	}
	return mRef->method;
}
