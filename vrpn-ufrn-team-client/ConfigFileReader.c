#include "ConfigFileReader.h"



void ConfigFileReader::printConfig(std::vector<std::string> &devs,
								   std::vector<KeyMap> &map,
								   Config &config) {

	printf("\n*******************\n");

	printf("\nKinect");

	printf("KINECT_X_INTERVAL=%.2f\n", KinectGestures::getKinectXInterval());


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

	//Lê o mapeamento
	for ( json::iterator it = js["keys"].begin(); it != js["keys"].end(); ++it ) {

		json js2 = (json)*it;
		KeyMap km = KeyMap(js2);
		map.push_back(km);

		//Cada novo dispositivo é adicionado em um vetor separado
		std::vector<std::string>::iterator check = std::find(devs.begin(), devs.end(), js2["dev"]);
		if ( check == devs.end() ) {
			//Se não existir
			devs.push_back(js2["dev"]);
		}
	}
	//Imprime configurações para check
	printConfig(devs, map, config);

	return true;
}