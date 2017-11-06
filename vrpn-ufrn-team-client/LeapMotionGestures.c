#include "LeapMotionGestures.h"


std::map<int, std::vector<double>> LeapMotionGestures::lastPositions[10];



int LeapMotionGestures::leftFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod) {
	
	std::map<int, std::vector<double>> points = getPoints(t, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int LeapMotionGestures::leftFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, DOWN);
}
int LeapMotionGestures::rightFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 1, 2, 0, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int LeapMotionGestures::rightFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(t, 1, 2, 0, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, DOWN);
}




// Mao fechada
int LeapMotionGestures::handClosed(float angle) {
	if ( angle == -1 ) {
		return -1;
	}
	
	if ( angle > closeAngle ) {
		return 1;
	}
	return 0;
}
int LeapMotionGestures::leftClosed(const vrpn_ANALOGCB a) {
	return handClosed(a.channel[0]);
}
int LeapMotionGestures::rightClosed(const vrpn_ANALOGCB a) {
	return handClosed(a.channel[2]);
}

//Pinça
int LeapMotionGestures::pinch(float angle) {
	if ( angle == -1 ) {
		return -1;
	}
	
	if ( angle < pinchAngle ) {
		return 1;
	}
	return 0;
}
int LeapMotionGestures::leftPinch(const vrpn_ANALOGCB a) {
	return pinch(a.channel[1]);
}
int LeapMotionGestures::rightPinch(const vrpn_ANALOGCB a) {
	return pinch(a.channel[3]);
}