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
		if ( it->getShowMsg() != 0 ) {
			printf("[%s] %d -> [ALERT/MESSAGE] %s \n", it->getDev().c_str(), it->getKey(), it->getMsg().c_str());
		} else {
			if ( it->getHandXPos() != -100 ) {
				printf("[%s] %d -> %c - XPOS %d YPOS %d\n", it->getDev().c_str(), it->getKey(), it->getToKey(), it->getHandXPos(), it->getHandTopLevel());
			} else 
			if ( it->getHandTopLevel() != -100 ) {
				printf("[%s] %d -> %c - YPOS %d\n", it->getDev().c_str(), it->getKey(), it->getToKey(), it->getHandTopLevel());
			} else {
				printf("[%s] %d -> %c\n", it->getDev().c_str(), it->getKey(), it->getToKey());
			}
		}
	}

	printf("\nOutros:\n");
	printf(config.toString().c_str());
	printf("\n*******************\n");
}


bool ConfigFileReader::readConfigFile(char * fileName,
									  std::vector<std::string> &devs,
									  std::vector<KeyMap> &map,
									  Config &config) {

	char *pch;
	char scrap[LINESIZE];
	char s1[LINESIZE], s2[LINESIZE], s3[LINESIZE], s4[LINESIZE];
	std::string lastDev;
	std::map<std::string, std::string> configMap;
	std::string line;

	if ( !openIn(fileName) ) {
		return false;
	}


	// Read lines from the file until we run out
	while ( getline(fileInput, line) ) {


		if ( ignoreLine(line) ) {
			continue;
		}

		strncpy(scrap, line.c_str(), LINESIZE - 1);
		scrap[sizeof(scrap) - 1] = '\0';


		//Identifica o tipo de configuracao que sera lido
		if ( !strcmp(pch = strtok(scrap, " \t"), "KEY") ) {

			//Pula a primeira string
			pch += strlen(pch) + 1;

			
			if ( sscanf(pch, "%s\t%s\t%s\t%s", s1, s2, s3, s4) != 4 ) {
				if ( sscanf(pch, "%s\t%s\t%s", s1, s2, s3) != 3 ) {
					if ( sscanf(pch, "%s\t%s", s1, s2) != 2 ) {
						fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
						return false;
					}
				}
			}

			//Caso seja alert ou message ira ler todo o resto como um unico argumento
			if ( !strcmp(s2, "ALERT") || !strcmp(s2, "MESSAGE") ) {

				//pch += strlen(pch) + 1;
				//pch = strtok(pch, "\t");

				if ( sscanf(line.c_str(), "%*s\t%*s\tALERT\t%[^\t\n]", s3) != 1 ) {
					sscanf(line.c_str(), "%*s\t%*s\tMESSAGE\t%[^\t\n]", s3);
				}
			}

			//Se nenhum dispositivo foi definido ainda
			if ( lastDev == "" ) {
				fprintf(stderr, "Nenhum dispositivo foi definido em %s para: %s\n", fileName, line.c_str());
				return false;
			}


			KeyMap km;
			if ( strcmp(s4, "") && (!strcmp(s1,"KINECT_RIGHT_HAND_TOP") || !strcmp(s1,"KINECT_LEFT_HAND_TOP")) ) {
				//Possui s4 e o comando é KINECT_RIGHT_HAND_TOP ou KINECT_LEFT_HAND_TOP
				//a configuracao deve estar na seguinte ordem KEY KINECT_RIGHT_HAND_TOP		BTN		XPOS	YPOS
				km = KeyMap(lastDev, s1, s2, s3, s4);
			} else
			if ( !strcmp(s3, "") ) {
				//Adiciona um mapeamento de input
				km = KeyMap(lastDev, s1, s2);
			} else {
				//Caso haja s3 sera alguma configuracao adicional
				km = KeyMap(lastDev, s1, s2, s3);
			}

			//zera s3 e s4
			strcpy(s3, "");
			strcpy(s4, "");

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