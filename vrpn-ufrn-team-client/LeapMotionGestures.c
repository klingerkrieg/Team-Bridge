#include "LeapMotionGestures.h"


std::map<int, std::vector<double>> LeapMotionGestures::lastPositions[10];

std::map<int, int> LeapMotionGestures::handSkeletonMap1 = LeapMotionGestures::create_handSkeletonMap1();
std::map<int, int> LeapMotionGestures::handSkeletonMap2 = LeapMotionGestures::create_handSkeletonMap2();


void LeapMotionGestures::assignChecker(std::vector<KeyMap> &map) {
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		KeyMap *keyMap = &map.at(keyMapId);
		//cada metodo que será utilizado é alocado em seus respecitvos keyMaps


		switch ( keyMap->getKey() ) {
			case LEAP_LEFT_CLOSED:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::leftClosed, this);
				break;
			case LEAP_LEFT_PINCH:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::leftPinch, this);
				break;
			case LEAP_RIGHT_CLOSED:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::rightClosed, this);
				break;
			case LEAP_RIGHT_PINCH:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::rightPinch, this);
				break;
			case LEAP_LEFT_WRIST_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::leftWristFlexedUp, this);
				break;
			case LEAP_LEFT_WRIST_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::leftWristFlexedDown, this);
				break;
			case LEAP_RIGHT_WRIST_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::rightWristFlexedUp, this);
				break;
			case LEAP_RIGHT_WRIST_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::rightWristFlexedDown, this);
				break;
		}
	}
}


int LeapMotionGestures::leftWristFlexedUp(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;

	std::map<int, std::vector<double>> points = getPoints(skelPart, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap->getAngle(), keyMap->getAngleMod(), UP);
}

int LeapMotionGestures::leftWristFlexedDown(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;

	std::map<int, std::vector<double>> points = getPoints(skelPart, 24, 25, 23, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap->getAngle(), keyMap->getAngleMod(), DOWN);
}


int LeapMotionGestures::rightWristFlexedUp(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;

	std::map<int, std::vector<double>> points = getPoints(skelPart, 1, 2, 0, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap->getAngle(), keyMap->getAngleMod(), UP);
}


int LeapMotionGestures::rightWristFlexedDown(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;

	std::map<int, std::vector<double>> points = getPoints(skelPart, 1, 2, 0, *lastPositions);
	if ( points.size() == 0 ) {
		return -1;
	}
	return flexed3d(points, keyMap->getAngle(), keyMap->getAngleMod(), DOWN);
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
int LeapMotionGestures::leftClosed(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;
	return handClosed((float)a.channel[0]);
}

int LeapMotionGestures::rightClosed(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;
	return handClosed((float)a.channel[2]);
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
int LeapMotionGestures::leftPinch(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;
	return pinch((float)a.channel[1]);
}

int LeapMotionGestures::rightPinch(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;
	return pinch((float)a.channel[3]);
}