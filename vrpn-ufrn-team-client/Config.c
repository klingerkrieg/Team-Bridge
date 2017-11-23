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

std::string Config::toString() {
	return "HOST:" + host + "\nUSER:" + user + "\nPASSWD:" + passwd + "\nDB:" + db + "\nPATIENT:" + patient + "\nSAVE_DIR:" + saveDir + "\nAPP Name:" + app;
}

void Config::readConfigJSON(json js) {
	for ( json::iterator it = js.begin(); it != js.end(); ++it ) {
		if ( it.key().compare("user") == 0 )
			user = it.value().get<std::string>();
		else if ( it.key().compare("passwd") == 0 )
			passwd = it.value().get<std::string>();
		else if ( it.key().compare("db") == 0 )
			db = it.value().get<std::string>();
		else if ( it.key().compare("host") == 0 )
			host = it.value().get<std::string>();
		else if ( it.key().compare("patientName") == 0 )
			patient = it.value().get<std::string>();
		else if ( it.key().compare("saveDir") == 0 )
			saveDir = it.value().get<std::string>();
		else if ( it.key().compare("appName") == 0 )
			app = it.value().get<std::string>();
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