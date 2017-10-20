#include "InputConverter.h"

int InputConverter::lastTimeTrack = 0;


void InputConverter::press(KeyMap key) {

	printf("Press: %s ", key.getToKeyRepr().c_str());

	if ( app != "" ) {

		HWND window = FindWindow(_T(app.c_str()), NULL);
		if ( window ) {
			HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);
			printf(" em %s.\n", app.c_str());
			if ( key.getToKeyIsConstant() ) {
				PostMessage(edit, WM_KEYDOWN, key.getToKey(), 0);
			} else {
				PostMessage(edit, WM_KEYDOWN, VkKeyScanEx(key.getToKey(), GetKeyboardLayout(0)), 0);
			}
			return;
		}

	}
	printf(" no Windows.\n");

	if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON
		|| key.getToKey() == VK_MOUSEMOVE
		|| key.getToKey() == VK_RBUTTON_DOWN || key.getToKey() == VK_LBUTTON_DOWN
		|| key.getToKey() == VK_RBUTTON_UP || key.getToKey() == VK_LBUTTON_UP ) {

		INPUT input, inputUp;
		input.type = INPUT_MOUSE;//Ele automaticamente soma com a posicao atual do mouse
		inputUp.type = INPUT_MOUSE;
		
		if ( key.getToKey() == VK_MOUSEMOVE ) {
			input.mi.dwFlags = MOUSEEVENTF_MOVE;
			input.mi.dx = key.getX();
			input.mi.dy = key.getY();
			std::cout << "move to x:" << input.mi.dx << "y:" << input.mi.dy << "\n";
		} else
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_RBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		} else 
		if ( key.getToKey() == VK_LBUTTON || key.getToKey() == VK_LBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		} else 
		if ( key.getToKey() == VK_MBUTTON ) {
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
		} else 
		if ( key.getToKey() == VK_RBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		} else 
		if ( key.getToKey() == VK_LBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		}

		/*input.mi.mouseData = 0;
		input.mi.dwExtraInfo = NULL;
		input.mi.time = 0;*/
		printf("press\n");
		SendInput(1, &input, sizeof(INPUT));
		ZeroMemory(&input, sizeof(INPUT));
		//So vai pressionar o soltar se for o evento normal
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON ) {
			printf("autoup\n");
			SendInput(1, &inputUp, sizeof(INPUT));
			ZeroMemory(&inputUp, sizeof(INPUT));
		}
		
	} else {
		printf("press tecla");
		//Caso nenhum app tenha sido configurado ou encontrado lanca evento no windows
		keybd_event(key.getToKey(), 0, 0, 0);
	}

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
		press(keyMap);
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
				press((*keyMap));
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

