#include "InputConverter.h"

int InputConverter::lastTimeTrack = 0;
bool InputConverter::nextDefineCenterPos = false;
int InputConverter::lastTimeCenterPos = 0;

std::vector<KeyMap> InputConverter::map;
std::vector<DeviceType> InputConverter::devs;



std::map<std::string, DeviceSensorCount> InputConverter::devicesSensorsCount;


#ifdef PERFORMANCE_TEST
double InputConverter::qtdMed = 0;
double InputConverter::usecMed = 0;
double InputConverter::secMed = 0;
#endif


void InputConverter::interpretKeyMap(KeyMap &keyMap) {
	
	if ( keyMap.getDetermineCenterPos()){
		int actualTime = (int)time(0);
		if ( nextDefineCenterPos == false &&
			actualTime - lastTimeCenterPos > centerPosDelay ) {
			nextDefineCenterPos = true;
		}
		
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

//Solta todas as teclas que foram pressionadas
InputConverter::~InputConverter() {
	KeyMap *keyMap;
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		keyMap = &map.at(keyMapId);

		if (keyMap->getWaitingLeave()) {
			interpretOnLeave(0, (*keyMap));
		}
	}
}


bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {

	SkeletonPart skelPart;

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
		vrpnToSkeleton(gr.skeleton, gr.skeletonMap1, t, skelPart);
	} else
		if ( devType == DEVTYPE_LEAPMOTION ) {
		//LeapMotion
		if ( t.sensor <= 22 ) {
			vrpnToSkeleton(gr.skeleton, gr.handSkeletonMap1, t, skelPart);
		} else {
			vrpnToSkeleton(gr.skeleton, gr.handSkeletonMap1, t, skelPart);
		}
	} else {
		printf("Dispositivo desconhecido:%s", userdata->name);
		return false;
	}

	int actualTime = (int)time(0);
	

	//Quando uma pessoa for reconhecida pelo Kinect ou LeapMotion ele ira avisar
	if ( lastTimeTrack == 0 || actualTime - lastTimeTrack > 1 ) {
		if ( viewOn )
			view->showMsg("Capturando");
	}
	lastTimeTrack = actualTime;

	int top = 0;
	int active;
	//int activeSecondary;
	bool pressed = false;
	KeyMap *keyMap;

	//primeiro loop
	//Esse loop é realizado duas vezes, a primeira vez é somente para verificar o status das teclas já ativas e que possuam evento onleave
	//isso é para evitar conflitos com aplicações que não suportam duas teclas pressionadas ao mesmo tempo
	//for (int firstLoop = 1; firstLoop > -1; firstLoop--){
		
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

					//será -1 quando não for o sensor responsável pelo gesto
					if ( active != -1 ) {
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
					printf("Nao possui metodo para checagem:%s\n", keyMap->toString().c_str());
				}

			}

			//primeiro loop
			/*if ( firstLoop == 1 && !keyMap->getWaitingLeave()  || keyMap->getVerified() ) {
				//Reseta para a proxima vez que chamar a funcao
				if ( firstLoop == 0 ) {
					keyMap->setVerified(false);
				}
				continue;
			} else
			//Nao permitir que no proximo loop as mesmas keys sejam verificadas
			if ( firstLoop == 1 && keyMap->getWaitingLeave() ) {
				keyMap->setVerified(true);
			}*/

			/*if ( !strcmp(userdata->name, keyMap->getDev().c_str()) ) {

				active = -1;
				activeSecondary = -1;

				//Caso seja para definir uma posicao central
				if ( nextDefineCenterPos) {
					//o sensor esperado é definido dentro do metodo, aqui todos os sensores são enviados
					//caso nao seja o sensor correto, terá retornado -1
					if ( gr.setCenterPos(skelPart) == 1) {
						lastTimeCenterPos = actualTime;
						printf("Posicao definida.\n");
						nextDefineCenterPos = false;
						if ( viewOn )
							view->showMsg("Posição definida.");
					}
				}


				//se ja foi calculado durante esse reconhecimento nao calcula novamente para as demais configuracoes de teclas
				if ( keyMap->getKey() == KINECT_STEP_UP ) {
					//o active ira controlar se essa acao foi acionada ou nao
					//caso nao esteja acionada o interpretOnLeave ira identificar se foi configurado para acionar alguma ação quando 
					//essa key nao estiver mais ativa
					//a ação ja sera chamada dentro de interpretOnLeave
					active = gr.detectTopChange(skelPart, keyMap->getSensivity(), KINECT_UP);
				} else
				if ( keyMap->getKey() == KINECT_STEP_DOWN ) {
					active = gr.detectTopChange(skelPart, keyMap->getSensivity(), KINECT_DOWN);
				} else
				if ( keyMap->getKey() == KINECT_STEP_NORMAL ) {
					active = gr.detectTopChange(skelPart, keyMap->getSensivity(), KINECT_NORMAL);
				} else
				if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP || keyMap->getKey() == KINECT_LEFT_HAND_TOP ) {
			
					if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP )
						active = gr.detectRightHandTop(skelPart, keyMap->getY(), keyMap->getCoordinateMod());
					else
						active = gr.detectLeftHandTop(skelPart, keyMap->getY(), keyMap->getCoordinateMod());

				

					//Caso o xpos seja != -100 quer dizer que a posição X tabém é requerida para esse comando
					if ( active == 1 && keyMap->getX() != -100 ) {
					
						if ( keyMap->getKey() == KINECT_RIGHT_HAND_TOP )
							activeSecondary = gr.detectRightHandXPos(skelPart, keyMap->getX());
						else
							activeSecondary = gr.detectLeftHandXPos(skelPart, keyMap->getX());

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
					active = gr.detectLeftHandFast(skelPart, keyMap->getMaxVelocityMs());
				} else
				if ( keyMap->getKey() == KINECT_RIGHT_HAND_FAST ) {
					active = gr.detectRightHandFast(skelPart, keyMap->getMaxVelocityMs());
				} else //BODY
				if ( keyMap->getKey() == KINECT_BODY_FRONT) {
					active = gr.detectBodyFront(skelPart, keyMap->getAngle());
				} else
				if ( keyMap->getKey() == KINECT_BODY_RIGHT) {
					active = gr.detectBodyRight(skelPart, keyMap->getAngle());
				} else
				if ( keyMap->getKey() == KINECT_BODY_LEFT ) {
					active = gr.detectBodyLeft(skelPart, keyMap->getAngle());
				} else
				if ( keyMap->getKey() == KINECT_BODY_BACK ) {
					active = gr.detectBodyBack(skelPart, keyMap->getAngle());
				} else
				if ( keyMap->getKey() == KINECT_WALK ) {
					active = gr.detectWalk(skelPart, keyMap->getDelay(), keyMap->getSensivity());
				} else
				if ( keyMap->getKey() == KINECT_TURN_LEFT  ) {
					active = gr.detectTurnLeft(skelPart);
				} else
				if ( keyMap->getKey() == KINECT_TURN_RIGHT ) {
					active = gr.detectTurnRight(skelPart);
				} else
				if ( keyMap->getKey() == KINECT_BALANCE ) {
					active = gr.bodyBalance(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == KINECT_LEFT_WRIST_UP  ) {
					active = gr.KinectGestures::leftWristFlexedUp(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == KINECT_LEFT_WRIST_DOWN ) {
					active = gr.KinectGestures::leftWristFlexedDown(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == KINECT_RIGHT_WRIST_UP  ) {
					active = gr.KinectGestures::rightWristFlexedUp(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == KINECT_RIGHT_WRIST_DOWN ) {
					active = gr.KinectGestures::rightWristFlexedDown(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == LEAP_LEFT_WRIST_UP  ) {
					active = gr.LeapMotionGestures::leftWristFlexedUp(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == LEAP_LEFT_WRIST_DOWN ) {
					active = gr.LeapMotionGestures::leftWristFlexedDown(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == LEAP_RIGHT_WRIST_UP  ) {
					active = gr.LeapMotionGestures::rightWristFlexedUp(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				} else
				if ( keyMap->getKey() == LEAP_RIGHT_WRIST_DOWN ) {
					active = gr.LeapMotionGestures::rightWristFlexedDown(skelPart, keyMap->getAngle(), keyMap->getAngleMod());
				}

			}*/
		
			

			
		//}

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
	KeyMap *keyMap;

	//primeiro loop
	//Esse loop é realizado duas vezes, a primeira vez é somente para verificar o status das teclas já ativas e que possuam evento onleave
	//isso é para evitar conflitos com aplicações que não suportam duas teclas pressionadas ao mesmo tempo
	//for ( int firstLoop = 1; firstLoop > -1; firstLoop-- ) {

		for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
			keyMap = &map.at(keyMapId);


			//primeiro loop
			/*if ( firstLoop == 1 && !keyMap->getWaitingLeave() || keyMap->getVerified() ) {
				//Reseta para a proxima vez que chamar a funcao
				if ( firstLoop == 0 ) {
					keyMap->setVerified(false);
				}
				continue;
			} else
			//Nao permitir que no proximo loop as mesmas keys sejam verificadas
			if ( firstLoop == 1 && keyMap->getWaitingLeave() ) {
				keyMap->setVerified(true);
			}*/
			
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

				/*
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
					case NEDGLOVE_GRAB:
						active = gr.NEDGloveGestures::closed((void*)&a, keyMap);
						break;
					case NEDGLOVE_PINCH:
						active = gr.NEDGloveGestures::pinch((void*)&a, keyMap);
						break;
				}
				*/
			
		
			
		}
	//}

	return true;
}

