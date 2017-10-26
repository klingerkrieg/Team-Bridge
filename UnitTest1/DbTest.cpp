#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <map>
#include "Db.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string host = "127.0.0.1:3306";
std::string dbName = "test";
std::string user = "root";
std::string passwd = "";
std::string tempTable = "CREATE TEMPORARY TABLE IF NOT EXISTS test (id INT NOT NULL, txttest VARCHAR(45) NULL, PRIMARY KEY(id))";



namespace DbTest {
	TEST_CLASS(DbTest) {
public:

	TEST_METHOD(DB_connect) {
		Db db = Db(host, dbName,user,passwd);
		Assert::IsTrue(db.connect());
		db.close();
	}
	
	TEST_METHOD(Db_exec) {
		Db db = Db(host, dbName, user, passwd);
		db.connect();
		Assert::IsTrue(db.exec(tempTable));
		db.close();
	}

	TEST_METHOD(Db_insert) {
		Db db = Db(host, dbName, user, passwd);
		db.connect();
		db.exec(tempTable);
		std::map<std::string, std::string> values = { { "txttest","test1" } };
		Assert::AreEqual(0, db.insert("test", values, false));//quando ele insere mas nao peço a id ele retorna 0
		db.close();
	}

	TEST_METHOD(Db_findOrCreate) {
		Db db = Db(host, dbName, user, passwd);
		db.connect();
		db.exec(tempTable);
		std::map<std::string, std::string> values = { { "txttest","test2" } };
		int id = db.findOrCreate("test", "id", values);
		Assert::AreEqual(id, db.findOrCreate("test", "id", values));
		db.close();
	}
};



}