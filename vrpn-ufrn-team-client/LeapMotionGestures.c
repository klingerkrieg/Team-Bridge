#include "LeapMotionGestures.h"


std::map<int, std::vector<double>> LeapMotionGestures::lastPositions[10];

int LeapMotionGestures::fistFlexed(const vrpn_TRACKERCB t, int side, int direction, int paramAngle) {
	std::vector<double> actPos = { t.pos[0], t.pos[1], t.pos[2] };
	std::vector<double> fist;
	std::vector<double> hand;

	if ( t.sensor == 0 || t.sensor == 2 || t.sensor == 23 || t.sensor == 25 ) {
		lastPositions->insert_or_assign(t.sensor, actPos);
	} else {
		return -1;
	}

	std::map<std::string, int>::iterator handFound;
	std::map<std::string, int>::iterator fistFound;

	if ( side == LEFT_HAND && (t.sensor == 23 || t.sensor == 25) ) {
		if ( lastPositions->find(23) == lastPositions->end() || lastPositions->find(25) == lastPositions->end() ) {
			return -1;
		}
		fist = lastPositions->at(25);
		hand = lastPositions->at(23);
	} else
	if ( side == RIGHT_HAND && (t.sensor == 0 || t.sensor == 2)) {
		if ( lastPositions->find(2) == lastPositions->end() || lastPositions->find(0) == lastPositions->end() ) {
			return -1;
		}
		fist = lastPositions->at(2);
		hand = lastPositions->at(0);
	}

	fist.resize(2);
	hand.resize(2);


	double angle = atan2(det(fist,hand), dot(fist,hand)) * 1000; //2d
	//double angle = std::acos(dot(fist, hand) / (mag(fist)*mag(hand)));//3d
	
	if ( paramAngle > angle ) {
		return 1;
	} else {
		return 0;
	}	
}

int LeapMotionGestures::leftFistFlexedUp(const vrpn_TRACKERCB t, int angle) {
	return fistFlexed(t, LEFT_HAND, UP, angle);
}
int LeapMotionGestures::leftFistFlexedDown(const vrpn_TRACKERCB t, int angle) {
	return fistFlexed(t, LEFT_HAND, DOWN, angle);
}
int LeapMotionGestures::rightFistFlexedUp(const vrpn_TRACKERCB t, int angle) {
	return fistFlexed(t, RIGHT_HAND, UP, angle);
}
int LeapMotionGestures::rightFistFlexedDown(const vrpn_TRACKERCB t, int angle) {
	return fistFlexed(t, RIGHT_HAND, DOWN, angle);
}




// Mao fechada
bool LeapMotionGestures::handClosed(float angle) {
	if ( angle < closeAngle ) {
		return true;
	}
	return false;
}
bool LeapMotionGestures::leftClosed(const vrpn_ANALOGCB a) {
	return handClosed(a.channel[0]);
}
bool LeapMotionGestures::rightClosed(const vrpn_ANALOGCB a) {
	return handClosed(a.channel[2]);
}

//Pinça
bool LeapMotionGestures::pinch(float angle) {
	if ( angle < pinchAngle ) {
		return true;
	}
	return false;
}
bool LeapMotionGestures::leftPinch(const vrpn_ANALOGCB a) {
	return pinch(a.channel[1]);
}
bool LeapMotionGestures::rightPinch(const vrpn_ANALOGCB a) {
	return pinch(a.channel[3]);
}