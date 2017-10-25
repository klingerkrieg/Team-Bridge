#pragma once
/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>


#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <Windows.h>


class Db {

private:
	
	static bool connected;
	std::string host;
	std::string db;
	std::string user;
	std::string passwd;

	sql::Driver *driver;
	static sql::Connection *con;
	sql::Statement *stmt;
	sql::PreparedStatement *pstmt;
	sql::ResultSet *res;

	
	void printSqlError(sql::SQLException &e, std::string sql);

	

public:
	int findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values);
	int insert(std::string table, std::map<std::string, std::string> values, bool getId = true);
	bool exec(std::string sql);

	void close() {
		if ( connected ) {
			con->close();
			connected = false;
		}
	}

	Db() {

	}

	Db(std::string host, std::string db, std::string user, std::string passwd) {
		this->host = "tcp://"+host;
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
			} catch ( sql::SQLException &e ) {
				printSqlError(e, "");
			}
		}
		return connected;
	}
	
};

