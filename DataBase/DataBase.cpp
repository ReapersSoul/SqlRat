#include "DataBase.h"

DataBase::DataBase()
{
}

DataBase::DataBase(string ip, string username, string pass, string DBName, int port)
{
	IP = ip;
	DatabaseUser = username;
	DatabasePass = pass;
	DatabaseName = DBName;
	Port = port;
}

bool DataBase::Init()
{
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, IP.c_str(), DatabaseUser.c_str(), DatabasePass.c_str(), DatabaseName.c_str(), Port, NULL, 0);
	if (conn) {
		return true;
	}
	else {
		return false;
	}
}

bool DataBase::Query(string query)
{
	const char* q = query.c_str();
	qstate = mysql_query(conn, q);
	if (!qstate) {
		res = mysql_store_result(conn);
		return true;
	}
	else {
		return false;
	}
}

bool DataBase::GetRow()
{
	if (row = mysql_fetch_row(res)) {
		return true;
	}
	else {
		return false;
	}
}

string DataBase::GetError()
{
	return string(mysql_error(conn));
}

int DataBase::GetNumColumns(string Table)
{
	if (Query("select count(column_name)as Number from information_schema.columns where table_name='"+Table+"'")) {
		GetRow();
		return atoi(row[0]);
	}
	return -99;
}

int DataBase::GetNumRows(string Table)
{
	if (Query("SELECT COUNT(*) FROM "+Table)) {
		GetRow();
		return atoi(row[0]);
	}
	return -99;
}

bool DataBase::AddColumn(string table, string Vname, string type, int size)
{
	string query;
	if (size != NULL) {
		query = "alter table " + table + " add column " + Vname + " " + type + "(" + to_string(size) + ")";
	}
	else {
		query = "alter table " + table + " add column " + Vname + " " + type;
	}
	return Query(query);
}
