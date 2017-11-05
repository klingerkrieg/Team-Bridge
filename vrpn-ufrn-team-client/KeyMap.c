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


std::string KeyMap::toString() {

	std::string ret = "";
	if ( getIsLeaving() ) {
		ret = "-->\t" + getToKeyRepr() + "";
	} else {
		ret = "[" + getDev() + "]\t" + getKeyRepr();
		
		if ( getAngle() != -1 ) {
			ret += "[" + std::to_string(getAngle()) + "]";
		}
			
		ret += "\t->\t" + getToKeyRepr();
	}
	

	if ( getIsBtn() ) {
		if ( getBtnDown() && getBtnUp() ) {
			ret += "[AUTO]\t";
		} else
			if ( getBtnDown() ) {
				ret += "[DOWN]\t";
			} else {
				ret += "[UP]\t";
			}
	}

	//return dev + " " + std::to_string(key) + " " + std::to_string(toKey) + " " + std::to_string(toKeyIsConstant) + " " + std::to_string(heightSens);
	if ( getShowMsg() != 0 ) {
		ret += "\t\""+getMsg()+"\"";
	} else
	if ( getToKey() == VK_MOUSEMOVE ) {
		ret += "\tX:" + std::to_string( getX() ) + "\tY:" + std::to_string(getY());
	} else
	if ( getHandXPos() != -100 ) {
		ret += "\tXPOS:" + std::to_string(getHandXPos());
	}
	if ( getHandTopLevel() != -100 ) {
		if ( getHandTopMod() == 1 ) {
			ret += "\tYPOS:>" + std::to_string(getHandTopLevel());
		} else
		if ( getHandTopMod() == -1 ) {
			ret += "\tYPOS:<" + std::to_string(getHandTopLevel());
		} else  {
			ret += "\tYPOS:" + std::to_string(getHandTopLevel());
		}
	}
	

	if ( getHasOnLeave() ) {
		ret += "\n\t\t\t\t" + getOnLeave()->toString();
	}


	ret += "\n";
	return ret;
}

KeyMap::KeyMap(std::string dev, std::string config) {

	this->dev = dev;

	replace_all(config, "  ", " ");
	replace_all(config, " ", "\t");

	std::replace(config.begin(), config.end(), ' ', '\t');
	std::vector<std::string> tokens = split(config, "\t");
	std::vector<std::string> options;

	bool first = true;

	for ( std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it ) {

		if ( !(*it).compare("") ) {
			continue;
		}

		if ( first ) {

			//char *keyChar = "";
			std::string keyChar;
			
			first = false;

			if ( contains((*it), "LEAVING") ) {
				isLeaving = true;
				continue;
			} else
			if ( contains((*it), "LEAP_RIGHT_FIST")
				|| contains((*it), "LEAP_LEFT_FIST")
				|| contains((*it), "KINECT_RIGHT_FIST")
				|| contains((*it), "KINECT_LEFT_FIST") ) {
				
				options = split((*it), "=");
				keyChar = options.front();
				if ( options.size() == 1 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}
				angle = atoi(options.at(1).c_str());


			} else
			if ( contains((*it), "HAND_TOP") ) {
				options = split((*it), "=");
				//Vai procurar na lista a constante exata
				//keyChar = options.front().c_str();
				//strcpy(keyChar, options.front().c_str());
				keyChar = options.front();

				if ( options.size() == 1 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}

				if ( contains(options.at(1), ",") ) {
					options = split(options.at(1), ",");
					//salva x e y
					handXPos = atoi(options.at(0).c_str());

					char* yChr;

					if ( starts_with(options.at(1), ">") ) {
						handTopMod = 1;
						yChr = &options.at(1)[1];
					} else
					if ( starts_with(options.at(1), "<") ) {
						handTopMod = -1;
						yChr = &options.at(1)[1];
					} else {
						handTopMod = 0;
						yChr = &options.at(1)[0];
					}

					handTopLevel = atoi(yChr);
				} else {
					//salva somente y
					handTopLevel = atoi(options.at(1).c_str());
				}

			} else {
				//strcpy(keyChar, (*it).c_str());
				keyChar = (*it);
			}


			
			
			if ( starts_with((*it), "KINECT_TOP_ADD") || starts_with((*it), "KINECT_TOP_DEC") ) {
				options = split((*it), "=");
				//lancar erro caso nao tenha msg
				if ( options.size() == 1 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}


				heightSens = atof(options.at(1).c_str());
				if ( starts_with((*it), "KINECT_TOP_ADD")  ) {
					key = KINECT_TOP_ADD;
				} else {
					key = KINECT_TOP_DEC;
				}
				keyRepr = (*it);

			} else {

				std::map<std::string, int>::iterator find;
				

				//Tenta localizar a constante em key
				find = KeyMap::configToScanCode.find(keyChar);
				if ( find == KeyMap::configToScanCode.end() ) {

					this->key = keyChar[0];//quando nao encontrar usa como char
					this->keyRepr = keyChar[0];

					if ( keyChar.length() > 1 ) {
						std::string msg = keyChar;
						msg = "Nao foi possivel encontrar mapeamento para " + msg;
						std::cout << msg << "\n";
						throw std::exception((msg).c_str());
					}
				} else {
					//Entretanto com o Kinect espera-se uma constante
					this->key = find->second;
					this->keyRepr = find->first;
				}
			}
			
		} else

		/* comandos para acionar */
			
		if ( starts_with((*it), "ONLEAVE") ) {
			options = split(config, "ONLEAVE");
			if ( options.size() != 2 ) {
				printf("Linha configurada incorretamente: %s\n", config.c_str());
			}

			hasOnLeave = true;
			onLeave = new KeyMap(dev, "LEAVING\t"+options.at(1));
			return;
		} else
		if ( starts_with((*it), "VK_MOUSEMOVE") ){
				options = split((*it), "=");
				//lancar erro caso nao tenha dois parametros
				if ( options.size() == 1 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}

				options = split(options.at(1), ",");
				//lancar erro caso nao tenha dois parametros
				if ( options.size() == 1 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}

				x = atoi(options.at(0).c_str());
				y = atoi(options.at(1).c_str());
				toKey = VK_MOUSEMOVE;
				toKeyRepr = "VK_MOUSEMOVE";

		} else
		if ( starts_with((*it), "ALERT") || starts_with((*it), "MESSAGE") ) {

			if ( (*it)[0] == 'M' ) {
				this->showMsg = MESSAGE;
				this->toKeyRepr = "[MESSAGE]";
				options = split(config, "MESSAGE=");
			} else {
				this->showMsg = ALERT;
				this->toKeyRepr = "[ALERT]";
				options = split(config, "ALERT=");
			}

			//lancar erro caso nao tenha msg
			if ( options.size() == 1 ) {
				printf("Linha configurada incorretamente: %s\n", config.c_str());
			}
			this->msg = options.at(1);
			return;
		} else 
		if ( starts_with((*it), "KINECT_DETERMINE_CENTER_POS") ) {
			this->determineCenterPos = true;
			this->toKeyRepr = "KINECT_DETERMINE_CENTER_POS";
		} else {

			std::string toKeyFind = (*it);
			//Sera um acionamento de botao
			isBtn = true;

			if ( starts_with((*it), "BTNDOWN") || starts_with((*it), "BTNUP") ) {
				options = split((*it), "=");
				if ( options.size() != 2 ) {
					printf("Linha configurada incorretamente: %s\n", config.c_str());
				}

				if ( starts_with((*it), "BTNDOWN") ) {
					btnDown = true;
					btnUp = false;
				} else {
					btnDown = false;
					btnUp = true;
				}

				toKeyFind = options.at(1);
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

		

	}
	/*std::vector<std::string> tokens;
	/*std::copy(std::istream_iterator<std::string>(config),
		 std::istream_iterator<std::string>(),
		 std::back_inserter(tokens));*/
}

/*
KeyMap::KeyMap(std::string dev, char key[SSIZE], char toKey[SSIZE], char config[SSIZE], char config2[SSIZE]) : KeyMap(dev, key, toKey, config2) {
	//Unico caso que isso acontecera é com o comando KINECT_LEFT_HAND_TOP ou KINECT_RIGHT_HAND_TOP
	//a configuracao deve estar na seguinte ordem KEY KINECT_RIGHT_HAND_TOP		BTN		XPOS	YPOS
	
	if ( this->toKey == VK_MOUSEMOVE ) {
		x = atoi(config);
		y = atoi(config2);
	} else {
		//Salva o XPOS
		handXPos = atoi(config);
	}

}

KeyMap::KeyMap(std::string dev, char key[SSIZE], char toKey[SSIZE], char config[SSIZE]) {

	//Cria normalmente
	init(dev, key, toKey);
	//Caso seja top no kinect a configuracao extra sera a sensibilidade
	if ( this->key == KINECT_TOP_ADD || this->key == KINECT_TOP_DEC ) {
		heightSens = atof(config);
	}
	//Caso seja altura da mao no kinect
	if ( this->key == KINECT_LEFT_HAND_TOP || this->key == KINECT_RIGHT_HAND_TOP ) {
		handTopLevel = atoi(config);
	}

	//Caso seja para exibir uma mensagem na tela
	if ( this->showMsg != 0 ) {
		msg = config;
	}

}

KeyMap::KeyMap(std::string dev, char key[SSIZE], char toKey[SSIZE]) {
	init(dev, key, toKey);
}
*/

void KeyMap::init(std::string dev, char key[SSIZE], char toKey[SSIZE]) {
	
	this->dev = dev;
	std::map<std::string, int>::iterator it;
	
	
	//Tenta localizar a constante em key
	it = KeyMap::configToScanCode.find(key);
	if ( it == KeyMap::configToScanCode.end() ) {
		//Quando sao enviados botoes via VRPN um inteiro sera esperado
		//this->key = atoi(key);
		this->key = key[0];//quando nao encontrar usa como char
		this->keyRepr = key[0];
		if ( strlen(key) > 1 ) {
			std::string msg = key;
			msg = "Nao foi possivel encontrar mapeamento para " + msg;
			throw std::exception((msg).c_str());
			std::cout << msg << "\n";
		}
	} else {
		//Entretanto com o Kinect espera-se uma constante
		this->key = it->second;
		this->keyRepr = it->first;
	}
	
	//Quando o toKey for algo diferente de uma tecla nao será possível te-lo em configToAScii
	
	 
	if ( !strcmp("VK_MOUSEMOVE", toKey ) ) {
		this->toKey = VK_MOUSEMOVE;
		this->toKeyRepr = "VK_MOUSEMOVE";
	} else
	if ( !strcmp("KINECT_DETERMINE_CENTER_POS", toKey) ) {
		this->determineCenterPos = true;
		this->toKeyRepr = "KINECT_DETERMINE_CENTER_POS";
	} else
	if ( !strcmp("ALERT", toKey) ) {
		this->showMsg = ALERT;
		this->toKeyRepr = "[ALERT]";
	} else
	if ( !strcmp("MESSAGE", toKey) ) {
		this->showMsg = MESSAGE;
		this->toKeyRepr = "[MESSAGE]";
	} else {

		//Tenta localizar a constante para toKey
		it = KeyMap::configToAscii.find(toKey);

		if ( it == KeyMap::configToAscii.end() ) {
			//A saida sera um botao representado por um char quando uma constante nao for encontrada
			this->toKey = toKey[0];
			this->toKeyRepr = toKey[0];
		} else {
			this->toKey = it->second;
			this->toKeyIsConstant = true;
			this->toKeyRepr = it->first;
		}
	}

	

}