// test_LXMysql.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "LXMysql.h"
#include <thread>
using namespace std;
using namespace LX;
int main()
{
	LXMysql my;
	//1 mysql 初始化
	cout << "my.Init() = "<<my.Init() << endl;
	my.SetConnectTimeout(3);	//连接超时秒
	my.SetReconnect(true);		//自动重连

	//2 连接mysql 如果没有调用Init 内部会自动调用
	if (my.Connect("127.0.0.1", "root", "123456", "laoxiaketang"))
	{
		cout << "my.Connect success！" << endl;
	}

	//3 执行sql语句创建表
	string sql = "";

	sql = "CREATE TABLE IF NOT EXISTS `t_video` \
			(`id` INT AUTO_INCREMENT, \
			`name` VARCHAR(1024) , \
			`data` BLOB,\
			`size` INT,PRIMARY KEY(`id`))";
	
	cout<<my.Query(sql.c_str())<<endl;

	//清空数据，包括自增id
	sql = "TRUNCATE t_video";
	my.Query(sql.c_str());


	//测试自动重连
	//sql = "set @TEST=1";
	//for (;;)
	//{
	//	cout << my.Query(sql.c_str())<<flush;
	//}


	//插入一条记录
	//sql = "insert into t_video(name) values('test001')";
	//cout << my.Query(sql.c_str()) << endl;
	//cout << my.Query(sql.c_str()) << endl;
	//cout << my.Query(sql.c_str()) << endl;
	//cout << my.Query(sql.c_str()) << endl;
	//cout << my.Query(sql.c_str()) << endl;
	//cout << my.Query(sql.c_str()) << endl;
	XDATA data1;
	
	//data1.insert(make_pair("name", LXData("insertname001")));

	data1["name"] = "insertname001";
	data1["size"] = "1024";
	//cout << my.GetInsertSql(data1, "t_video") << endl;
	my.Insert(data1,"t_video"); //id=1

	data1["name"] = "insertname002";
	my.Insert(data1, "t_video"); //id=2


	///////////////////////////////////////////////

	///// 二进制数据（图片）封装插入
	//mysql.jpg
	data1["name"] = "mysql_new1.jpg";
	//data1["size"] =
	LXData file1;
	file1.LoadFile("mysql.jpg");
	data1["data"] = file1;
	data1["size"] = &file1.size;
	my.InsertBin(data1, "t_video");  //id=3

	data1["name"] = "mysql_new2.jpg";
	my.InsertBin(data1, "t_video");	//id=4

	file1.Drop();
	//my.Insert(data1, "t_video");


	///修改普通数据 修改id=1的数据
	XDATA udata;
	udata["name"] = "updatename001";
	cout << "my.Update = " << my.Update(udata, "t_video", "where id=1") << endl;


	///修改二进制数据 修改id=3 的data 图标
	XDATA udata2;
	LXData file2;
	file2.LoadFile("mysql2.jpg");
	udata2["data"] = file2;
	cout << "my.UpdateBin = "<<my.UpdateBin(udata2, "t_video", "where id=3") << endl;
	file2.Drop();



	//使用事务插入多条记录
	XDATA data3;
	data3["name"] = "trans001";
	data3["size"] = "1024";
	my.StartTransaction();
	my.Insert(data3,"t_video");
	my.Insert(data3, "t_video");
	my.Insert(data3, "t_video");
	my.Rollback(); //回滚

	data3["name"] = "trans002";
	my.Insert(data3, "t_video");

	data3["name"] = "trans003";
	my.Insert(data3, "t_video");
	my.Commit();
	my.StopTransaction();

	//获取结果集
	sql = "select * from t_video";
	cout << my.Query(sql.c_str()) << endl;
	my.StoreResult();	//结果集本地全部存储
	
	for (;;)
	{
		//获取一行数据
		auto row = my.FetchRow();
		if (row.size() == 0)break;

		row[2].SaveFile(row[1].data);


		for (int i = 0; i < row.size(); i++)
		{
			if(row[i].data)
				cout << row[i].data<<" ";
		}
		cout << endl;
	}
	
	
	my.FreeResult();

	cout << my.Query(sql.c_str()) << endl;
	my.UseResult();		//开始接收结果集
	my.FreeResult();

	///开始测试字符集 问题， 插入，读取 GBK utf-8
	cout << "开始测试字符集" << endl;
	//测试utf8 指定字段name的 utf 字符集
	sql = "CREATE TABLE IF NOT EXISTS `t_utf8` \
		(`id` INT AUTO_INCREMENT,	\
		`name` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_bin,\
		PRIMARY KEY(`id`))";
	my.Query(sql.c_str());
	//清空数据
	my.Query("TRUNCATE t_utf8");
	//指定与mysql处理的字符集
	my.Query("set names utf8");
	{
		XDATA data;
		data["name"] = u8"测试的UTF中文";
		my.Insert(data, "t_utf8");
	}


	//插入gbk的数据
	sql = "CREATE TABLE IF NOT EXISTS `t_gbk` \
		(`id` INT AUTO_INCREMENT,	\
		`name` VARCHAR(1024) CHARACTER SET gbk COLLATE gbk_bin,\
		PRIMARY KEY(`id`))";
	my.Query(sql.c_str());
	//清空数据
	my.Query("TRUNCATE t_gbk");
	//指定与mysql处理的字符集
	my.Query("set names gbk");
	{
		XDATA data;
		LXData tmp = u8"测试的GBK中文";
		string gbk = tmp.UTF8ToGBK();
		data["name"] = gbk.c_str();
		my.Insert(data, "t_gbk");
	}
	cout << "==== Print names gbk string ==== " << endl;
	my.Query("set names gbk");
	my.Query("select * from t_gbk");
	my.StoreResult();
	for (;;)
	{
		//获取一行数据
		auto row = my.FetchRow();
		if (row.size() == 0)break;
		
#ifdef _WIN32
		cout << "id:" << row[0].data << " name:" << row[1].data << endl;
#else
		cout << "id:" << row[0].data << " name:" << row[1].GBKToUTF8() << endl;
#endif
	}
	my.FreeResult();


	cout << "==== Print utf-8 string ==== " << endl;
	my.Query("set names utf8");
	my.Query("select * from t_utf8");
	my.StoreResult();
	for (;;)
	{
		//获取一行数据
		auto row = my.FetchRow();
		if (row.size() == 0)break;

#ifdef _WIN32
		cout << "id:" << row[0].data << " name:" << row[1].UTF8ToGBK() << endl;
#else
		cout << "id:" << row[0].data << " name:" << row[1].data << endl;
#endif
	}
	my.FreeResult();


	XROWS rows = my.GetResult("select * from t_video");
	for (int i = 0; i < rows.size(); i++)
	{
		auto row = rows[i];
		for (int i = 0; i < row.size(); i++)
		{
			if (!row[i].data)
			{
				cout << "[NULL],";
				continue;
			}
			switch (row[i].type)
			{
			case LX_TYPE_BLOB:
				cout << "[BLOB]";
				break;
			case LX_TYPE_LONG:
			case LX_TYPE_STRING:
			default:
				cout << row[i].data;
				break;
			}
				
			cout << ",";
		}

		cout << endl;

	}

	//订票模拟(事务) t_tickets(id int,sold int)
	//插入gbk的数据
	sql = "CREATE TABLE IF NOT EXISTS `t_tickets` \
		(`id` INT AUTO_INCREMENT,	\
		`sold` INT,\
		PRIMARY KEY(`id`))";
	my.Query(sql.c_str());


	{
		XDATA data;
		data["sold"] = "0";
		my.Insert(data, "t_tickets"); //id=1
		my.StartTransaction();

		//行锁
		XROWS rows = my.GetResult("select * from t_tickets where sold=0 order by id for update");
		string id = rows[0][0].data;
		cout << "Buy ticket id is " << id << endl;

		//模拟冲突
		this_thread::sleep_for(10s);
		data["sold"] = "1";
		string where = "where id=";
		where += id;
		cout<<my.Update(data, "t_tickets", where)<<endl;

		cout << "Buy ticket id  " << id  <<" success!"<< endl;
		//my.GetResult("select * from t_tickets where sold=0 for update");
		my.Commit();
		my.StopTransaction();
	}




	//清理资源
	my.Close();
    std::cout << "test_LXMysql!\n"; 
	getchar();
}

