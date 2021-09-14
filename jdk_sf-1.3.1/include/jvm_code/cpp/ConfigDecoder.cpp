
#include "..\headers\ConfigDecoder.h"

string getConfig(){
	//����ColdSpot_JVM.exe��JAVA_HOME\\bin\\ColdSpot_JVM.exe����ȡJAVA_HOME
	char szapipath[1000];
	GetModuleFileNameA(NULL,szapipath,1000);
	int index=999;
	while(szapipath[index--]!='\\');//������һ��(XXXX\\jdkĿ¼\\bin)
	while(szapipath[index--]!='\\');//�ٷ�����һ��(XXXX\\jdkĿ¼)
	szapipath[++index]='\0';//��������н�ȡ,���JAVA_HOME:XXXX\\jdkĿ¼

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

	//�����ļ�
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