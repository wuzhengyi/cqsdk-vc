#include <winsock.h> //注意顺序，要放在mysql.h前
#include <mysql.h>//控制台项目中要在mysql.h之前include <winsock.h>
void test1()
{
	MYSQL *pConn;
	pConn = mysql_init(NULL);
	//第2、3、4、5参数的意思分别是：服务器地址、用户名、密码、数据库名，第6个为mysql端口号（0为默认值3306）
	if (!mysql_real_connect(pConn, "localhost", "root", "root", "test", 0, NULL, 0))
	{
		printf("无法连接数据库:%s", mysql_error(pConn));
		return;
	}
	mysql_query(pConn, "set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码
										//SET NAMES x 相当于 SET character_set_client = x;SET character_set_results = x;SET character_set_connection = x;
										//写set character set gbk;查询不会乱码，但是参数化插入会报错。而set names gbk则都不会乱码

										//mysql_real_query比mysql_query多了个参数: 字符串query的长度, 所以适合有二进制数据的query, 而mysql_query的字符串query不能包含二进制,因为它以\0为结尾
										//mysql_query() 不能传二进制BLOB字段，因为二进制信息中的\0会被误判为语句结束。 mysql_real_query() 则可以。
	if (mysql_query(pConn, "select * from persons"))
	{
		printf("查询失败:%s", mysql_error(pConn));
		return;
	}
	//mysql_store_result是把查询结果一次性取到客户端的离线数据集，当结果比较大时耗内存。
	//mysql_use_result则是查询结果放在服务器上，客户端通过指针逐行读取，节省客户端内存。但是一个MYSQL*连接同时只能有一个未关闭的mysql_use_result查询
	MYSQL_RES *result = mysql_store_result(pConn);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result))
	{
		printf("%s %s\n", row[1], row[2]);
	}
	mysql_free_result(result);
	mysql_close(pConn);
}