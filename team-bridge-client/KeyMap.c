#include "KeyMap.h"


//char GestureCheckerNotDefined::err[500];

std::map<std::string, int> KeyMap::configToCreatedConstants = KeyMap::create_configToCreatedConstants();
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

	std::ostringstream ret;

	if ( getIsLeaving() ) {
		ret << getToKeyRepr();
	} else {
		ret << "[" + getDev() + "] " + getKeyRepr();
		
		if ( getStrengthMax() != 0 ) {
			ret << " Max:" + std::to_string(getStrengthMax());
		}
		if ( getStrengthMin() != 0 ) {
			ret << " Min:" + std::to_string(getStrengthMin());
		}
		if ( getThumb() != -1 ) {
			ret << " Thumb:" + std::to_string(getThumb());
		}
		if ( getIndex() != -1 ) {
			ret << " Index:" + std::to_string(getIndex());
		}


		if ( getAngle() != -1 ) {
			ret << "[";
			if ( getAngleMod() == 1 ) {
				ret << ">";
			} else
			if ( getAngleMod() == -1 ) {
				ret << "<";
			} else
			if ( getAngleMod() == 0 ) {
				ret << "=";
			}
			ret << std::to_string(getAngle()) + "]";
		}
			
		ret << " -> " + getToKeyRepr();
	}
	

	if ( getIsBtn() ) {
		if ( getBtnDown() && getBtnUp() ) {
			ret << "[AUTO] ";
		} else
		if ( getBtnDown() ) {
			ret << "[DOWN]";
		} else {
			ret << "[UP]";
		}
	}

	//return dev + " " + std::to_string(key) + " " + std::to_string(toKey) + " " + std::to_string(toKeyIsConstant) + " " + std::to_string(heightSens);
	if ( getToKey() == ALERT || getToKey() == MESSAGE ) {
		ret << " \""+getMsg()+"\"";
	} else
	if ( getToKey() == VK_MOUSEMOVE ) {
		ret << " X:" + std::to_string( getX() ) + " Y:" + std::to_string(getY());
	} else
	if ( getX() != -100 ) {
		ret << " XPOS:" + std::to_string(getX());
		if ( getHandWidthInterval() != 0.40 ) {
			
			// Set Fixed -Point Notation
			ret << " [" << std::fixed << std::setprecision(2) << getHandWidthInterval() << "]";
			
		}
	}
	if ( getY() != -100 ) {
		if ( getCoordinateMod() == 1 ) {
			ret << " YPOS:>=" + std::to_string(getY());
		} else
		if ( getCoordinateMod() == -1 ) {
			ret << " YPOS:<=" + std::to_string(getY());
		} else  {
			ret << " YPOS:=" + std::to_string(getY());
		}
	}

	if ( getHasOnLeave() ) {
		ret << "\nAo sair:" + getOnLeave()->toString() + " ";
	}
	
	ret << "\n";
	return ret.str();
}

void KeyMap::setDevType(std::string devType) {
	if ( !devType.compare("mouse") ) {
		this->idDevType = DEVTYPE_MOUSE;
		this->devTypeStr = DEVTYPE_STR[this->idDevType];
	} else
	if ( !devType.compare("keyboard") ) {
		this->idDevType = DEVTYPE_KEYBOARD;
		this->devTypeStr = DEVTYPE_STR[this->idDevType];
	} else
	if ( !devType.compare("leapMotion") ) {
		this->idDevType = DEVTYPE_LEAPMOTION;
		this->devTypeStr = DEVTYPE_STR[this->idDevType];
	} else
	if ( !devType.compare("kinect") ) {
		this->idDevType = DEVTYPE_KINECT;
		this->devTypeStr = DEVTYPE_STR[this->idDevType];
	} else
	if ( !devType.compare("nedglove") ) {
		this->idDevType = DEVTYPE_NEDGLOVE;
		this->devTypeStr = DEVTYPE_STR[this->idDevType];
	}
}


KeyMap::KeyMap(json js) {

	if ( !js["dev"].is_null() ) {
		this->dev = js["dev"].get<std::string>();
	}


	if ( !js["devType"].is_null() ) {
		setDevType(js["devType"].get<std::string>());
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

	if ( !js["handWidthInterval"].is_null() ) {
		this->handWidthInterval = js["handWidthInterval"].get<double>();
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


	std::string finalKey;


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
		finalKey = this->toKeyRepr;
		this->toKeyRepr = "[" + this->toKeyRepr + "]";
	} else {

		if ( !js["toKeyWhile"].is_null() ) {
			setToKey(js["toKeyWhile"].get<std::string>());
			finalKey = js["toKeyWhile"].get<std::string>();
		} else {

			this->hasOnLeave = false;
			this->btnDown = true;
			this->btnUp = false;
			
			if ( !js["toKeyDown"].is_null() ) {
				setToKey(js["toKeyDown"].get<std::string>());
				finalKey = js["toKeyDown"].get<std::string>();
			}
				

			if ( !js["toKeyUp"].is_null() ) {
				onLeave = new KeyMap(this->dev, js["toKeyUp"].get<std::string>());
				this->hasOnLeave = true;
				finalKey = js["toKeyUp"].get<std::string>();
			}

		}

	}

	if ( !js["key"].is_null() )
		setKey(js["key"].get<std::string>());

	//Checagem para verificar se o que está no json foi realmente aplicado
	if ( finalKey.compare(toKeyRepr) && this->hasOnLeave == false ) {

		if ( this->hasOnLeave ) {
			std::string onLeavKeyRepr = this->onLeave->getToKeyRepr();
			if ( onLeavKeyRepr.compare(toKeyRepr) != 0 ) {
				printf("Falha na leitura do mapeamento %s -> %s.\n", finalKey.c_str(), toKeyRepr.c_str());
			}
		}
	}

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

	
	/*if ( !toKeyFind.compare("VK_MOUSEMOVE") ) {
		this->toKeyRepr = "VK_MOUSEMOVE";
		this->toKey = VK_MOUSEMOVE;
	}*/
	

	std::map<std::string, int>::iterator find;


	find = KeyMap::configToCreatedConstants.find(toKeyFind);

	if ( find != KeyMap::configToCreatedConstants.end() ) {
		this->toKey = find->second;
		this->toKeyIsConstant = true;
		this->toKeyRepr = find->first;
		return;
	}


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
