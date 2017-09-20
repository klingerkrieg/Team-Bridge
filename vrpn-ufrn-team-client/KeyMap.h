#pragma once
#include <map>
#include <string>
#include <Windows.h>

static const int SSIZE = 512;


class KeyMap {
public:
	KeyMap(char key, char toKey);
	KeyMap(char key[SSIZE], char toKey[SSIZE]);
	char key;
	char toKey;

	static std::map<std::string, int> create_map() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( constantMap.size() > 0 ) {
			return constantMap;
		}

		std::map < std::string, int > m;
		m["WM_LBUTTONDOWN"] = WM_LBUTTONDOWN;
		m["WM_RBUTTONDOWN"] = WM_RBUTTONDOWN;
		m["WM_MBUTTONDOWN"] = WM_MBUTTONDOWN;
		m["VK_ESCAPE"]		= VK_ESCAPE;
		m["VK_RETURN"]		= VK_RETURN;
		m["VK_UP"]			= VK_UP;
		m["VK_DOWN"]		= VK_DOWN;
		m["VK_LEFT"]		= VK_LEFT;
		m["VK_RIGHT"]		= VK_RIGHT;
		
		return m;
	}
	static  std::map<std::string, int> constantMap;

};


std::map<std::string, int> KeyMap::constantMap = KeyMap::create_map();


KeyMap::KeyMap(char key, char toKey) {
	this->key = key;
	this->toKey = toKey;
}


KeyMap::KeyMap(char key[SSIZE], char toKey[SSIZE]) {
	
	if ( strlen(key) == 1) {
		//o evento que vem do VRPN sempre sera um numero inteiro ele nao envia eventos do tipo do Windows
		this->key = atoi(key);
	} else {
		this->key = KeyMap::constantMap[key];
	}

	if ( strlen(toKey) == 1 ) {
		this->toKey = toKey[0];
	} else {
		this->toKey = KeyMap::constantMap[toKey];
	}

}