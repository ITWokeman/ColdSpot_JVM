
#include "..\headers\ClassLoadActuator.h"
#include "..\headers\methodArea.h"
#include "..\headers\Heap.h"

bool test=false;//�Ƿ����ģʽ

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
			//�÷����Ƿ���Ҫ��hack���ǣ�ִ�У��񣬷���
			methodRef* mRef = (methodRef*)malloc(sizeof(methodRef));
			mRef->name = clinit->name;
			mRef->cRef = (classRef*)malloc(sizeof(classRef));
			mRef->cRef->name = clinit->Class->name;
			if(hackMethod(frame,mRef)==false){ 
				invokeMethod(frame,clinit);
			}
			
		}
	}
	//��ʼ������
	initSuperClass(frame,thread,fClass);
}

//ʵ���ֶ�������
void calcInstanceFieldSlotIds(MethodAreaClass* Class){

	int slotId=0;	

	//����и���Ļ�
	if(IsInterface(Class->accessFlag)==false && Class->superClass!=NULL){
		slotId=Class->superClass->instanceSlotCount;
		slotId < 0 ? slotId = 0 : slotId=slotId;
	}

	map<string,Field*>::iterator mit;//������������

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

//��̬�ֶ�������
void calcStaticFieldSlotIds(MethodAreaClass* Class){

	int slotId=0;
	char* obj="java/lang/Object";

	if(Class->fieldsLen<=0)return;//�ֶ�����СΪ0

	map<string,Field*>::iterator mit;//������������

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

//��ʼ����̬final����(final static)
void initStaticFinalVar(MethodAreaClass* Class,Field* field){
	
	Slot** vars=Class->staticVars;
	ConstPool* cp=Class->cp;
	u2 cpIndex=field->constValueIndex;
	u2 slotId=field->slotId;

	if(field->constValueIndex > 0){
		char des=field->description[0];
		if(des=='Z' || des=='B' || des=='C' || des=='S' || des=='I'){//����int
			u1* var=cp->consts[field->constValueIndex]->val;
			vars[slotId]->num=toB4(var);
			vars[slotId]->flag=3;
		}
		if(des=='J'){//����long
			b4* val=eightB1ToTwoB4(cp->consts[cpIndex]->val);
			vars[slotId]->num=val[0];
			vars[slotId+1]=(Slot*)malloc(sizeof(Slot));
			vars[slotId+1]->num=val[1];
		}
		if(des=='F'){//����float
			b4 val = toB4(cp->consts[cpIndex]->val);
			vars[slotId]->num=val;
		}
		if(des=='D'){//����double
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

//���䲢��ʼ����̬����
void allocAndInitStaticVars(MethodAreaClass* Class){

	if(Class==NULL)return;

	if(Class->fieldsLen<=0)return;//�ֶ�����СΪ0

	Class->staticVars=(Slot**)calloc(Class->staticSlotCount,sizeof(Slot*));

	for(int i=0;i<Class->staticSlotCount;++i){
		Class->staticVars[i] = (Slot*)malloc(sizeof(Slot));
		Class->staticVars[i]->flag = 0;
		Class->staticVars[i]->num = 0;
		Class->staticVars[i]->ref = NULL;
	}

	map<string,Field*>::iterator mit;//������������

	map<string,Field*> fieldMap=Class->floader->getFieldMap();

	for(mit = fieldMap.begin();mit!=fieldMap.end();mit++){
		Field* field = mit->second;
		u2 accessFlag=field->accessFlag;
		if(IsStatic(accessFlag)==true && IsFinal(accessFlag)==true){
			initStaticFinalVar(Class,field);
		}
		
	}
	
}

//׼��
void prepare(MethodAreaClass* Class){
	Class->instanceSlotCount=0;
	Class->staticSlotCount=0;
	calcInstanceFieldSlotIds(Class);//ʵ���ֶ�������
	calcStaticFieldSlotIds(Class);////��̬�ֶ�������
	allocAndInitStaticVars(Class);//���䲢��ʼ����̬����
}

//��֤
void verify(MethodAreaClass* mac){
	//todo
}

//���س���
void resolveSuperClass(MethodAreaClass* mac){
	char* obj="java/lang/Object";
	if(isEqual((u1*)obj,mac->name)==0){
		string macSuper=charArrayToString((char*)mac->superClassName);
		if(test==true){
			printf("\n[����%s�ุ��]...\n",mac->name);
		}
		mac->superClass=loadNonArrayClass(mac->loader,macSuper,test);
	}
}


//�����������á��������û�Ϊֱ������
//��ʵ�����Ǹ���jvm����ػ��Ƶġ�
//��ʵʹ�������أ������õ��÷���ʱ�ٽ����ȽϺ�
//��Ϊ�������԰�����������ý���һ�ѿ����ò��������ã�������Ч��
void symbolResolve(ConstPool* cp){

	if(cp == NULL) return;

	methodArea* cloader = cp->Class->loader;
	string cname;

	/**
	��������ת��Ϊֱ������:
	��classRef��fieldRef��methodRef��Щ�������ý���Ϊֱ������
	�������������classRef* a������a���õ���A��Ȼ����a->Class = A

	ʹ�ò��ֽ���:
	ֻ�з������õ����ڷ�����ʱ�Ž����÷�������
	���緽������methodRef* b������������ΪB�����B���ڷ��������棬�Ͳ�������������B
	Ϊɶ��
	��Ϊ�������b�Ļ����Ʊػ������B��Ȼ��ͻ������B��������B�ķ������á�
	�����B��ĳ���������õ���C���ڷ������Ļ����ֻᴥ����C�Ľ������γ�����
	������һ�ѿ����ò����Ķ���������jvm������Ч�ʣ�ռ��jvm���ڴ棡
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

//����ʵ�ֽӿ�
void resolveInterfaces(MethodAreaClass* mac){

	int len=mac->interfacesLen;
	if(len>0){
		mac->interfaces=(MethodAreaClass**)calloc(len,sizeof(MethodAreaClass*));
		for(int i=0;i<len;++i){
			string interfaceName=charArrayToString((char*)mac->interfacesNames[i]);
			mac->interfaces[i]=loadNonArrayClass(mac->loader,interfaceName,test);
			if(test==true){
				printf("[���ؽӿ�:%s",interfaceName.c_str());
				if(mac->interfaces[i]==NULL){
					printf("ʧ��!]");
				}else{
					printf("�ɹ�!]");
				}
			}
		}
	}

}

void resolveInterfaceAndSuper(MethodAreaClass* Class){
	if(IsInterface(Class->accessFlag)==false){
		resolveSuperClass(Class);//����mac����
	}
	resolveInterfaces(Class);//����macʵ�ֽӿ�
	Class->loader->loadClass(Class);//��mac���뵽�������
}

//������ͨ��
MethodAreaClass* loadNonArrayClass(methodArea* loader,string cn,bool isTest){
	MethodAreaClass* mac=loader->getLoadClass(cn);
	
	if(mac!=NULL){
		if(isTest==true){
			printf("\n[ͨ����������ڷ������ҵ� %s ������Ϣ!]\n",cn.c_str());
		}
		return mac;
	}else{
		if(isTest==true){
			printf("\n[��������޷��ڷ������ҵ� %s ������Ϣ!]\n",cn.c_str());
		}
	}


	//1.����----------------------------------------------------------------------------------------

	//ί��.CLASS���ļ�����������.class�ļ�
	FileData* fd=classFileLoader(cn,isTest);
	if(fd==NULL)return NULL;
	//ί��.CLASS�ļ���������.class�ļ��ֽ�������ת������ṹ��
	mac=byteCodeDecoder(fd,loader);
	
	if(isTest==true){
		printf("\n[ͨ�� %s.class �ļ���������Ϣ�ɹ�!]\n",mac->name);
	}

	//1.5���ظ����ʵ�ֽӿ�-------------------------------------------------------------------------
	resolveInterfaceAndSuper(mac);

	//2.��֤----------------------------------------------------------------------------------------
	verify(mac);

	//3.׼��----------------------------------------------------------------------------------------
	prepare(mac);

	//4.����----------------------------------------------------------------------------------------
	symbolResolve(mac->cp);
	
	return mac;
}

//����������
MethodAreaClass* loadArrayClass(methodArea* loader,string cn,bool isTest){

	//���뿽������Ȼ�ַ���cn�ں���ִ����󱻻��գ����������Ҳû���ˣ�
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


//���ػ��������ࣨ����������û�и��ࣩ
void loadPrimitiveClasses(methodArea* loader,MethodAreaClass* jlClass){

	//���ػ���������(char void boolean short int long byte float double) û�г��࣬û�нӿ�
	string pClassName[9]={"char","void","boolean","short","int","long","byte","float","double"};
	int len;
	for(int i=0;i<9;++i){
		MethodAreaClass* pClass=(MethodAreaClass*)malloc(sizeof(MethodAreaClass));
		//���û�������������
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

