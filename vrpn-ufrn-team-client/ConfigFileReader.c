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
									  std::vector<DeviceType> &devs,
									  std::vector<KeyMap> &map,
									  Config &config) {

	using json = nlohmann::json;
	//Abre o json
	std::ifstream inputFile;
	/*inputFile.imbue(std::locale(
		inputFile.getloc(),
		new std::codecvt_utf8_utf16<wchar_t, 0x10FFFF, std::consume_header>));*/
	//inputFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

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
			devs.push_back(dt);
		}
	}

	for ( size_t i = 0; i < devs.size(); i++ ) {
		
		if ( setDevType(js,"kinect",DEVTYPE_KINECT,devs.at(i)) ) {
			continue;
		}

		if ( setDevType(js, "leapMotion", DEVTYPE_LEAPMOTION, devs.at(i)) ) {
			continue;
		}
		//somente dispositivos de tracking necessitam do devtype para a escolha do skeleton correto
		//Mesmo assim ainda estao presentes aqui para fins de teste
		if ( setDevType(js, "keyboard", DEVTYPE_KEYBOARD, devs.at(i)) ) {
			continue;
		}

		if ( setDevType(js, "mouse", DEVTYPE_MOUSE, devs.at(i)) ) {
			continue;
		}

		if ( setDevType(js, "nedglove", DEVTYPE_NEDGLOVE, devs.at(i)) ) {
			continue;
		}

	}

	//Imprime configurações para check
	printConfig(devs, map, config);

	return true;
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
