#pragma once
#include <string>
#include <map>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

class Config {

private:
	std::string host = "";
	std::string user = "";
	std::string passwd = "";
	std::string db = "";
	std::string patient = "";
	std::string saveDir = "";
	std::string app = "";

	std::map<std::string, std::string> config;

	std::string getConfig(std::string conf);

public:

	Config();

	Config(std::map<std::string, std::string> configMap);

	std::string toString();

	void readConfigMap(std::map<std::string, std::string> configMap);

	void readConfigJSON(json js);

	std::string& getHost();

	std::string& getUser();

	std::string& getPasswd();

	std::string& getDb();

	std::string& getPatient();

	std::string& getSaveDir();

	std::string& getApp();
};

