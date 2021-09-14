/**
 .class文件读取器	
 
 1 作用
   (1) 在classpath配置文件路径按名寻找.class文件

 2 对外提供方法		
   (1)fileReader		
   在classpath下找到并读取对应文件字节码，并返回读取内容
   
   (2)showPaths
   显示路径配置文件内容

**/
#pragma once//防止重复定义
#include "..\headers\ClassLoader.h"
#include <direct.h>

//改一下。缓存读取过的jar包，能节省很多时间
FileData* jarReader(string path,string className){

	int err;
	char szFileName[200];

	//声明结构体
	unz_global_info global_info;
	unz_file_info file_info;
	unzFile uzf;

	//打开压缩文件
	uzf = unzOpen((char*)path.c_str());

	//定位到指定文件
	err = unzLocateFile(uzf,(char*)className.c_str(), 0);
	if (UNZ_OK != err){
		return NULL;
	}
	//获取当前选择的内部压缩文件的信息
	err = unzGetCurrentFileInfo(uzf, &file_info, szFileName, sizeof(szFileName), NULL, 0, NULL, 0);

	if (UNZ_OK != err){
		return NULL;
	}

	//选择打开定位到的文件
	err = unzOpenCurrentFile(uzf);
	if (err != UNZ_OK){
		return NULL;
	}

	//读取内容
	int size = file_info.uncompressed_size+1;
	unsigned char* data = (unsigned char*)malloc(size); //动态分配内存

	unzReadCurrentFile(uzf,data, file_info.uncompressed_size);
	FILE* fp = fopen("./1.dex","wb+"); 
	fwrite(data,1,size,fp);	//每次写入1字节的内容，总共写入size次
	data[size]='\0';

	//关闭文件
	unzCloseCurrentFile(uzf);
	unzClose(uzf);
	fclose(fp);

	return new FileData(data,size);
 
}

//读取文件内容
FileData *readFile(string absPath){//绝对路径
	
	const char *path=absPath.data();
	
	FILE *fp = fopen(path, "rb");
	if(fp==NULL){
		return NULL;
	}

	u1* fdata;//字节数组
  
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);//文件大小
	fseek(fp, 0, SEEK_SET);
	if(size > 0){
		fdata = (u1*)calloc(size+1,sizeof(u1));
	}

	/*读文件内容存入内存*/
	fread(fdata, size, 1, fp);
	fclose(fp);
	fdata[size] = '\0';
	return new FileData(fdata,size);
}

//寻找某个文件（绝对路径）
//如果有多个同名文件的话，只会返回最后一个找到的文件！
//建议改成非递归，以提高文件查找效率
//参数:文件目录，类文件全称，类文件包名，类文件名，获取寻找文件的路径，存放字节码的fileData，是否测试模式
FileData* findFile(string classpath,string fullname,string cname,string pname,string &findpath,bool testing){//文件句柄 

	long hFile = 0;
	
	struct _finddata_t fInfo;//文件信息
	string p;
	if ((hFile = _findfirst(p.assign(classpath).append("\\*").c_str(), &fInfo)) != -1){
		do{

			if ((fInfo.attrib & _A_SUBDIR)){
				if (strcmp(fInfo.name, ".") != 0 && strcmp(fInfo.name, "..") != 0){
					return findFile(p.assign(classpath).append("\\\\").append(fInfo.name),fullname,cname,pname,findpath,testing);		
				}
			}else{
				if(suffix(fInfo.name,".jar")==true){//读取jar
					string p="";
					p.append(classpath).append("\\\\").append(fInfo.name);
					FileData* data=jarReader(p,fullname);
					if(data!=NULL){
						if(testing==true){
							printf("\n[在 %s 下找到文件%s!]\n",p.c_str(),fullname.c_str()); 
						}
						return data;
					}
				}

				//非jar
				if(cname==fInfo.name){

					//包名检查(路径必须以包名作为结尾)
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
						printf("\n[在 %s 下找到文件%s!]\n",findpath.c_str(),cname.c_str()); 
					}
					return readFile(findpath);
					
				}
			}
		} while (_findnext(hFile, &fInfo) == 0);
		_findclose(hFile);
	}
	return NULL;
}

//启动类加载器
//加载jre/lib目录下的核心API
class BootstrapClassLoader{
	string javaHome;
	string bootPath;//$JAVA_HOME/jre/lib
	vector<string> bootPaths;
	public :
		BootstrapClassLoader(ConfigInfo* config){
			bootPaths = config->getBootPaths();
			
			//根据ColdSpot_JVM.exe在JAVA_HOME\\bin\\ColdSpot_JVM.exe来获取JAVA_HOME
			char szapipath[1000];
			GetModuleFileNameA(NULL,szapipath,1000);
			int index=999;
			while(szapipath[index--]!='\\');//返回上一级(XXXX\\jdk目录/bin)
			while(szapipath[index--]!='\\');//再返回上一级(XXXX\\jdk目录)
			szapipath[++index]='\0';//在这里进行截取,获得JAVA_HOME:XXXX\\jdk目录

			javaHome.append(szapipath);
			if(_access(javaHome.c_str(),0) == -1){
				string info[1]={javaHome};
				printError(0x000,info);
			}
		}
		
		FileData* loadClassFile(string fullName,string pName,string cName,bool isTest){

			if(isTest==true){
				printf("\n[启动类加载器] 开始查找%s\n↓",fullName.c_str());
			}
			
			FileData* data = NULL;
			for(int i=0;i<bootPaths.size();++i){
				string bootPath;
				bootPath.append(javaHome).append("\\").append(bootPaths[i]);
				data = findFile(bootPath,fullName,cName,pName,string(""),false);
				if(isTest==true){
					printf("\n[启动类加载器] %s%s!(路径%s)\n↓",data==NULL?"找不到":"找到"
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

//扩展类加载器
//加载jre/ext
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
				printf("\n[扩展类加载器] 委托启动类加载器查找\n↓");
			}
			//委托启动类加载器加载.class文件
			FileData* data = btcl->loadClassFile(fullName,pName,cName,isTest);
			if(data==NULL){//启动类加载器没有加载成功

				if(isTest == true){
					printf("\n[扩展类加载器] 委托失败,开始查找%s\n↓",fullName.c_str());
				}
				for(int i=0;i<extPaths.size();++i){
					string extPath;
					extPath.append(javaHome).append("\\").append(extPaths[i]);
					data = findFile(extPath,fullName,cName,pName,string(""),false);
					if(isTest == true){
						printf("\n[扩展类加载器] %s%s!(路径%d:%s)\n↓",data==NULL?"找不到":"找到",fullName.c_str(),i+1,
							extPath.c_str());
					}
					if(data != NULL) return data;
				}
			}else if(isTest == true){
				printf("\n[扩展类加载器] 委托启动类加载器查找成功\n↓");
			}
			return data;
		}

		string getJavaHome(){
			return btcl->getJavaHome();
		}

};

//应用类加载器
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
				printf("\n双亲委派机制查找%s流程\n↓",fullName.c_str());
				printf("\n[应用类加载器] 委托扩展类加载器查找\n↓");
			}

			//委托扩展类加载器加载.class文件
			FileData* data = extcl->loadClassFile(fullName,pName,cName,isTest);
			
			if(data==NULL){//如果扩展类加载器加载失败
				if(isTest == true){
					printf("\n[应用类加载器] 委托失败,开始查找%s\n↓",fullName.c_str());
				}
				for(int i=0;i<appPaths.size();++i){
					string classpath = appPaths[i];
					data = findFile(classpath,fullName,cName,pName,string(""),false);
					if(data!=NULL){
						if(isTest == true){
							printf("\n[应用类加载器] 查找%s成功(路径%d:%s)\n↓",fullName.c_str(),i+1,classpath.c_str());
						}
						data->name=fullName;
						return data;
					}else if(isTest == true){
						printf("\n[应用类加载器] 查找%s失败(路径%d:%s)\n↓",fullName.c_str(),i+1,classpath.c_str());
					}
				}
			}else if(isTest == true){
				printf("\n[应用类加载器] 委托扩展类加载器查找成功\n↓");
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
			printf("\n[找到%s!]\n",findName.c_str());
		}
		return data;
	}else if(isTest == false){
		printf("\n[找不到%s!]\n",findName.c_str());
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