#pragma once
#include <stdlib.h>                 // for strtol, atoi, strtod
#include <string.h>                 // for strcmp, strlen, strtok, etc
//#include <locale>                  // To enable setting parsing for .cfg file
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Config.h"
#include "KeyMap.h"
#include "FileParser.h"
#include <iostream>

class ConfigFileReader : public FileParser {
public:
	bool readConfigFile(char * fileName,
							   std::vector<std::string> &devs,
							   std::vector<KeyMap> &map,
							   Config &config);

	ConfigFileReader() {

	}

	

private:
	void printConfig(std::vector<std::string> &devs,
							   std::vector<KeyMap> &map,
							   Config &config);


	/*static bool open(char * fileName, FILE *&config_file) {

		if ( (config_file = fopen(fileName, "r")) == NULL ) {
			perror("ConfigFileReader::open(): "
				   "Cannot open config file");
			fprintf(stderr, "  (filename %s)\n", fileName);
			return false;
		}
		return true;
	}*/

	

	/// File-static constant of max line size.
	static const int LINESIZE = 512;
};


void ConfigFileReader::printConfig(std::vector<std::string> &devs,
								   std::vector<KeyMap> &map,
								   Config &config) {
	
	printf("\n*******************\n");
	printf("\nDispositivos:\n");
	for ( std::vector<std::string>::iterator it = devs.begin(); it != devs.end(); ++it ) {
		printf("%s\n", it->c_str());
	}

	printf("\nMapeamento:\n");
	for ( std::vector<KeyMap>::iterator it = map.begin(); it != map.end(); ++it ) {
		printf("[%s] %d -> %c\n", it->getDev().c_str() ,it->getKey(), it->getToKey());
	}

	printf("\nOutros:\n");
	printf(config.toString().c_str());
	printf("\n*******************\n");
}


bool ConfigFileReader::readConfigFile(char * fileName,
									  std::vector<std::string> &devs,
									  std::vector<KeyMap> &map,
									  Config &config) {

	char *pch;
	char scrap[LINESIZE];
	char s1[LINESIZE], s2[LINESIZE], s3[LINESIZE];
	std::string lastDev;
	std::map<std::string, std::string> configMap;
	std::string line;

	if ( !openIn(fileName) ) {
		return false;
	}
	

	// Read lines from the file until we run out
	while ( getline(fileInput, line) ) {


		if ( ignoreLine(line) ) {
			continue;
		}

		strncpy(scrap, line.c_str(), LINESIZE - 1);
		scrap[sizeof(scrap) - 1] = '\0';

		
		//Identifica o tipo de configuracao que sera lido
		if ( !strcmp(pch = strtok(scrap, " \t"), "KEY") ) {

			

			//Pula a primeira string
			pch += strlen(pch) + 1;
			if ( sscanf(pch, "%s\t%s\t%s", s1, s2, s3) != 3 ) {
				if ( sscanf(pch, "%s\t%s", s1, s2) != 2 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					return false;
				}
			}

			//Se nenhum dispositivo foi definido ainda
			if ( lastDev == "" ) {
				fprintf(stderr, "Nenhum dispositivo foi definido em %s para: %s\n", fileName, line.c_str());
				return false;
			}


			KeyMap km;
			if ( !strcmp(s3, "") ) {
				//Adiciona um mapeamento de input
				km = KeyMap(lastDev, s1, s2);
			} else {
				//Caso haja s3 sera alguma configuracao adicional
				km = KeyMap(lastDev, s1, s2, s3);
			}

			//zera a s3
			strcpy(s3, "");

			map.push_back(km);
		} else
		if ( !strcmp(pch = strtok(scrap, " \t"), "DEV") ) {
			
			if ( sscanf(line.c_str(), "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
				return false;
			}

			lastDev = s2;
			//Adiciona um dispositivo
			devs.push_back(s2);
		} else {
			if ( sscanf(line.c_str(), "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
				return false;
			}
			//Adiciona configuracao

			configMap[s1] = s2;

		}

	}

	config.readConfigMap(configMap);

	printConfig(devs, map, config);

	return true;
}