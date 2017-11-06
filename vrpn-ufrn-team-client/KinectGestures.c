#include "KinectGestures.h"

double KinectGestures::lastHeight = 0;
bool KinectGestures::lastHeightDefined = false;

double KinectGestures::lastHeadXPos = 0;
bool KinectGestures::lastHeadXPosDefined = false;

double KinectGestures::lastHeadHeight = 0;
bool KinectGestures::lastHeadHeightDefined = false;

std::map<int, std::vector<double>> KinectGestures::lastMemberPos;
std::map<long, long> KinectGestures::lastMemberTime;

bool KinectGestures::centerPosDefined = false;
double KinectGestures::centerPos[3] = { 0,0,0 };

//17
double KinectGestures::leftKneeLastHeight = 0;
//13
double KinectGestures::rightKneeLastHeight = 0;

double KinectGestures::turnZeroQuat = 0;

long int KinectGestures::lastWalk = 0;

int KinectGestures::detectLeftHandFast(const vrpn_TRACKERCB t) {
	if ( t.sensor == 7) {
		return detectMemberFast(t);
	}
	return -1;
}

int KinectGestures::detectRightHandFast(const vrpn_TRACKERCB t) {
	if ( t.sensor == 11) {
		return detectMemberFast(t);
	}
	return -1;
}

std::vector<double> KinectGestures::getLastMemberPos(int sensor) {
	if ( lastMemberPos[sensor].empty() ) {
		return { 0,0,0 };
	} else {
		return lastMemberPos[sensor];
	}
}

int KinectGestures::getLastMemberTime(int sensor) {
	return lastMemberTime[sensor];
}

double KinectGestures::euclidianDistance(std::vector<double> pos1, std::vector<double> pos2) {
	return sqrt((pos2[0] - pos1[0]) * (pos2[0] - pos1[0]) + (pos2[1] - pos1[1]) * (pos2[1] - pos1[1]) + (pos2[2] - pos1[2]) * (pos2[2] - pos1[2]));
}

struct timeval tp;

bool KinectGestures::detectMemberFast(const vrpn_TRACKERCB t) {
	
	std::vector<double> pos = getLastMemberPos(t.sensor);
	std::vector<double> actPos = { t.pos[0], t.pos[1], t.pos[2] };
	

	gettimeofday(&tp, NULL);
	long now = (tp.tv_sec * 1000 + tp.tv_usec / 1000);

	//Primeira execucao
	if ( lastMemberTime[t.sensor] == 0 ) {
		lastMemberPos[t.sensor] = actPos;
		lastMemberTime[t.sensor] = now;
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito curto
	if ( now - lastMemberTime[t.sensor] < fastMemberDelay ) {
		return false;
	} else
	//Delay para nao calcular a velocidade com espaço de tempo muito curto
	if ( now - lastMemberTime[t.sensor] > maxFastMemberDelay ) {
		lastMemberPos[t.sensor] = actPos;
		lastMemberTime[t.sensor] = now;
		return false;
	}
	
	long last = lastMemberTime[t.sensor];
	double dst = euclidianDistance(pos, actPos);

	lastMemberPos[t.sensor] = actPos;
	lastMemberTime[t.sensor] = now;

	//printf("%.2f / (%d - %d [%d]) =  %.8f\n", dst ,now , last, (now - last), dst / (now - last));
	if ( dst / (now - last) > fastMemberFator ) {
		return true;
	} else {
		return false;
	}

}




int KinectGestures::detectHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod) {
	//pega a posicao da cabeca
	if ( t.sensor == 0 ) {
		lastHeadHeight = t.pos[1];
		lastHeadHeightDefined = true;

		//pega a posicao da cabeca para calcular xpos tambem, porque xpos depende de handtop
		lastHeadXPos = t.pos[0];
		lastHeadXPosDefined = true;
		return -1;
	}

	if ( lastHeadHeightDefined == false ) {
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
	//printf("head:%.2f hand:%.2f\n", lastHeadHeight, t.pos[1]);

	if ( topLevel == 5 &&
		(t.pos[1] > lastHeadHeight + (handTopInterval * 2))
		|| handTopMod == -1) {
		return true;
	} else
	if ( topLevel == 4 &&
		(t.pos[1] <= lastHeadHeight + (handTopInterval * 2) && t.pos[1] > lastHeadHeight + handTopInterval && handTopMod == 0)
		|| (t.pos[1] > lastHeadHeight + handTopInterval && handTopMod == 1)
		|| (t.pos[1] <= lastHeadHeight + (handTopInterval * 2) && handTopMod == -1) ) {
		return true;
	} else
	if ( topLevel == 3 && 
		(t.pos[1] <= lastHeadHeight + handTopInterval && t.pos[1] > lastHeadHeight - (handTopInterval * 2))
		|| (t.pos[1] > lastHeadHeight - (handTopInterval * 2) && handTopMod == 1)
		|| (t.pos[1] <= lastHeadHeight + handTopInterval && handTopMod == -1)	) {
		return true;
	} else
	if ( topLevel == 2 &&
		(t.pos[1] <= lastHeadHeight - (handTopInterval * 2) && t.pos[1] > lastHeadHeight - (handTopInterval * 5))
		|| (t.pos[1] > lastHeadHeight - (handTopInterval * 5)  && handTopMod == 1)
		|| (t.pos[1] <= lastHeadHeight - (handTopInterval * 2) && handTopMod == -1) ) {
		return true;
	} else
	if ( topLevel == 1 &&
		(t.pos[1] <= lastHeadHeight - (handTopInterval * 5))
		|| handTopMod == 1) {
		return true;
	}

	return false;
}


int KinectGestures::detectLeftHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod) {
	if ( t.sensor == 7 || t.sensor == 0 ) {
		return detectHandTop(t, topLevel, handTopMod);
	}
	return -1;
}

int KinectGestures::detectRightHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod) {
	if ( t.sensor == 11 || t.sensor == 0 ) {
		return detectHandTop(t, topLevel, handTopMod);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectLeftHandXPos(const vrpn_TRACKERCB t, int xPos) {
	if ( t.sensor == 7 || t.sensor == 0 ) {
		return detectHandXPos(t, xPos);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectRightHandXPos(const vrpn_TRACKERCB t, int xPos) {
	if ( t.sensor == 11 || t.sensor == 0 ) {
		return detectHandXPos(t, xPos);
	}
	return -1;
}

/**
* Esse metodo depende do detectHandTop, ele deve ser chamado primeiro para captura da posicao central
*/
int KinectGestures::detectHandXPos(const vrpn_TRACKERCB t, int xPos) {
	
	if ( lastHeadXPosDefined == false ) {
		return -1;
	}


	//printf("%.2f > %.2f + %.2f | %d\n", t.pos[0], lastHeadXPos, (handXPosInterval * 2), xPos);

	if ( t.pos[0] >= lastHeadXPos + handXPosInterval && xPos == 1 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos + handXPosInterval &&
		t.pos[0] > lastHeadXPos - handXPosInterval && xPos == 0 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos - handXPosInterval && xPos == -1 ) {
		return true;
	}

	return false;
}


int KinectGestures::detectTopChange(const vrpn_TRACKERCB t, double heightSens) {
	if ( t.sensor == 0 ) {

		
		if ( lastHeightDefined == false ) {
			lastHeight = t.pos[1];
			lastHeightDefined = true;
			return false;
		}
		//printf("%.2f\n", lastHeadHeight);
		//Desceu
		//last - pos
		//0.57 - 0.55 = 0.02 >= 0.02
		if ( lastHeight - t.pos[1] >= (float)heightSens ) {
			lastHeight = t.pos[1];
			return -1;
		} else
		//Subiu
		//pos  - last
		//0.59 - 0.57 = 0.02 >= 0.02
		if ( t.pos[1] - lastHeight >= (float)heightSens ) {
			lastHeight = t.pos[1];
			return true;
		}

	}
	return false;
}

int KinectGestures::setCenterPos(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		centerPos[0] = t.pos[0];
		centerPos[1] = t.pos[1];
		centerPos[2] = t.pos[2];
		turnZeroQuat = t.quat[2];
		centerPosDefined = true;
		return true;
	}
	return -1;
}

int KinectGestures::detectBody(const vrpn_TRACKERCB t, int direction) {
	if ( t.sensor != 3 ) {
		return -1;
	}

	if ( centerPosDefined == false ) {
		return -1;
	}

	if ( direction == GEST_FRONT ) {
		return t.pos[2] < centerPos[2] - bodyCenterDistance;
	} else
	if ( direction == GEST_BACK ) {
		return t.pos[2] > centerPos[2] + bodyCenterDistance;
	} else
	if ( direction == GEST_RIGHT ) {
		return t.pos[0] > centerPos[0] + bodyCenterDistance;
	} else
	if ( direction == GEST_LEFT ) {
		return t.pos[0] < centerPos[0] - bodyCenterDistance;
	}
	return false;
}

int KinectGestures::detectBodyFront(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_FRONT);
}
int KinectGestures::detectBodyRight(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_RIGHT);
}
int KinectGestures::detectBodyLeft(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_LEFT);
}
int KinectGestures::detectBodyBack(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_BACK);
}



bool KinectGestures::detectWalkHeight(double &kneeLastHeight, const vrpn_TRACKERCB t) {
	if ( kneeLastHeight == 0 ) {
		kneeLastHeight = t.pos[1];
		return false;
	}

	gettimeofday(&tp, NULL);
	long int actualTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	//int actualTime = (int)time(0);
	bool ret = false;

	if ( kneeLastHeight - kneeHeightFactor >= t.pos[1] ) {//abaixou o joelho
		kneeLastHeight = t.pos[1];
		ret = true;
	} else
	if ( kneeLastHeight + kneeHeightFactor <= t.pos[1] ) {//levantou o joelho
		kneeLastHeight = t.pos[1];
		ret = true;
	}


	if ( ret ) {
		lastWalk = actualTime;
		return true;
	} else
	if ( actualTime - lastWalk < 1500 ) {
		return true;
	} else {
		return false;
	}
}


int KinectGestures::detectWalk(const vrpn_TRACKERCB t) {
	//17 13
	
	if ( t.sensor == 13 ) {
		return detectWalkHeight(rightKneeLastHeight, t);
	} else 
	if ( t.sensor == 17 ) {
		return detectWalkHeight(leftKneeLastHeight, t);
	}
	return -1;
}



//Z Axis
int KinectGestures::detectTurnLeft(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		if ( t.quat[2] < turnZeroQuat - turnFactor ) {
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}

int KinectGestures::detectTurnRight(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		if ( t.quat[2] > turnZeroQuat + turnFactor ) {
			return 1;
		} else {
			return 0;
		}
	}
	return -1;
}





//Flexao punho

std::map<int, std::vector<double>> KinectGestures::lastPositions[10];


int KinectGestures::leftFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 9, 10, 11, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod);
}
int KinectGestures::leftFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 9, 10, 11, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod);
}
int KinectGestures::rightFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 5, 6, 7, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod);
}
int KinectGestures::rightFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 5, 6, 7, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod);
}