#pragma once
#include <stdlib.h>                 // for strtol, atoi, strtod
#include <string.h>                 // for strcmp, strlen, strtok, etc
#include <locale>                   // To enable setting parsing for .cfg file
#include <string>
#include <vector>
#include <map>
#include "KeyMap.h"


class ConfigFileReader {
public:
	static bool readKeyMap(char * fileName, std::vector<KeyMap> &map);
	static bool readConfigFile(char * fileName, std::map<std::string, std::string> &config);


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


bool ConfigFileReader::readKeyMap(char * fileName, std::vector<KeyMap> &map) {
	FILE* config_file;
	
	
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

		char s1[LINESIZE], s2[LINESIZE];
		// Line will be: vrpn_raw_SGIBox NAME PORT [list of buttons to toggle]
		if ( sscanf(line, "%s %s", s1, s2) != 2 ) {
			fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line);
			return false;
		}

		KeyMap km = KeyMap(s1, s2);

		map.push_back(km);

	}

	return true;
}




bool ConfigFileReader::readConfigFile(char * fileName, std::map<std::string, std::string> &config) {
	FILE *config_file;

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


		for ( int i = 0; i < 4; i++ ) {

			char s1[LINESIZE], s2[LINESIZE];
				
			if ( sscanf(line, "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line);
				return false;
			}
			config[s1] = s2;
			
		}
		

	}

	return true;
}