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




using namespace std;


// Get current date/time, format is YYYY-MM-DD_HH-mm
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &tstruct);

	return buf;
}

class Storage {

private:
	static ofstream file;
	static bool open;
	static std::string patient;
	static std::string host;
	static std::string db;
	static std::string user;
	static std::string passwd;
	static std::string saveDir;
	static std::map<std::string, std::string> config;

	static std::string getConfig(std::string conf) {
		std::map<string, string>::iterator it = config.find(conf);
		if ( it != config.end() ) {
			return it->second;
		} else {
			return "";
		}
	}

public:
	static void saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);
	static void closeFile();
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


ofstream Storage::file;
bool Storage::open = false;
std::string Storage::patient;
std::string Storage::host;
std::string Storage::db;
std::string Storage::user;
std::string Storage::passwd;
std::string Storage::saveDir;
std::map<std::string, std::string> Storage::config;


void Storage::saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( open == false ) {
		std::string date = currentDateTime();
		file = ofstream( Storage::saveDir + "/" + date + ".txt");
		open = true;
		file << "DEV\t" << userdata->name << "\n";
		file << "DATE\t" << date << "\n";
		file << "PATIENT\t" << patient << "\n";

	}
	//sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
	if ( file.is_open() ) {
		file << "sensor\t" << t.sensor
							<< "\tpos\t" << t.pos[0] << "\t" << t.pos[1] << "\t" << t.pos[2]
							<< "\tquat\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
	}
}

void Storage::closeFile() {
	file.close();
}




/*
int sqlRun() {
	cout << endl;
	//cout << "Running 'SELECT 'Hello World!' » AS _message'..." << endl;

		try {


		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		

		// Create a connection 
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		// Connect to the MySQL test database 
		con->setSchema("vrpn");

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