/**
jvm线程调度器
调度执行线程

  线程status值对应的六种状态：
  0:初始 1:运行(包含就绪) 2:阻塞 3:等待 4:超时等待 5:终止

								Thread thread = new Thread()
											↓  
										+--------+
										|  初始  |                     Object.wait(long)
										|  (NEW) |                     Thread.join(long)
										+--------+                     Thread.sleep(long)
	Object.wait()							↓ thread.start()       LockSupport.parkNanos()
	Thread.join()			 +-----------------------------+        LockSupport.parkUntil()       
  LockSupport.park()		 |		     运     行         |--------------------------                
       ----------------------|			(RUNNABLE)         |                         ↓
	  ↓                     |		 +-------------+       |                 +-----------------+
  +-----------+	             |		 |	 运 行 中  |       |                 |    超时等待     |
  |   等待	  |	             |		 |  (RUNNING)  |       |                 | (TIMED_WAITING) |
  | (WAITING) |	             |       +-------------+       |                 +-----------------+
  +-----------+	             |  系统调度↑     ↓系统调度  |←-------------------------|
	  |  			         |		 +-------------+       |                   Object.notify()
	   --------------------→|		 |	 就    绪  |       |                  Object.notifyAll()
	   Object.notify()	     |		 |    (READY)  |       |               LockSupport.unpark(Thread)
      Object.notifyAll()     |       +-------------+       | 
 LockSupport.unpark(Thread)	 |                             |←------------------------------            
                             +-----------------------------+                               | 获取到锁
                                            ↓ 执行完成  |                           +-----------+
					                  +--------------+   |                           |    阻塞   |
									  |  终      止  |   |-------------------------→| (BLOCKED) |
									  | (TERMINATED) |     等待进入synchronized方法  +-----------+
									  +--------------+     等待进入synchronized块                
									                                     
**/


#pragma once//防止重复定义
#include "Thread.h"
#include "UtilTypes.h"
#include "ClassFileDecoder.h"
#include "AccessFlagDecoder.h"
#include "InstDecoder.h"
#include <iostream>
#include <windows.h>


//线程调度器
class ThreadScheduler{

	private:	
		
		//线程容器（键：线程id，值：线程）
		map<int,Thread*> newThreads;			//初始态线程容器
		map<int,Thread*> runnableThreads;		//运行态线程容器(正在运行的+就绪的线程,即能够运行的线程)
		map<int,Thread*> blockedThreads;		//阻塞态线程容器
		map<int,Thread*> waitingThreads;		//等待态线程容器
		map<int,Thread*> timeWaitingThreads;	//超时等待态线程容器

		map<int,Thread*>::iterator mit;			//线程容器迭代器

		//正在运行的线程
		Thread* runningThread;

		JInst* instTable;						//指令集
		int flag[300];							//栈帧更新标志
		bool logInst;							//测试,显示执行步骤
		int instNum;							//执行的指令条数

		int threadId;							//自增线程id;

		long getTime(){
			return getNS();
		}

		//执行某线程
		//如果是<clinit>,在没执行完成前不得切换!
		void loop(int time){
			
			Stack* stack = runningThread->stack;
			Frame* currentFrame = stack->getStackTop();
			Method* m = currentFrame->method;
			
			int opNum=0;
			int interruptible = false;			//栈帧是否能被中断
			int interruptibleFrameDepth = 0;	//如果不能被中断的话，栈帧深度是多少(-1代表可以被中断)

			while(stack->top > 0){
				
				++instNum;
				if(flag[opNum]==1){//对于特定的指令，才更新当前栈帧
					currentFrame=stack->getStackTop();
					m=currentFrame->method;

					//如果栈帧A方法为类构造方法，则保证其能够执行完成。
					//即栈帧A栈深大于当前栈顶大小(栈帧A已经弹出方法栈了)
					isEqual((u1*)m->name,(u1*)"<clinit>") ? interruptible = false,interruptibleFrameDepth=stack->top
						: interruptible = true;
				
				}
				runningThread->pc=currentFrame->pc;
				opNum=currentFrame->readIns();		//解码
				if(logInst==true){//测试,显示执行步骤
					showLoop(currentFrame,currentFrame->pc,opNum,instNum);
				}
				if(*instTable[opNum].func){
					(*instTable[opNum].func)(currentFrame,opNum);
				}else{
					string infos[]={charArrayToString(getInstName(opNum))};
					printError(0x004,infos);
				}

				if(--time < 0 && interruptible == true && interruptibleFrameDepth <= stack->top) break;
			}
			if(stack->top == 0)	runnableThreads.erase(runningThread->id);	//线程执行完毕，移除
		}

		//随机调度算法
		void randomScheduleThread(){
			int index = getUS() % runnableThreads.size();
			mit = runnableThreads.begin();
			for(mit = runnableThreads.begin();--index>=0 ;mit++);
			runningThread = mit->second;
		}
	

	public:

		//运行线程调度器
		int startThreadScheduler(){
			while(runnableThreads.size()>0){
				loop(1000);				//至少执行1000条指令就切换一次线程
				if(runnableThreads.size() == 0) break;
				randomScheduleThread();
			}
			return instNum;
		}

		ThreadScheduler(Thread* mainThread,methodArea* ma,bool logInstFlag){

			instTable = buildInstTable();
			logInst = logInstFlag;
			threadId = 0;
			instNum = 0;
			for(int i=0;i<300;++i){
				//对于以下指令，设置flag[i]=1。即对于操作数为i的指令，允许更新栈帧
				if((i>=0xb2 && i<=0xb9) || (i>=0xac && i<=0xb1) || i==0xb2 || i==0xbb || i==0xfe){
					flag[i]=1;
				}
			}
			mainThread->id = ++threadId;
			runnableThreads[mainThread->id] = mainThread;

			//创建主线程对应的线程Object-------------------------------------------------------------------------
			MethodAreaClass* threadClass = loadNonArrayClass(ma,"java/lang/Thread",false);
			Object* mainThreadObject = newObject(threadClass);

			//设置主线程对象的tid
			int tIdSlotId = mainThreadObject->objClass->floader->getFieldByName("tid")->slotId;
			mainThreadObject->fields[tIdSlotId]->num = mainThread->id;
			mainThreadObject->fields[tIdSlotId]->flag = 3;

			//获取主线程对象的name字段(char数组)threadNameField
			int threadNameSlotId = mainThreadObject->objClass->floader->getFieldByName("name")->slotId;
			Slot* threadNameField = mainThreadObject->fields[threadNameSlotId];

			//将threadName的value字段(char数组)赋值给Thread对象的threadNameField
			threadNameField->flag = 10;
			threadNameField->ref = newPrimitiveTypesArray(ma,5,4);
			threadNameField->ref->array->B1Array = (b1*)mainThread->name;

			//-----------------------------------------------------------------------------------------------------

			mainThread->threadObject = mainThreadObject;
			runningThread = mainThread;
		}


		//注册线程(新建，还未调用start方法)
		//runMethod方法是该线程要执行的方法。
		//如果是new Thread(),则该runMethod为Thread或其子类对象的run方法
		//如果是new Thread(Runnable对象),则runMethod为该实现Runnable接口的类的run方法
		void newThread(Thread* newThread,Method* runMethod){
			newThread->id = ++threadId;
			newThreads[newThread->id] = newThread;
				
			//新建栈帧(run方法,将其压入线程)
			Frame* newFrame=(Frame*)malloc(sizeof(Frame));
			newFrame->newFrame(newThread,runMethod);
			newThread->stack->pushFrame(newFrame);	//压入线程	

			newFrame->localVars[0]->flag = 10;
			newFrame->localVars[0]->ref = newThread->threadObject;
		}

		//线程运行(初始态转换到运行态),将run方法压入栈
		//调用java.lang.Thread.start方法后执行
		void startThread(int threadId){

			Thread* thread = newThreads[threadId];
			Object* threadObject = thread->threadObject;

			if(thread != NULL){//将run方法压入线程栈帧
				runnableThreads[threadId] = thread;
				newThreads.erase(threadId);
			}
		}

		//线程阻塞(运行态转等待态)
		void blockedThread(int threadId){
			Thread* thread = runnableThreads[threadId];
			if(thread != NULL){
				blockedThreads[threadId] = thread;
				runnableThreads.erase(threadId);
			}
		}

		//线程等待(等待态转运行态)
		void waitingThread(){

		}

		//线程唤醒(等待态转运行态)
		void notifyThread(){

		}

		b1* getDefaultThreadName(){
			string name = "Thread-";
			name+=(threadId+'0');

			b1* charArray = (b1*)calloc(name.size()+1,sizeof(b1));
			int len = name.size();
			b1* nameValue =  (b1*)name.c_str();

			for(int i=0;i<len;++i){
				charArray[i] =nameValue[i];
			}
			charArray[len] = '\0';
			return charArray;
		}

};
