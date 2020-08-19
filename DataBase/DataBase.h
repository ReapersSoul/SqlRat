#pragma once
#include <mysql.h>

#include <string>

using namespace std;

class DataBase
{
private:
	string DatabaseName;
	string DatabasePass;
	string DatabaseUser;
	string IP;
	int Port;
	MYSQL* conn;
	int qstate;
public:
	MYSQL_ROW row;
	MYSQL_RES* res;

	DataBase();
	DataBase(string ip, string username, string pass, string DBName, int port);

	bool Init();

	bool Query(string query);

	bool GetRow();

	string GetError();

	int GetNumColumns(string Table);
	int GetNumRows(string Table);

	
	bool AddColumn(string table, string Vname, string type, int size);

};

