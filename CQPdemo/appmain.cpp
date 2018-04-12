/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdio>
#include <direct.h>
#include <stdlib.h>
#include <memory>

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;
void saveToFile(vector<string> order);
vector<string> openFromFile();
void clearFile();

void saveToCSV(vector<string> order);
vector<string> openFromCSV(const char * group);
void clearCSV(const char * group);
int CheckDir(char* Dir);

/* 
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	stringstream ss(msg);  // �ַ���ss 
	string buf;
	ss >> buf;
	if(buf.compare("��ȡ����")==0) {		
		//if (fromQQ != 914349145) {
		//	CQ_sendPrivateMsg(ac, fromQQ, "��Ȩ��");
		//	return EVENT_IGNORE;
		//}
		ss >> buf;
		vector<string> s = openFromCSV(buf.c_str());
		buf += "��������:";
		CQ_sendPrivateMsg(ac, fromQQ, buf.c_str());	
		vector<string>::iterator iter = s.begin();
		while (iter != s.end()) {
			CQ_sendPrivateMsg(ac, fromQQ, (*iter).c_str());
			iter++;
		}
		return EVENT_BLOCK;
	}
	else if (buf.compare("��ն���") == 0) {
		ss >> buf;
		clearCSV(buf.c_str());
		CQ_sendPrivateMsg(ac, fromQQ, "ɾ���ɹ�");
		
	}
	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	time_t rawtime;
	time(&rawtime);
	char fileName[20];
	strftime(fileName, 20, "%Y-%m-%d", localtime(&rawtime));
	//CQ_sendGroupMsg(ac, fromGroup, fileName);	
	string s = msg;
	string buf;
	stringstream ss(s);  // �ַ���ss 
	vector<string> tokens;  // vector
	ss >> buf;
	if (buf.compare("��") != 0 && buf.compare("��") != 0) {
		CQ_sendGroupMsg(ac, fromGroup, "���ַ���ƥ��\n�밴�ո�ʽ����,�հ�����д'��'���м��пո�\
			\n\"��/�� [�ͺ�] [��ѹ�ȼ�] [���] [����] [�绰] [�����ص�] [�۸�] [��ע]\"\n\
			����\"�� yjv22 35v 50�� 3*300 188****5175 ����ʡ ��̸ ��\"");
		return EVENT_IGNORE;
	}
	if (buf.compare("��") == 0) {
		tokens.push_back(to_string(fromGroup)+"��");
		tokens.push_back(to_string(fromQQ));
		while (ss >> buf)
			tokens.push_back(buf);
		if (tokens.size() < 7 || tokens.size()>10) {
			CQ_sendGroupMsg(ac, fromGroup, "��ǩ������ƥ��\n�밴�ո�ʽ����,�հ�����д'��'���м��пո�\
			\n\"��/�� [�ͺ�] [��ѹ�ȼ�] [���] [����] [�绰] [�����ص�] [�۸�] [��ע]\"\n\
			����\"�� yjv22 35v 50�� 3*300 188****5175 ����ʡ ��̸ ��\"");
			return EVENT_IGNORE;
		}
		tokens.push_back("��");
		tokens.push_back("��");
		tokens.push_back("��");
		tokens.push_back("��");
		tokens[10] = msg;
		saveToCSV(tokens);
	}
	else {
		tokens.push_back(to_string(fromGroup));
		tokens.push_back(to_string(fromQQ));
		while (ss >> buf)
			tokens.push_back(buf);
		if (tokens.size() < 7 || tokens.size()>10) {
			CQ_sendGroupMsg(ac, fromGroup, "��ǩ������ƥ��\n�밴�ո�ʽ����,�հ�����д'��'���м��пո�\
			\n\"��/�� [�ͺ�] [��ѹ�ȼ�] [���] [����] [�绰] [�����ص�] [�۸�] [��ע]\"\n\
			����\"�� yjv22 35v 50�� 3*300 188****5175 ����ʡ ��̸ ��\"");
			return EVENT_IGNORE;
		}
		tokens.push_back("��");
		tokens.push_back("��");
		tokens.push_back("��");
		tokens.push_back("��");
		tokens[10] = msg;
		saveToCSV(tokens);
	}
	
	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}

void saveToFile(vector<string> order)
{
	time_t rawtime;
	time(&rawtime);
	char tmp[20];
	strftime(tmp, 20, "%Y-%m-%d", localtime(&rawtime));
	//cout << tmp << endl;
	char fileName[100] = "data/order/";
	strcat(fileName, tmp);
	strcat(fileName, ".txt");
	ofstream file(fileName, ios::app | ios::out);
	vector<string>::iterator iter = order.begin();
	while (iter != order.end()) {
		file << *iter << " ";
		iter++;
	}
	file << "\n";
	file.close();
}

vector<string> openFromFile()
{
	vector<string> order;
	time_t rawtime;
	time(&rawtime);
	char tmp[20];
	strftime(tmp, 20, "%Y-%m-%d", localtime(&rawtime));
	//cout << tmp << endl;
	char fileName[100] = "data/order/";
	strcat(fileName, tmp);
	strcat(fileName, ".txt");

	ifstream file(fileName);
	vector<string>::iterator iter = order.begin();
	string  s;
	while (getline(file, s))
	{
		order.push_back(s);
	}
	file.close();
	return order;
}

void clearFile()
{
	time_t rawtime;
	time(&rawtime);
	char tmp[20];
	strftime(tmp, 20, "%Y-%m-%d", localtime(&rawtime));
	//cout << tmp << endl;
	char fileName[100] = "data/order/";
	strcat(fileName, tmp);
	strcat(fileName, ".txt");
	remove(fileName);
}

int CheckDir(char* Dir)
{
	FILE *fp = NULL;
	char TempDir[200];
	memset(TempDir, '\0', sizeof(TempDir));
	sprintf(TempDir, Dir);
	strcat(TempDir, "\\");
	strcat(TempDir, ".temp.fortest");
	fp = fopen(TempDir, "w");
	if (!fp)
	{
		if (_mkdir(Dir) == 0)
		{
			return 1;//�ļ��д����ɹ�
		}
		else
		{
			return -1;//can not make a dir;
		}
	}
	else
	{
		fclose(fp);
	}
	return 0;
}

void getFileName(const char* group, char* fileName ) {
	time_t rawtime;
	time(&rawtime);
	char tmp[20];
	strftime(tmp, 20, "%Y-%m-%d", localtime(&rawtime));
	//cout << tmp << endl;
	strcpy(fileName, "data/order/");
	strcat(fileName, tmp);
	CheckDir(fileName);
	strcat(fileName, "/");
	strcat(fileName, group);
	strcat(fileName, ".csv");
}

void saveToCSV(vector<string> order)
{
	char fileName[100];
	int i = 0;
	getFileName(order[0].c_str(),fileName);
	i++;
	ofstream file(fileName, ios::app);
	while (i<=10) {
		file << order[i] << ",";
		i++;
	}
	file << endl;
	file.close();
}
//ɾ���ַ����пո��Ʊ��tab����Ч�ַ�  
string Trim(string& str)
{
	//str.find_first_not_of(" \t\r\n"),���ַ���str�д�����0��ʼ�������״β�ƥ��"\t\r\n"��λ��  
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
	return str;
}

vector<string> openFromCSV(const char* group)
{
	vector<string> order;
	char fileName[100];
	getFileName(group, fileName);
	ifstream file(fileName);
	vector<string> orders; //����һ���ַ�������  
	string line;
	while (getline(file, line))   //���ж�ȡ�����з���\n�����֣������ļ�β��־eof��ֹ��ȡ  
	{
		istringstream sin(line); //�������ַ���line���뵽�ַ�����istringstream��  
		
		string field;
		string order;
		getline(sin, field, ',');
		order = field + ":";
		while (getline(sin, field, ',')) //���ַ�����sin�е��ַ����뵽field�ַ����У��Զ���Ϊ�ָ���  
		{
			order += "\n" + field;
		}
		orders.push_back(order); //���ոն�ȡ���ַ�����ӵ�����orders��  
	}
	return orders;
}

void clearCSV(const char* group)
{
	char fileName[100];
	getFileName(group, fileName);
	remove(fileName);
}

