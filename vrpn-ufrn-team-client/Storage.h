#pragma once
/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#include "DeviceInfo.h"
#include <vrpn_Tracker.h>


#include "FileParser.h"
#include "Config.h"
#include "Db.h"
#include "ConfigFileReader.h"
#include <Windows.h>
#include <time.h>

const int LINESIZE = 512;

// Get current date/time, format is YYYY-MM-DD_HH-mm
const std::string currentDateTime(std::string format);

class Storage : public FileParser {

private:
	 std::string patient;
	 std::string saveDir;
	 std::string fileName;
	 std::string dateStr;
	 Db db;

	 //Variaveis que sao exportadas de arquivo para banco
	 std::string expDev;
	 std::string expPatientName;
	 std::string expDateTime;
	 std::string expFullContent;

	 //bool getValue(std::string &var, char * scrap, char * fileName, std::string line);
	
public:

	std::string getExpDev() {
		return expDev;
	}

	std::string getExpPatientName() {
		return expPatientName;
	}

	std::string getExpDateTime() {
		return expDateTime;
	}

	std::string getExpFullContent() {
		return expFullContent;
	}


	 bool saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);
	 bool checkSent();
	 bool sendFileToDb(char * fileName);

	 std::string getFileName() {
		 return fileName;
	 }

	 std::string getDateStr() {
		 return dateStr;
	 }

	 Storage() {

	 }

	 Storage (Config config){
		 db = Db(config.getHost(), config.getDb(), config.getUser(), config.getPasswd());
		 saveDir = config.getSaveDir();
		 patient = config.getPatient();
	 }

	 void close() {
		 db.close();
		 closeOut();
		 closeIn();
	 }
};


