
#include "..\headers\ConfigDecoder.h"

string getConfig(){
	//根据ColdSpot_JVM.exe在JAVA_HOME\\bin\\ColdSpot_JVM.exe来获取JAVA_HOME
	char szapipath[1000];
	GetModuleFileNameA(NULL,szapipath,1000);
	int index=999;
	while(szapipath[index--]!='\\');//返回上一级(XXXX\\jdk目录\\bin)
	while(szapipath[index--]!='\\');//再返回上一级(XXXX\\jdk目录)
	szapipath[++index]='\0';//在这里进行截取,获得JAVA_HOME:XXXX\\jdk目录

	string path;
	path.append(szapipath);
	if(_access(path.c_str(),0) == -1){
		string info[1]={path};
		printError(0x000,info);
	}

	path.append("\\config\\CONFIG.INI");
	return path;
}

ConfigInfo* ConfigDecoder(){

	ConfigInfo* config = new ConfigInfo();

	string path = getConfig();

	//解析文件
	ifstream infile(path.c_str());
	string temp;
	while(getline(infile,temp)){
		char* lineVars = (char*)temp.c_str();
		if(lineVars[0]=='$'){
			int index=1;
			string key;
			string val;
			while(lineVars[index]!='='){
				key+=lineVars[index++];
			}
			++index;
			while(lineVars[index]!='\0' && lineVars[index]!='#' &&  lineVars[index]!='\n'){
				val+=lineVars[index++];
			}
			config->setCofing(key,val);
		}
	}

	return config;
}