#include "ConfigFileReader.h"


void ConfigFileReader::printConfig(std::vector<std::string> &devs,
								   std::vector<KeyMap> &map,
								   Config &config) {

	printf("\n*******************\n");
	printf("\nDispositivos:\n");
	for ( std::vector<std::string>::iterator it = devs.begin(); it != devs.end(); ++it ) {
		printf("%s\n", it->c_str());
	}

	printf("\nMapeamento:\n");
	for ( std::vector<KeyMap>::iterator it = map.begin(); it != map.end(); ++it ) {
		printf("%s", it->toString().c_str());
	}

	printf("\nOutros:\n");
	printf(config.toString().c_str());

	//HWND window = FindWindow(_T(config.getApp().c_str()), NULL);
	HWND window = FindWindowA(NULL, config.getApp().c_str());
	if ( window ) {
		printf("\nAPP: Encontrada.");
	} else {
		printf("\nAPP: Nao encontrada.");
	}
	printf("\n*******************\n");
}


bool ConfigFileReader::readConfigFile(char * fileName,
									  std::vector<std::string> &devs,
									  std::vector<KeyMap> &map,
									  Config &config) {

	char *pch;
	char scrap[LINESIZE];
	char s1[LINESIZE], s2[LINESIZE];
	std::string lastDev;
	std::map<std::string, std::string> configMap;
	std::string line;
	

	if ( !openIn(fileName) ) {
		return false;
	}

	//Remove todos os comentarios
	std::string withouBlockComment = "";
	while ( getline(fileInput, line) ) {
		withouBlockComment += line + "\n";
	}
	withouBlockComment = removeComments(withouBlockComment);



	std::istringstream iss(withouBlockComment);
	while ( std::getline(iss, line) ) {

		if ( ignoreLine(line) ) {
			continue;
		}

		strncpy(scrap, line.c_str(), LINESIZE - 1);
		scrap[sizeof(scrap) - 1] = '\0';


		//Identifica o tipo de configuracao que sera lido
		if ( !strcmp(pch = strtok(scrap, " \t"), "KEY") ) {

			//Pula a primeira string
			pch += strlen(pch) + 1;

			KeyMap km = KeyMap(lastDev, pch);

			map.push_back(km);
		} else
		if ( !strcmp(pch = strtok(scrap, " \t"), "DEV") ) {

			if ( sscanf(line.c_str(), "%s\t%s", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
				return false;
			}

			lastDev = s2;
			//Adiciona um dispositivo
			devs.push_back(s2);
		} else {
			if ( sscanf(line.c_str(), "%s\t%[^\t\n]", s1, s2) != 2 ) {
				fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
				return false;
			}
			//Adiciona configuracao

			configMap[s1] = s2;

		}

	}

	config.readConfigMap(configMap);

	printConfig(devs, map, config);

	return true;
}