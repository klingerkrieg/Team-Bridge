#include "InputConverter.h"

int InputConverter::lastTimeTrack = 0;
bool InputConverter::nextDefineCenterPos = false;
int InputConverter::lastTimeCenterPos = 0;

std::vector<KeyMap> InputConverter::map;
std::vector<DeviceType> InputConverter::devs;


SkeletonPart InputConverter::skelPart;

std::map<std::string, DeviceSensorCount> InputConverter::devicesSensorsCount;


#ifdef PERFORMANCE_TEST
double InputConverter::qtdMed = 0;
double InputConverter::usecMed = 0;
double InputConverter::secMed = 0;
#endif


void InputConverter::interpretKeyMap(KeyMap &keyMap) {
	
	if ( keyMap.getToKey() == KINECT_SET_CENTER_POS ) {
		gr.setCenterPos(&skelPart, &keyMap);
		printf("Center pos defined.");
		if ( viewOn )
			view->showMsg("Posição definida.");
	} else
	if ( keyMap.getToKey() == ALERT ) {
		if ( viewOn )
			view->showAlert(keyMap.getMsg());
	} else
	if ( keyMap.getToKey() == MESSAGE ){
		if ( viewOn )
			view->showMsg(keyMap.getMsg());
	} else {
		//Executa a ação, para essa aplicação são inputs de mouse e teclado
		act->run(keyMap);
	}
}


bool InputConverter::interpretOnLeave(bool active, KeyMap &keyMap) {

	//Se esta esperando para soltar a tecla e a tecla foi solta
	if ( keyMap.getActive() && !active ) {
		
		if ( keyMap.getHasOnLeave() ) {
			interpretKeyMap((*keyMap.getOnLeave()));
		}
		keyMap.setActive(false);
		return true;
	} else
	if ( active ) {
		//Se foi ativada
		if ( keyMap.getHasOnLeave() ) {
			//Verifica se tem evento de saida
			if ( !keyMap.getActive() ) {
				//Se tiver so aciona o comando novamente se ele nao estiver esperando evento de saida
				interpretKeyMap(keyMap);
			}
			keyMap.setActive(true);
		} else {

			if ( keyMap.getActive() == false ) {
				//Se nao tiver evento de saida aciona normalmente
				interpretKeyMap(keyMap);
			}

			//se não tem hasOnLeave
			if ( keyMap.getBtnUp() == false ) {
				keyMap.setActive(true);
			}

		}
		return true;
	}
	return false;
}

//Solta todas as teclas que foram pressionadas
InputConverter::~InputConverter() {
	KeyMap *keyMap;
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		keyMap = &map.at(keyMapId);

		if (keyMap->getActive()) {
			interpretOnLeave(0, (*keyMap));
		}
	}
}


bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {

	skelPart;

	//Identifica qual dispositivo esta sendo usado
	int devType = -1;
	for ( size_t i = 0; i < devs.size(); i++ ) {
		if ( !devs.at(i).name.compare(userdata->name) ) {
			devType = devs.at(i).type;
			break;
		}
	}

	//De acordo com cada dispositivo um esqueleto diferente sera utilizado
	if ( devType == DEVTYPE_KINECT ) {
		vrpnToSkeleton(gr.KinectGestures::skeleton[(std::string)userdata->name], gr.skeletonMap1, t, skelPart, (std::string)userdata->name);
	} else
		if ( devType == DEVTYPE_LEAPMOTION ) {
		//LeapMotion
		vrpnToSkeleton(gr.LeapMotionGestures::skeleton[(std::string)userdata->name], gr.handSkeletonMap1, t, skelPart, (std::string)userdata->name);
	} else {
		printf("Dispositivo desconhecido:%s\n", userdata->name);
		return false;
	}

	int actualTime = (int)time(0);
	

	//Quando uma pessoa for reconhecida pelo Kinect ou LeapMotion ele ira avisar
	if ( lastTimeTrack == 0 || actualTime - lastTimeTrack > 1 ) {
		if ( viewOn )
			view->showMsg("Capturando");
	}
	lastTimeTrack = actualTime;

	
	int active;
	//int activeSecondary;
	bool pressed = false;
	KeyMap *keyMap;
	int z = 0;


	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++){
		keyMap = &map.at(keyMapId);

		if ( !strcmp(userdata->name, keyMap->getDev().c_str()) ) {
			if ( keyMap->getGestureCheckerDefined() ) {


				
				try {
					active = keyMap->callGestureChecker(skelPart);
				} catch ( ... ) {
					printf("Falha ao checar:%s\n", keyMap->toString().c_str());
					active = -1;
				}

				
				z++;

				//será -1 quando não for o sensor responsável pelo gesto
				if ( active != -1 ) {

					/*if ( keyMap->getToKeyRepr().compare("VK_LBUTTON_DOWN") ) {
						printf("%d\n", active);
					}*/
					
					//Se pelo menos uma foi pressionada
					//nao pode retornar aqui porque podem ter outros comandos
					if ( interpretOnLeave(active, (*keyMap)) ) {
						pressed = true;

					#ifdef PERFORMANCE_TEST
						timeval t2;
						vrpn_gettimeofday(&t2, NULL);

						int pressedUsecSize = std::to_string(t2.tv_usec).length();
						int sentUsecSize = std::to_string(t.msg_time.tv_usec).length();

						double pressed = (double)t2.tv_sec + ((double)t2.tv_usec / pow(10, pressedUsecSize));
						double sent = (double)t.msg_time.tv_sec + ((double)t.msg_time.tv_usec / pow(10, pressedUsecSize));
						double delay = pressed - sent;

						secMed += delay;
						qtdMed++;

						printf("Delay: %.4f - %.4f = ", pressed, sent);
						printf("%.4f", delay);
						printf(" Media: %.3f\n", (secMed / qtdMed));
					#endif
					}
				}
			} else {
				GestureCheckerNotDefined err = GestureCheckerNotDefined(keyMap);
				printf("%s", err.what());
				throw err;
			}

		}

	}

	return pressed;
}

bool InputConverter::checkButton(const char * name, const vrpn_BUTTONCB b) {
	bool pressed = false;
	int active;

	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {

		if ( !strcmp(name, keyMap->getDev().c_str()) ) {
			if ( keyMap->getGestureCheckerDefined() ) {
				try {
					active = keyMap->callGestureChecker(b);
				} catch ( ... ) {
					printf("Falha ao checar:%s", keyMap->toString().c_str());
				}

				if ( active != -1 ) {
					pressed = interpretOnLeave(active, (*keyMap));
				}
			}
		}
	}

	return pressed;

}


bool InputConverter::checkAnalog(const char *name, const vrpn_ANALOGCB a) {
	bool pressed = false;
	int active;
	KeyMap *keyMap;
	
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		keyMap = &map.at(keyMapId);

		if ( !strcmp(name, keyMap->getDev().c_str()) ) {
			//Caso o keyMap possua um método responsável por sua checagem ele será chamado
			//Semelhante ao padrão Observer
			if ( keyMap->getGestureCheckerDefined() ) {
				try {
					active = keyMap->callGestureChecker(a);
				} catch ( ... ) {
					printf("Falha ao checar:%s", keyMap->toString().c_str());
				}
				
				if ( active != -1 ) {
					pressed = interpretOnLeave(active, (*keyMap));
				}
			} else {
				printf("Nao possui metodo para checagem:%s\n", keyMap->toString().c_str());
			}
		}

	}
	

	return true;
}

