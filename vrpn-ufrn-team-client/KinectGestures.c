#include "KinectGestures.h"


double KinectGestures::handXPosInterval = 0.40;


Skeleton KinectGestures::skeleton;
KinectDetection KinectGestures::kinectDetection;


std::map<int, int> KinectGestures::skeletonMap1 = KinectGestures::create_SkeletonMap1();

int KinectGestures::detectLeftHandFast(SkeletonPart skelPart, double maxVelMs) {
	if ( skelPart.skelConstant == SKELETON_HAND_L ) {
		return detectMemberFast(skelPart, maxVelMs);
	}
	return -1;
}

int KinectGestures::detectRightHandFast(SkeletonPart skelPart, double maxVelMs) {
	if ( skelPart.skelConstant == SKELETON_HAND_R ) {
		return detectMemberFast(skelPart, maxVelMs);
	}
	return -1;
}

std::vector<double> KinectGestures::getLastMemberPos(int sensor) {
	if ( kinectDetection.lastMemberPos[sensor].empty() ) {
		return { 0,0,0 };
	} else {
		return kinectDetection.lastMemberPos[sensor];
	}
}

int KinectGestures::getLastMemberTime(int sensor) {
	return kinectDetection.lastMemberTime[sensor];
}

double KinectGestures::euclidianDistance(std::vector<double> pos1, std::vector<double> pos2) {
	return sqrt((pos2[0] - pos1[0]) * (pos2[0] - pos1[0]) + (pos2[1] - pos1[1]) * (pos2[1] - pos1[1]) + (pos2[2] - pos1[2]) * (pos2[2] - pos1[2]));
}

struct timeval tp;

bool KinectGestures::detectMemberFast(SkeletonPart skelPart, double maxVelMs) {
	
	std::vector<double> pos = getLastMemberPos(skelPart.sensor);
	std::vector<double> actPos = { skelPart.x, skelPart.y, skelPart.z };
	

	gettimeofday(&tp, NULL);
	long now = (tp.tv_sec * 1000 + tp.tv_usec / 1000);


	//Primeira execucao
	if ( kinectDetection.lastMemberTime[skelPart.sensor] == 0 ) {
		kinectDetection.lastMemberPos[skelPart.sensor] = actPos;
		kinectDetection.lastMemberTime[skelPart.sensor] = now;
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito curto
	if ( now - kinectDetection.lastMemberTime[skelPart.sensor] < fastMemberDelay ) {
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito superior ao esperado
	if ( now - kinectDetection.lastMemberTime[skelPart.sensor] > fastMemberDelay + 50 ) {
		//reseta a contagem
		kinectDetection.lastMemberPos[skelPart.sensor] = actPos;
		kinectDetection.lastMemberTime[skelPart.sensor] = now;
		return false;
	}
	
	long last = kinectDetection.lastMemberTime[skelPart.sensor];
	double dst = euclidianDistance(pos, actPos);
	kinectDetection.lastMemberPos[skelPart.sensor] = actPos;
	kinectDetection.lastMemberTime[skelPart.sensor] = now;

	double time = ( (double)(now - last) / 1000.0);
	double velocityMs = (dst / time);

	if ( velocityMs > maxVelMs ) {
		return true;
	} else {
		return false;
	}

}




int KinectGestures::detectHandTop(SkeletonPart skelPart, int topLevel, int handTopMod) {
	
	if ( skeleton.head.defined == false ) {
		return -1;
	}

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

	if ( topLevel == 5 &&
		(skelPart.y > skeleton.head.y + (handTopInterval * 2) || handTopMod == -1) ) {
		return true;
	} else
	if ( topLevel == 4 && (
		(skelPart.y <= skeleton.head.y + (handTopInterval * 2) && skelPart.y > skeleton.head.y + handTopInterval && handTopMod == 0)
		|| (skelPart.y > skeleton.head.y + handTopInterval && handTopMod == 1)
		|| (skelPart.y <= skeleton.head.y + (handTopInterval * 2) && handTopMod == -1) )) {
		return true;
	} else
	if ( topLevel == 3 && (
		(skelPart.y <= skeleton.head.y + handTopInterval && skelPart.y > skeleton.head.y - (handTopInterval * 2) && handTopMod == 0)
		|| (skelPart.y > skeleton.head.y - (handTopInterval * 2) && handTopMod == 1)
		|| (skelPart.y <= skeleton.head.y + handTopInterval && handTopMod == -1) )) {
		return true;
	} else
	if ( topLevel == 2 && (
		(skelPart.y <= skeleton.head.y - (handTopInterval * 2) && skelPart.y > skeleton.head.y - (handTopInterval * 5) && handTopMod == 0)
		|| (skelPart.y > skeleton.head.y - (handTopInterval * 5)  && handTopMod == 1)
		|| (skelPart.y <= skeleton.head.y - (handTopInterval * 2) && handTopMod == -1) )) {

		return true;
	} else
	if ( topLevel == 1 &&
		(skelPart.y <= skeleton.head.y - (handTopInterval * 5) || handTopMod == 1) ) {
		return true;
	}

	return false;
}


int KinectGestures::detectLeftHandTop(SkeletonPart skelPart, int topLevel, int handTopMod) {
	
	//Caso seja cabeca ou bacia salva
	if ( skelPart.skelConstant == SKELETON_HEAD || skelPart.skelConstant == SKELETON_HIP_CENTER ) {
		//nao faz nada
	} else
		if ( skelPart.skelConstant == SKELETON_HAND_L ) {
		return detectHandTop(skelPart, topLevel, handTopMod);
	}
	return -1;
}

int KinectGestures::detectRightHandTop(SkeletonPart skelPart, int topLevel, int handTopMod) {
	if ( skelPart.skelConstant == SKELETON_HEAD || skelPart.skelConstant == SKELETON_HIP_CENTER ) {
		//nao faz nada
	} else
	if ( skelPart.skelConstant == SKELETON_HAND_R ) {
		return detectHandTop(skelPart, topLevel, handTopMod);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectLeftHandXPos(SkeletonPart skelPart, int xPos) {
	if ( skelPart.skelConstant == SKELETON_HAND_L ) {
		return detectHandXPos(skelPart, xPos);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectRightHandXPos(SkeletonPart skelPart, int xPos) {
	if ( skelPart.skelConstant == SKELETON_HAND_R ) {
		return detectHandXPos(skelPart, xPos);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectHandXPos(SkeletonPart skelPart, int xPos) {
	if ( skeleton.hipCenter.defined == false ) {
		return -1;
	}


	//printf("%.2f > %.2f + %.2f | %d\n", t.pos[0], lastHeadXPos, (handXPosInterval * 2), xPos);

	if ( xPos == 2 && skelPart.x >= skeleton.hipCenter.x + (handXPosInterval*2) ) {
		return true;
	} else
	if ( xPos == 1 && skelPart.x >= skeleton.hipCenter.x + handXPosInterval &&
		skelPart.x < skeleton.hipCenter.x + (handXPosInterval * 2) ) {
		return true;
	} else
	if ( xPos == 0 && skelPart.x <= skeleton.hipCenter.x + handXPosInterval &&
		skelPart.x > skeleton.hipCenter.x - handXPosInterval ) {
		return true;
	} else
	if ( xPos == -1 && skelPart.x <= skeleton.hipCenter.x - handXPosInterval &&
		skelPart.x > skeleton.hipCenter.x - (handXPosInterval * 2)  ) {
		return true;
	} else
	if ( xPos == -2 && skelPart.x <= skeleton.hipCenter.x - (handXPosInterval*2)  ) {
		return true;
	}

	return false;
}



std::vector<double> KinectGestures::headTopPositions;
double KinectGestures::normalStepHeight = -100;

int KinectGestures::detectTopChange(SkeletonPart skelPart, double heightSens, int direction) {
	if ( skelPart.skelConstant == SKELETON_HEAD ) {

		
		if ( normalStepHeight == -100 ) {
			normalStepHeight = skelPart.y;
			return -1;
		}

		//Subiu
		//pos  - last
		//0.59 - 0.57 = 0.02 >= 0.02
		if ( direction == KINECT_UP ) {
			if ( skelPart.y - normalStepHeight >= (float)heightSens ) {
				return true;
			} else {
				return 0;
			}
		} else
		if ( direction == KINECT_DOWN ) {
			//Desceu
			//last - pos
			//0.57 - 0.55 = 0.02 >= 0.02
			//printf("%.2f >= %.2f\n", lastHeight - t.pos[1], heightSens);
			if ( normalStepHeight - skelPart.y >= (float)heightSens ) {
				return true;
			} else {
				return 0;
			}
		} else 
		if ( direction == KINECT_NORMAL ) {
			if ( normalStepHeight - skelPart.y < (float)heightSens && skelPart.y - normalStepHeight < (float)heightSens ) {
				return true;
			} else {
				return 0;
			}
		}
	}
	return -1;
}

int KinectGestures::setCenterPos(SkeletonPart skelPart) {
	
	if ( skelPart.skelConstant == SKELETON_HIP_CENTER ) {
		kinectDetection.centerPos[0] = skelPart.x;
		kinectDetection.centerPos[1] = skelPart.y;
		kinectDetection.centerPos[2] = skelPart.z;
		kinectDetection.turnZeroQuat = skelPart.quat_z;
		kinectDetection.centerPosDefined = true;
		return true;
	}
	return -1;
}


std::map<int, std::vector<double>> KinectGestures::bodyDirectionPoints;

int KinectGestures::detectBody(SkeletonPart skelPart, int direction, int angle) {
	
	if ( skelPart.skelConstant == SKELETON_HIP_CENTER || skelPart.skelConstant == SKELETON_HEAD ) {
		//Cria vetor com pontos para formação do angulo
		std::vector<double> vec = { skelPart.x, skelPart.y, skelPart.z };
		if ( skelPart.skelConstant == SKELETON_HIP_CENTER ) {
			vec = { skelPart.x, 0, skelPart.z };
			std::vector<double> vec2 = { skelPart.x, 1, skelPart.z };
			bodyDirectionPoints.insert_or_assign(2, vec2);
		}
		int pos = 0;
		if ( skelPart.skelConstant == SKELETON_HIP_CENTER ) {
			pos = 1;
		}
		bodyDirectionPoints.insert_or_assign(pos, vec);
	}

	if ( bodyDirectionPoints.size() == 3 && skelPart.skelConstant == SKELETON_HEAD ) {
		//Está inclinado
		if ( flexed3d(bodyDirectionPoints, angle, 1) ) {

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

int KinectGestures::detectBodyFront(SkeletonPart skelPart, int angle) {
	return detectBody(skelPart, KINECT_FRONT, angle);
}
int KinectGestures::detectBodyRight(SkeletonPart skelPart, int angle) {
	return detectBody(skelPart, KINECT_RIGHT, angle);
}
int KinectGestures::detectBodyLeft(SkeletonPart skelPart, int angle) {
	return detectBody(skelPart, KINECT_LEFT, angle);
}
int KinectGestures::detectBodyBack(SkeletonPart skelPart, int angle) {
	return detectBody(skelPart, KINECT_BACK, angle);
}



bool KinectGestures::detectWalkHeight(double &kneeLastHeight, SkeletonPart skelPart, int delay, double sensitivity) {
	
	if ( kneeLastHeight == 0 ) {
		kneeLastHeight = skelPart.y;
		return false;
	}

	gettimeofday(&tp, NULL);
	long int actualTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	//int actualTime = (int)time(0);
	bool ret = false;

	if ( kneeLastHeight - sensitivity >= skelPart.y ) {//abaixou o joelho
		kneeLastHeight = skelPart.y;
		ret = true;
	} else
	if ( kneeLastHeight + sensitivity <= skelPart.y ) {//levantou o joelho
		kneeLastHeight = skelPart.y;
		ret = true;
	}


	if ( ret ) {
		kinectDetection.lastWalk = actualTime;
		return true;
	} else
	if ( kinectDetection.lastWalk != 0 && actualTime - kinectDetection.lastWalk < delay ) {
		return true;
	} else {
		return false;
	}
}


int KinectGestures::detectWalk(SkeletonPart skelPart, int delay, double sensitivity) {
	if ( skelPart.skelConstant == SKELETON_KNEE_R ) {
		return detectWalkHeight(kinectDetection.rightKneeLastHeight, skelPart, delay, sensitivity);
	} else 
	if ( skelPart.skelConstant == SKELETON_KNEE_L ) {
		return detectWalkHeight(kinectDetection.leftKneeLastHeight, skelPart, delay, sensitivity);
	}
	return -1;
}



//Z Axis
int KinectGestures::detectTurnLeft(SkeletonPart skelPart) {
	
	if ( skelPart.skelConstant == SKELETON_HIP_CENTER ) {
		if ( skelPart.quat_z < kinectDetection.turnZeroQuat - turnFactor ) {
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}

int KinectGestures::detectTurnRight(SkeletonPart skelPart) {
	
	if ( skelPart.skelConstant == SKELETON_HIP_CENTER ) {
		if ( skelPart.quat_z > kinectDetection.turnZeroQuat + turnFactor ) {
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}





//Flexao punho

std::map<int, std::vector<double>> KinectGestures::lastPositions[20];


int KinectGestures::leftWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod) {
	std::map<int, std::vector<double>> points = getPoints(skelPart, SKELETON_ELBOW_L, SKELETON_WRIST_L, SKELETON_HAND_L, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int KinectGestures::leftWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod) {
	std::map<int, std::vector<double>> points = getPoints(skelPart, SKELETON_ELBOW_L, SKELETON_WRIST_L, SKELETON_HAND_L, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, DOWN);
}
int KinectGestures::rightWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod) {
	std::map<int, std::vector<double>> points = getPoints(skelPart, SKELETON_ELBOW_R, SKELETON_WRIST_R, SKELETON_HAND_R, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int KinectGestures::rightWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod) {
	std::map<int, std::vector<double>> points = getPoints(skelPart, SKELETON_ELBOW_R, SKELETON_WRIST_R, SKELETON_HAND_R, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod,DOWN);
}


int  KinectGestures::bodyBalance(SkeletonPart skelPart, int angle, int angleMod) {
	
	std::map<int, std::vector<double>> points = getPoints(skelPart, SKELETON_SPINE, SKELETON_HIP_CENTER, SKELETON_SHOULDER_CENTER, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}

	//o ponto 0 nao sera real, sera o angulo 1(PELVIS) com y = 0, fazendo uma linha reta para cima
	bool y = points.at(1)[1];
	points.insert_or_assign(0, points.at(1));
	points.at(0)[1] = y;

	return flexed3d(points, angle, angleMod, 1);
}