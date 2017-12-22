#include "LeapMotionGestures.h"


std::map<std::string, Skeleton> LeapMotionGestures::skeleton;
std::map<int, int> LeapMotionGestures::handSkeletonMap1 = LeapMotionGestures::create_handSkeletonMap1();



bool LeapMotionGestures::assignChecker(CheckerSubject *checker, KeyMap *keyMap) {
	
	switch ( keyMap->getKey() ) {
		case LEAP_CLOSED:
			checker->attach((AnalogCheckerMethod)&LeapMotionGestures::closed, keyMap, this);
			return true;
			break;
		case LEAP_PINCH:
			checker->attach((AnalogCheckerMethod)&LeapMotionGestures::pinch, keyMap, this);
			return true;
			break;
		case LEAP_WRIST_UP:
			checker->attach(SKELETON_HAND_L, (TrackerCheckerMethod)&LeapMotionGestures::wristFlexedUp, keyMap, this);
			return true;
			break;
		case LEAP_WRIST_DOWN:
			checker->attach(SKELETON_HAND_L, (TrackerCheckerMethod)&LeapMotionGestures::wristFlexedDown, keyMap, this);
			return true;
			break;
	}
	return false;
}


int LeapMotionGestures::wristFlexedUp(SkeletonPart skelPart, KeyMap * keyMap) {
	//O nome do skeleton é salvo somente para poder capturar dentro desses metodos
	//o nome do skeleton é o nome do dispostivo = Tracker0 Tracker1 etc
	Skeleton skel = skeleton[skelPart.skeletonName];

	std::map<int, std::vector<double>> points = getPoints(skel.elbowL , skel.wristL, skel.handL );
	if ( points.size() < 3 ) {
		return -1;
	}
	
	return flexed3d(points, keyMap, UP);
}

int LeapMotionGestures::wristFlexedDown(SkeletonPart skelPart, KeyMap * keyMap) {
	Skeleton skel = skeleton[skelPart.skeletonName];

	std::map<int, std::vector<double>> points = getPoints(skel.elbowL, skel.wristL, skel.handL);
	if ( points.size() < 3 ) {
		return -1;
	}
	return flexed3d(points, keyMap, DOWN);
}



// Mao fechada
int LeapMotionGestures::closed(vrpn_ANALOGCB a, KeyMap * keyMap) {
	float angle = (float)a.channel[0];
	if ( angle == -1 ) {
		return -1;
	}

	float angleMax = keyMap->getAngleMax() == 0 ? handAngleMax : keyMap->getAngleMax();
	float angleMin = keyMap->getAngleMin() == 0 ? 0 : keyMap->getAngleMin();
	
	if ( angleMin <= angle && angle < angleMax ) {

	#ifdef THERAPY_MODULE
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), angle);
		}
	#endif

		return 1;
}
	return 0;
}

//Pinça
int LeapMotionGestures::pinch(vrpn_ANALOGCB a, KeyMap * keyMap) {

	float dist = (float)a.channel[1];
	if ( dist == -1 ) {
		return -1;
	}

	float maxDist = keyMap->getDistanceMax() == 0 ? pinchDistanceMax : keyMap->getDistanceMax();
	float minDist = keyMap->getDistanceMin() == 0 ? 0 : keyMap->getDistanceMin();
	
	if ( minDist <= dist && dist < maxDist ) {

	#ifdef THERAPY_MODULE
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), angle);
		}
	#endif

		return 1;
	}
	return 0;
}
