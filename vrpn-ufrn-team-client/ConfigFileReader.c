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
		KeyMap km = KeyMap(js2);
		map.push_back(km);

		dt.name = js2["dev"].get<std::string>();
		//Cada novo dispositivo é adicionado em um vetor separado
		std::vector<DeviceType>::iterator check = std::find(devs.begin(), devs.end(), dt);
		if ( check == devs.end() ) {
			//Se não existir
			identifyDevType(js,dt);
			devs.push_back(dt);
		}
		
	}

	//Imprime configurações para check
	printConfig(devs, map, config);

	return true;
}


int ConfigFileReader::identifyDevType(json js, DeviceType &dev) {
	//Identifica o tipo de cada dispositivo
	if ( setDevType(js, "kinect", DEVTYPE_KINECT, dev) ) {
		return DEVTYPE_KINECT;
	}

	if ( setDevType(js, "leapMotion", DEVTYPE_LEAPMOTION, dev) ) {
		return DEVTYPE_LEAPMOTION;
	}

	if ( setDevType(js, "keyboard", DEVTYPE_KEYBOARD, dev) ) {
		return DEVTYPE_KEYBOARD;
	}

	if ( setDevType(js, "mouse", DEVTYPE_MOUSE, dev) ) {
		return DEVTYPE_MOUSE;
	}

	if ( setDevType(js, "nedglove", DEVTYPE_NEDGLOVE, dev) ) {
		return DEVTYPE_NEDGLOVE;
	}
	return -1;
}

bool ConfigFileReader::setDevType(json js,std::string textDev, int devConstant, DeviceType &devT) {
	for ( json::iterator it = js["common"]["devs"][textDev].begin(); it != js["common"]["devs"][textDev].end(); ++it ) {
		json js2 = (json)*it;
		//Percorre os arrays de dispositivos para setar o tipo
		//Atualmente isso so e necessario para dispositivos de tracking
		if ( !devT.name.compare(js2) ) {
			devT.type = devConstant;
			devT.type_str = DEVTYPE_STR[devConstant];
			return true;
		}
	}
	return false;
}


