#include "InputConverter.h"

int InputConverter::lastTimeTrack = 0;
bool InputConverter::nextDefineCenterPos = false;
int InputConverter::lastTimeCenterPos = 0;

std::vector<KeyMap> InputConverter::map;

bool InputConverter::mouseLeftPressed = false;
bool InputConverter::mouseRightPressed = false;
bool InputConverter::mouseMiddlePressed = false;

void InputConverter::press(KeyMap key) {

	bool print = true;
	int actualTime = (int)time(0);

	if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON
		|| key.getToKey() == VK_MOUSEMOVE
		|| key.getToKey() == VK_RBUTTON_DOWN || key.getToKey() == VK_LBUTTON_DOWN
		|| key.getToKey() == VK_RBUTTON_UP || key.getToKey() == VK_LBUTTON_UP
		|| key.getToKey() == VK_MBUTTON_UP || key.getToKey() == VK_MBUTTON_UP ) {


		INPUT input, inputUp;
		input.type = INPUT_MOUSE;//Ele automaticamente soma com a posicao atual do mouse
		inputUp.type = INPUT_MOUSE;

		input.mi.mouseData = 0;
		input.mi.time = 0;
		inputUp.mi.mouseData = 0;
		inputUp.mi.time = 0;
		
		if ( key.getToKey() == VK_MOUSEMOVE ) {
			input.mi.dwFlags = MOUSEEVENTF_MOVE;
			input.mi.dx = key.getX();
			input.mi.dy = key.getY();
			//std::cout << "move to x:" << input.mi.dx << "y:" << input.mi.dy << "\n";
		} else
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_RBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			//Se o botao ja esta pressionado nao pressiona de novo
			if ( mouseRightPressed ) {
				return;
			}
			mouseRightPressed = true;
		} else 
		if ( key.getToKey() == VK_LBUTTON || key.getToKey() == VK_LBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			//Se o botao ja esta pressionado nao pressiona de novo
			if ( mouseLeftPressed ) {
				return;
			}
			mouseLeftPressed = true;
		} else 
		if ( key.getToKey() == VK_MBUTTON || key.getToKey() == VK_MBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			if ( mouseMiddlePressed ) {
				return;
			}
			mouseMiddlePressed = true;
		} else 
		if ( key.getToKey() == VK_RBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			//Se o botao nao esta pressionado, nao tem nada pra despressionar
			if ( mouseRightPressed == false ) {
				return;
			}
			mouseRightPressed = false;
		} else 
		if ( key.getToKey() == VK_LBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			if ( mouseLeftPressed == false ) {
				return;
			}
			mouseLeftPressed = false;
		} else 
		if ( key.getToKey() == VK_MBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			if ( mouseMiddlePressed == false ) {
				return;
			}
			mouseMiddlePressed = false;
		}

		/*input.mi.mouseData = 0;
		input.mi.dwExtraInfo = NULL;
		input.mi.time = 0;*/
		if ( print && mouseMiddlePressed )
			printf("Press: %s on windows.\n", key.getToKeyRepr().c_str());
		else if ( print && !mouseMiddlePressed )
			printf("Unpress: %s on windows.\n", key.getToKeyRepr().c_str());

		SendInput(1, &input, sizeof(INPUT));
		ZeroMemory(&input, sizeof(INPUT));
		//So vai pressionar o soltar se for o evento normal
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON ) {
			SendInput(1, &inputUp, sizeof(INPUT));
			ZeroMemory(&inputUp, sizeof(INPUT));

			//Caso tenha sido os dois eventos em um só
			if ( key.getToKey() == VK_LBUTTON ) {
				mouseLeftPressed = false;
			} else
			if ( key.getToKey() == VK_RBUTTON ) {
				mouseRightPressed = false;
			} else
			if ( key.getToKey() == VK_MBUTTON ) {
				mouseMiddlePressed = false;
			}
		}
		
	} else {
		

		if ( app != "" ) {
			//Não irá funcionar em jogos com DirectInput, para funcionar não use APP
			//HWND window = FindWindow(_T(app.c_str()), NULL);
			HWND window = FindWindowA(NULL, app.c_str());
			if ( window ) {
				HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);
				if ( print )
					printf(" em %s [%d].\n", app.c_str(), actualTime);
				//btnKeyDown e btnKeyUp podem nao funcionar aqui
				if ( key.getToKeyIsConstant() ) {
					PostMessage(edit, WM_KEYDOWN, key.getToKey(), 0);
				} else {
					PostMessage(edit, WM_KEYDOWN, VkKeyScanEx(key.getToKey(), GetKeyboardLayout(0)), 0);
				}
				return;
			}
		}

		//Caso nenhum app tenha sido configurado ou encontrado lanca evento no windows
		//keybd_event(key.getToKey(), 0, 0, 0);
		INPUT input;
		WORD vkey = key.getToKey();
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = vkey;

		if ( key.getBtnDown() ) {
			input.ki.dwFlags = 0;// there is no KEYEVENTF_KEYDOWN
			SendInput(1, &input, sizeof(INPUT));

			//Um delay para o game realizar a acao
			//Sleep(10);
			if ( print )
				printf("Press: %s on Windows [%d].\n", key.getToKeyRepr().c_str(), actualTime);
		}
		if ( key.getBtnUp() ) {
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input, sizeof(INPUT));

			if ( print )
				printf("Unpress: %s on Windows [%d].\n", key.getToKeyRepr().c_str(), actualTime);
		}
		
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

void InputConverter::interpretKeyMap(KeyMap &keyMap) {
	
	if ( keyMap.getDetermineCenterPos()){
		int actualTime = (int)time(0);
		if ( nextDefineCenterPos == false &&
			actualTime - lastTimeCenterPos > centerPosDelay ) {
			nextDefineCenterPos = true;
		}
		
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


bool InputConverter::interpretOnLeave(bool active, KeyMap &keyMap) {

	//Se esta esperando para soltar a tecla e a tecla foi solta
	if ( keyMap.getWaitingLeave() && !active ) {
		interpretKeyMap((*keyMap.getOnLeave()));
		keyMap.setWaitingLeave(false);
		return true;
	} else
	if ( active ) {
		//Se foi ativada
		if ( keyMap.getHasOnLeave() ) {
			//Verifica se tem evento de saida
			if ( !keyMap.getWaitingLeave() ) {
				//Se tiver so aciona o comando novamente se ele nao estiver esperando evento de saida
				interpretKeyMap(keyMap);
			}
			keyMap.setWaitingLeave(true);
		} else {
			//Se nao tiver evento de saida aciona normalmente
			interpretKeyMap(keyMap);
		}
		return true;
	}
	return false;
}


bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {


	int actualTime = (int)time(0);


	//Quando uma pessoa for reconhecida pelo Kinect ele ira avisar
	
	if ( lastTimeTrack == 0 || actualTime - lastTimeTrack > 1 ) {
		printf("Kinect\n");
		if ( viewOn )
			view->showMsg("Kinect");
	}
	lastTimeTrack = actualTime;

	int top = 0;
	bool topCalculated = false;
	int active;
	int activeSecondary;
	bool pressed = false;


	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {



		if ( !strcmp(userdata->name, keyMap->getDev().c_str()) ) {

			active = -1;
			activeSecondary = -1;

			//Caso seja para definir uma posicao central
			if ( nextDefineCenterPos) {
				//o sensor esperado é definido dentro do metodo, aqui todos os sensores são enviados
				//caso nao seja o sensor correto, terá retornado -1
				if ( gr.setCenterPos(t) == 1) {
					lastTimeCenterPos = actualTime;
					printf("Posicao definida.\n");
					nextDefineCenterPos = false;
					if ( viewOn )
						view->showMsg("Posição definida.");
				}
			}

			//Caso seja reconhecimento de mudanca na altura ele calcula somente uma vez nesse metodo
			if ( topCalculated == false && (keyMap->getKey() == KINECT_TOP_ADD || keyMap->getKey() == KINECT_TOP_DEC) ) {
				top = gr.detectTopChange(t, keyMap->getHeightSens());
				topCalculated = true;
			}




			//se ja foi calculado durante esse reconhecimento nao calcula novamente para as demais configuracoes de teclas
			if ( keyMap->getKey() == KINECT_TOP_ADD && topCalculated == true ) {
				//o active ira controlar se essa acao foi acionada ou nao
				//caso nao esteja acionada o interpretOnLeave ira identificar se foi configurado para acionar alguma ação quando 
				//essa key nao estiver mais ativa
				//a ação ja sera chamada dentro de interpretOnLeave
				active = top == 1;
			} else
			if ( keyMap->getKey() == KINECT_TOP_DEC && topCalculated == true ) {
				active = top == -1;
			} else
			if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP || keyMap->getKey() == KINECT_LEFT_HAND_TOP ) {
			
				if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP )
					active = gr.detectRightHandTop(t, keyMap->getHandTopLevel(), keyMap->getHandTopMod());
				else
					active = gr.detectLeftHandTop(t, keyMap->getHandTopLevel(), keyMap->getHandTopMod());

				//Caso o xpos seja != -100 quer dizer que a posição X tabém é requerida para esse comando
				if ( keyMap->getHandXPos() != -100 ) {

					if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP )
						activeSecondary = gr.detectRightHandXPos(t, keyMap->getHandXPos());
					else
						activeSecondary = gr.detectLeftHandXPos(t, keyMap->getHandXPos());

				} else {
					//Caso nao exista acao secundaria
					activeSecondary = true;
				}

				//Caso o sensor atual nao seja o responsavel pela ação a funcao de reconhecimento retornara -1
				if ( active == -1 || activeSecondary == -1 ) {
					active = -1;
				} else {
					//Caso qualquer um dos dois seja desativado o evento de sair sera chamado
					active = active && activeSecondary;
				}
		
			} else //FAST HAND
			if ( keyMap->getKey() == KINECT_LEFT_HAND_FAST ) {
				active = gr.detectLeftHandFast(t);
			} else
			if ( keyMap->getKey() == KINECT_RIGHT_HAND_FAST ) {
				active = gr.detectRightHandFast(t);
			} else //BODY
			if ( keyMap->getKey() == KINECT_BODY_FRONT) {
				active = gr.detectBodyFront(t);
			} else
			if ( keyMap->getKey() == KINECT_BODY_RIGHT) {
				active = gr.detectBodyRight(t);
			} else
			if ( keyMap->getKey() == KINECT_BODY_LEFT ) {
				active = gr.detectBodyLeft(t);
			} else
			if ( keyMap->getKey() == KINECT_BODY_BACK ) {
				active = gr.detectBodyBack(t);
			} else
			if ( keyMap->getKey() == KINECT_WALK ) {
				active = gr.detectWalk(t);
			} else
			if ( keyMap->getKey() == KINECT_TURN_LEFT  ) {
				active = gr.detectTurnLeft(t);
			} else
			if ( keyMap->getKey() == KINECT_TURN_RIGHT ) {
				active = gr.detectTurnRight(t);
			} else
			if ( keyMap->getKey() == KINECT_LEFT_FIST_UP  ) {
				active = gr.KinectGestures::leftFistFlexedUp(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == KINECT_LEFT_FIST_DOWN ) {
				active = gr.KinectGestures::leftFistFlexedDown(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == KINECT_RIGHT_FIST_UP  ) {
				active = gr.KinectGestures::rightFistFlexedUp(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == KINECT_RIGHT_FIST_DOWN ) {
				active = gr.KinectGestures::rightFistFlexedDown(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == LEAP_LEFT_FIST_UP  ) {
				active = gr.LeapMotionGestures::leftFistFlexedUp(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == LEAP_LEFT_FIST_DOWN ) {
				active = gr.LeapMotionGestures::leftFistFlexedDown(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == LEAP_RIGHT_FIST_UP  ) {
				active = gr.LeapMotionGestures::rightFistFlexedUp(t, keyMap->getAngle(), keyMap->getAngleMod());
			} else
			if ( keyMap->getKey() == LEAP_RIGHT_FIST_DOWN ) {
				active = gr.LeapMotionGestures::rightFistFlexedDown(t, keyMap->getAngle(), keyMap->getAngleMod());
			}

		}
		
		//será -1 quando não for o sensor responsável pelo gesto
		if ( active != -1 ) {
			//Se pelo menos uma foi pressionada
			//nao pode retornar aqui porque podem ter outros comandos
			if ( interpretOnLeave(active, (*keyMap)) ) {
				pressed = true;
			}
		}
	}

	return pressed;
}

bool InputConverter::checkButton(const char * name, const vrpn_BUTTONCB b) {
	bool pressed = false;
	
	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {

		if ( !strcmp(name, keyMap->getDev().c_str()) && keyMap->getKey() == b.button ) {
			pressed = interpretOnLeave(b.state, (*keyMap));
		}
	}

	return pressed;

}


bool InputConverter::checkAnalog(const char *name, const vrpn_ANALOGCB a) {
	bool pressed = false;
	int active;
	
	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {

		if ( !strcmp(name, keyMap->getDev().c_str()) ) {

			switch ( keyMap->getKey() ) {
				case LEAP_LEFT_CLOSED:
					active = gr.leftClosed(a);
					break;
				case LEAP_LEFT_PINCH:
					active = gr.leftPinch(a);
					break;
				case LEAP_RIGHT_CLOSED:
					active = gr.rightClosed(a);
					break;
				case LEAP_RIGHT_PINCH:
					active = gr.rightPinch(a);
					break;
			}
		}
		
		if ( active != -1 ) {
			pressed = interpretOnLeave(active, (*keyMap));
		}
	}

	return true;
}

