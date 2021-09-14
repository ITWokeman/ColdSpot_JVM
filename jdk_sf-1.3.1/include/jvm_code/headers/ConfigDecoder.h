
#pragma once
#include "ClassLoader.h"
#include <map>

class ConfigInfo{
	map<string,string> infoMap;
	map<string,string>::iterator mit;
	vector<string> bootPaths;
	vector<string> extPaths;
	vector<string> appPaths;

	int isEqual(u1* str1,u1* str2){
		int len1=0;
		int len2=0;

		while(str1[len1++]);
		while(str2[len2++]);
	
		if(len1 != len2){return 0;}
		for(int i=0;i<len1-1;++i){
			if(str1[i]!=str2[i]){return 0;}
		}
		return 1;
	}

	public:
		void setCofing(string key,string value){
			if(isEqual((u1*)key.c_str(),(u1*)"BOOTCLASSPATH") == 1){
				bootPaths.push_back(value);
				return;
			}
			if(isEqual((u1*)key.c_str(),(u1*)"EXTCLASSPATH") == 1){
				extPaths.push_back(value);
				return;
			}
			if(isEqual((u1*)key.c_str(),(u1*)"APPCLASSPATH") == 1){
				appPaths.push_back(value);
				return;
			}
			

			infoMap[key] = value;
		}

		string getConfig(string key){
			return infoMap[key];
		}

		void showConfig(){
			for(mit = infoMap.begin();mit!=infoMap.end();mit++){
				printf("\n参数名:%s 参数:%s",mit->first.c_str(),mit->second.c_str());
			}
			showBootPaths();
			showExtPaths();
			showAppPaths();
		}

		void showBootPaths(){
			printf("\n\n启动类加载器路径:");
			for(int i=0;i<bootPaths.size();++i){
				printf("\n路径%d:	$JAVA_HOME\\%s",i+1,bootPaths[i].c_str());
			}
		}

		void showExtPaths(){
			printf("\n\n扩展类加载器路径:");
			for(int i=0;i<extPaths.size();++i){
				printf("\n路径%d:	$JAVA_HOME\\%s",i+1,extPaths[i].c_str());
			}
		}

		void showAppPaths(){
			printf("\n\n应用类加载器路径:");
			for(int i=0;i<appPaths.size();++i){
				printf("\n路径%d:	%s",i+1,appPaths[i].c_str());
			}
		}


		vector<string> getBootPaths(){
			return bootPaths;
		}

	    vector<string> getExtPaths(){
			return extPaths;
		}

	    vector<string> getAppPaths(){
			return appPaths;
		}



};

ConfigInfo* ConfigDecoder();
