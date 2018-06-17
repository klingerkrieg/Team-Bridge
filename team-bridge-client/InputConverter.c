#include "InputConverter.h"


std::vector<KeyMap> InputConverter::map;
std::vector<DeviceType> InputConverter::devs;

int InputConverter::lastTimeTrack = 0;
SkeletonPart InputConverter::skelPart;



void InputConverter::interpretKeyMap(KeyMap *keyMap) {

	if ( keyMap->getToKey() == KINECT_SET_CENTER_POS ) {
		gr->setCenterPos(skelPart, keyMap);
		printf("Center pos defined.\n");
		if ( viewOn )
			view->showMsg("Posição definida.");
	} else
	if ( keyMap->getToKey() == ALERT ) {
		printf("ALERT: %s\n", keyMap->getMsg().c_str());
		if ( viewOn )
			view->showAlert(keyMap->getMsg());
	} else
	if ( keyMap->getToKey() == MESSAGE ) {
		printf("MESSAGE: %s\n", keyMap->getMsg().c_str());
		if ( viewOn )
			view->showMsg(keyMap->getMsg());
	} else {
		//Executa a ação, para essa aplicação são inputs de mouse e teclado
		act->run((*keyMap));
	}
}


bool InputConverter::interpretOnLeave(bool active, KeyMap *keyMap) {

	if ( keyMap->getKey() == JOINT_ANGLE ) {
		act->run((*keyMap));
	}

	//Se esta esperando para soltar a tecla e a tecla foi solta
	if ( keyMap->getActive() && active == false ) {
		
		if ( keyMap->getToKeyUp() ) {
			interpretKeyMap(keyMap->getOnLeave());
		}
		keyMap->setActive(false);
		return true;
	} else
		if ( active == true) {
			//Se foi ativada

			//se é enquanto, sempre interpreta
			if ( keyMap->getToKeyWhile() ) {
				interpretKeyMap(keyMap);
				//nao muda o status de ativa, visto que a interpretacao irá apertar e soltar imediatamente
			} else {
				
				//Se a tecla nao está ativada e possui toKeyDown
				if ( keyMap->getActive() == false && keyMap->getToKeyDown() == true ) {
					//Se nao tiver evento de saida aciona normalmente
					interpretKeyMap(keyMap);
					keyMap->setActive(true);
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
			interpretOnLeave(0, keyMap);
		}
	}
}


bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {

	//Identifica qual dispositivo esta sendo usado
	int devType = -1;
	size_t i;
	for ( i = 0; i < devs.size(); i++ ) {
		if ( !devs.at(i).name.compare(userdata->name) ) {
			devType = devs.at(i).type;
			break;
		}
	}


	//De acordo com cada dispositivo um esqueleto diferente sera utilizado
	if ( devType == DEVTYPE_KINECT_V2 ) {
		vrpnToSkeleton(gr->KinectGestures::skeleton[(std::string)userdata->name], gr->skeletonMapV2, t, skelPart, (std::string)userdata->name);
	} else
	if ( devType == DEVTYPE_KINECT ) {

		//Se ele identificar sensor 20 ele muda o tipo para KinectV2
		if ( t.sensor == 20 ) {
			devs.at(i).type = DEVTYPE_KINECT_V2;
			//Na próxima atualização ele envia para o local correto
			return false;
		}

		vrpnToSkeleton(gr->KinectGestures::skeleton[(std::string)userdata->name], gr->skeletonMap1, t, skelPart, (std::string)userdata->name);
	} else
	if ( devType == DEVTYPE_LEAPMOTION ) {
		//LeapMotion
		vrpnToSkeleton(gr->LeapMotionGestures::skeleton[(std::string)userdata->name], gr->handSkeletonMap1, t, skelPart, (std::string)userdata->name);
	} else {
		printf("Dispositivo desconhecido:%s\n", userdata->name);
		return false;
	}
	

	int actualTime = (int)time(0);
	

	//Quando uma pessoa for reconhecida pelo Kinect ou LeapMotion ele ira avisar
	if ( lastTimeTrack == 0 || actualTime - lastTimeTrack > 1 ) {
		printf("Capturando.\n");
		if ( viewOn )
			view->showMsg("Capturando");

		gr->clearNormalStepHeight();
	}
	lastTimeTrack = actualTime;

	
	return checkers.at(userdata->name).changeState(skelPart, this);
}

bool InputConverter::checkButton(const char * name, const vrpn_BUTTONCB b) {
	printf("Button: %d\n", b.button);
	return checkers.at(name).changeState(b, this);
}


bool InputConverter::checkAnalog(const char *name, const vrpn_ANALOGCB a) {
	return checkers.at(name).changeState(a, this);
}

