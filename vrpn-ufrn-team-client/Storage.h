#pragma once
/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>

#include "DeviceInfo.h"
#include <vrpn_Tracker.h>

#include <Windows.h>
#include "FileParser.h"
#include "Config.h"
#include "Db.h"
#include "ConfigFileReader.h"
#include <Windows.h>
#include <time.h>

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

class Storage : public FileParser {

private:
	 std::string patient;
	 std::string saveDir;
	 Db db;

	 bool getValue(std::string &var, char * scrap, char * fileName, std::string line);
	
public:

	 void saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);
	 void checkSent();
	 bool sendFileToDb(char * fileName);

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



void Storage::saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( isOpenOut == false ) {
		std::string date = currentDateTime("%Y-%m-%d %H:%M:%S");
		std::string fileName = Storage::saveDir + "/" + currentDateTime("%Y-%m-%d_%H-%M-%S") + ".txt";
		
		if ( !openOut(fileName) ) {
			std::cout << "Nao foi possivel abrir arquivo: " << fileName << "\n";
			return;
		} else {
			fileOutput << "DEV\t" << userdata->name << "\n";
			fileOutput << "DATE\t" << date << "\n";
			fileOutput << "PATIENT\t" << patient << "\n";
			std::cout << fileName << " Criado.\n";
		}
	}
	//sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
	if ( fileOutput.is_open() ) {
		fileOutput << "SENSOR\t" << t.sensor << "\tTIMESTAMP\t" << t.msg_time.tv_usec
							<< "\tPOS\t" << t.pos[0] << "\t" << t.pos[1] << "\t" << t.pos[2]
							<< "\tQUAT\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
	}
}


void Storage::checkSent() {
	HANDLE hFind;
	WIN32_FIND_DATA data;
	
	hFind = FindFirstFile( (Storage::saveDir + "/*.txt").c_str() , &data);
	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			if ( !sendFileToDb(data.cFileName) ) {
				printf("Envio interrompido, %s.\n", data.cFileName);
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
	if ( !db.connect() ) {
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
			if ( ignoreLine((char *)line.c_str()) ) {
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
					idPatient = db.findOrCreate("patients", "idpatient", values);

					values.clear();
					values["device_name"] = dev;
					idDevice = db.findOrCreate("devices", "iddevice", values);

					values.clear();
					values["date_time"] = dateTime;
					values["iddevice"] = std::to_string(idDevice);
					values["idpatient"] = std::to_string(idPatient);
					idSession = db.findOrCreate("sessions", "idsession", values);
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

				db.insert("trackers", values, false);
			}


			//Mostra a porcentagem de envio
			int perc = (processedLines * 100) / lines;
			std::cout << "\r " << perc << "%";
			processedLines++;

		}

		std::cout << "\r 100%\n";

		//Grava no arquivo a palavra chave SYNC para identificar que ja foi enviado ao banco
		openOut(Storage::saveDir + "/" + fileName);
		if ( fileOutput.is_open() ) {
			fileOutput << fullContent;
		}
		//Fecha todos os arquivos
		closeOut();
		closeIn();
		
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

