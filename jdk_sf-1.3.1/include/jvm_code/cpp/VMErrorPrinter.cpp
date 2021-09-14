#pragma once
#include "..\headers\VMErrorPrinter.h"

//����
void printError(int errorNum,string info[]){
	color(4);
	printf("\n-----------------------------------------------------------------------------------------");

	switch(errorNum){

	case 0x000:
		printf("\n[����0x000]�Ҳ���$JAVA_HOME·��%s\n����C:\\my_jvm_config\\config.ini��Ϊ$JAVA_HOME����������ȷ·��!\n",info[0].c_str());
		break;
	//δ�ҵ�.class�ļ�
	case 0x001:
		printf("\n[����0x001]�Ҳ���%s�ļ�!\n��ȷ�����������Ƿ���ȷ\n",info[0].c_str());
		printf("\n�� %s\\config\\CLASSPATH����ȷ����·��(ʹ��txt�༭)\n",info[1].c_str());
		break;

	//�����ؽ����쳣
	case 0x003:printf("\n[����0x003]%s.class�����ؽ����쳣!\n",info[0].c_str());break;
	
	//������޷�ʶ��ָ��
	case 0x004:printf("\n[����0x004]�޷�ʶ�� \" %s \" ָ��!\n",info[0].c_str());break;

	//����Ȩ�޴���
	case 0x005:printf("\n[����0x005]����Ȩ�޴����޷����� \" %s \" ��\n",info[0].c_str());break;

	case 0x006:printf("\n[����0x006]�Ҳ��������� public static void main(String[] arg)! \n");break;

	case 0x008:printf("\n[����0x008]�Ҳ������� %s!\n",info[0].c_str());break;

	case 0x009:printf("\n[����0x009]Ȩ�޲���,�޷����ʷ��� %s!\n",info[0].c_str());break;

	case 0x010:printf("\n[����0x010]%s�ǽӿڻ������,�޷�ʵ����!\n",info[0].c_str());break;

	case 0x011:printf("\n[����0x011]�Ҳ���%s��(�ӿ�)������ָ��!\n",info[0].c_str());break;

	case 0x012:printf("\n[����0x012]�޷�ͨ��%s������ָ�뵽%s����Ϣ!\n",info[0].c_str());break;

	case 0x013:printf("\n[����0x013]��ָ���쳣!\n");break;

	case 0x014:printf("\n[����0x014]�����ջ���!\n");break;

	case 0x015:printf("\n[����0x015]���鿽���쳣!\n");break;

	case 0x016:printf("\n[����0x016]�Ҳ������ط���%s!\n",info[0].c_str());break;

	case 0x017:printf("\n[����0x017]����Խ��! ���鳤��:%s   Խ������:%s\n",info[0].c_str(),info[1].c_str());break;

	case 0x018:printf("\n[����0x018]���ڴ����!\n");break;

	case 0x019:printf("\n[����0x019]int�������������! ����ԭ��:%s\n",info[0].c_str());break;

	default:break;
	}
	printf("-----------------------------------------------------------------------------------------\n");
	color(7);
	exit(1);
}