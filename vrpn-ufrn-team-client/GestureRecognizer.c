#include "GestureRecognizer.h"

double GestureRecognizer::lastHeight = 0;
bool GestureRecognizer::lastHeightDefined = false;

double GestureRecognizer::lastHeadXPos = 0;
bool GestureRecognizer::lastHeadXPosDefined = false;

double GestureRecognizer::lastHeadHeight = 0;
bool GestureRecognizer::lastHeadHeightDefined = false;

std::map<int, std::vector<double>> GestureRecognizer::lastMemberPos;
std::map<long, long> GestureRecognizer::lastMemberTime;


double GestureRecognizer::centerPos[2] = { 0,0 };

//17
double GestureRecognizer::leftKneeLastHeight = 0;
//13
double GestureRecognizer::rightKneeLastHeight = 0;

double GestureRecognizer::turnZeroQuat = 0;




bool GestureRecognizer::detectLeftHandFast(const vrpn_TRACKERCB t) {
	if ( t.sensor == 7) {
		return detectMemberFast(t);
	}
	return false;
}

bool GestureRecognizer::detectRightHandFast(const vrpn_TRACKERCB t) {
	if ( t.sensor == 11) {
		return detectMemberFast(t);
	}
	return false;
}

std::vector<double> GestureRecognizer::getLastMemberPos(int sensor) {
	if ( lastMemberPos[sensor].empty() ) {
		return { 0,0,0 };
	} else {
		return lastMemberPos[sensor];
	}
}

int GestureRecognizer::getLastMemberTime(int sensor) {
	return lastMemberTime[sensor];
}

double GestureRecognizer::euclidianDistance(std::vector<double> pos1, std::vector<double> pos2) {
	return sqrt((pos2[0] - pos1[0]) * (pos2[0] - pos1[0]) + (pos2[1] - pos1[1]) * (pos2[1] - pos1[1]) + (pos2[2] - pos1[2]) * (pos2[2] - pos1[2]));
}

struct timeval tp;

bool GestureRecognizer::detectMemberFast(const vrpn_TRACKERCB t) {
	
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


bool GestureRecognizer::detectLeftHandTop(const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 7 || t.sensor == 0 ) {
		return detectHandTop(t, topLevel);
	}
	return false;
}

bool GestureRecognizer::detectRightHandTop(const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 11 || t.sensor == 0 ) {
		return detectHandTop(t, topLevel);
	}
	return false;
}


bool GestureRecognizer::detectLeftHandXPos(const vrpn_TRACKERCB t, int xPos) {
	if ( t.sensor == 7 || t.sensor == 0 ) {
		return detectHandXPos(t, xPos);
	}
	return false;
}

bool GestureRecognizer::detectRightHandXPos(const vrpn_TRACKERCB t, int xPos) {
	if ( t.sensor == 11 || t.sensor == 0 ) {
		return detectHandXPos(t, xPos);
	}
	return false;
}

bool GestureRecognizer::detectHandXPos(const vrpn_TRACKERCB t, int xPos) {
	//pega a posicao da cabeca
	if ( t.sensor == 0 ) {
		lastHeadXPos = t.pos[0];
		lastHeadXPosDefined = true;
		return false;
	}

	if ( lastHeadXPosDefined == false ) {
		return false;
	}


	if ( t.pos[0] > lastHeadXPos + (handXPosInterval * 2) && xPos == 2 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos + (handXPosInterval * 2) &&
		t.pos[0] > lastHeadXPos + handXPosInterval && xPos == 1 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos + handXPosInterval &&
		t.pos[0] > lastHeadXPos - handXPosInterval && xPos == 0 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos - handXPosInterval &&
		t.pos[0] > lastHeadXPos - (handXPosInterval *2) && xPos == -1 ) {
		return true;
	} else
	if ( t.pos[0] <= lastHeadXPos - (handXPosInterval * 2) && xPos == -2 ) {
		return true;
	}

	return false;
}

bool GestureRecognizer::detectHandTop(const vrpn_TRACKERCB t, int topLevel) {
	//pega a posicao da cabeca
	if ( t.sensor == 0 ) {
		lastHeadHeight = t.pos[1];
		lastHeadHeightDefined = true;
		return false;
	}

	if ( lastHeadHeightDefined == false ) {
		return false;
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

	if ( t.pos[1] > lastHeadHeight + (handTopInterval * 2) && topLevel == 5 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight + (handTopInterval * 2) &&
		t.pos[1] > lastHeadHeight + handTopInterval && topLevel == 4 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight + handTopInterval &&
		t.pos[1] > lastHeadHeight - (handTopInterval * 2) && topLevel == 3 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight - (handTopInterval * 2) &&
		t.pos[1] > lastHeadHeight - (handTopInterval*5) && topLevel == 2 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight - (handTopInterval * 5) && topLevel == 1 ) {
		return true;
	}

	return false;
}

int GestureRecognizer::detectTopChange(const vrpn_TRACKERCB t, double heightSens) {
	if ( t.sensor == 0 ) {

		
		if ( lastHeightDefined == false ) {
			lastHeight = t.pos[1];
			lastHeightDefined = true;
			return 0;
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
			return 1;
		}

	}
	return 0;
}

void GestureRecognizer::setCenterPos(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		centerPos[0] = t.pos[0];
		centerPos[1] = t.pos[2];
		turnZeroQuat = t.quat[2];
	}
}

bool GestureRecognizer::detectBody(const vrpn_TRACKERCB t, int direction) {
	if ( t.sensor != 3 ) {
		return false;
	}

	if ( direction == GEST_FRONT ) {
		return t.pos[2] > centerPos[1] + bodyCenterDistance;
	} else
	if ( direction == GEST_BACK ) {
		return t.pos[2] < centerPos[1] - bodyCenterDistance;
	} else
	if ( direction == GEST_RIGHT ) {
		return t.pos[0] > centerPos[0] + bodyCenterDistance;
	} else
	if ( direction == GEST_LEFT ) {
		return t.pos[0] < centerPos[0] - bodyCenterDistance;
	}
	return false;
}

bool GestureRecognizer::detectBodyFront(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_FRONT);
}
bool GestureRecognizer::detectBodyRight(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_RIGHT);
}
bool GestureRecognizer::detectBodyLeft(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_LEFT);
}
bool GestureRecognizer::detectBodyBack(const vrpn_TRACKERCB t) {
	return detectBody(t, GEST_BACK);
}




bool GestureRecognizer::detectWalkHeight(double &kneeLastHeight, const vrpn_TRACKERCB t) {
	if ( kneeLastHeight == 0 ) {
		kneeLastHeight = t.pos[1];
		return false;
	}

	if ( kneeLastHeight - kneeHeightFactor >= t.pos[1] ) {//abaixou o joelho
		kneeLastHeight = t.pos[1];
		return true;
	} else
	if ( kneeLastHeight + kneeHeightFactor <= t.pos[1] ) {//levantou o joelho
		kneeLastHeight = t.pos[1];
		return true;
	}

	return false;
}


bool GestureRecognizer::detectWalk(const vrpn_TRACKERCB t) {
	//17 13
	if ( t.sensor == 13 ) {
		return detectWalkHeight(rightKneeLastHeight, t);
	} else 
	if ( t.sensor == 17 ) {
		return detectWalkHeight(leftKneeLastHeight, t);
	}
	return false;
}



//Z Axis
bool GestureRecognizer::detectTurnLeft(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		if ( t.quat[2] < turnZeroQuat - turnFactor ) {
			return true;
		}
	}
	return false;
}

bool GestureRecognizer::detectTurnRight(const vrpn_TRACKERCB t) {
	if ( t.sensor == 3 ) {
		if ( t.quat[2] > turnZeroQuat + turnFactor ) {
			return true;
		}
	}
	return false;
}