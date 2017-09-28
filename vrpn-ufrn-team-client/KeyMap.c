#include "KeyMap.h"


std::map<std::string, int> KeyMap::constantMap = KeyMap::create_map();


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

	//Tenta localizar a constante em key
	std::map<std::string, int>::iterator it = KeyMap::constantMap.find(key);

	if ( it == KeyMap::constantMap.end() ) {
		//Quando sao enviados botoes via VRPN um inteiro sera esperado
		this->key = atoi(key);
	} else {
		//Entretanto com o Kinect espera-se uma constante
		this->key = it->second;
	}


	//Tenta localizar a constante para toKey
	it = KeyMap::constantMap.find(toKey);

	if ( it == KeyMap::constantMap.end() ) {
		//A saida sera um botao representado por um char quando uma constante nao for encontrada
		this->toKey = toKey[0];
	} else {
		this->toKey = it->second;
		this->toKeyIsConstant = true;
	}

}