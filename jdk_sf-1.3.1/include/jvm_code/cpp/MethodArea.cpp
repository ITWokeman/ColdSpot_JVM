#include "..\headers\methodArea.h"

//�ж�other�Ƿ�Ϊc����
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

//�ж�c�Ƿ�ʵ�ֽӿ�iface
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

//�����ò���-------------------------------------------------------------------------------------------------------------------------

//���d�������c�ࣺ
//c��Ȩ��Ϊpublic������
//c���d����ͬһ���������У�����
bool isAccessibleTo(MethodAreaClass* d,MethodAreaClass* c){//d�������c��

	//���c��Ȩ��
	if(IsPublic(c->accessFlag)==1){return true;}

	//���c��d����
	char* dName=getPackageName((char*)d->name);//d�İ���
	char* cName=getPackageName((char*)c->name);//c�İ���
	if(isEqual((u1*)dName,(u1*)cName)==true){return true;}

	return false;
}

//ͨ��������������
MethodAreaClass* loadClassByName(methodArea* loader,u1* name){
	string className=charArrayToString((char*)name);
	if(name[0]=='['){//����������
		return loadArrayClass(loader,className,false);
	}else{//������ͨ��
		string className=charArrayToString((char*)name);
		return loadNonArrayClass(loader,className,false);
	}
}

//ͨ���������ҵ���
MethodAreaClass* resolveClass(classRef* cRef){
	MethodAreaClass* d=cRef->cp->Class;
	string name=charArrayToString((char*)cRef->name);
	MethodAreaClass* c=d->loader->getLoadClass(name);
	if(c==NULL){
		return loadClassByName(d->loader,cRef->name);
	}else{
		//�����Ȩ����
		if(isAccessibleTo(d,c)==false){
			string info[]={name};
			printError(0x005,info);
		}
	}
	return c;
}

//�ֶ����ò���-----------------------------------------------------------------------------------------------------------------------

Field* lookupField(MethodAreaClass* c,fieldRef* fRef,bool istest){
	Field* field=c->floader->getField(fRef);//������c��Ѱ���ֶ�
	if(field!=NULL){
		if(istest==true){
			printf("[��%s�����ҵ��ֶ�%s!]\n",c->name,field->name);
		}
		return field;
	}
	for(int i=0;i<c->interfacesLen;++i){//�Ҳ�����������c�Ľӿ�
		field=c->interfaces[i]->floader->getField(fRef);
		if(field!=NULL){
			if(istest==true){
				printf("[��%s�ӿ����ҵ��ֶ�%s!]\n",c->name,field->name);
			}
			return field;
		}
	}	
	//����и���Ļ�
	if(c->superClass!=NULL){
		return lookupField(c->superClass,fRef,istest);
	}
	return NULL;

}

void resolveFieldRef(fieldRef* fRef,bool istest){
	
	MethodAreaClass* d=fRef->cp->Class;//Ҫ����fRef����d
	MethodAreaClass* c=resolveClass(fRef->cRef);//fRef���ڵ���
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

	if(fRef->field==NULL){//�������Ϊ��
		resolveFieldRef(fRef,istest);
	}
	return fRef->field;
}

//�������ò���-----------------------------------------------------------------------------------------------------------------------

Method* lookupMethod(MethodAreaClass* c,methodRef* mRef,bool istest){
	Method* method;

	method=c->mloader->getMethod(mRef);//������c��Ѱ���ֶ�

	if(method!=NULL){
		if(istest==true){
			printf("[��%s�����ҵ�����%s!]\n",c->name,method->name);
		}
		return method;
	}
	
	for(int i=0;i<c->interfacesLen;++i){//�Ҳ�����������c�Ľӿ�
		if(c->interfaces[i]!=NULL){
			method=c->interfaces[i]->mloader->getMethod(mRef);
		}
		if(method!=NULL){
			if(istest==true){
				printf("[��%s�ӿ����ҵ�����%s!]\n",c->name,mRef->name);
			}
			return method;
		}
	}
	//����и���Ļ�
	if(c->superClass!=NULL){
		return lookupMethod(c->superClass,mRef,istest);
	}
	return NULL;
}

void resolveMethodRef(methodRef* mRef,bool istest){

	MethodAreaClass* c=mRef->cRef->Class;
	if(mRef->cRef->Class==NULL){
		c=resolveClass(mRef->cRef);//mRef���ڵ���
	}
	if(IsInterface(c->accessFlag)==true){
		//panic("java.lang.IncompatibleClassChangeError")
	}
	Method* method=lookupMethod(c,mRef,istest);

	//��ʱ�����ַ����ǽӿڵĻ�������ˡ�û��Ҫ
	string info[]={charArrayToString((char*)mRef->name)};
	if(method==NULL){
		printError(0x008,info);return;
	}

	if(isEqual(mRef->name,(u1*)"main")==false){//����������
		MethodAreaClass* d=mRef->cp->Class;//Ҫ����mRef����d
		if(isAccessibleTo(d,c)==false){
			printError(0x009,info);return;
		}
	}
	mRef->method=method;
}

Method* resolveMethod(methodRef* mRef,bool istest){
	if(mRef->method==NULL){//�������Ϊ��
		resolveMethodRef(mRef,istest);
	}
	return mRef->method;
}
