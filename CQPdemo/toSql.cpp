#include <winsock.h> //ע��˳��Ҫ����mysql.hǰ
#include <mysql.h>//����̨��Ŀ��Ҫ��mysql.h֮ǰinclude <winsock.h>
void test1()
{
	MYSQL *pConn;
	pConn = mysql_init(NULL);
	//��2��3��4��5��������˼�ֱ��ǣ���������ַ���û��������롢���ݿ�������6��Ϊmysql�˿ںţ�0ΪĬ��ֵ3306��
	if (!mysql_real_connect(pConn, "localhost", "root", "root", "test", 0, NULL, 0))
	{
		printf("�޷��������ݿ�:%s", mysql_error(pConn));
		return;
	}
	mysql_query(pConn, "set names gbk");//��ֹ���롣���ú����ݿ�ı���һ�¾Ͳ�������
										//SET NAMES x �൱�� SET character_set_client = x;SET character_set_results = x;SET character_set_connection = x;
										//дset character set gbk;��ѯ�������룬���ǲ���������ᱨ����set names gbk�򶼲�������

										//mysql_real_query��mysql_query���˸�����: �ַ���query�ĳ���, �����ʺ��ж��������ݵ�query, ��mysql_query���ַ���query���ܰ���������,��Ϊ����\0Ϊ��β
										//mysql_query() ���ܴ�������BLOB�ֶΣ���Ϊ��������Ϣ�е�\0�ᱻ����Ϊ�������� mysql_real_query() ����ԡ�
	if (mysql_query(pConn, "select * from persons"))
	{
		printf("��ѯʧ��:%s", mysql_error(pConn));
		return;
	}
	//mysql_store_result�ǰѲ�ѯ���һ����ȡ���ͻ��˵��������ݼ���������Ƚϴ�ʱ���ڴ档
	//mysql_use_result���ǲ�ѯ������ڷ������ϣ��ͻ���ͨ��ָ�����ж�ȡ����ʡ�ͻ����ڴ档����һ��MYSQL*����ͬʱֻ����һ��δ�رյ�mysql_use_result��ѯ
	MYSQL_RES *result = mysql_store_result(pConn);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result))
	{
		printf("%s %s\n", row[1], row[2]);
	}
	mysql_free_result(result);
	mysql_close(pConn);
}