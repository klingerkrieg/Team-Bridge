#include "KinectGestures.h"


double KinectGestures::handXPosInterval = 0.40;


std::map<std::string, Skeleton> KinectGestures::skeleton;
std::map<std::string, KinectDetection> KinectGestures::kinectDetection;


std::map<int, int> KinectGestures::skeletonMap1 = KinectGestures::create_SkeletonMap1();

void KinectGestures::assignChecker(std::vector<KeyMap> &map) {
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		KeyMap *keyMap = &map.at(keyMapId);

		//cada metodo que será utilizado é alocado em seus respecitvos keyMaps

		switch ( keyMap->getKey() ) {
			case KINECT_LEFT_HAND_FAST:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectLeftHandFast, this);
				break;
			case KINECT_RIGHT_HAND_FAST:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectRightHandFast, this);
				break;
			case KINECT_BODY_FRONT:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectBodyFront, this);
				break;
			case KINECT_BODY_RIGHT:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectBodyRight, this);
				break;
			case KINECT_BODY_LEFT:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectBodyLeft, this);
				break;
			case KINECT_BODY_BACK:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectBodyBack, this);
				break;
			case KINECT_WALK:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectWalk, this);
				break;
			case KINECT_TURN_LEFT:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectTurnLeft, this);
				break;
			case KINECT_TURN_RIGHT:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectTurnRight, this);
				break;
			case KINECT_BALANCE:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::bodyBalance, this);
				break;
			case KINECT_LEFT_WRIST_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::leftWristFlexedUp, this);
				break;
			case KINECT_LEFT_WRIST_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::leftWristFlexedDown, this);
				break;
			case KINECT_RIGHT_WRIST_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::leftWristFlexedDown, this);
				break;
			case KINECT_RIGHT_WRIST_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::rightWristFlexedDown, this);
				break;
			case KINECT_RIGHT_HAND_TOP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectRightHandTop, this);//trazer pra cá o conteudo externo
				break;
			case KINECT_LEFT_HAND_TOP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectLeftHandTop, this);//trazer pra cá o conteudo externo
				break;
			case KINECT_STEP_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectTopChangeUp, this);//criar um metodo pra cada
				break;
			case KINECT_STEP_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectTopChangeDown, this);//criar um metodo pra cada
				break;
			case KINECT_STEP_NORMAL:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::detectTopChangeNormal, this);//criar um metodo pra cada
				break;
			case KINECT_SET_CENTER_POS:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&KinectGestures::setCenterPos, this);//criar um metodo pra cada
				break;
		}
	}
}




/*
		Velocidade das mãos
*/

int KinectGestures::detectLeftHandFast(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;

	if ( skelPart->skelConstant == SKELETON_HAND_L ) {
		return detectMemberFast(skelPart, keyMap);
	}
	return -1;
}

int KinectGestures::detectRightHandFast(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;

	if ( skelPart->skelConstant == SKELETON_HAND_R ) {
		return detectMemberFast(skelPart, keyMap);
	}
	return -1;
}

std::vector<double> KinectGestures::getLastMemberPos(SkeletonPart * skelPart) {
	if ( kinectDetection[skelPart->skeletonName].lastMemberPos[skelPart->sensor].empty() ) {
		return { 0,0,0 };
	} else {
		return kinectDetection[skelPart->skeletonName].lastMemberPos[skelPart->sensor];
	}
}

int KinectGestures::getLastMemberTime(SkeletonPart * skelPart) {
	return kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor];
}

double KinectGestures::euclidianDistance(std::vector<double> pos1, std::vector<double> pos2) {
	return sqrt((pos2[0] - pos1[0]) * (pos2[0] - pos1[0]) + (pos2[1] - pos1[1]) * (pos2[1] - pos1[1]) + (pos2[2] - pos1[2]) * (pos2[2] - pos1[2]));
}

struct timeval tp;

bool KinectGestures::detectMemberFast(SkeletonPart * skelPart, KeyMap * keyMap) {
	
	std::vector<double> pos = getLastMemberPos(skelPart);
	std::vector<double> actPos = { skelPart->x, skelPart->y, skelPart->z };
	

	gettimeofday(&tp, NULL);
	long now = (tp.tv_sec * 1000 + tp.tv_usec / 1000);


	//Primeira execucao
	if ( kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] == 0 ) {
		kinectDetection[skelPart->skeletonName].lastMemberPos[skelPart->sensor] = actPos;
		kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] = now;
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito curto
	if ( now - kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] < fastMemberDelay ) {
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito superior ao esperado
	if ( now - kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] > fastMemberDelay + 50 ) {
		//reseta a contagem
		kinectDetection[skelPart->skeletonName].lastMemberPos[skelPart->sensor] = actPos;
		kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] = now;
		return false;
	}
	
	long last = kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor];
	double dst = euclidianDistance(pos, actPos);
	kinectDetection[skelPart->skeletonName].lastMemberPos[skelPart->sensor] = actPos;
	kinectDetection[skelPart->skeletonName].lastMemberTime[skelPart->sensor] = now;

	double time = ( (double)(now - last) / 1000.0);
	double velocityMs = (dst / time);

	if ( velocityMs > keyMap->getMaxVelocityMs() ) {

#ifdef THERAPY_MODULE
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), velocityMs);
		}
#endif

		return true;
	} else {
		return false;
	}

}

/*
	Posição das mãos (eixo Y)
*/


int KinectGestures::detectHandTop(SkeletonPart * skelPart, KeyMap * keyMap) {
	
	if ( skeleton[skelPart->skeletonName].head.defined == false ) {
		return -1;
	}
	double headY = skeleton[skelPart->skeletonName].head.y;
	//
	//...  - 5
	//1.20 - 4
	//1.10 - 3
	//1.0  - head
	//0.80 - 3
	//0.50 - 2
	// ... - 1
	//if (t.sensor == 11)
	//printf("head:%.2f hand:%.2f\n", skeleton.head.y, t.pos[1]);
	bool ret = false;

	if ( keyMap->getY() == 5 &&
		(skelPart->y > headY + (handTopInterval * 2) || keyMap->getCoordinateMod() == -1) ) {
		ret = true;
	} else
	if ( keyMap->getY() == 4 && (
		(skelPart->y <= headY + (handTopInterval * 2) && skelPart->y > headY + handTopInterval && keyMap->getCoordinateMod() == 0)
		|| (skelPart->y > headY + handTopInterval && keyMap->getCoordinateMod() == 1)
		|| (skelPart->y <= headY + (handTopInterval * 2) && keyMap->getCoordinateMod() == -1) )) {
		ret = true;
	} else
	if ( keyMap->getY() == 3 && (
		(skelPart->y <= headY + handTopInterval && skelPart->y > headY - (handTopInterval * 2) && keyMap->getCoordinateMod() == 0)
		|| (skelPart->y > headY - (handTopInterval * 2) && keyMap->getCoordinateMod() == 1)
		|| (skelPart->y <= headY + handTopInterval && keyMap->getCoordinateMod() == -1) )) {
		ret = true;
	} else
	if ( keyMap->getY() == 2 && (
		(skelPart->y <= headY - (handTopInterval * 2) && skelPart->y > headY - (handTopInterval * 5) && keyMap->getCoordinateMod() == 0)
		|| (skelPart->y > headY - (handTopInterval * 5)  && keyMap->getCoordinateMod() == 1)
		|| (skelPart->y <= headY - (handTopInterval * 2) && keyMap->getCoordinateMod() == -1) )) {

		ret = true;
	} else
	if ( keyMap->getY() == 1 &&
		(skelPart->y <= headY - (handTopInterval * 5) || keyMap->getCoordinateMod() == 1) ) {
		ret = true;
	}

#ifdef THERAPY_MODULE
	if ( ret ) {
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData() + "-Y", skelPart->y);
		}
	}
#endif

	return ret;
}


int KinectGestures::detectLeftHandTop(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	
	if ( skelPart->skelConstant == SKELETON_HAND_L ) {
		return detectHandTop(skelPart, keyMap);
	}
	return -1;
}

int KinectGestures::detectRightHandTop(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	
	if ( skelPart->skelConstant == SKELETON_HAND_R ) {
		return detectHandTop(skelPart, keyMap);
	}
	return -1;
}



/*
	Posição das mãos eixo X
*/


/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectLeftHandXPos(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;

	if ( skelPart->skelConstant == SKELETON_HAND_L ) {
		return detectHandXPos(skelPart, keyMap);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectRightHandXPos(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;

	if ( skelPart->skelConstant == SKELETON_HAND_R ) {
		return detectHandXPos(skelPart, keyMap);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectHandXPos(SkeletonPart * skelPart, KeyMap * keyMap) {
	
	if ( skeleton[skelPart->skeletonName].hipCenter.defined == false ) {
		return -1;
	}
	double hipCenterX = skeleton[skelPart->skeletonName].hipCenter.x;
	//Verifica se está na altura correta
	if ( detectHandTop(skelPart, keyMap) == false) {
		return false;
	}

	bool ret = false;

	//printf("%.2f > %.2f + %.2f | %d\n", t.pos[0], lastHeadXPos, (handXPosInterval * 2), xPos);
	//Depois de verificar a altura verifica o eixo X
	if ( keyMap->getX() == 2 && skelPart->x >= hipCenterX + (handXPosInterval*2) ) {
		ret = true;
	} else
	if ( keyMap->getX() == 1 && skelPart->x >= hipCenterX + handXPosInterval &&
		skelPart->x < hipCenterX + (handXPosInterval * 2) ) {
		ret = true;
	} else
	if ( keyMap->getX() == 0 && skelPart->x <= hipCenterX + handXPosInterval &&
		skelPart->x > hipCenterX - handXPosInterval ) {
		ret = true;
	} else
	if ( keyMap->getX() == -1 && skelPart->x <= hipCenterX - handXPosInterval &&
		skelPart->x > hipCenterX - (handXPosInterval * 2)  ) {
		ret = true;
	} else
	if ( keyMap->getX() == -2 && skelPart->x <= hipCenterX - (handXPosInterval*2)  ) {
		ret = true;
	}

#ifdef THERAPY_MODULE
	if ( ret ) {
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData() + "-X", skelPart->x);
		}
	}
#endif

	return ret;
}


/*
	STEP
*/

std::vector<double> KinectGestures::headTopPositions;
double KinectGestures::normalStepHeight = -100;

int KinectGestures::detectTopChangeUp(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectTopChange(skelPart, keyMap, KINECT_UP);
}

int KinectGestures::detectTopChangeDown(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectTopChange(skelPart, keyMap, KINECT_DOWN);
}

int KinectGestures::detectTopChangeNormal(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectTopChange(skelPart, keyMap, KINECT_NORMAL);
}


int KinectGestures::detectTopChange(SkeletonPart * skelPart, KeyMap * keyMap, int direction) {
	
	int ret = -1;

	if ( skelPart->skelConstant == SKELETON_HEAD ) {
		if ( normalStepHeight == -100 ) {
			normalStepHeight = skelPart->y;
			return -1;
		}

		

		//Subiu
		//pos  - last
		//0.59 - 0.57 = 0.02 >= 0.02
		if ( direction == KINECT_UP ) {
			if ( skelPart->y - normalStepHeight >= keyMap->getSensivity() ) {
				ret = 1;
			} else {
				ret = 0;
			}
		} else
		if ( direction == KINECT_DOWN ) {
			//Desceu
			//last - pos
			//0.57 - 0.55 = 0.02 >= 0.02
			//printf("%.2f >= %.2f\n", lastHeight - t.pos[1], heightSens);
			if ( normalStepHeight - skelPart->y >= keyMap->getSensivity() ) {
				ret = 1;
			} else {
				ret = 0;
			}
		} else 
		if ( direction == KINECT_NORMAL ) {
			if ( normalStepHeight - skelPart->y < keyMap->getSensivity() && skelPart->y - normalStepHeight < keyMap->getSensivity() ) {
				ret = 1;
			} else {
				ret = 0;
			}
		}

	#ifdef THERAPY_MODULE
		if ( ret == 1 ) {
			if ( keyMap->getSaveData().compare("") ) {
				storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), skelPart->y);
			}
		}
	#endif
	}



	return ret;
}





int KinectGestures::setCenterPos(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	//Salvao o estado atual do hipcenter
	kinectDetection[skelPart->skeletonName].hipCenter = skeleton[skelPart->skeletonName].hipCenter;
	kinectDetection[skelPart->skeletonName].centerPosDefined = true;
	return true;
	
}



/*
	Inclinação do corpo
*/

std::map<int, std::vector<double>> KinectGestures::bodyDirectionPoints;

int KinectGestures::detectBody(SkeletonPart * skelPart, KeyMap * keyMap, int direction) {
	 
	if ( skelPart->skelConstant == SKELETON_HIP_CENTER || skelPart->skelConstant == SKELETON_HEAD ) {
		//Cria vetor com pontos para formação do angulo
		std::vector<double> vec = { skelPart->x, skelPart->y, skelPart->z };
		if ( skelPart->skelConstant == SKELETON_HIP_CENTER ) {
			vec = { skelPart->x, 0, skelPart->z };
			std::vector<double> vec2 = { skelPart->x, 1, skelPart->z };
			bodyDirectionPoints.insert_or_assign(2, vec2);
		}
		int pos = 0;
		if ( skelPart->skelConstant == SKELETON_HIP_CENTER ) {
			pos = 1;
		}
		bodyDirectionPoints.insert_or_assign(pos, vec);
	}

	if ( bodyDirectionPoints.size() == 3 && skelPart->skelConstant == SKELETON_HEAD ) {
		//Está inclinado
		if ( flexed3d(bodyDirectionPoints, keyMap) ) {

			if ( direction == KINECT_FRONT ) {
				return bodyDirectionPoints.at(0)[2] < bodyDirectionPoints.at(1)[2];
			} else
			if ( direction == KINECT_BACK ) {
				return bodyDirectionPoints.at(0)[2] > bodyDirectionPoints.at(1)[2];
			} else
			if ( direction == KINECT_RIGHT ) {
				return bodyDirectionPoints.at(0)[0] > bodyDirectionPoints.at(1)[0];
			} else
			if ( direction == KINECT_LEFT ) {
				return bodyDirectionPoints.at(0)[0] < bodyDirectionPoints.at(1)[0];
			}
		} else {
			return false;
		}
	}

	return -1;
}

int KinectGestures::detectBodyFront(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectBody(skelPart, keyMap ,KINECT_FRONT);
}
int KinectGestures::detectBodyRight(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectBody(skelPart, keyMap, KINECT_RIGHT);
}
int KinectGestures::detectBodyLeft(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectBody(skelPart, keyMap, KINECT_LEFT);
}
int KinectGestures::detectBodyBack(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	return detectBody(skelPart, keyMap, KINECT_BACK);
}



/*
Desequilíbrio
*/


int  KinectGestures::bodyBalance(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	
	std::map<int, std::vector<double>> points = getPoints(skeleton[skelPart->skeletonName].spine, skeleton[skelPart->skeletonName].hipCenter, skeleton[skelPart->skeletonName].shoulderCenter);
	if ( points.size() == 0 ) {
		return -1;
	}

	//o ponto 0 nao sera real, sera o angulo 1(HIP_CENTER) com y = 0, fazendo uma linha reta para cima
	bool y = points.at(1)[1];
	points.insert_or_assign(0, points.at(1));
	points.at(0)[1] = y;

	return flexed3d(points, keyMap, 1);
}


/*
	Marcha estacionária
*/

bool KinectGestures::detectWalkHeight(SkeletonPart * skelPart, KeyMap * keyMap, double &kneeLastHeight) {
	
	if ( kneeLastHeight == 0 ) {
		kneeLastHeight = skelPart->y;
		return false;
	}

	gettimeofday(&tp, NULL);
	long int actualTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	//int actualTime = (int)time(0);
	bool ret = false;

	if ( kneeLastHeight - keyMap->getSensivity() >= skelPart->y ) {//abaixou o joelho
		kneeLastHeight = skelPart->y;
		ret = true;
	} else
	if ( kneeLastHeight + keyMap->getSensivity() <= skelPart->y ) {//levantou o joelho
		kneeLastHeight = skelPart->y;
		ret = true;
	}


	if ( ret ) {
		kinectDetection[skelPart->skeletonName].lastWalk = actualTime;
	} else
	if ( kinectDetection[skelPart->skeletonName].lastWalk != 0 && actualTime - kinectDetection[skelPart->skeletonName].lastWalk < keyMap->getDelay() ) {
		ret = true;
	}

#ifdef THERAPY_MODULE
	if ( ret) {
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), kneeLastHeight);
		}
	}
#endif

	return ret;
}


int KinectGestures::detectWalk(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	if ( skelPart->skelConstant == SKELETON_KNEE_R ) {
		return detectWalkHeight(skelPart, keyMap, kinectDetection[skelPart->skeletonName].rightKneeLastHeight);
	} else 
	if ( skelPart->skelConstant == SKELETON_KNEE_L ) {
		return detectWalkHeight(skelPart, keyMap, kinectDetection[skelPart->skeletonName].leftKneeLastHeight);
	}
	return -1;
}



/*
	Girar corpo
*/


//Z Axis
int KinectGestures::detectTurnLeft(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	
	if ( skelPart->skelConstant == SKELETON_HIP_CENTER ) {
		if ( skelPart->quat_z < kinectDetection[skelPart->skeletonName].hipCenter.quat_z - turnFactor ) {

		#ifdef THERAPY_MODULE
			if ( keyMap->getSaveData().compare("") ) {
				storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), skelPart->quat_z);
			}
		#endif
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}

int KinectGestures::detectTurnRight(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	
	if ( skelPart->skelConstant == SKELETON_HIP_CENTER ) {
		if ( skelPart->quat_z > kinectDetection[skelPart->skeletonName].hipCenter.quat_z + turnFactor ) {
		#ifdef THERAPY_MODULE
			if ( keyMap->getSaveData().compare("") ) {
				storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), skelPart->quat_z);
			}
		#endif
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}




/*
	Flexao punho
*/

std::map<int, std::vector<double>> KinectGestures::lastPositions[20];


int KinectGestures::leftWristFlexedUp(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	std::map<int, std::vector<double>> points = getPoints(skeleton[skelPart->skeletonName].elbowL, skeleton[skelPart->skeletonName].wristL, skeleton[skelPart->skeletonName].handL);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap, UP);
}
int KinectGestures::leftWristFlexedDown(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	std::map<int, std::vector<double>> points = getPoints(skeleton[skelPart->skeletonName].elbowL, skeleton[skelPart->skeletonName].wristL, skeleton[skelPart->skeletonName].handL);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap, DOWN);
}
int KinectGestures::rightWristFlexedUp(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	std::map<int, std::vector<double>> points = getPoints(skeleton[skelPart->skeletonName].elbowR, skeleton[skelPart->skeletonName].wristR, skeleton[skelPart->skeletonName].handR);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap, UP);
}
int KinectGestures::rightWristFlexedDown(void * data, KeyMap * keyMap) {
	SkeletonPart * skelPart = (SkeletonPart*)data;
	std::map<int, std::vector<double>> points = getPoints(skeleton[skelPart->skeletonName].elbowR, skeleton[skelPart->skeletonName].wristR, skeleton[skelPart->skeletonName].handR);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap,DOWN);
}



