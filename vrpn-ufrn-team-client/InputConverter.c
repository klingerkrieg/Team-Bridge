#include "InputConverter.h"

int InputConverter::lastTimeTrack = 0;


void InputConverter::press(char key, bool isConstant) {
	printf("Press: %d [%c]", key, key);

	if ( app != "" ) {

		HWND window = FindWindow(_T(app.c_str()), NULL);
		if ( window ) {
			HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);
			printf(" em %s.\n", app.c_str());
			if ( isConstant ) {
				PostMessage(edit, WM_KEYDOWN, VK_LEFT, 0);
			} else {
				PostMessage(edit, WM_KEYDOWN, VkKeyScanEx(key, GetKeyboardLayout(0)), 0);
			}
			return;
		}

	}
	printf(" no Windows.\n");
	//Caso nenhum app tenha sido configurado ou encontrado lanca evento no windows
	keybd_event(key, 0, 0, 0);
}
/*
void InputConverter::release(char key) {
printf("Release: %d\n", key);

if ( app != "" ) {

HWND window = FindWindow(_T(app.c_str()), NULL);
HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);

if ( edit != NULL ) {
printf(" em %s.\n", app.c_str());
//PostMessage(edit, WM_KEYUP, VkKeyScanEx(key, GetKeyboardLayout(0)), 0);
PostMessage(edit, WM_KEYUP,VK_LEFT, 0);
return;
}


}
printf(" no Windows.\n");
keybd_event(key, 0, KEYEVENTF_KEYUP, 0);

}

*/

void InputConverter::interpretKeyMap(KeyMap keyMap, const vrpn_TRACKERCB t) {
	if ( keyMap.getDetermineCenterPos() ) {
		gr.setCenterPos(t);
	} else
	if ( keyMap.getShowMsg() == 0 ) {
		press(keyMap.getToKey(), keyMap.getToKeyIsConstant());
	} else
	if ( keyMap.getShowMsg() == ALERT ) {
		if ( viewOn )
			view->showAlert(keyMap.getMsg());
	} else {
		if ( viewOn )
			view->showMsg(keyMap.getMsg());
	}
}


bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {

	//Quando uma pessoa for reconhecida pelo Kinect ele ira avisar
	int actualTime = (int)time(0);
	if ( lastTimeTrack == 0 || actualTime - lastTimeTrack > 1 ) {
		printf("Kinect\n");
		if ( viewOn )
			view->showMsg("Kinect");
	}
	lastTimeTrack = actualTime;

	int top = 0;
	bool topCalculated = false;
	bool pressed = false;

	if (t.sensor == 3)
	printf("%.2f\n", t.pos[0]);//0.15 diferenca para movimentos com o corpo

	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {

		//Caso seja reconhecimento de mudanca na altura ele calcula somente uma vez nesse metodo
		if ( topCalculated == false && (keyMap->getKey() == KINECT_TOP_ADD || keyMap->getKey() == KINECT_TOP_DEC) ) {
			top = gr.detectTopChange(t, keyMap->getHeightSens());
			topCalculated = true;
		}

		//se ja foi calculado durante esse reconhecimento nao calcula novamente para as demais configuracoes de teclas
		if ( keyMap->getKey() == KINECT_TOP_ADD && topCalculated == true && top == 1 ) {
			//Se houve uma mudanca para cima e isso e esperado
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_TOP_DEC && topCalculated == true && top == -1 ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP && gr.detectRightHandTop(t, keyMap->getHandTopLevel()) ) {
			//Caso o xpos seja != -100 quer dizer que a posição X tabém é requerida para esse comando
			bool canPress = true;
			if ( keyMap->getHandXPos() != -100 ) {
				canPress = false;
				if ( gr.detectRightHandXPos(t, keyMap->getHandXPos())) {
					canPress = true;
				}
			}
			if ( canPress ) {
				interpretKeyMap((*keyMap), t);
				pressed = true;
			}
			
		} else
		if ( keyMap->getKey() == KINECT_LEFT_HAND_TOP && gr.detectLeftHandTop(t, keyMap->getHandTopLevel()) ) {
			bool canPress = true;
			if ( keyMap->getHandXPos() != -100 ) {
				canPress = false;
				if ( gr.detectLeftHandXPos(t, keyMap->getHandXPos()) ) {
					canPress = true;
				}
			}
			if ( canPress ) {
				interpretKeyMap((*keyMap), t);
				pressed = true;
			}
		} else //FAST HAND
		if ( keyMap->getKey() == KINECT_LEFT_HAND_FAST && gr.detectLeftHandFast(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_RIGHT_HAND_FAST && gr.detectRightHandFast(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else //BODY
		if ( keyMap->getKey() == KINECT_BODY_FRONT && gr.detectBodyFront(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_BODY_RIGHT && gr.detectBodyRight(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_BODY_LEFT && gr.detectBodyLeft(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_BODY_BACK && gr.detectBodyBack(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_WALK && gr.detectWalk(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_TURN_LEFT && gr.detectTurnLeft(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		} else
		if ( keyMap->getKey() == KINECT_TURN_RIGHT && gr.detectTurnRight(t) ) {
			interpretKeyMap((*keyMap), t);
			pressed = true;
		}
			

	}

	return pressed;
}

bool InputConverter::checkButton(const char * name, const vrpn_BUTTONCB b) {
	bool pressed = false;

	

	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {
		//printf("%s == %s && %d == %d\n", name, keyMap->getDev().c_str(),  keyMap->getKey(), b.button);

		if ( !strcmp(name, keyMap->getDev().c_str()) && keyMap->getKey() == b.button ) {
			if ( b.state == 1 ) {
				press(keyMap->getToKey(), keyMap->getToKeyIsConstant());
				pressed = true;
			} else {
				//o release esta ocorrendo em duplicacao do evento
				//release(keyMap->toKey);
			}
		}
	}

	return pressed;

}

bool InputConverter::checkAnalog() {
	return true;
}

