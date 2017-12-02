#include "KeyMap.h"


char GestureCheckerNotDefined::err[500];

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



std::string KeyMap::toString() {

	std::string ret = "";
	if ( getIsLeaving() ) {
		ret = getToKeyRepr();
	} else {
		ret = "[" + getDev() + "] " + getKeyRepr();
		
		if ( getStrengthMax() != 0 ) {
			ret += " Max:" + std::to_string(getStrengthMax());
		}
		if ( getStrengthMin() != 0 ) {
			ret += " Min:" + std::to_string(getStrengthMin());
		}
		if ( getThumb() != -1 ) {
			ret += " Thumb:" + std::to_string(getThumb());
		}
		if ( getIndex() != -1 ) {
			ret += " Index:" + std::to_string(getIndex());
		}


		if ( getAngle() != -1 ) {
			ret += "[";
			if ( getAngleMod() == 1 ) {
				ret += ">";
			} else
			if ( getAngleMod() == -1 ) {
				ret += "<";
			} else
			if ( getAngleMod() == 0 ) {
				ret += "=";
			}
			ret += std::to_string(getAngle()) + "]";
		}
			
		ret += " -> " + getToKeyRepr();
	}
	

	if ( getIsBtn() ) {
		if ( getBtnDown() && getBtnUp() ) {
			ret += "[AUTO] ";
		} else
			if ( getBtnDown() ) {
				ret += "[DOWN]";
			} else {
				ret += "[UP]";
			}

		if ( getHasOnLeave() ) {
			ret += "|" + getOnLeave()->toString() + " ";
		}
	}

	//return dev + " " + std::to_string(key) + " " + std::to_string(toKey) + " " + std::to_string(toKeyIsConstant) + " " + std::to_string(heightSens);
	if ( getToKey() == ALERT || getToKey() == MESSAGE ) {
		ret += " \""+getMsg()+"\"";
	} else
	if ( getToKey() == VK_MOUSEMOVE ) {
		ret += " X:" + std::to_string( getX() ) + " Y:" + std::to_string(getY());
	} else
	if ( getX() != -100 ) {
		ret += " XPOS:" + std::to_string(getX());
	}
	if ( getY() != -100 ) {
		if ( getCoordinateMod() == 1 ) {
			ret += " YPOS:>=" + std::to_string(getY());
		} else
		if ( getCoordinateMod() == -1 ) {
			ret += " YPOS:<=" + std::to_string(getY());
		} else  {
			ret += " YPOS:=" + std::to_string(getY());
		}
	}
	
	ret += "\n";
	return ret;
}


KeyMap::KeyMap(json js) {

	if ( !js["dev"].is_null() ) {
		this->dev = js["dev"].get<std::string>();
	}
	if ( !js["idDevType"].is_null() ) {
		this->idDevType = js["idDevType"].get<int>();
	}
	
	if ( !js["x"].is_null() ) {
		this->x = js["x"].get<int>();
	}

	if ( !js["y"].is_null() ) {
		this->y = js["y"].get<int>();
	}

	if ( !js["coordinateMod"].is_null() ) {
		std::string coordinateModStr = js["coordinateMod"].get<std::string>();
		if ( !coordinateModStr.compare("<=") ) {
			this->coordinateMod = -1;
		} else
		if ( !coordinateModStr.compare(">=") ) {
			this->coordinateMod = 1;
		} else 
		if ( !coordinateModStr.compare("=") ) {
			this->coordinateMod = 0;
		}	
	}

	if ( !js["sensivity"].is_null() ) {
		this->sensivity = js["sensivity"].get<double>();
	}

	//NedGlove angulo para fechar e abrir mao, cada luva pode ter um valor diferente
	//Depende da calibração
	if ( !js["strengthMax"].is_null() ) {
		this->strengthMax = js["strengthMax"].get<int>();
	}
	if ( !js["strengthMin"].is_null() ) {
		this->strengthMin = js["strengthMin"].get<int>();
	}
	if ( !js["angleMax"].is_null() ) {
		this->angleMax = js["angleMax"].get<float>();
	}
	if ( !js["angleMin"].is_null() ) {
		this->angleMin = js["angleMin"].get<float>();
	}
	if ( !js["distanceMax"].is_null() ) {
		this->distanceMax = js["distanceMax"].get<float>();
	}
	if ( !js["distanceMin"].is_null() ) {
		this->distanceMin = js["distanceMin"].get<float>();
	}
	if ( !js["thumb"].is_null() ) {
		this->thumb = js["thumb"].get<int>();
	}
	if ( !js["index"].is_null() ) {
		this->index = js["index"].get<int>();
	}

	if ( !js["saveData"].is_null() ) {
		this->saveData = js["saveData"].get<std::string>();
	}

	if ( !js["maxVelociyMs"].is_null() ) {
		this->maxVelociyMs = js["maxVelociyMs"].get<double>();
	}

	if ( !js["angle"].is_null() ) {
		this->angle = js["angle"].get<int>();
		this->angleMod = 1;//padrão
	}

	if ( !js["angleMod"].is_null() ) {
		std::string angleModStr = js["angleMod"].get<std::string>();
		if ( !angleModStr.compare("<") ) {
			this->angleMod = -1;
		} else
		if ( !angleModStr.compare(">") ) {
			this->angleMod = 1;
		} else
		if ( !angleModStr.compare("=") ) {
			this->angleMod = 0;
		}
	}

	if ( !js["delay"].is_null() ) {
		this->delay = js["delay"].get<int>();
	}


	if ( !js["mouseX"].is_null() ) {
		this->mouseX = js["mouseX"].get<int>();
	}

	if ( !js["mouseY"].is_null() ) {
		this->mouseY = js["mouseY"].get<int>();
	}


	if ( !js["msg"].is_null() ) {
		this->msg = js["msg"].get<std::string>();
		
		if ( !js["toKeyDown"].is_null() ) {
			this->toKeyRepr = js["toKeyDown"].get<std::string>();
		} else
		if ( !js["toKeyUp"].is_null() ) {
			this->toKeyRepr = js["toKeyUp"].get<std::string>();
		} else
		if ( !js["toKeyWhile"].is_null() ) {
			this->toKeyRepr = js["toKeyWhile"].get<std::string>();
		}

		if ( !this->toKeyRepr.compare("ALERT") ) {
			this->toKey = ALERT;
		} else {
			this->toKey = MESSAGE;
		}

		this->toKeyRepr = "[" + this->toKeyRepr + "]";
	} else {

		if ( !js["toKeyWhile"].is_null() ) {
			setToKey(js["toKeyWhile"].get<std::string>());
		} else {

			this->hasOnLeave = true;
			this->btnDown = true;
			this->btnUp = false;
			
			if ( !js["toKeyDown"].is_null() )
				setToKey(js["toKeyDown"].get<std::string>());

			if ( !js["toKeyUp"].is_null() ) {
				onLeave = new KeyMap(this->dev, js["toKeyUp"].get<std::string>());
				this->hasOnLeave = true;
			}

		}

	}

	if ( !js["key"].is_null() )
		setKey(js["key"].get<std::string>());

}

KeyMap::KeyMap(std::string dev, std::string toKeyUp) {
	this->dev = dev;
	setToKey(toKeyUp);
	this->isLeaving = true;
	this->btnDown = false;
	this->btnUp = true;
}



void KeyMap::setKey(std::string key) {
	std::map<std::string, int>::iterator it;
	it = KeyMap::configToScanCode.find(key);
	if ( it == KeyMap::configToScanCode.end() ) {
		//Quando sao enviados botoes via VRPN um inteiro sera esperado
		this->key = key[0];//quando nao encontrar usa como char
		this->keyRepr = key[0];
		if ( strlen(key.c_str()) > 1 ) {
			std::string msg = key;
			msg = "Nao foi possivel encontrar mapeamento para " + msg;
			std::cout << msg << "\n";
			throw std::exception((msg).c_str());
		}
	} else {
		//Entretanto com o Kinect espera-se uma constante
		this->key = it->second;
		this->keyRepr = it->first;
	}
}

void KeyMap::setToKey(std::string toKeyFind) {
	//Sera um acionamento de botao
	isBtn = true;

	
	if ( !toKeyFind.compare("VK_MOUSEMOVE") ) {
		this->toKeyRepr = "VK_MOUSEMOVE";
		this->toKey = VK_MOUSEMOVE;
	}
	

	std::map<std::string, int>::iterator find;

	//Tenta localizar a constante para toKey
	find = KeyMap::configToAscii.find(toKeyFind);

	if ( find == KeyMap::configToAscii.end() ) {
		//A saida sera um botao representado por um char quando uma constante nao for encontrada
		this->toKey = toKeyFind[0];
		this->toKeyRepr = toKeyFind[0];
	} else {
		this->toKey = find->second;
		this->toKeyIsConstant = true;
		this->toKeyRepr = find->first;
	}
}
