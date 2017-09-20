#pragma once
#include <stdlib.h>                 // for strtol, atoi, strtod
#include <string.h>                 // for strcmp, strlen, strtok, etc
#include <locale>                   // To enable setting parsing for .cfg file
#include <string>
#include <vector>
#include <map>
#include "KeyMap.h"
#include <iostream>

class ConfigFileReader {
public:
	static bool readConfigFile(char * fileName,
							   std::vector<std::string> &devs,
							   std::vector<KeyMap> &map,
							   std::map<std::string, std::string> &config);

private:
	static void printConfig(std::vector<std::string> &devs,
							   std::vector<KeyMap> &map,
							   std::map<std::string, std::string> &config);


	static bool open(char * fileName, FILE *&config_file) {

		if ( (config_file = fopen(fileName, "r")) == NULL ) {
			perror("ConfigFileReader::open(): "
				   "Cannot open config file");
			fprintf(stderr, "  (filename %s)\n", fileName);
			return false;
		}
		return true;
	}

	static bool ignore(char line[]) {
		// Ignore comments and empty lines.  Skip white space before comment
		// mark (#).
		if ( strlen(line) < 3 ) {
			return true;
		}
		bool ignore = false;
		for ( int j = 0; line[j] != '\0'; j++ ) {
			if ( line[j] == ' ' || line[j] == '\t' ) {
				return true;
			}
			if ( line[j] == '#' ) {
				ignore = true;
			}
			break;
		}
		return ignore;
	}

	/// File-static constant of max line size.
	static const int LINESIZE = 512;
};


void ConfigFileReader::printConfig(std::vector<std::string> &devs,
								   std::vector<KeyMap> &map,
								   std::map<std::string, std::string> &config) {
	
	printf("\n*******************\n");
	printf("\nDispositivos:\n");
	for ( std::vector<std::string>::iterator it = devs.begin(); it != devs.end(); ++it ) {
		printf("%s\n", it->c_str());
	}

	printf("\nMapeamento:\n");
	for ( std::vector<KeyMap>::iterator it = map.begin(); it != map.end(); ++it ) {
		printf("%d -> %d\n", it->key, it->toKey);
	}

	printf("\nBanco:\n");
	for ( std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); ++it ) {
		std::cout << it->first << " = " << it->second << "\n";
	}
	printf("\n*******************\n");
}


bool ConfigFileReader::readConfigFile(char * fileName,
									  std::vector<std::string> &devs,
									  std::vector<KeyMap> &map,
									  std::map<std::string, std::string> &config) {
	FILE *config_file;
	char *pch;
	char scrap[LINESIZE];
	char s1[LINESIZE], s2[LINESIZE];

	if ( !open(fileName, config_file) ) {
		return false;
	}

	// Store the locale that was set before we came in here.
	// The global locale is obtained by using the default
	// constructor.
	std::locale const orig_locale = std::locale();

	char line[LINESIZE]; // Line read from the input file

	// Read lines from the file until we run out
	while ( fgets(line, LINESIZE, config_file) != NULL ) {

		// Set the global locale to be "C", the classic one, so that
		// when we parse the configuration file it will use dots for
		// decimal points even if the local standard is commas.
		// putting them into the global locale.  We tried putting this
		// code above, outside the parsing loop, but it did not have
		// the desired effect when placed there.  This is the earliest
		// we were able to put it and have it work.
		std::locale::global(std::locale("C"));

		// Make sure the line wasn't too long
		if ( strlen(line) >= LINESIZE - 1 ) {
			printf("Linha com tamanho acima do permitido: %s\n%s", fileName, line);
			return false;
		}

		if ( ignore(line) ) {
			continue;
		}

		strncpy(scrap, line, LINESIZE - 1);
		scrap[sizeof(scrap) - 1] = '\0';

		
		//Identifica o tipo de configuracao que sera lido
		if ( !strcmp(pch = strtok(scrap, " \t"), "KEY") ) {
			//Pula a primeira string
			pch += strlen(pch) + 1;
			if ( sscanf(pch, "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line);
				return false;
			}
			//Adiciona um mapeamento de input
			KeyMap km = KeyMap(s1, s2);
			map.push_back(km);
		} else
		if ( !strcmp(pch = strtok(scrap, " \t"), "DEV") ) {
			
			if ( sscanf(line, "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line);
				return false;
			}
			//Adiciona um dispositivo
			devs.push_back(s2);
		} else {
			if ( sscanf(line, "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line);
				return false;
			}
			//Adiciona configuracao
			config[s1] = s2;

		}

	}

	printConfig(devs, map, config);

	return true;
}