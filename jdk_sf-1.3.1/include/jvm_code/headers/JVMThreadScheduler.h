/**
jvm�̵߳�����
����ִ���߳�

  �߳�statusֵ��Ӧ������״̬��
  0:��ʼ 1:����(��������) 2:���� 3:�ȴ� 4:��ʱ�ȴ� 5:��ֹ

								Thread thread = new Thread()
											��  
										+--------+
										|  ��ʼ  |                     Object.wait(long)
										|  (NEW) |                     Thread.join(long)
										+--------+                     Thread.sleep(long)
	Object.wait()							�� thread.start()       LockSupport.parkNanos()
	Thread.join()			 +-----------------------------+        LockSupport.parkUntil()       
  LockSupport.park()		 |		     ��     ��         |--------------------------                
       ----------------------|			(RUNNABLE)         |                         ��
	  ��                     |		 +-------------+       |                 +-----------------+
  +-----------+	             |		 |	 �� �� ��  |       |                 |    ��ʱ�ȴ�     |
  |   �ȴ�	  |	             |		 |  (RUNNING)  |       |                 | (TIMED_WAITING) |
  | (WAITING) |	             |       +-------------+       |                 +-----------------+
  +-----------+	             |  ϵͳ���ȡ�     ��ϵͳ����  |��-------------------------|
	  |  			         |		 +-------------+       |                   Object.notify()
	   --------------------��|		 |	 ��    ��  |       |                  Object.notifyAll()
	   Object.notify()	     |		 |    (READY)  |       |               LockSupport.unpark(Thread)
      Object.notifyAll()     |       +-------------+       | 
 LockSupport.unpark(Thread)	 |                             |��------------------------------            
                             +-----------------------------+                               | ��ȡ����
                                            �� ִ�����  |                           +-----------+
					                  +--------------+   |                           |    ����   |
									  |  ��      ֹ  |   |-------------------------��| (BLOCKED) |
									  | (TERMINATED) |     �ȴ�����synchronized����  +-----------+
									  +--------------+     �ȴ�����synchronized��                
									                                     
**/


#pragma once//��ֹ�ظ�����
#include "Thread.h"
#include "UtilTypes.h"
#include "ClassFileDecoder.h"
#include "AccessFlagDecoder.h"
#include "InstDecoder.h"
#include <iostream>
#include <windows.h>


//�̵߳�����
class ThreadScheduler{

	private:	
		
		//�߳������������߳�id��ֵ���̣߳�
		map<int,Thread*> newThreads;			//��ʼ̬�߳�����
		map<int,Thread*> runnableThreads;		//����̬�߳�����(�������е�+�������߳�,���ܹ����е��߳�)
		map<int,Thread*> blockedThreads;		//����̬�߳�����
		map<int,Thread*> waitingThreads;		//�ȴ�̬�߳�����
		map<int,Thread*> timeWaitingThreads;	//��ʱ�ȴ�̬�߳�����

		map<int,Thread*>::iterator mit;			//�߳�����������

		//�������е��߳�
		Thread* runningThread;

		JInst* instTable;						//ָ�
		int flag[300];							//ջ֡���±�־
		bool logInst;							//����,��ʾִ�в���
		int instNum;							//ִ�е�ָ������

		int threadId;							//�����߳�id;

		long getTime(){
			return getNS();
		}

		//ִ��ĳ�߳�
		//�����<clinit>,��ûִ�����ǰ�����л�!
		void loop(int time){
			
			Stack* stack = runningThread->stack;
			Frame* currentFrame = stack->getStackTop();
			Method* m = currentFrame->method;
			
			int opNum=0;
			int interruptible = false;			//ջ֡�Ƿ��ܱ��ж�
			int interruptibleFrameDepth = 0;	//������ܱ��жϵĻ���ջ֡����Ƕ���(-1������Ա��ж�)

			while(stack->top > 0){
				
				++instNum;
				if(flag[opNum]==1){//�����ض���ָ��Ÿ��µ�ǰջ֡
					currentFrame=stack->getStackTop();
					m=currentFrame->method;

					//���ջ֡A����Ϊ�๹�췽������֤���ܹ�ִ����ɡ�
					//��ջ֡Aջ����ڵ�ǰջ����С(ջ֡A�Ѿ���������ջ��)
					isEqual((u1*)m->name,(u1*)"<clinit>") ? interruptible = false,interruptibleFrameDepth=stack->top
						: interruptible = true;
				
				}
				runningThread->pc=currentFrame->pc;
				opNum=currentFrame->readIns();		//����
				if(logInst==true){//����,��ʾִ�в���
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
			if(stack->top == 0)	runnableThreads.erase(runningThread->id);	//�߳�ִ����ϣ��Ƴ�
		}

		//��������㷨
		void randomScheduleThread(){
			int index = getUS() % runnableThreads.size();
			mit = runnableThreads.begin();
			for(mit = runnableThreads.begin();--index>=0 ;mit++);
			runningThread = mit->second;
		}
	

	public:

		//�����̵߳�����
		int startThreadScheduler(){
			while(runnableThreads.size()>0){
				loop(1000);				//����ִ��1000��ָ����л�һ���߳�
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
				//��������ָ�����flag[i]=1�������ڲ�����Ϊi��ָ��������ջ֡
				if((i>=0xb2 && i<=0xb9) || (i>=0xac && i<=0xb1) || i==0xb2 || i==0xbb || i==0xfe){
					flag[i]=1;
				}
			}
			mainThread->id = ++threadId;
			runnableThreads[mainThread->id] = mainThread;

			//�������̶߳�Ӧ���߳�Object-------------------------------------------------------------------------
			MethodAreaClass* threadClass = loadNonArrayClass(ma,"java/lang/Thread",false);
			Object* mainThreadObject = newObject(threadClass);

			//�������̶߳����tid
			int tIdSlotId = mainThreadObject->objClass->floader->getFieldByName("tid")->slotId;
			mainThreadObject->fields[tIdSlotId]->num = mainThread->id;
			mainThreadObject->fields[tIdSlotId]->flag = 3;

			//��ȡ���̶߳����name�ֶ�(char����)threadNameField
			int threadNameSlotId = mainThreadObject->objClass->floader->getFieldByName("name")->slotId;
			Slot* threadNameField = mainThreadObject->fields[threadNameSlotId];

			//��threadName��value�ֶ�(char����)��ֵ��Thread�����threadNameField
			threadNameField->flag = 10;
			threadNameField->ref = newPrimitiveTypesArray(ma,5,4);
			threadNameField->ref->array->B1Array = (b1*)mainThread->name;

			//-----------------------------------------------------------------------------------------------------

			mainThread->threadObject = mainThreadObject;
			runningThread = mainThread;
		}


		//ע���߳�(�½�����δ����start����)
		//runMethod�����Ǹ��߳�Ҫִ�еķ�����
		//�����new Thread(),���runMethodΪThread������������run����
		//�����new Thread(Runnable����),��runMethodΪ��ʵ��Runnable�ӿڵ����run����
		void newThread(Thread* newThread,Method* runMethod){
			newThread->id = ++threadId;
			newThreads[newThread->id] = newThread;
				
			//�½�ջ֡(run����,����ѹ���߳�)
			Frame* newFrame=(Frame*)malloc(sizeof(Frame));
			newFrame->newFrame(newThread,runMethod);
			newThread->stack->pushFrame(newFrame);	//ѹ���߳�	

			newFrame->localVars[0]->flag = 10;
			newFrame->localVars[0]->ref = newThread->threadObject;
		}

		//�߳�����(��ʼ̬ת��������̬),��run����ѹ��ջ
		//����java.lang.Thread.start������ִ��
		void startThread(int threadId){

			Thread* thread = newThreads[threadId];
			Object* threadObject = thread->threadObject;

			if(thread != NULL){//��run����ѹ���߳�ջ֡
				runnableThreads[threadId] = thread;
				newThreads.erase(threadId);
			}
		}

		//�߳�����(����̬ת�ȴ�̬)
		void blockedThread(int threadId){
			Thread* thread = runnableThreads[threadId];
			if(thread != NULL){
				blockedThreads[threadId] = thread;
				runnableThreads.erase(threadId);
			}
		}

		//�̵߳ȴ�(�ȴ�̬ת����̬)
		void waitingThread(){

		}

		//�̻߳���(�ȴ�̬ת����̬)
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
