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
#include <time.h>

#include <Windows.h>

#include "ConfigFileReader.h"

const int LINESIZE = 512;

// Get current date/time, format is YYYY-MM-DD_HH-mm
const std::string currentDateTime(std::string format) {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), format.c_str(), &tstruct);

	return buf;
}

class Storage {

private:
	static std::ofstream file;
	static bool open;
	static std::string patient;
	static std::string host;
	static std::string db;
	static std::string user;
	static std::string passwd;
	static std::string saveDir;
	static std::map<std::string, std::string> config;

	static sql::Driver *driver;
	static sql::Connection *con;
	static sql::Statement *stmt;
	static sql::PreparedStatement *pstmt;
	static sql::ResultSet *res;

	static std::string getConfig(std::string conf) {
		std::map<std::string, std::string>::iterator it = config.find(conf);
		if ( it != config.end() ) {
			return it->second;
		} else {
			return "";
		}
	}

	static bool getValue(std::string &var, char * scrap, char * fileName, std::string line);
	static bool connect();
	static int findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values);
	static void printSqlError(sql::SQLException &e, std::string sql);
	static int insert(std::string table, std::map<std::string, std::string> values);

public:
	static void saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);
	static void closeFile();
	static void checkSent();
	static bool sendFileToDb(char * fileName);
	static void setConfig(std::map<std::string,std::string> config) {

		Storage::config = config;
		
		Storage::patient	= Storage::getConfig("PATIENT");
		Storage::host		= Storage::getConfig("HOST");
		Storage::db			= Storage::getConfig("DB");
		Storage::user		= Storage::getConfig("USER");
		Storage::passwd		= Storage::getConfig("PASSWD");
		Storage::saveDir	= Storage::getConfig("SAVE_DIR");
	}
};


std::ofstream Storage::file;
bool Storage::open = false;
std::string Storage::patient;
std::string Storage::host = "";
std::string Storage::db = "";
std::string Storage::user = "";
std::string Storage::passwd = "";
std::string Storage::saveDir;
std::map<std::string, std::string> Storage::config;
sql::Driver* Storage::driver;
sql::Connection* Storage::con;
sql::Statement*	Storage::stmt;
sql::ResultSet*	Storage::res;
sql::PreparedStatement* Storage::pstmt;

void Storage::saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( open == false ) {
		std::string date = currentDateTime("%Y-%m-%d %H:%M:%S");
		file = std::ofstream( Storage::saveDir + "/" + currentDateTime("%Y-%m-%d_%H-%M-%S") + ".txt");
		open = true;
		file << "DEV\t" << userdata->name << "\n";
		file << "DATE\t" << date << "\n";
		file << "PATIENT\t" << patient << "\n";

	}
	//sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
	if ( file.is_open() ) {
		file << "SENSOR\t" << t.sensor
							<< "\tPOS\t" << t.pos[0] << "\t" << t.pos[1] << "\t" << t.pos[2]
							<< "\tQUAT\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
	}
}

void Storage::closeFile() {
	file.close();
}


void Storage::checkSent() {
	HANDLE hFind;
	WIN32_FIND_DATA data;
	
	hFind = FindFirstFile( (Storage::saveDir + "/*.txt").c_str() , &data);
	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			if ( !sendFileToDb(data.cFileName) ) {
				printf("Envio interrompido.\n");
			}
			printf("%s\n", data.cFileName);
		} while ( FindNextFile(hFind, &data) );
		FindClose(hFind);
	}
}

bool Storage::sendFileToDb(char * fileName) {
	std::string line;
	std::ifstream fileS(Storage::saveDir + "/" + fileName);
	char *pch;
	
	char scrap[LINESIZE];


	std::string dev;
	std::string patientName;
	std::string dateTime;
	std::string data;

	if ( fileS.is_open() ) {
		while ( getline(fileS, line) ) {

			//Ignora linhas que iniciam com #
			if ( ConfigFileReader::ignoreLine((char *)line.c_str()) ) {
				continue;
			}

			strncpy(scrap, line.c_str(), LINESIZE - 1);
			scrap[sizeof(scrap) - 1] = '\0';


			//Identifica o tipo de configuracao que sera lido
			if ( !strcmp(pch = strtok(scrap, " \t"), "DEV") ) {
				if( !getValue(dev, scrap, fileName, line)) {
					return false;
				}
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "PATIENT") ) {
				if (!getValue(patientName, scrap, fileName, line)) {
					return false;
				}
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "DATE") ) {
				if ( !getValue(dateTime, scrap, fileName, line) ) {
					return false;
				}
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "SENSOR") ) {
				data += line+"\n";
			}

		}
		fileS.close();
	}

	//Conecta
	if ( !con && !Storage::connect() ) {
		return false;
	}
	
	std::map<std::string, std::string> values;
	values["name"] = patientName;
	std::cout << Storage::findOrCreate("patients", "idpatient", values) << "<<<\n";

	values.clear();
	values["device_name"] = dev;
	std::cout << Storage::findOrCreate("devices", "iddevice", values) << "<<<\n";
	//Salva no banco
	//stmt = con->createStatement();
	//stmt->execute("insert into teste (nome) values ('alan')");

	return true;
}

bool Storage::getValue(std::string &var,char * scrap, char * fileName, std::string line) {
	char s1[LINESIZE], s2[LINESIZE];
	if ( sscanf(line.c_str(), "%s\t%s", s1, s2) != 2 ) {
		fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
		return false;
	}
	var = s2;
	return true;
}


bool Storage::connect() {
	try {
		driver = get_driver_instance();
		con = driver->connect( Storage::host.c_str(), Storage::user.c_str(), Storage::passwd.c_str());
		con->setSchema(Storage::db.c_str());
		std::cout << "db connected.\n";
		return true;
	} catch ( sql::SQLException &e ) {
		Storage::printSqlError(e, "");
		return false;
	}
}


int Storage::insert(std::string table, std::map<std::string,std::string> values) {
	std::string sql = "insert into " + table + " (";
	std::string vals = "";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		sql += it->first + ", ";
		vals += "'"+it->second + "', ";
	}

	sql = sql.substr(0, sql.length() - 2) + " ) values (" + vals.substr(0, vals.length() - 2) +")" ;
	std::cout << sql << "\n";
	try {
		stmt = con->createStatement();
		stmt->execute((sql).c_str());
		delete stmt;
	} catch ( sql::SQLException &e ) {
		Storage::printSqlError(e,sql);
		delete stmt;
		return -1;
	}

	//Retorna a ultima id inserida
	try {
		pstmt = con->prepareStatement("SELECT LAST_INSERT_ID() AS id");
		res = pstmt->executeQuery();
	} catch ( sql::SQLException &e ) {
		Storage::printSqlError(e,sql);
		delete pstmt;
		return -1;
	}
	res->first();

	int id = res->getInt("id");
	delete pstmt;
	return id;
}

int Storage::findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values) {

	std::string sql = "select " + idName + " from " + table;
	std::string where = " where ";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		where += it->first +" = '" + it->second + "', ";
	}

	sql += where.substr(0, where.length() - 2);
	
	try{
		pstmt = con->prepareStatement((sql).c_str());
		res = pstmt->executeQuery();
	} catch ( sql::SQLException &e ) {
		Storage::printSqlError(e, sql);
		return -1;
	}
	
	if ( res->rowsCount() == 0) {
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

void Storage::printSqlError(sql::SQLException &e, std::string sql) {
	std::cout << "[" << sql << "]\n";
	std::cout << "# ERR: SQLException in " << __FILE__;
	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << "\n";
	std::cout << "# ERR: " << e.what();
	std::cout << " (MySQL error code: " << e.getErrorCode();
	std::cout << ", SQLState: " << e.getSQLState().c_str() << " )" << "\n";
	
}

/*
int sqlRun() {
	cout << endl;
	//cout << "Running 'SELECT 'Hello World!' » AS _message'..." << endl;

		try {


		

		//stmt = con->createStatement();

		//res = stmt->executeQuery("SELECT 'Hello World!' AS _message");

		//stmt2->execute("insert into teste (nome) values ('alan')");

		/*
		res = stmt->executeQuery("SELECT idteste, nome from teste");
		while ( res->next() ) {
			cout << "... MySQL replies: ";
			// Access column data by alias or column name 
			//cout << res->getString("_message") << endl;
			//cout << "\t... MySQL says it again: ";
			// Access column data by numeric offset, 1 is the first column 
			cout << res->getString(2) << endl;
		}

		sql::PreparedStatement *pstmt;
		pstmt = con->prepareStatement("SELECT idteste, nome FROM teste ORDER BY idteste ASC");
		res = pstmt->executeQuery();

		/* Fetch in reverse = descending order! 
		res->afterLast();
		while ( res->previous() )
			cout << "\t... MySQL counts: " << res->getInt("idteste") << " " << res->getString("nome").c_str() << endl;
		

		delete res;
		delete pstmt;
		delete con;

	} catch ( sql::SQLException &e ) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}*/