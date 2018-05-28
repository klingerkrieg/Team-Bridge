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
	return "HOST:" + host + "\nUSER:" + user + "\nPASSWD:" + passwd + "\nDB:" + db + "\nPATIENT:" + patient + "\nSAVE_DIR:" + saveDir + "\n";
}

void Config::readConfigJSON(json js) {

	for ( json::iterator it = js.begin(); it != js.end(); ++it ) {
		std::string k = it.key();
		if ( it.key().compare("user") == 0 )
			user = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("passwd") == 0 )
			passwd = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("database") == 0 )
			db = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("host") == 0 )
			host = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("patientName") == 0 )
			patient = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("saveDir") == 0 )
			saveDir = utf8to16(it.value().get<std::string>());
		else if ( it.key().compare("csv") == 0 )
			csvMode = it.value().get<bool>();
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

bool Config::getCSVMode() {
	return csvMode;
}
