/**
 .class�ļ���ȡ��	
 
 1 ����
   (1) ��classpath�����ļ�·������Ѱ��.class�ļ�

 2 �����ṩ����		
   (1)fileReader		
   ��classpath���ҵ�����ȡ��Ӧ�ļ��ֽ��룬�����ض�ȡ����
   
   (2)showPaths
   ��ʾ·�������ļ�����

**/
#pragma once//��ֹ�ظ�����
#include "..\headers\ClassLoader.h"
#include <direct.h>

//��һ�¡������ȡ����jar�����ܽ�ʡ�ܶ�ʱ��
FileData* jarReader(string path,string className){

	int err;
	char szFileName[200];

	//�����ṹ��
	unz_global_info global_info;
	unz_file_info file_info;
	unzFile uzf;

	//��ѹ���ļ�
	uzf = unzOpen((char*)path.c_str());

	//��λ��ָ���ļ�
	err = unzLocateFile(uzf,(char*)className.c_str(), 0);
	if (UNZ_OK != err){
		return NULL;
	}
	//��ȡ��ǰѡ����ڲ�ѹ���ļ�����Ϣ
	err = unzGetCurrentFileInfo(uzf, &file_info, szFileName, sizeof(szFileName), NULL, 0, NULL, 0);

	if (UNZ_OK != err){
		return NULL;
	}

	//ѡ��򿪶�λ�����ļ�
	err = unzOpenCurrentFile(uzf);
	if (err != UNZ_OK){
		return NULL;
	}

	//��ȡ����
	int size = file_info.uncompressed_size+1;
	unsigned char* data = (unsigned char*)malloc(size); //��̬�����ڴ�

	unzReadCurrentFile(uzf,data, file_info.uncompressed_size);
	FILE* fp = fopen("./1.dex","wb+"); 
	fwrite(data,1,size,fp);	//ÿ��д��1�ֽڵ����ݣ��ܹ�д��size��
	data[size]='\0';

	//�ر��ļ�
	unzCloseCurrentFile(uzf);
	unzClose(uzf);
	fclose(fp);

	return new FileData(data,size);
 
}

//��ȡ�ļ�����
FileData *readFile(string absPath){//����·��
	
	const char *path=absPath.data();
	
	FILE *fp = fopen(path, "rb");
	if(fp==NULL){
		return NULL;
	}

	u1* fdata;//�ֽ�����
  
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);//�ļ���С
	fseek(fp, 0, SEEK_SET);
	if(size > 0){
		fdata = (u1*)calloc(size+1,sizeof(u1));
	}

	/*���ļ����ݴ����ڴ�*/
	fread(fdata, size, 1, fp);
	fclose(fp);
	fdata[size] = '\0';
	return new FileData(fdata,size);
}

//Ѱ��ĳ���ļ�������·����
//����ж��ͬ���ļ��Ļ���ֻ�᷵�����һ���ҵ����ļ���
//����ĳɷǵݹ飬������ļ�����Ч��
//����:�ļ�Ŀ¼�����ļ�ȫ�ƣ����ļ����������ļ�������ȡѰ���ļ���·��������ֽ����fileData���Ƿ����ģʽ
FileData* findFile(string classpath,string fullname,string cname,string pname,string &findpath,bool testing){//�ļ���� 

	long hFile = 0;
	
	struct _finddata_t fInfo;//�ļ���Ϣ
	string p;
	if ((hFile = _findfirst(p.assign(classpath).append("\\*").c_str(), &fInfo)) != -1){
		do{

			if ((fInfo.attrib & _A_SUBDIR)){
				if (strcmp(fInfo.name, ".") != 0 && strcmp(fInfo.name, "..") != 0){
					return findFile(p.assign(classpath).append("\\\\").append(fInfo.name),fullname,cname,pname,findpath,testing);		
				}
			}else{
				if(suffix(fInfo.name,".jar")==true){//��ȡjar
					string p="";
					p.append(classpath).append("\\\\").append(fInfo.name);
					FileData* data=jarReader(p,fullname);
					if(data!=NULL){
						if(testing==true){
							printf("\n[�� %s ���ҵ��ļ�%s!]\n",p.c_str(),fullname.c_str()); 
						}
						return data;
					}
				}

				//��jar
				if(cname==fInfo.name){

					//�������(·�������԰�����Ϊ��β)
					if(pname.size()>classpath.size()){
						return NULL;
					}
					for(int i=classpath.size()-1,j=pname.size()-1;j>=0;--j,--i){
						if(classpath[i] != pname[j]){
							return NULL;
						}
					}
					
					findpath=classpath.append("\\\\").append(fInfo.name);

					if(testing==true){
						printf("\n[�� %s ���ҵ��ļ�%s!]\n",findpath.c_str(),cname.c_str()); 
					}
					return readFile(findpath);
					
				}
			}
		} while (_findnext(hFile, &fInfo) == 0);
		_findclose(hFile);
	}
	return NULL;
}

//�����������
//����jre/libĿ¼�µĺ���API
class BootstrapClassLoader{
	string javaHome;
	string bootPath;//$JAVA_HOME/jre/lib
	vector<string> bootPaths;
	public :
		BootstrapClassLoader(ConfigInfo* config){
			bootPaths = config->getBootPaths();
			
			//����ColdSpot_JVM.exe��JAVA_HOME\\bin\\ColdSpot_JVM.exe����ȡJAVA_HOME
			char szapipath[1000];
			GetModuleFileNameA(NULL,szapipath,1000);
			int index=999;
			while(szapipath[index--]!='\\');//������һ��(XXXX\\jdkĿ¼/bin)
			while(szapipath[index--]!='\\');//�ٷ�����һ��(XXXX\\jdkĿ¼)
			szapipath[++index]='\0';//��������н�ȡ,���JAVA_HOME:XXXX\\jdkĿ¼

			javaHome.append(szapipath);
			if(_access(javaHome.c_str(),0) == -1){
				string info[1]={javaHome};
				printError(0x000,info);
			}
		}
		
		FileData* loadClassFile(string fullName,string pName,string cName,bool isTest){

			if(isTest==true){
				printf("\n[�����������] ��ʼ����%s\n��",fullName.c_str());
			}
			
			FileData* data = NULL;
			for(int i=0;i<bootPaths.size();++i){
				string bootPath;
				bootPath.append(javaHome).append("\\").append(bootPaths[i]);
				data = findFile(bootPath,fullName,cName,pName,string(""),false);
				if(isTest==true){
					printf("\n[�����������] %s%s!(·��%s)\n��",data==NULL?"�Ҳ���":"�ҵ�"
					,fullName.c_str(),bootPath.c_str());
				}
				if(data != NULL) return data;
			}
			return data;
		}

		string getJavaHome(){
			return javaHome;
		}
};

//��չ�������
//����jre/ext
class ExtensionClassLoader{
	BootstrapClassLoader* btcl;	
	string javaHome;
	vector<string> extPaths;

	public :
		ExtensionClassLoader(ConfigInfo* config){
			extPaths = config->getExtPaths();
			btcl = new BootstrapClassLoader(config);
			javaHome = btcl->getJavaHome();
		}
		
		FileData* loadClassFile(string fullName,string pName,string cName,bool isTest){

			if(isTest == true){
				printf("\n[��չ�������] ί�����������������\n��");
			}
			//ί�����������������.class�ļ�
			FileData* data = btcl->loadClassFile(fullName,pName,cName,isTest);
			if(data==NULL){//�����������û�м��سɹ�

				if(isTest == true){
					printf("\n[��չ�������] ί��ʧ��,��ʼ����%s\n��",fullName.c_str());
				}
				for(int i=0;i<extPaths.size();++i){
					string extPath;
					extPath.append(javaHome).append("\\").append(extPaths[i]);
					data = findFile(extPath,fullName,cName,pName,string(""),false);
					if(isTest == true){
						printf("\n[��չ�������] %s%s!(·��%d:%s)\n��",data==NULL?"�Ҳ���":"�ҵ�",fullName.c_str(),i+1,
							extPath.c_str());
					}
					if(data != NULL) return data;
				}
			}else if(isTest == true){
				printf("\n[��չ�������] ί����������������ҳɹ�\n��");
			}
			return data;
		}

		string getJavaHome(){
			return btcl->getJavaHome();
		}

};

//Ӧ���������
class ApplicationClassLoader{

	ExtensionClassLoader* extcl;
	vector<string> appPaths;
	public:
		ApplicationClassLoader(ConfigInfo* config){
			appPaths = config->getAppPaths();
			extcl = new ExtensionClassLoader(config);
		}

		FileData* loadClassFile(string fullName,string pName,string cName,bool isTest){
			if(isTest == true){
				printf("\n˫��ί�ɻ��Ʋ���%s����\n��",fullName.c_str());
				printf("\n[Ӧ���������] ί����չ�����������\n��");
			}

			//ί����չ�����������.class�ļ�
			FileData* data = extcl->loadClassFile(fullName,pName,cName,isTest);
			
			if(data==NULL){//�����չ�����������ʧ��
				if(isTest == true){
					printf("\n[Ӧ���������] ί��ʧ��,��ʼ����%s\n��",fullName.c_str());
				}
				for(int i=0;i<appPaths.size();++i){
					string classpath = appPaths[i];
					data = findFile(classpath,fullName,cName,pName,string(""),false);
					if(data!=NULL){
						if(isTest == true){
							printf("\n[Ӧ���������] ����%s�ɹ�(·��%d:%s)\n��",fullName.c_str(),i+1,classpath.c_str());
						}
						data->name=fullName;
						return data;
					}else if(isTest == true){
						printf("\n[Ӧ���������] ����%sʧ��(·��%d:%s)\n��",fullName.c_str(),i+1,classpath.c_str());
					}
				}
			}else if(isTest == true){
				printf("\n[Ӧ���������] ί����չ����������ҳɹ�\n��");
			}
			return data;
		}

		
		string getJavaHome(){
			return extcl->getJavaHome();
		}
};


ApplicationClassLoader* apcl;

void initClassLoader(ConfigInfo* config){
	apcl = new ApplicationClassLoader(config);
}

FileData* classFileLoader(string findName,bool isTest){

	string suf=".class";

	if(suffix(findName,suf)==false){
		for(int i=0;i<6;findName+=suf[i++]);
	}

	string pName=charArrayToString( getPackageName((char*)findName.data()) );
	string cName=charArrayToString( getClassName((char*)findName.data()) );

	FileData* data = apcl->loadClassFile(findName,pName,cName,isTest);

	if(data != NULL){
		if(isTest == true){
			printf("\n[�ҵ�%s!]\n",findName.c_str());
		}
		return data;
	}else if(isTest == false){
		printf("\n[�Ҳ���%s!]\n",findName.c_str());
	}

	string info[]={findName,apcl->getJavaHome()};
	printError(0x001,info);
	return NULL;
}

void showPaths(ConfigInfo* config){
	config->showBootPaths();
	config->showExtPaths();
	config->showAppPaths();
}

void showJavaHome(ConfigInfo* config){
	BootstrapClassLoader* btcl = new BootstrapClassLoader(config);
	printf("\n$JAVA_HOME:");
	color(4);
	printf("%s\n",btcl->getJavaHome().c_str());
	color(7);
}