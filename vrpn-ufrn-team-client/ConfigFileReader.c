#include "ConfigFileReader.h"



void ConfigFileReader::printConfig(std::vector<DeviceType> &devs,
								   std::vector<KeyMap> &map,
								   Config &config) {

	printf("\n*******************\n");

	printf("\nKinect");

	printf("KINECT_X_INTERVAL=%.2f\n", KinectGestures::getKinectXInterval());


	printf("\nDispositivos:\n");
	for ( std::vector<DeviceType>::iterator it = devs.begin(); it != devs.end(); ++it ) {
		printf("%s[%s]\n", it->name.c_str(), it->type_str.c_str());
	}

	printf("\nMapeamento:\n");
	for ( std::vector<KeyMap>::iterator it = map.begin(); it != map.end(); ++it ) {
		printf("%s", it->toString().c_str());
	}

	printf("\nOutros:\n");
	printf(config.toString().c_str());

	printf("\n*******************\n");
}


bool ConfigFileReader::readConfigFile(char * fileName,
									  std::vector<DeviceType> &devs,
									  std::vector<KeyMap> &map,
									  Config &config) {

	using json = nlohmann::json;
	//Abre o json
	std::ifstream inputFile;
	
	inputFile.open(fileName);
	json js;
	inputFile >> js;

	//Lê configurações gerais
	config.readConfigJSON(js["common"]);

	DeviceType dt;

	//Lê o mapeamento
	for ( json::iterator it = js["keys"].begin(); it != js["keys"].end(); ++it ) {

		json js2 = (json)*it;
		

		dt.name = js2["dev"].get<std::string>();
		//Cada novo dispositivo é adicionado em um vetor separado
		std::vector<DeviceType>::iterator check = std::find(devs.begin(), devs.end(), dt);
		if ( check == devs.end() ) {
			//Se não existir
			
			setDevType(js2["devType"].get<std::string>(), dt);
			js2["idDevType"] = dt.type;
			devs.push_back(dt);
		}

		//espera o idDevType
		KeyMap km = KeyMap(js2);
		map.push_back(km);
		
	}

	//Imprime configurações para check
	printConfig(devs, map, config);

	return true;
}


void ConfigFileReader::setDevType(std::string devType, DeviceType &dt) {
	if ( !devType.compare("mouse") ) {
		dt.type = DEVTYPE_MOUSE;
		dt.type_str = DEVTYPE_STR[dt.type];
	} else
	if ( !devType.compare("keyboard") ) {
		dt.type = DEVTYPE_KEYBOARD;
		dt.type_str = DEVTYPE_STR[dt.type];
	} else 
	if ( !devType.compare("leapMotion") ) {
		dt.type = DEVTYPE_LEAPMOTION;
		dt.type_str = DEVTYPE_STR[dt.type];
	} else
	if ( !devType.compare("kinect") ) {
		dt.type = DEVTYPE_KINECT;
		dt.type_str = DEVTYPE_STR[dt.type];
	} else 
	if ( !devType.compare("nedglove") ) {
		dt.type = DEVTYPE_NEDGLOVE;
		dt.type_str = DEVTYPE_STR[dt.type];
	}
}


