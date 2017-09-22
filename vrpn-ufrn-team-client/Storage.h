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

#include <chrono> //test

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
	static int insert(std::string table, std::map<std::string, std::string> values, bool getId = true);

public:

	static void test();

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
		file << "SENSOR\t" << t.sensor << "\tTIMESTAMP\t" << t.msg_time.tv_usec
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
		} while ( FindNextFile(hFind, &data) );
		FindClose(hFind);
	}
}

bool Storage::sendFileToDb(char * fileName) {

	std::string fullContent = "SYNC\n";
	std::string line;
	std::ifstream fileS(Storage::saveDir + "/" + fileName);
	char *pch;
	
	char scrap[LINESIZE];


	std::string dev;
	std::string patientName;
	std::string dateTime;
	std::string data;
	
	int idPatient = -1;
	int idDevice = -1;
	int idSession = -1;

	int timestamp;
	int sensor;
	double pos[3];
	double quat[4];

	std::map<std::string, std::string> values;
	int processedLines = 0;

	//Conecta
	if ( !con && !Storage::connect() ) {
		return false;
	}
	

	if ( fileS.is_open() ) {
		
		int lines = (int)std::count(std::istreambuf_iterator<char>(fileS),
							   std::istreambuf_iterator<char>(), '\n');

		printf("Enviando: %s\n", fileName);
		fileS.seekg(0);


		while ( getline(fileS, line) ) {

			
			fullContent += line + "\n";

			//Ignora linhas que iniciam com #
			if ( ConfigFileReader::ignoreLine((char *)line.c_str()) ) {
				continue;
			}

			strncpy(scrap, line.c_str(), LINESIZE - 1);
			scrap[sizeof(scrap) - 1] = '\0';


			//Identifica o tipo de configuracao que sera lido
			if ( !strcmp(pch = strtok(scrap, " \t"), "SYNC") ) {
				//Se ja tiver sido enviado ao banco
				printf("Ja enviado.\n");
				return true;
			} else
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
				//Caso chegue nos dados dos sensores e porque o cabecalho com as informacoes ja acabou

				if ( idSession == -1 ) {
					//Se nao tem a id da sessao
					values.clear();
					values["name"] = patientName;
					idPatient = Storage::findOrCreate("patients", "idpatient", values);

					values.clear();
					values["device_name"] = dev;
					idDevice = Storage::findOrCreate("devices", "iddevice", values);

					values.clear();
					values["date_time"] = dateTime;
					values["iddevice"] = std::to_string(idDevice);
					values["idpatient"] = std::to_string(idPatient);
					idSession = Storage::findOrCreate("sessions", "idsession", values);
				}


				//sensor	0	time	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
				if ( sscanf(line.c_str(), "SENSOR\t%d\tTIMESTAMP\t%d\tPOS\t%lf\t%lf\t%lf\tQUAT\t%lf\t%lf\t%lf\t%lf",
							&sensor,
							&timestamp,
							&pos[0], &pos[1], &pos[2],
							&quat[0], &quat[1], &quat[2], &quat[3]) != 9 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					return false;
				}

				//uint = 4294967295
				//meia hora = 540.000 registros
				//cabem 7953 sessoes
				values.clear();
				values["idsession"] = std::to_string(idSession);
				values["sensor"] = std::to_string(sensor);
				values["pos_0"] = std::to_string(pos[0]);
				values["pos_1"] = std::to_string(pos[1]);
				values["pos_2"] = std::to_string(pos[2]);
				values["quat_0"] = std::to_string(quat[0]);
				values["quat_1"] = std::to_string(quat[1]);
				values["quat_2"] = std::to_string(quat[2]);
				values["quat_3"] = std::to_string(quat[3]);

				insert("trackers", values, false);
			}


			//Mostra a porcentagem de envio
			int perc = (processedLines * 100) / lines;
			std::cout << "\r " << perc << "%";
			processedLines++;

		}

		std::cout << "\r 100%\n";

		fileS.close();

		//Grava no arquivo a palavra chave SYNC para identificar que ja foi enviado ao banco
		file = std::ofstream(Storage::saveDir + "/" + fileName);
		if ( file.is_open() ) {
		file << fullContent;
		}
		file.close();
		
	}

	
	return true;
}

bool Storage::getValue(std::string &var,char * scrap, char * fileName, std::string line) {
	char s1[LINESIZE], s2[LINESIZE], s3[LINESIZE];
	if ( sscanf(line.c_str(), "%s\t%s\t%s", s1, s2, s3) != 3 ) {
		if ( sscanf(line.c_str(), "%s\t%s", s1, s2) != 2 ) {
			fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
			return false;
		} else {
			var = s2;
		}
	} else {
		var = strcat(strcat(s2," "),s3);//caso do datetime
	}
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


int Storage::insert(std::string table, std::map<std::string,std::string> values, bool getId) {
	std::string sql = "insert into " + table + " (";
	std::string vals = "";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		sql += it->first + ", ";
		vals += "'"+it->second + "', ";
	}

	sql = sql.substr(0, sql.length() - 2) + " ) values (" + vals.substr(0, vals.length() - 2) +")" ;
	
	try {
		stmt = con->createStatement();
		stmt->execute((sql).c_str());
		delete stmt;
	} catch ( sql::SQLException &e ) {
		Storage::printSqlError(e,sql);
		delete stmt;
		return -1;
	}

	if ( getId ) {
		//Retorna a ultima id inserida
		try {
			pstmt = con->prepareStatement("SELECT LAST_INSERT_ID() AS id");
			res = pstmt->executeQuery();
		} catch ( sql::SQLException &e ) {
			Storage::printSqlError(e, sql);
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

int Storage::findOrCreate(std::string table, std::string idName, std::map<std::string, std::string> values) {

	std::string sql = "select " + idName + " from " + table;
	std::string where = " where ";
	for ( std::map<std::string, std::string>::iterator it = values.begin(); it != values.end(); ++it ) {
		where += it->first +" = '" + it->second + "' and ";
	}

	sql += where.substr(0, where.length() - 4);
	
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
void Storage::test() {
	std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);

	int qtd = 500;
	
	std::cout << "START FILE:" << currentDateTime("%Y-%m-%d %H:%M:%S") << "\n";
	file = std::ofstream(Storage::saveDir + "/testFile.txt");

	//sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
	std::string str = "sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85\n";
	if ( file.is_open() ) {
		for ( int i = 0; i < qtd; i++ ) {
			file << str;
		}
	}
	std::cout << "END FILE:" << currentDateTime("%Y-%m-%d %H:%M:%S") << "\n";

	

	std::chrono::milliseconds ms2 = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);

	std::cout << ms2.count() - ms.count() << "\n";

	ms = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);

	std::cout << "START DB:" << currentDateTime("%Y-%m-%d %H:%M:%S") << "\n";
	Storage::connect();
	//std::string sql = "update performance_test set text = concat(text,'" + str + "') where id = 1";
	std::string sql = "insert into performance_test (text) values ('" + str + "')";
	std::cout << sql << "\n";
	
	for ( int i = 0; i < qtd; i++ ) {
		stmt = con->createStatement();
		stmt->execute(sql.c_str());
		stmt->close();
	}
	
	std::cout << "END DB:" << currentDateTime("%Y-%m-%d %H:%M:%S") << "\n";


	ms2 = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
		);


	std::cout << ms2.count() - ms.count() << "\n";
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