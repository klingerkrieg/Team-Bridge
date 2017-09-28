#include "KeyMap.h"


std::map<std::string, int> KeyMap::configToAscii = KeyMap::create_configToAscii();
std::map<std::string, int> KeyMap::configToScanCode = KeyMap::create_configToScanCode();

int KeyMap::scan2ascii(DWORD scancode, USHORT* result) {
	HKL layout = GetKeyboardLayout(0);
	UCHAR State[256];
	if ( GetKeyboardState(State) == FALSE )
		return 0;
	UINT vk = MapVirtualKeyEx(scancode, 1, layout);
	return ToAsciiEx(vk, scancode, State, result, 0, layout);
}


KeyMap::KeyMap(std::string dev, int key, char toKey) {

	this->dev = dev;
	this->key = key;
	this->toKey = toKey;
}

KeyMap::KeyMap(std::string dev, char key[SSIZE], char toKey[SSIZE], char config[SSIZE]) {

	//Cria normalmente
	init(dev, key, toKey);
	//Caso seja top no kinect a configuracao extra sera a sensibilidade
	if ( this->key == KINECT_TOP_ADD || this->key == KINECT_TOP_DEC ) {
		heightSens = atof(config);
	}

}

KeyMap::KeyMap(std::string dev, char key[SSIZE], char toKey[SSIZE]) {
	init(dev, key, toKey);
}


void KeyMap::init(std::string dev, char key[SSIZE], char toKey[SSIZE]) {

	this->dev = dev;
	std::map<std::string, int>::iterator it;
	
	
	//Tenta localizar a constante em key
	it = KeyMap::configToScanCode.find(key);
	if ( it == KeyMap::configToScanCode.end() ) {
		//Quando sao enviados botoes via VRPN um inteiro sera esperado
		//this->key = atoi(key);
		this->key = key[0];//quando nao encontrar usa como char
		if ( strlen(key) > 1 ) {
			std::string msg = key;
			msg = "Nao foi possivel encontrar mapeamento para " + msg;
			throw std::exception((msg).c_str());
			std::cout << msg << "\n";
		}
	} else {
		//Entretanto com o Kinect espera-se uma constante
		this->key = it->second;
	}
	

	//Tenta localizar a constante para toKey
	it = KeyMap::configToAscii.find(toKey);

	if ( it == KeyMap::configToAscii.end() ) {
		//A saida sera um botao representado por um char quando uma constante nao for encontrada
		this->toKey = toKey[0];
	} else {
		this->toKey = it->second;
		this->toKeyIsConstant = true;
	}

}