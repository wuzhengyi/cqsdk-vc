/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
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

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;
void saveToFile(vector<string> order);
vector<string> openFromFile();
void clearFile();

void saveToCSV(vector<string> order);
vector<string> openFromCSV(const char * group);
void clearCSV(const char * group);
int CheckDir(char* Dir);

/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	stringstream ss(msg);  // 字符流ss 
	string buf;
	ss >> buf;
	if(buf.compare("获取订单")==0) {		
		//if (fromQQ != 914349145) {
		//	CQ_sendPrivateMsg(ac, fromQQ, "无权限");
		//	return EVENT_IGNORE;
		//}
		ss >> buf;
		vector<string> s = openFromCSV(buf.c_str());
		buf += "订单如下:";
		CQ_sendPrivateMsg(ac, fromQQ, buf.c_str());	
		vector<string>::iterator iter = s.begin();
		while (iter != s.end()) {
			CQ_sendPrivateMsg(ac, fromQQ, (*iter).c_str());
			iter++;
		}
		return EVENT_BLOCK;
	}
	else if (buf.compare("清空订单") == 0) {
		ss >> buf;
		clearCSV(buf.c_str());
		CQ_sendPrivateMsg(ac, fromQQ, "删除成功");
		
	}
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	time_t rawtime;
	time(&rawtime);
	char fileName[20];
	strftime(fileName, 20, "%Y-%m-%d", localtime(&rawtime));
	//CQ_sendGroupMsg(ac, fromGroup, fileName);	
	string s = msg;
	string buf;
	stringstream ss(s);  // 字符流ss 
	vector<string> tokens;  // vector
	ss >> buf;
	if (buf.compare("买") != 0 && buf.compare("卖") != 0) {
		CQ_sendGroupMsg(ac, fromGroup, "首字符不匹配\n请按照格式发送,空白项填写'无'，中间有空格。\
			\n\"买/卖 [型号] [电压等级] [规格] [数量] [电话] [到货地点] [价格] [备注]\"\n\
			例如\"买 yjv22 35v 50米 3*300 188****5175 江苏省 面谈 无\"");
		return EVENT_IGNORE;
	}
	if (buf.compare("卖") == 0) {
		tokens.push_back(to_string(fromGroup)+"卖");
		tokens.push_back(to_string(fromQQ));
		while (ss >> buf)
			tokens.push_back(buf);
		if (tokens.size() < 7 || tokens.size()>10) {
			CQ_sendGroupMsg(ac, fromGroup, "标签个数不匹配\n请按照格式发送,空白项填写'无'，中间有空格。\
			\n\"买/卖 [型号] [电压等级] [规格] [数量] [电话] [到货地点] [价格] [备注]\"\n\
			例如\"买 yjv22 35v 50米 3*300 188****5175 江苏省 面谈 无\"");
			return EVENT_IGNORE;
		}
		tokens.push_back("无");
		tokens.push_back("无");
		tokens.push_back("无");
		tokens.push_back("无");
		tokens[10] = msg;
		saveToCSV(tokens);
	}
	else {
		tokens.push_back(to_string(fromGroup));
		tokens.push_back(to_string(fromQQ));
		while (ss >> buf)
			tokens.push_back(buf);
		if (tokens.size() < 7 || tokens.size()>10) {
			CQ_sendGroupMsg(ac, fromGroup, "标签个数不匹配\n请按照格式发送,空白项填写'无'，中间有空格。\
			\n\"买/卖 [型号] [电压等级] [规格] [数量] [电话] [到货地点] [价格] [备注]\"\n\
			例如\"买 yjv22 35v 50米 3*300 188****5175 江苏省 面谈 无\"");
			return EVENT_IGNORE;
		}
		tokens.push_back("无");
		tokens.push_back("无");
		tokens.push_back("无");
		tokens.push_back("无");
		tokens[10] = msg;
		saveToCSV(tokens);
	}
	
	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
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
			return 1;//文件夹创建成功
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
//删除字符串中空格，制表符tab等无效字符  
string Trim(string& str)
{
	//str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置  
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
	vector<string> orders; //声明一个字符串向量  
	string line;
	while (getline(file, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取  
	{
		istringstream sin(line); //将整行字符串line读入到字符串流istringstream中  
		
		string field;
		string order;
		getline(sin, field, ',');
		order = field + ":";
		while (getline(sin, field, ',')) //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符  
		{
			order += "\n" + field;
		}
		orders.push_back(order); //将刚刚读取的字符串添加到向量orders中  
	}
	return orders;
}

void clearCSV(const char* group)
{
	char fileName[100];
	getFileName(group, fileName);
	remove(fileName);
}

