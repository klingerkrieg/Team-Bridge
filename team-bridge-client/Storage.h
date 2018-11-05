#pragma once
/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#include "DeviceInfo.h"
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include <vrpn_Button.h>

#include "util.h"
#include "FileParser.h"
#include "Config.h"
#include "Db.h"
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
	 bool csvMode  = false;
	 bool infoData = true; 
#ifdef THERAPY_MODULE
	 Db db;
#endif
	 bool exportDb;

	 //Variaveis que sao exportadas de arquivo para banco
	 std::string expDev;
	 std::string expPatientName;
	 std::string expDateTime;
	 std::string expFullContent;

	 //bool getValue(std::string &var, char * scrap, char * fileName, std::string line);
	
public:

	void setFileName(std::string fileName) {
		this->fileName = fileName;
	}

	void setInfoData(bool save) {
		this->infoData = save;
	}

	void setSaveDir(std::string saveDir) {
		this->saveDir = saveDir;
	}

	void setCSVMode(bool mode) {
		csvMode = mode;
	}

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


	bool startFile(const char * name);
	bool saveToFile(const char * dev, std::string actionName, double value);
	bool saveToFile(const char * name, const vrpn_ANALOGCB a);
	bool saveToFile(const char * name, const vrpn_BUTTONCB b);
	bool saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	int checkSent();

	bool sendFileToDb(char * fileName);

	std::string getFileName() {
		return fileName;
	}

	std::string getDateStr() {
		return dateStr;
	}

	Storage() {

	}

	Storage (Config config, bool exportDb ){
#ifdef THERAPY_MODULE
		if (exportDb){
			db = Db(config.getHost(), config.getDb(), config.getUser(), config.getPasswd());
			this->exportDb = exportDb;
		}
#endif
		saveDir = config.getSaveDir();
		patient = config.getPatient();
		csvMode = config.getCSVMode();
	}

	void close() {
#ifdef THERAPY_MODULE
		if ( exportDb ) {
			db.close();
		}
#endif
		closeOut();
		closeIn();
	}
};


class AlreadySent : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Arquivo ja enviado";
	}

	void noZero() {

	}

};