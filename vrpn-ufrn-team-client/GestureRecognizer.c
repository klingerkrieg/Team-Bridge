#include "GestureRecognizer.h"

double GestureRecognizer::lastHeight = 0;
bool GestureRecognizer::lastHeightDefined = false;

double GestureRecognizer::lastHeadHeight = 0;
bool GestureRecognizer::lastHeadHeightDefined = false;

std::map<int, std::vector<double>> GestureRecognizer::lastMemberPos;
std::map<int, int> GestureRecognizer::lastMemberTime;


bool GestureRecognizer::detectLeftHandFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( t.sensor == 7) {
		return detectMemberFast(userdata, t);
	}
	return false;
}

bool GestureRecognizer::detectRightHandFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( t.sensor == 11) {
		return detectMemberFast(userdata, t);
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

bool GestureRecognizer::detectMemberFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	
	std::vector<double> pos = getLastMemberPos(t.sensor);
	std::vector<double> zero = { 0,0,0 };
	std::vector<double> actPos = { t.pos[0], t.pos[1], t.pos[2] };
	

	//printf("last %.2f %.2f %.2f\n", pos[0], pos[1], pos[2] );

	if ( std::distance(pos.begin(), pos.end()) == std::distance(zero.begin(), zero.end()) && std::equal(pos.begin(), pos.end(), zero.begin()) ) {
		lastMemberPos[t.sensor] = actPos;
		lastMemberTime[t.sensor] = (int)time(0);
		return false;
	}

	
	int now = (int)time(0);
	int last = lastMemberTime[t.sensor];
	double dst = euclidianDistance(pos, actPos);

	printf("vel: %.2f\n", dst / (now - last) );
	if ( dst / (now - last) > fastMemberFator ) {
		return true;
	} else {
		return false;
	}

}


bool GestureRecognizer::detectLeftHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 7 || t.sensor == 0 ) {
		return detectHandTop(userdata, t, topLevel);
	}
	return false;
}

bool GestureRecognizer::detectRightHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 11 || t.sensor == 0 ) {
		return detectHandTop(userdata, t, topLevel);
	}
	return false;
}

bool GestureRecognizer::detectHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
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
	//1.30 - 4
	//1.15 - 3
	//1.0  - 3
	//0.85 - 2
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

int GestureRecognizer::detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens) {
	if ( t.sensor == 0 ) {

		
		if ( lastHeightDefined == false ) {
			lastHeight = t.pos[1];
			lastHeightDefined = true;
			return 0;
		}
		printf("%.2f\n", lastHeadHeight);
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

