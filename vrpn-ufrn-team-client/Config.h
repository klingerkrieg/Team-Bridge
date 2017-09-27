#pragma once
#include <string>
#include <map>
#include <iostream>
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

	std::string getConfig(std::string conf) {
		std::map<std::string, std::string>::iterator it = config.find(conf);
		if ( it != config.end() ) {
			return it->second;
		} else {
			return "";
		}
	}

public:

	std::string toString() {
		return "HOST:" + host + "\nUSER:" + user + "\nPASSWD:" + passwd + "\nDB:" + db + "\nPATIENT:" + patient + "\nSAVE_DIR:" + saveDir + "\nAPP:" + app;
	}

	void readConfigMap(std::map<std::string, std::string> configMap) {
		for ( std::map<std::string, std::string>::iterator it = configMap.begin(); it != configMap.end(); ++it ) {

			if ( it->first.compare("USER") == 0 )
				user = it->second;
			else if ( it->first.compare("PASSWD") == 0 )
				passwd = it->second;
			else if ( it->first.compare("DB") == 0 )
				db = it->second;
			else if ( it->first.compare("HOST") == 0 )
				host = it->second;
			else if ( it->first.compare("PATIENT") == 0 )
				patient = it->second;
			else if ( it->first.compare("SAVE_DIR") == 0 )
				saveDir = it->second;
			else if ( it->first.compare("APP") == 0 )
				app = it->second;
		}
	}

	std::string& getHost() {
		return host;
	}

	std::string& getUser() {
		return user;
	}

	std::string& getPasswd() {
		return passwd;
	}

	std::string& getDb() {
		return db;
	}

	std::string& getPatient() {
		return patient;
	}

	std::string& getSaveDir() {
		return saveDir;
	}

	std::string& getApp() {
		return app;
	}
};