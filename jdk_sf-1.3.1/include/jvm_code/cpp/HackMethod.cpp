#include "..\headers\HackMethod.h"
#include "..\headers\JVMThreadScheduler.h"

struct LineInputBuffer{
	b1* inputData;//输入的内容
	int len;//输入长度
	int size;//inputData大小

	void init(){
		size = 10;
		len = -1;
		inputData = (b1*)calloc(size,sizeof(b1));
		fflush(stdin);
		char c;
		scanf("%c",&c);
		while(c=='\n'){
			scanf("%c",&c);
		}
		setInputChar(c);
	}

	void setInputChar(b1 inputChar){
		if(len>=size-1){
			b1* newInputData = (b1*)calloc(size+10,sizeof(b1));
			for(int i=0;i<size;++i){
				newInputData[i]=inputData[i];
			}
			size+=10;
			inputData = newInputData;
		}
		inputData[++len]=inputChar;
	}

	b1* getInputLine(){
		init();
		b1 c;
		scanf("%c",&c);
		while(c!='\n'){
			setInputChar(c);
			scanf("%c",&c);
		}
		setInputChar('\0');

		std::string str(inputData); 
		return toUTF8(str);
	}

	b1* getInput(){
		init();
		b1 c;
		scanf("%c",&c);
		while(c!='\n' && c!=' '){
			setInputChar(c);
			scanf("%c",&c);
		}
		setInputChar('\0');
		std::string str(inputData); 
		return toUTF8(str);
	}

	int getInputIntVal(){
		int intval;
		scanf("%d",&intval);
		return intval;
	}
	
	//将输入的数据转换为utf-8编码
	b1* toUTF8(const std::string& str) 
	{ 
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0); 
		wchar_t* pwBuf = new wchar_t[nwLen + 1];    //一定要加1，不然会出现尾巴 
		ZeroMemory(pwBuf, nwLen * 2 + 2); 
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen); 
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 
		char* pBuf = new char[nLen + 1]; 
		ZeroMemory(pBuf, nLen + 1); 
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

		delete []pwBuf; 
		pwBuf = NULL; 
		return (char*)pBuf; 
	} 

};

void putString(Frame* frame,b1* inputData){
	methodArea* ma=frame->method->Class->loader;
	Heap* heap=ma->heap;
	stringPool* sPool=heap->getStrPool();
	Object* inputStrObj=sPool->JString(inputData);
	frame->popRef();//弹出java/lang/System对象
	frame->popRef();//弹出java/uitl/Scanner对象
	frame->pushRef(inputStrObj);
}

typedef struct HackMethod{
	bool (*func)(Frame* frame,methodRef* mRef);
}HackMethod;

map<string,HackMethod*> hackMethods;

bool isInit=false;

//写得太乱了，要重新整理过！
bool printMethod(Frame* frame,methodRef* mRef){

	u1* des = mRef->descriptor;
	switch(des[1]){
	case ')' :frame->popSlot();break;//()V
	case 'Z' : printf("%s",frame->popInt()==0 ? "false" : "true");break;
	case 'C' : printf("%c",frame->popInt());break;
	
	case 'I' : case 'B' : case 'S' :printf("%d",frame->popInt());break;
	case 'F' : printf("%f",frame->popFloat());break;
	case 'J' : printf("%lld",frame->popB8());break;
	case 'D' : printf("%lf",frame->popDouble());break;
	case '[' : 
		if(des[2]=='C'){//字符数组
			printfUTF8(frame->popRef()->array->B1Array);
		}
	case 'L' :
		if(isEqual((u1*)"(Ljava/lang/String;)V",des)==true){//字符串输出
			Object* jStr=frame->popRef();
			b1* strVal=frame->method->Class->loader->heap->getStrPool()->getJStrValue(jStr);
			if(strVal==NULL){
				printf("null");
				return true;
			}
			printfUTF8(strVal);
		}
		else if(isEqual((u1*)"(Ljava/lang/Object;)V",des)==true){//Object的value字段输出

			Object* obj=frame->popRef();

			if(obj==NULL){
				printf("null");
				return true;
			}else if(isEqual((u1*)"java/lang/StringBuilder",obj->objClass->name)==true || 
				isEqual((u1*)"java/lang/StringBuffer",obj->objClass->name)==true){//StringBuilder,StringBuffer
				/**
				输出StringBuilder,StringBuffer的内容。需要value及count字段
				输出的有效内容是value[0]...value[count-1]
				value字段是字符数组。存放内容,SlotId为1
				count字段是有效内容长度。SlotId为0
				**/
				b1* value=obj->fields[1]->ref->array->B1Array;//value字段
				int count=obj->fields[0]->num;//count字段
				value[count]='\0';
				printfUTF8(value);
				return true;
			}

			Field* valField=obj->objClass->floader->getFieldByName("value");
			if(valField==NULL){
				printf("%s@%d",obj->objClass->name,obj);
				return true;
			}

			
			Slot* valSlot=obj->fields[valField->slotId];

			

			switch(valSlot->flag){
			case 0:printf("null");break;
			case 3:printf("%d",valSlot->num);break;
			case 4:printf("%f",B4ToFloat(valSlot->num));break;
			case 5:
				printf("%d",valSlot->num+(obj->fields[valField->slotId+1]->num << 32));break;
			case 6:
				printf("%lf",B8ToDouble(valSlot->num+(obj->fields[valField->slotId+1]->num << 32)));
				break;
			default:
				printf("%s@%d",obj->objClass,obj);break;
			}
		}
		break;

	default : 
		Slot* slot=frame->popSlot();
		printf("printfln: %s",des);break;
	}
	return true;
}

bool printlnMethod(Frame* frame,methodRef* mRef){
	printMethod(frame,mRef);
	printf("\n");
	return true;
}

bool scannerInit(Frame* frame,methodRef* mRef){
	return true;
}
bool scannerCLinit(Frame* frame,methodRef* mRef){
	frame->popRef();	
	return true;
}

bool scannerNextLine(Frame* frame,methodRef* mRef){
	fflush(stdin);
	LineInputBuffer* bf = (LineInputBuffer*)malloc(sizeof(LineInputBuffer));
	b1* inputData = bf->getInputLine();//以换行符结束
	putString(frame,inputData);
	return true;
}

bool scannerNext(Frame* frame,methodRef* mRef){
	fflush(stdin);
	LineInputBuffer* bf = (LineInputBuffer*)malloc(sizeof(LineInputBuffer));
	b1* inputData =bf->getInput();//以空格符或换行符结束
	putString(frame,inputData);
	return true;
}

bool scannerNextInt(Frame* frame,methodRef* mRef){
	fflush(stdin);
	LineInputBuffer* bf = (LineInputBuffer*)malloc(sizeof(LineInputBuffer));
	b4 intval = bf->getInputIntVal();//以空格或换行符结束

	frame->popRef();//弹出java/lang/System对象

	Slot* slot = frame->popSlot();//弹出java/uitl/Scanner对象
	if(slot->flag==3){//slot->flag = 3 说明不是java/uitl/Scanner对象，装回去
		frame->pushSlot(slot);
	}
	frame->pushB4(intval,3);
	return true;
}



//java/lang/Integer
bool integerInit(Frame* frame,methodRef* mRef){
	fflush(stdin);
	if(mRef->descriptor[1]!='L') return false; //不是<init>(Ljava/lang/String;)V
	
	Object* str = frame->popRef();
	MethodAreaClass* Class = str->objClass;
	Heap* heap = Class->loader->heap;
	stringPool* strPool = heap->getStrPool();
	char* val = strPool->getJStrValue(str);
	int len=0;
	while(val[len++]!='\0');
	len--;

	int num=0,digit=1;
	for(int i=len-1;i>=0;--i){
		num+=digit*(val[i]-'0');
		digit*=10;
	}

	Object* integer = frame->popRef();
	fieldLoader* floader = integer->objClass->floader;
	Field* value = floader->getFieldByName("value");
	integer->fields[value->slotId]->num=num;
	integer->fields[value->slotId]->flag=3;

	return true;
}

bool IntegerCacheCLInit(Frame* frame,methodRef* mRef){
	return true;
}


bool randomCLInit(Frame* frame,methodRef* mRef){
	return true;
}

bool randomInit(Frame* frame,methodRef* mRef){
	frame->popRef();	//弹出java/util/Random对象
	return true;
}

int randomNum(){
	
	return (int)rand();
}

bool randomNextBoolean(Frame* frame,methodRef* mRef){
	frame->popRef();
	int rand = randomNum()%2;
	frame->pushB4(rand<0 ? rand=-rand :rand=rand,3);
	return true;
}

bool randomNextFloat(Frame* frame,methodRef* mRef){
	frame->popRef();
	int rand1 = randomNum();
	rand1 < 0 ? rand1 = -rand1 : rand1 = rand1;
	int rand2 = randomNum();
	rand2 < 0 ? rand2 = -rand2 : rand2 = rand2;

	if(rand2>rand1){
		frame->pushFloat((float)(rand1+1)/(float)(rand2+1));
	}else{
		frame->pushFloat((float)(rand2+1)/(float)(rand1+1));
	}
	return true;
}


bool randomNextInt(Frame* frame,methodRef* mRef){
	int rand = randomNum();
	if(mRef->descriptor[1] == ')'){//()V
		frame->popRef();
		frame->pushB4(rand,3);
	}else{//(I)I
		int n = frame->popB4();
		if(n <= 0){
			printf("bound 必须是正数!");
			exit(1);
		}
		rand < 0 ? rand = -rand : rand = rand;
		frame->popRef();
		frame->pushB4(rand%n,3);
	}
	return true;
}

bool getSavedProperty(Frame* frame,methodRef* mRef){
	return true;
}

bool digit(Frame* frame,methodRef* mRef){
	int radix = frame->popB4();
	int ch = frame->popB4();

	if(ch >= 'A' && ch <= 'Z' && ch < radix + 'A' - 10){
		frame->pushB4(3,ch - 'A' + 10);
	}
	else if(ch >= 'a' && ch <= 'z' && ch < radix + 'a' - 10){
		frame->pushB4(3,ch - 'a' + 10);
	}else{
		frame->pushB4(3,ch);
	}
	return true;
}

//线程类构造方法
bool threadClinit(Frame* frame,methodRef* mRef){
	return true;
}

//初始化线程名称
void threadInitName(Object* threadNameStr,Object* Thread){
	Heap* heap = Thread->objClass->loader->heap;	
				
	//获取Thread对象的name字段(char数组)threadNameField
	int threadNameSlotId = Thread->objClass->floader->getFieldByName("name")->slotId;
	Slot* threadNameField = Thread->fields[threadNameSlotId];

	//将threadName的value字段(char数组)赋值给Thread对象的threadNameField
	threadNameField->flag = 10;
	threadNameField->ref = heap->getStrPool()->getJStrValueFieldRef(threadNameStr);	
}

//创建线程
bool threadInit(Frame* frame,methodRef* mRef){

	ThreadScheduler* threadScheduler = frame->thisThread->threadScheduler;
	
	//在堆区创建一个java.lang.Thread对象，在jvm创建一个Thread实体并在线程调度器注册
	Object* This = frame->opStack[0]->ref;		//Thread对象的this引用
	methodArea* ma = mRef->cp->Class->loader;
	Heap* heap = ma->heap;

	//在jvm创建Thread实体，然后在线程调度器注册
	Thread* newThread=(Thread*)malloc(sizeof(Thread));
	newThread->name = (u1*)threadScheduler->getDefaultThreadName();			//线程默认名称为Thread@id
	newThread->buildThread(frame->thisThread->stackMaxSize);

	Object* threadNameStr = heap->getStrPool()->JString((b1*)newThread->name);

	Method* runMethod = This->objClass->mloader->getMethodByNameAndDes("run","()V");

	//如果调用的是Thread(String name)方法，则要获取对象name属性(char数组)并赋值
	if(isEqual(mRef->descriptor,(u1*)"(Ljava/lang/String;)V")){
		threadNameStr = frame->popRef();		//获取线程名称
	}else if(isEqual(mRef->descriptor,(u1*)"(Ljava/lang/Runnable;)V")){
		runMethod = frame->popRef()->objClass->mloader->getMethodByNameAndDes("run","()V");
	}else if(isEqual(mRef->descriptor,(u1*)"(Ljava/lang/Runnable;Ljava/lang/String;)V")){
		threadNameStr = frame->popRef();		//获取线程名称
		runMethod = frame->popRef()->objClass->mloader->getMethodByNameAndDes("run","()V");

	}

	//注册线程名称
	threadInitName(threadNameStr,This);

	newThread->threadObject = frame->popRef();	//将线程和线程对象绑定

	threadScheduler->newThread(newThread,runMethod);

	//设置java.lang.Thread对象的tid
	int tIdSlotId = This->objClass->floader->getFieldByName("tid")->slotId;
	This->fields[tIdSlotId]->num = newThread->id;
	This->fields[tIdSlotId]->flag = 3;

	return true;
}


bool threadStart(Frame* frame,methodRef* mRef){

	Object* This = frame->popRef();		//Thread对象的this引用	

	methodArea* ma = mRef->cp->Class->loader;
	MethodAreaClass* threadClass = loadNonArrayClass(ma,"java/lang/Thread",false);

	ThreadScheduler* threadScheduler = frame->thisThread->threadScheduler;
	int tIdSlotId = threadClass->floader->getFieldByName("tid")->slotId;

	threadScheduler->startThread(This->fields[tIdSlotId]->num);
	
	return true;
}

void hackMethodInit(){

	//java/lang/Thread类
	hackMethods["java/lang/Thread~<init>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/lang/Thread~<init>"]->func = threadInit;
	hackMethods["java/lang/Thread~start"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/lang/Thread~start"]->func = threadStart;
	hackMethods["java/lang/Thread~<clinit>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/lang/Thread~<clinit>"]->func = threadClinit;

	//java/io/PrintStream类
	hackMethods["java/io/PrintStream~println"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/io/PrintStream~println"]->func = printlnMethod;
	hackMethods["java/io/PrintStream~print"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/io/PrintStream~print"]->func = printMethod;

	//java/util/Scanner类
	hackMethods["java/util/Scanner~<init>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Scanner~<init>"]->func = scannerInit;
	hackMethods["java/util/Scanner~<clinit>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Scanner~<clinit>"]->func = scannerCLinit;
	hackMethods["java/util/Scanner~nextInt"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Scanner~nextInt"]->func = scannerNextInt;
	hackMethods["java/util/Scanner~next"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Scanner~next"]->func = scannerNext;
	hackMethods["java/util/Scanner~nextLine"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Scanner~nextLine"]->func = scannerNextLine;

	//java/lang/Integer类
	hackMethods["java/lang/Integer~<init>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/lang/Integer~<init>"]->func = integerInit;
//	hackMethods["java/lang/Integer$IntegerCache~<clinit>"] = (HackMethod*)malloc(sizeof(HackMethod));
//	hackMethods["java/lang/Integer$IntegerCache~<clinit>"]->func = IntegerCacheCLInit;

	hackMethods["sun/misc/VM~getSavedProperty"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["sun/misc/VM~getSavedProperty"]->func = getSavedProperty;

	
	hackMethods["java/lang/Character~digit"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/lang/Character~digit"]->func = digit;

	//java/util/Random类
	hackMethods["java/util/Random~<clinit>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Random~<clinit>"]->func = randomCLInit;
	hackMethods["java/util/Random~<init>"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Random~<init>"]->func = randomInit;
	hackMethods["java/util/Random~nextBoolean"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Random~nextBoolean"]->func = randomNextBoolean;
	hackMethods["java/util/Random~nextInt"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Random~nextInt"]->func = randomNextInt;
	hackMethods["java/util/Random~nextFloat"] = (HackMethod*)malloc(sizeof(HackMethod));
	hackMethods["java/util/Random~nextFloat"]->func = randomNextFloat;

	isInit=true;
	srand(getUS());//用于生成随机数
}

bool hackMethod(Frame* frame,methodRef* mRef){
	mRef->cp->Class->loader;
	if(isInit==false){
		hackMethodInit();
	}

	string methodKey;
	methodKey.append((char*)mRef->cRef->name);
	methodKey.append("~");
	methodKey.append((char*)mRef->name);

	HackMethod* hmethod = hackMethods[methodKey];

	if(hmethod == NULL){

		//如果该方法名称为start，则很有可能是多线程类(即继承了java.lang.Runnable接口的类及其子类)
		if(isEqual(mRef->name,(u1*)"start")==true){
			methodArea* ma = mRef->cp->Class->loader;
			MethodAreaClass* This = loadNonArrayClass(ma,(char*)mRef->cRef->name,false);
			MethodAreaClass* runnable = loadNonArrayClass(ma,"java/lang/Runnable",false);
			if(isImplements(This,runnable) == true){
				return threadStart(frame,mRef);
			}
		}

		return false;//说明不是要通过hack执行的方法
	}

	return (*hackMethods[methodKey]->func)(frame,mRef);
}