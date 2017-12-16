#pragma once
#include <string>
#include <map>
#include <iostream>
#include "json.hpp"
#include "util.h"

using json = nlohmann::json;

class Config {

private:
	std::string host = "";
	std::string user = "";
	std::string passwd = "";
	std::string db = "";
	std::string patient = "";
	std::string saveDir = "";

	std::map<std::string, std::string> config;

	std::string getConfig(std::string conf);

public:

	Config();
	Config(json js) {
		readConfigJSON(js);
	}

	std::string toString();

	void readConfigJSON(json js);

	std::string& getHost();

	std::string& getUser();

	std::string& getPasswd();

	std::string& getDb();

	std::string& getPatient();

	std::string& getSaveDir();

};

