/**
 .class�ļ�������	

   1 ����
     (1) .class�ļ��ֽ������

   2 �����ṩ����	
     (1) DecodeClass
	 ��.class�ļ��ֽ�������methodArea�ṹ��

**/
#pragma once//��ֹ�ظ�����
#include "..\headers\VMTools.h"
#include "..\headers\ClassFileDecoder.h"

//��������������
//����,�ṹ���vector�������,ֻ����string������һ��
void parseDes(Method* method){
	u1* des=method->description;
//	printf("\ndes: %s name:%s \n",des,method->name);
	vector<string> vd;
	int i=1;
	method->argSlotCount=0;

	while(des[i]!=')' && des[i]!='\0'){
		if(des[i]=='B' || des[i]=='C' || des[i]=='I' || des[i]=='F' || des[i]=='D' || des[i]=='J' || des[i]=='Z'){
			string d;
			d.append(1,des[i]);
			method->argSlotCount++;
			vd.push_back(d);
			if(des[i]=='D' || des[i]=='J'){
				method->argSlotCount++;
			}
		}else if(des[i]=='L'){
			string d;
			method->argSlotCount++;
			while(des[i]!=')' && des[i]!=';'){
				d.append(1,des[i++]);
			}
			--i;
			vd.push_back(d);
		}else if(des[i]=='['){
			string d;
			method->argSlotCount++;
			while(des[i]=='['){
				d.append(1,des[i++]);
			}
			if(des[i]=='B' || des[i]=='C' || des[i]=='I' || des[i]=='F' || des[i]=='D' || des[i]=='J' || des[i]=='Z'){
				d.append(1,des[i]);
				vd.push_back(d);
			}else if(des[i]=='L'){
				while(des[i]!=')' && des[i]!=';'){
					d.append(1,des[i++]);
				}
				--i;
				vd.push_back(d);
			}
		}
		++i;
	}
	string d;
	while(des[++i]!='\0'){
		d.append(1,des[i]);
	}
	vd.push_back(d);

	if(IsStatic(method->accessFlag)==0){
		method->argSlotCount++;//this����(���췽������)
	}

	method->des=(string*)calloc(vd.size(),sizeof(string));
	method->desNum=vd.size();
	for(int j=0;j<vd.size();++j){
		method->des[j]=vd[j];
	}
	vd.clear();
}

ConstPool* constPoolDecoder(FileData *d){//�����ؽ�����ע�⣬Ҫ�Ƚ����������ٽ������á���������ص��ֽ����Ȼ��棩
	
	ConstPool* constPool=(ConstPool*)malloc(sizeof(ConstPool));
	constPool->cpSize=d->readU2();//�����ش�С��������������1��ʼ��
	constPool->consts=(Constant**)calloc(constPool->cpSize,sizeof(Constant*));
	int type,index,len=constPool->cpSize;
	u1* val;
	classRef* cRef;
	u1** buffers=(u1**)calloc(len,sizeof(u1*));
	char* bufferTypes=(char*)calloc(len,sizeof(char));
	for(int i1=1;i1<len;++i1){//�������������������õ�������
		Constant* constant = (Constant*)malloc(sizeof(Constant));
		constant->type = d->readU1();
		switch(constant->type){
		case 1://����utf8
			constant->val = d->readBytes(d->readU2());
			constPool->consts[i1] = constant;
			break;
		case 3: case 4://����Integer��Float��
			constant->val = d->readBytes(4);
			constPool->consts[i1] = constant;
			break;
		case 5: case 6: //����Long��Double����Ҫռ��λ
			constant->val = d->readBytes(4);
			constPool->consts[i1] = constant;
			constPool->consts[++i1] = (Constant*)malloc(sizeof(Constant));
			constPool->consts[i1]->val = d->readBytes(4);
			break;

		//�����ã��ַ����á��Ȼ�����������Ϊutf8��Integer��Float��Long��Double��������û������֮ǰ���޷�������Щ��
		//�����������е����⣬������֮ǰ���ֶ����á��������á��ӿڷ�������Ҫ�õ��������ȴ���
		case 7: constant->ref=(Ref*)malloc(sizeof(Ref));
				constant->ref->_classRef=(classRef*)malloc(sizeof(classRef));
		case 8:
			buffers[i1]=d->readBytes(2);
			bufferTypes[i1]=constant->type;
			constPool->consts[i1]=constant;
			break;
		case 9: case 10: case 11: //�ֶ����á��������á��ӿڷ������á�ͬ�ϣ�����
			buffers[i1]=d->readBytes(4);
			bufferTypes[i1]=constant->type;
			constPool->consts[i1]=constant;
			break;
		case 12://���ֺ���������
			constant->val=d->readBytes(4);
			constPool->consts[i1]=constant;
			break;
		case 15://������ò������Ȳ��ܣ�����
			d->readU1();d->readU2();
			break;
		case 16://��־������Ҳ����
			d->readU2();
			break;
		case 18://��̬���õ㡣������
			d->readU2();d->readU2();
			break;
		}
	}

	for(int i2=1;i2<len;++i2){//��������
		Constant* constant = constPool->consts[i2];
		switch(bufferTypes[i2]){
			
		case 7://����������
			cRef=constant->ref->_classRef;
			cRef->cp=constPool;
			val=buffers[i2];

			//����������
			index=(val[0]<<8)+val[1];
			cRef->name=constPool->consts[index]->val;
			cRef->Class=NULL;
			constPool->consts[i2]=constant;
			break;
		case 8://�����ַ���
			constPool->consts[i2]->val=buffers[i2];
			break;
		case 9://�����ֶ�����
		{
			constant->ref = (Ref*)malloc(sizeof(Ref));
			fieldRef* fRef = (fieldRef*)malloc(sizeof(fieldRef));
			fRef->cp=constPool;

			//copyMemberRefInfo
			val=buffers[i2];
			index=(buffers[i2][0]<<8)+buffers[i2][1];//�ֶ��������ڳ����ص�����
			fRef->cRef=constPool->consts[index]->ref->_classRef;//�ֶ��������������

			index=(val[2]<<8)+val[3];//�ֶ�������������
			val=constPool->consts[index]->val;
			
			fRef->name=constPool->consts[(val[0]<<8)+val[1]]->val;//�ֶ�����
			fRef->descriptor=constPool->consts[(val[2]<<8)+val[3] ]->val;//����
			fRef->field=NULL;
			constant->ref->_fieldRef=fRef;
			constPool->consts[i2]=constant;
			break;
		}
		case 10://������������
		{
			constant->ref=(Ref*)malloc(sizeof(Ref));
			methodRef* mRef=(methodRef*)malloc(sizeof(methodRef));
			mRef->cp=constPool;

			//copyMemberRefInfo
			index=(buffers[i2][0]<<8)+buffers[i2][1];//�����������ڳ����ص�����
			mRef->cRef=constPool->consts[index]->ref->_classRef;//�����������������
			index=(buffers[i2][2]<<8)+buffers[i2][3];//����������������
			val=constPool->consts[index]->val;
			mRef->name=constPool->consts[ (val[0]<<8)+val[1] ]->val;//������
			mRef->descriptor=constPool->consts[ (val[2]<<8)+val[3] ]->val;//�������ԣ��ֶ����ͣ�
			mRef->method=NULL;
			constant->ref->_methodRef=mRef;
			constPool->consts[i2]=constant;
			break;
		}
		case 11://�����ӿڷ�������
		{
			constant->ref=(Ref*)malloc(sizeof(Ref));
			interfaceMethodRef* iRef=(interfaceMethodRef*)malloc(sizeof(interfaceMethodRef));
			iRef->classIndex=(buffers[i2][0]<<8)+buffers[i2][1];
			iRef->nameAndTypeIndex=(buffers[i2][2]<<8)+buffers[i2][3];
			constant->ref->_interfaceMethodRef=iRef;
			constPool->consts[i2]=constant;

			methodRef* mRef=(methodRef*)malloc(sizeof(methodRef));
			mRef->cp=constPool;
			mRef->cRef=constPool->consts[ iRef->classIndex ]->ref->_classRef;//������

			val=constPool->consts[ iRef->nameAndTypeIndex ]->val;//����������������������
			mRef->name=constPool->consts[ (val[0]<<8)+val[1] ]->val;//������
			mRef->descriptor=constPool->consts[ (val[2]<<8)+val[3] ]->val;//������
			constant->ref->_methodRef=mRef;
			mRef->method=NULL;
			break;
		}
		}
	}
	return constPool;
}

//����Code���Խ�����
Code* codeDecoder(FileData *d,MethodAreaClass* Class,u2 nameIndex){

	Code* codes=(Code*)malloc(sizeof(Code));
	codes->attrNameIndex=nameIndex;						//��������
	codes->codeLen=d->readU4();							//code����
	codes->maxStack=d->readU2();						//����ջ������
	codes->maxLocals=d->readU2();						//�ֲ�����������Ҫ�Ŀռ�
	codes->insLen=d->readU4();							//�ֽ���ָ�����鳤��java������淶��һ���������ܳ���65535��ָ
	//Code���Ե�ָ���---------------
	codes->insArray=(u1*)calloc(codes->insLen,sizeof(u1));
	for(int i=0;i<codes->insLen;++i){
		codes->insArray[i]=d->readU1();
	}

	//Code���Ե��쳣��Ϣ��-----------
	codes->exceptionInfoNum=d->readU2();
	if(codes->exceptionInfoNum>0){
		codes->excs=(ExceptionInfo**)calloc(codes->exceptionInfoNum,sizeof(ExceptionInfo*));
		for(int k=0;k<codes->exceptionInfoNum;++k){
			ExceptionInfo* exi=(ExceptionInfo*)malloc(sizeof(ExceptionInfo));
			exi->start_pc=d->readU2();
			exi->end_pc=d->readU2();
			exi->handler_pc=d->readU2();
			exi->catch_type=d->readU2();
			codes->excs[k]=exi;
		}
	}
	
	//Code����������Ϣ
	codes->attrCount=d->readU2();
	int nIndex;//,surplusAttrs=codes->attrCount;
	u1* tag;
	for(int j=0;j<codes->attrCount;++j){
		nIndex=d->readU2();		
		if(codes->attrCount>0){
			tag=Class->cp->consts[nIndex]->val;

			if(isEqual(tag,(u1*)"LocalVariableTable")==true){//LocalVariableTable����ʱ����
					d->readU4();
					int localVarTableLen=d->readU2();
					for(int i=0;i<localVarTableLen;++i){
						d->readU2();d->readU2();d->readU2();d->readU2();d->readU2();
					}
			}else if(isEqual(tag,(u1*)"LineNumberTable")==true){
				d->readU4();
				int lineNumTableLen=d->readU2();
				for(int i=0;i<lineNumTableLen;++i){
					d->readU2();d->readU2();
				}
			}else{//Other���ԡ�����
				int len=d->readU4();//���Գ���
				for(int i=0;i<len;++i){
					d->readU1();
				}
			}
		}
	}
	return codes;
}


//�ֶ�,�������������type=0�������ֶα�type=1������������
void fieldAndMethodTableDecoder(FileData *d,MethodAreaClass* Class,int type){

	int len=d->readU2();
	if(type==0){
		Class->fieldsLen=len;
		Class->floader=new fieldLoader(Class);
	}else{
		Class->methodsLen=len;
		Class->mloader=new methodLoader(Class);
	}
	Field* field;
	Method* method;

	for(int k=0;k<len;++k){
		if(type==0){
			field=(Field*)malloc(sizeof(Field));
			field->accessFlag=d->readU2();										//���η�
			int nameIndex=d->readU2();											//��������
			field->name=Class->cp->consts[nameIndex]->val;						//�ֶ�����
			int desIndex=d->readU2();											//��������
			field->description=Class->cp->consts[desIndex]->val;				//��������
			field->constValueIndex=0;//����������ֵΪ0��˵��û�о�̬����
			field->slotId=0;
			field->Class=Class;
			Class->floader->addField(field);									//������������ֶ�
		}else{
			method=(Method*)malloc(sizeof(Method));
			method->accessFlag=d->readU2();										//���η�
			int nameIndex=d->readU2();											//��������
			method->name=Class->cp->consts[nameIndex]->val;						//�ֶ�����
			int desIndex=d->readU2();											//��������
			method->description=Class->cp->consts[desIndex]->val;				//��������
			method->Class=Class;
			Class->mloader->addMethod(method);									//����������ӷ���							
		}

		int attrCount=d->readU2();											//���Ը���
		int j=0;
		if(attrCount>0){
			for(int i=0;i<attrCount;++i){
				int nameIndex=d->readU2();//�����س�������
				u1* tag=Class->cp->consts[nameIndex]->val;
				if(isEqual(tag,(u1*)"ConstantValue")==true){//ConstantValue����--fieldר��
					int attrLen=d->readU4();
					field->constValueIndex=d->readU2();

				}else if(isEqual(tag,(u1*)"Code")==true){//Code����--methodר��
					method->codes=codeDecoder(d,Class,nameIndex);
				}else if(isEqual(tag,(u1*)"Exceptions")==true){//����ҪExceptions������
					d->readU4();
					int exceptionsNum=d->readU2();
					for(int i=0;i<exceptionsNum;++i){
						d->readU2();
					}
				}else if(isEqual(tag,(u1*)"Deprecate")==true){//ͬ�ϣ�����
					d->readU4();
				}else if(isEqual(tag,(u1*)"Synthetic")==true){//ͬ�ϣ�����
					d->readU4();
				}else if(isEqual(tag,(u1*)"SourceFile")==true){//����
					d->readU2();d->readU2();
				}else if(isEqual(tag,(u1*)"RuntimeVisibleAnnotations")==true){//����
					int len=d->readU4();//���Գ���
					for(int i=0;i<len;++i){
						d->readU1();
					}
				}else if(isEqual(tag,(u1*)"LineNumberTable")==true){//����
					d->readU4();
					int lineNumTableLen=d->readU2();
					for(int i=0;i<lineNumTableLen;++i){
						d->readU2();d->readU2();
					}
				}else if(isEqual(tag,(u1*)"LocalVariableTable")==true){//����
					d->readU4();
					int localVarTableLen=d->readU2();
					for(int i=0;i<localVarTableLen;++i){
						d->readU2();d->readU2();d->readU2();d->readU2();d->readU2();
					}
				}else{//Other���ԡ�����
					int len=d->readU4();//���Գ���
					for(int i=0;i<len;++i){
						d->readU1();
					}
				}
			}
		}
		if(type==1){
			//calcArgSlotCount(���������б�)
			parseDes(method);//������������(�������б�)
		
			if(IsNative(method->accessFlag)==true){//����Ǳ��ط���
				string returnType=method->des[method->desNum-1];//���������б�����һ�����Ƿ�������
				//injectCodeAttribute------------------
				//���ط�����class�ļ���û��Code���ԣ�������Ҫ��maxStack�� maxLocals�ֶθ�ֵ��
				Code* c=(Code*)malloc(sizeof(Code));
				c->maxLocals=4;
				//c->maxStack=method->argSlotCount;
				c->maxStack=4;
				method->codes=c;
				//���ط�����Code�����ǿյģ�����Ҫ�����ط�������Codeֵ
				u1* addCode=(u1*)calloc(2,sizeof(u1));
				addCode[0]=0xfe;//����invokenativeָ�ͨ�����ط����������Ӧ����ִ�з���
				switch(returnType.c_str()[0]){
				case 'V':addCode[1]=0xb1;break;
				case 'D':addCode[1]=0xaf;break;
				case 'F':addCode[1]=0xae;break;
				case 'J':addCode[1]=0xad;break;
				case 'L': case '[':addCode[1]=0xb0;break;
				default :addCode[1]=0xac;break;
				}
				//-------------------------------------
				c->insArray=addCode;
			}
		}
	}
}

//����.class�ֽ���ΪMethodAreaClass
MethodAreaClass* byteCodeDecoder(FileData *d,methodArea* loader){

	//����ռ�
	MethodAreaClass* Class = (MethodAreaClass*)malloc(sizeof(MethodAreaClass));
	Class->instanceSlotCount = 0;
	Class->loader = loader;
	Class->superClass = NULL;
	Class->InitStarted = false;

	//���������ò���
	int magic=d->readU4();//ħ��
	int minorVersion=d->readU2();//�ΰ汾
	int majorVersion=d->readU2();//���汾

	//����������------------------------------------------------------------------------
	ConstPool* constPool=constPoolDecoder(d);//�����ؽ���
	Class->cp=constPool;
	Class->cp->Class=Class;

	//���Գؽ���------------------------------------------------------------------------------------------
	Class->accessFlag=d->readU2();//�����������η�
	int classIndex=d->readU2();//������������(������������ڳ����ص�λ��)
	Class->name=constPool->consts[classIndex]->ref->_classRef->name;//��ȡ��������

	int superClassIndex=d->readU2();
	if(superClassIndex!=0){//˵������java/lang/Object
		Class->superClassName=constPool->consts[superClassIndex]->ref->_classRef->name;//��ȡ��������
	}
	
	//����ʵ�ֽӿڳ�
	int interFacesLen=d->readU2();
	Class->interfacesLen=interFacesLen;
	Class->interfacesNames=(u1**)calloc(interFacesLen,sizeof(u1*));
	for(int k=0;k<interFacesLen;++k){
		int id=d->readU2();		
		Class->interfacesNames[k]=constPool->consts[id]->ref->_classRef->name;
	}

	//�����ֶα�(���Ա����)
	fieldAndMethodTableDecoder(d,Class,0);

	//����������(���Ա����)
	fieldAndMethodTableDecoder(d,Class,1);

	free(d);
	return Class;
}