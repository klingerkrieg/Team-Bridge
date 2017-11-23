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

	std::ifstream inputFile(fileName);
	json js;
	inputFile >> js;

	config.readConfigJSON(js["common"]);


	for ( json::iterator it = js["keys"].begin(); it != js["keys"].end(); ++it ) {
		KeyMap km = KeyMap((json)it);
		map.push_back(km);
	}

	Sleep(3000);

	return 0;




	/*map.push_back(km);
	devs.push_back(s2);

	config.readConfigMap(configMap);

	printConfig(devs, map, config);*/

	return true;
}