#include "LeapMotionGestures.h"


std::map<int, std::vector<double>> LeapMotionGestures::lastPositions[10];

std::map<int, int> LeapMotionGestures::handSkeletonMap1 = LeapMotionGestures::create_handSkeletonMap1();
std::map<int, int> LeapMotionGestures::handSkeletonMap2 = LeapMotionGestures::create_handSkeletonMap2();

int LeapMotionGestures::leftWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod) {
	
	std::map<int, std::vector<double>> points = getPoints(skelPart, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int LeapMotionGestures::leftWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(skelPart, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, DOWN);
}
int LeapMotionGestures::rightWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(skelPart, 1, 2, 0, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, angle, angleMod, UP);
}
int LeapMotionGestures::rightWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod) {

	std::map<int, std::vector<double>> points = getPoints(skelPart, 1, 2, 0, *lastPositions);
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