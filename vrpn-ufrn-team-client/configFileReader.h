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
#include "KinectGestures.h"
#include <iostream>
#include <Tchar.h>
#include <sstream>
#include "json.hpp"
#include <locale>
#include <codecvt>


struct DeviceType {
	std::string name;
	int type = -1;
	std::string type_str = "";

	bool operator== (const DeviceType &rhs) {
		return !rhs.name.compare(name);
	}
};


class ConfigFileReader {
public:
	bool readConfigFile(char * fileName,
							   std::vector<DeviceType> &devs,
							   std::vector<KeyMap> &map,
							   Config &config);

	ConfigFileReader() {

	}

	

private:
	void printConfig(std::vector<DeviceType> &devs,
							   std::vector<KeyMap> &map,
							   Config &config);


};

