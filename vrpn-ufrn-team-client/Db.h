#pragma once
#pragma once
/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#include "DeviceInfo.h"
#include <vrpn_Tracker.h>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <Windows.h>


class Db {

private:
	
	bool connected = false;
	std::string host;
	std::string db;
	std::string user;
	std::string passwd;

	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::PreparedStatement *pstmt;
	sql::ResultSet *res;

	
	void printSqlError(sql::SQLException &e, std::string sql);

	

public:
	int findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values);
	int insert(std::string table, std::map<std::string, std::string> values, bool getId = true);

	void close() {
		if ( connected ) {
			con->close();
		}
	}

	Db() {

	}

	Db(std::string host, std::string db, std::string user, std::string passwd) {
		this->host = host;
		this->user = user;
		this->db = db;
		this->passwd = passwd;
	}

	bool connect() {
		if ( !connected ) {
			try {
				driver = get_driver_instance();
				con = driver->connect(host.c_str(), user.c_str(), passwd.c_str());
				con->setSchema(db.c_str());
				std::cout << "db connected.\n";
				connected = true;
				return true;
			} catch ( sql::SQLException &e ) {
				printSqlError(e, "");
				return false;
			}
		}
		return true;
	}
	
};


int Db::insert(std::string table, std::map<std::string, std::string> values, bool getId) {
	std::string sql = "insert into " + table + " (";
	std::string vals = "";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		sql += it->first + ", ";
		vals += "'" + it->second + "', ";
	}

	sql = sql.substr(0, sql.length() - 2) + " ) values (" + vals.substr(0, vals.length() - 2) + ")";

	try {
		stmt = con->createStatement();
		stmt->execute((sql).c_str());
		delete stmt;
	} catch ( sql::SQLException &e ) {
		printSqlError(e, sql);
		delete stmt;
		return -1;
	}

	if ( getId ) {
		//Retorna a ultima id inserida
		try {
			pstmt = con->prepareStatement("SELECT LAST_INSERT_ID() AS id");
			res = pstmt->executeQuery();
		} catch ( sql::SQLException &e ) {
			printSqlError(e, sql);
			delete pstmt;
			return -1;
		}
		res->first();

		int id = res->getInt("id");
		delete pstmt;
		return id;
	} else {
		return 0;
	}

}

int Db::findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values) {

	std::string sql = "select " + idName + " from " + table;
	std::string where = " where ";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		where += it->first + " = '" + it->second + "' and ";
	}

	sql += where.substr(0, where.length() - 4);

	try {
		pstmt = con->prepareStatement((sql).c_str());
		res = pstmt->executeQuery();
	} catch ( sql::SQLException &e ) {
		printSqlError(e, sql);
		return -1;
	}


	if ( res->rowsCount() == 0 ) {
		//se inseriu
		return insert(table, values);
	} else {
		res->first();
		int id = res->getInt(idName.c_str());
		delete pstmt;
		delete res;

		return id;//retorna a id procurada
	}
}

void Db::printSqlError(sql::SQLException &e, std::string sql) {
	std::cout << "[" << sql << "]\n";
	std::cout << "# ERR: SQLException in " << __FILE__;
	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << "\n";
	std::cout << "# ERR: " << e.what();
	std::cout << " (MySQL error code: " << e.getErrorCode();
	std::cout << ", SQLState: " << e.getSQLState().c_str() << " )" << "\n";

}


/* teste executado, escrita em arquivo se mostra muito melhor
500 registros
17ms em arquivo
5486ms em banco com update

500 registros
4ms em arquivo
365ms em banco com insert

10000 registros
14ms em arquivo
18967ms em banco com insert
*/