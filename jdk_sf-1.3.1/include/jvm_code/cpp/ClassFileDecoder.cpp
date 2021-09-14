/**
 .class文件解码器	

   1 作用
     (1) .class文件字节码解码

   2 对外提供方法	
     (1) DecodeClass
	 将.class文件字节码解码成methodArea结构体

**/
#pragma once//防止重复定义
#include "..\headers\VMTools.h"
#include "..\headers\ClassFileDecoder.h"

//方法描述解析器
//很迷,结构体带vector会出问题,只好用string数组解决一下
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
		method->argSlotCount++;//this引用(构造方法引用)
	}

	method->des=(string*)calloc(vd.size(),sizeof(string));
	method->desNum=vd.size();
	for(int j=0;j<vd.size();++j){
		method->des[j]=vd[j];
	}
	vd.clear();
}

ConstPool* constPoolDecoder(FileData *d){//常量池解析。注意，要先解析常量，再解析引用。（引用相关的字节码先缓存）
	
	ConstPool* constPool=(ConstPool*)malloc(sizeof(ConstPool));
	constPool->cpSize=d->readU2();//常量池大小。常量池索引从1开始！
	constPool->consts=(Constant**)calloc(constPool->cpSize,sizeof(Constant*));
	int type,index,len=constPool->cpSize;
	u1* val;
	classRef* cRef;
	u1** buffers=(u1**)calloc(len,sizeof(u1*));
	char* bufferTypes=(char*)calloc(len,sizeof(char));
	for(int i1=1;i1<len;++i1){//解析常量，缓存引用用到的内容
		Constant* constant = (Constant*)malloc(sizeof(Constant));
		constant->type = d->readU1();
		switch(constant->type){
		case 1://解析utf8
			constant->val = d->readBytes(d->readU2());
			constPool->consts[i1] = constant;
			break;
		case 3: case 4://解析Integer、Float。
			constant->val = d->readBytes(4);
			constPool->consts[i1] = constant;
			break;
		case 5: case 6: //解析Long、Double。需要占两位
			constant->val = d->readBytes(4);
			constPool->consts[i1] = constant;
			constPool->consts[++i1] = (Constant*)malloc(sizeof(Constant));
			constPool->consts[i1]->val = d->readBytes(4);
			break;

		//类引用，字符引用。先缓存起来，因为utf8、Integer、Float、Long、Double、类引用没解析完之前，无法解析这些。
		//但是类引用有点特殊，解析其之前，字段引用、方法引用、接口方法引用要用到，所以先创建
		case 7: constant->ref=(Ref*)malloc(sizeof(Ref));
				constant->ref->_classRef=(classRef*)malloc(sizeof(classRef));
		case 8:
			buffers[i1]=d->readBytes(2);
			bufferTypes[i1]=constant->type;
			constPool->consts[i1]=constant;
			break;
		case 9: case 10: case 11: //字段引用、方法引用、接口方法引用。同上，缓存
			buffers[i1]=d->readBytes(4);
			bufferTypes[i1]=constant->type;
			constPool->consts[i1]=constant;
			break;
		case 12://名字和属性索引
			constant->val=d->readBytes(4);
			constPool->consts[i1]=constant;
			break;
		case 15://句柄。用不到，先不管，跳过
			d->readU1();d->readU2();
			break;
		case 16://标志方法。也跳过
			d->readU2();
			break;
		case 18://动态调用点。还跳过
			d->readU2();d->readU2();
			break;
		}
	}

	for(int i2=1;i2<len;++i2){//解析引用
		Constant* constant = constPool->consts[i2];
		switch(bufferTypes[i2]){
			
		case 7://解析类引用
			cRef=constant->ref->_classRef;
			cRef->cp=constPool;
			val=buffers[i2];

			//加载类名称
			index=(val[0]<<8)+val[1];
			cRef->name=constPool->consts[index]->val;
			cRef->Class=NULL;
			constPool->consts[i2]=constant;
			break;
		case 8://解析字符串
			constPool->consts[i2]->val=buffers[i2];
			break;
		case 9://解析字段引用
		{
			constant->ref = (Ref*)malloc(sizeof(Ref));
			fieldRef* fRef = (fieldRef*)malloc(sizeof(fieldRef));
			fRef->cp=constPool;

			//copyMemberRefInfo
			val=buffers[i2];
			index=(buffers[i2][0]<<8)+buffers[i2][1];//字段所属类在常量池的索引
			fRef->cRef=constPool->consts[index]->ref->_classRef;//字段所属的类的引用

			index=(val[2]<<8)+val[3];//字段名和属性索引
			val=constPool->consts[index]->val;
			
			fRef->name=constPool->consts[(val[0]<<8)+val[1]]->val;//字段名称
			fRef->descriptor=constPool->consts[(val[2]<<8)+val[3] ]->val;//属性
			fRef->field=NULL;
			constant->ref->_fieldRef=fRef;
			constPool->consts[i2]=constant;
			break;
		}
		case 10://解析方法引用
		{
			constant->ref=(Ref*)malloc(sizeof(Ref));
			methodRef* mRef=(methodRef*)malloc(sizeof(methodRef));
			mRef->cp=constPool;

			//copyMemberRefInfo
			index=(buffers[i2][0]<<8)+buffers[i2][1];//方法所属类在常量池的索引
			mRef->cRef=constPool->consts[index]->ref->_classRef;//方法所属的类的引用
			index=(buffers[i2][2]<<8)+buffers[i2][3];//方法名和属性索引
			val=constPool->consts[index]->val;
			mRef->name=constPool->consts[ (val[0]<<8)+val[1] ]->val;//方法名
			mRef->descriptor=constPool->consts[ (val[2]<<8)+val[3] ]->val;//方法属性（字段类型）
			mRef->method=NULL;
			constant->ref->_methodRef=mRef;
			constPool->consts[i2]=constant;
			break;
		}
		case 11://解析接口方法引用
		{
			constant->ref=(Ref*)malloc(sizeof(Ref));
			interfaceMethodRef* iRef=(interfaceMethodRef*)malloc(sizeof(interfaceMethodRef));
			iRef->classIndex=(buffers[i2][0]<<8)+buffers[i2][1];
			iRef->nameAndTypeIndex=(buffers[i2][2]<<8)+buffers[i2][3];
			constant->ref->_interfaceMethodRef=iRef;
			constPool->consts[i2]=constant;

			methodRef* mRef=(methodRef*)malloc(sizeof(methodRef));
			mRef->cp=constPool;
			mRef->cRef=constPool->consts[ iRef->classIndex ]->ref->_classRef;//类引用

			val=constPool->consts[ iRef->nameAndTypeIndex ]->val;//方法名和属性索引的内容
			mRef->name=constPool->consts[ (val[0]<<8)+val[1] ]->val;//方法名
			mRef->descriptor=constPool->consts[ (val[2]<<8)+val[3] ]->val;//方法名
			constant->ref->_methodRef=mRef;
			mRef->method=NULL;
			break;
		}
		}
	}
	return constPool;
}

//方法Code属性解析器
Code* codeDecoder(FileData *d,MethodAreaClass* Class,u2 nameIndex){

	Code* codes=(Code*)malloc(sizeof(Code));
	codes->attrNameIndex=nameIndex;						//名称索引
	codes->codeLen=d->readU4();							//code长度
	codes->maxStack=d->readU2();						//操作栈最大深度
	codes->maxLocals=d->readU2();						//局部变量储存需要的空间
	codes->insLen=d->readU4();							//字节码指令数组长。java虚拟机规范中一个方法不能超过65535条指
	//Code属性的指令表---------------
	codes->insArray=(u1*)calloc(codes->insLen,sizeof(u1));
	for(int i=0;i<codes->insLen;++i){
		codes->insArray[i]=d->readU1();
	}

	//Code属性的异常信息表-----------
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
	
	//Code属性属性信息
	codes->attrCount=d->readU2();
	int nIndex;//,surplusAttrs=codes->attrCount;
	u1* tag;
	for(int j=0;j<codes->attrCount;++j){
		nIndex=d->readU2();		
		if(codes->attrCount>0){
			tag=Class->cp->consts[nIndex]->val;

			if(isEqual(tag,(u1*)"LocalVariableTable")==true){//LocalVariableTable，暂时跳过
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
			}else{//Other属性。跳过
				int len=d->readU4();//属性长度
				for(int i=0;i<len;++i){
					d->readU1();
				}
			}
		}
	}
	return codes;
}


//字段,方法表解析器。type=0，解析字段表；type=1，解析方法表
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
			field->accessFlag=d->readU2();										//修饰符
			int nameIndex=d->readU2();											//名称索引
			field->name=Class->cp->consts[nameIndex]->val;						//字段名称
			int desIndex=d->readU2();											//描述索引
			field->description=Class->cp->consts[desIndex]->val;				//描述内容
			field->constValueIndex=0;//如果这个属性值为0，说明没有静态变量
			field->slotId=0;
			field->Class=Class;
			Class->floader->addField(field);									//往加载器添加字段
		}else{
			method=(Method*)malloc(sizeof(Method));
			method->accessFlag=d->readU2();										//修饰符
			int nameIndex=d->readU2();											//名称索引
			method->name=Class->cp->consts[nameIndex]->val;						//字段名称
			int desIndex=d->readU2();											//描述索引
			method->description=Class->cp->consts[desIndex]->val;				//描述内容
			method->Class=Class;
			Class->mloader->addMethod(method);									//往加载器添加方法							
		}

		int attrCount=d->readU2();											//属性个数
		int j=0;
		if(attrCount>0){
			for(int i=0;i<attrCount;++i){
				int nameIndex=d->readU2();//常量池常量索引
				u1* tag=Class->cp->consts[nameIndex]->val;
				if(isEqual(tag,(u1*)"ConstantValue")==true){//ConstantValue属性--field专有
					int attrLen=d->readU4();
					field->constValueIndex=d->readU2();

				}else if(isEqual(tag,(u1*)"Code")==true){//Code属性--method专有
					method->codes=codeDecoder(d,Class,nameIndex);
				}else if(isEqual(tag,(u1*)"Exceptions")==true){//不需要Exceptions，跳过
					d->readU4();
					int exceptionsNum=d->readU2();
					for(int i=0;i<exceptionsNum;++i){
						d->readU2();
					}
				}else if(isEqual(tag,(u1*)"Deprecate")==true){//同上，跳过
					d->readU4();
				}else if(isEqual(tag,(u1*)"Synthetic")==true){//同上，跳过
					d->readU4();
				}else if(isEqual(tag,(u1*)"SourceFile")==true){//跳过
					d->readU2();d->readU2();
				}else if(isEqual(tag,(u1*)"RuntimeVisibleAnnotations")==true){//跳过
					int len=d->readU4();//属性长度
					for(int i=0;i<len;++i){
						d->readU1();
					}
				}else if(isEqual(tag,(u1*)"LineNumberTable")==true){//跳过
					d->readU4();
					int lineNumTableLen=d->readU2();
					for(int i=0;i<lineNumTableLen;++i){
						d->readU2();d->readU2();
					}
				}else if(isEqual(tag,(u1*)"LocalVariableTable")==true){//跳过
					d->readU4();
					int localVarTableLen=d->readU2();
					for(int i=0;i<localVarTableLen;++i){
						d->readU2();d->readU2();d->readU2();d->readU2();d->readU2();
					}
				}else{//Other属性。跳过
					int len=d->readU4();//属性长度
					for(int i=0;i<len;++i){
						d->readU1();
					}
				}
			}
		}
		if(type==1){
			//calcArgSlotCount(解析参数列表)
			parseDes(method);//解析方法描述(即参数列表)
		
			if(IsNative(method->accessFlag)==true){//如果是本地方法
				string returnType=method->des[method->desNum-1];//方法描述列表的最后一个就是返回类型
				//injectCodeAttribute------------------
				//本地方法在class文件中没有Code属性，所以需要给maxStack和 maxLocals字段赋值。
				Code* c=(Code*)malloc(sizeof(Code));
				c->maxLocals=4;
				//c->maxStack=method->argSlotCount;
				c->maxStack=4;
				method->codes=c;
				//本地方法的Code属性是空的，所以要给本地方法加上Code值
				u1* addCode=(u1*)calloc(2,sizeof(u1));
				addCode[0]=0xfe;//调用invokenative指令。通过本地方法表调用相应函数执行方法
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

//解析.class字节码为MethodAreaClass
MethodAreaClass* byteCodeDecoder(FileData *d,methodArea* loader){

	//分配空间
	MethodAreaClass* Class = (MethodAreaClass*)malloc(sizeof(MethodAreaClass));
	Class->instanceSlotCount = 0;
	Class->loader = loader;
	Class->superClass = NULL;
	Class->InitStarted = false;

	//下面四项用不到
	int magic=d->readU4();//魔数
	int minorVersion=d->readU2();//次版本
	int majorVersion=d->readU2();//主版本

	//解析常量池------------------------------------------------------------------------
	ConstPool* constPool=constPoolDecoder(d);//常量池解析
	Class->cp=constPool;
	Class->cp->Class=Class;

	//属性池解析------------------------------------------------------------------------------------------
	Class->accessFlag=d->readU2();//解析本类修饰符
	int classIndex=d->readU2();//解析本类索引(本类符号引用在常量池的位置)
	Class->name=constPool->consts[classIndex]->ref->_classRef->name;//获取本类名称

	int superClassIndex=d->readU2();
	if(superClassIndex!=0){//说明不是java/lang/Object
		Class->superClassName=constPool->consts[superClassIndex]->ref->_classRef->name;//获取父类名称
	}
	
	//解析实现接口池
	int interFacesLen=d->readU2();
	Class->interfacesLen=interFacesLen;
	Class->interfacesNames=(u1**)calloc(interFacesLen,sizeof(u1*));
	for(int k=0;k<interFacesLen;++k){
		int id=d->readU2();		
		Class->interfacesNames[k]=constPool->consts[id]->ref->_classRef->name;
	}

	//解析字段表(类成员变量)
	fieldAndMethodTableDecoder(d,Class,0);

	//解析方法表(类成员变量)
	fieldAndMethodTableDecoder(d,Class,1);

	free(d);
	return Class;
}