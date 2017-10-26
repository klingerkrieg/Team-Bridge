#include "LeapMotionGestures.h"

int LeapMotionGestures::fistFlexed(const vrpn_TRACKERCB t) {
	return -1;
}

int LeapMotionGestures::leftFistFlexedUp(const vrpn_TRACKERCB t) {
	return -1;
}
int LeapMotionGestures::leftFistFlexedDown(const vrpn_TRACKERCB t) {
	return -1;
}
int LeapMotionGestures::rightFistFlexedUp(const vrpn_TRACKERCB t) {
	return -1;
}
int LeapMotionGestures::rightFistFlexedDown(const vrpn_TRACKERCB t) {
	return -1;
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