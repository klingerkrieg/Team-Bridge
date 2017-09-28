#include "Config.h"


std::string Config::getConfig(std::string conf) {
	std::map<std::string, std::string>::iterator it = config.find(conf);
	if ( it != config.end() ) {
		return it->second;
	} else {
		return "";
	}
}

Config::Config() {}

Config::Config(std::map<std::string, std::string> configMap) {
	readConfigMap(configMap);
}

std::string Config::toString() {
	return "HOST:" + host + "\nUSER:" + user + "\nPASSWD:" + passwd + "\nDB:" + db + "\nPATIENT:" + patient + "\nSAVE_DIR:" + saveDir + "\nAPP:" + app;
}

void Config::readConfigMap(std::map<std::string, std::string> configMap) {
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

std::string& Config::getHost() {
	return host;
}

std::string& Config::getUser() {
	return user;
}

std::string& Config::getPasswd() {
	return passwd;
}

std::string& Config::getDb() {
	return db;
}

std::string& Config::getPatient() {
	return patient;
}

std::string& Config::getSaveDir() {
	return saveDir;
}

std::string& Config::getApp() {
	return app;
}