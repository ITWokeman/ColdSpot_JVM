#pragma once
#include "..\headers\VMErrorPrinter.h"

//报错
void printError(int errorNum,string info[]){
	color(4);
	printf("\n-----------------------------------------------------------------------------------------");

	switch(errorNum){

	case 0x000:
		printf("\n[错误0x000]找不到$JAVA_HOME路径%s\n请在C:\\my_jvm_config\\config.ini下为$JAVA_HOME参数配置正确路径!\n",info[0].c_str());
		break;
	//未找到.class文件
	case 0x001:
		printf("\n[错误0x001]找不到%s文件!\n请确定输入名称是否正确\n",info[0].c_str());
		printf("\n或到 %s\\config\\CLASSPATH下正确配置路径(使用txt编辑)\n",info[1].c_str());
		break;

	//常量池解析异常
	case 0x003:printf("\n[错误0x003]%s.class常量池解析异常!\n",info[0].c_str());break;
	
	//虚拟机无法识别指令
	case 0x004:printf("\n[错误0x004]无法识别 \" %s \" 指令!\n",info[0].c_str());break;

	//访问权限错误
	case 0x005:printf("\n[错误0x005]访问权限错误：无法访问 \" %s \" 类\n",info[0].c_str());break;

	case 0x006:printf("\n[错误0x006]找不到主方法 public static void main(String[] arg)! \n");break;

	case 0x008:printf("\n[错误0x008]找不到方法 %s!\n",info[0].c_str());break;

	case 0x009:printf("\n[错误0x009]权限不足,无法访问方法 %s!\n",info[0].c_str());break;

	case 0x010:printf("\n[错误0x010]%s是接口或抽象类,无法实例化!\n",info[0].c_str());break;

	case 0x011:printf("\n[错误0x011]找不到%s类(接口)的引用指针!\n",info[0].c_str());break;

	case 0x012:printf("\n[错误0x012]无法通过%s类引用指针到%s类信息!\n",info[0].c_str());break;

	case 0x013:printf("\n[错误0x013]空指针异常!\n");break;

	case 0x014:printf("\n[错误0x014]虚拟机栈溢出!\n");break;

	case 0x015:printf("\n[错误0x015]数组拷贝异常!\n");break;

	case 0x016:printf("\n[错误0x016]找不到本地方法%s!\n",info[0].c_str());break;

	case 0x017:printf("\n[错误0x017]数组越界! 数组长度:%s   越界索引:%s\n",info[0].c_str(),info[1].c_str());break;

	case 0x018:printf("\n[错误0x018]堆内存溢出!\n");break;

	case 0x019:printf("\n[错误0x019]int型数据输入错误! 错误原因:%s\n",info[0].c_str());break;

	default:break;
	}
	printf("-----------------------------------------------------------------------------------------\n");
	color(7);
	exit(1);
}