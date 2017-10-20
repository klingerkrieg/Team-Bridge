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
#include <Tchar.h>

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


	
	/// File-static constant of max line size.
	static const int LINESIZE = 512;
};

