#include "LeapMotionGestures.h"


std::map<std::string, Skeleton> LeapMotionGestures::skeleton;
std::map<int, int> LeapMotionGestures::handSkeletonMap1 = LeapMotionGestures::create_handSkeletonMap1();



void LeapMotionGestures::assignChecker(std::vector<KeyMap> &map) {
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		KeyMap *keyMap = &map.at(keyMapId);
		//cada metodo que será utilizado é alocado em seus respecitvos keyMaps


		switch ( keyMap->getKey() ) {
			case LEAP_CLOSED:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::closed, this);
				break;
			case LEAP_PINCH:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::pinch, this);
				break;
			case LEAP_WRIST_UP:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::wristFlexedUp, this);
				break;
			case LEAP_WRIST_DOWN:
				keyMap->assignGestureChecker(TRACK_TYPE, (KeyMap::gestureCheckerMethod)&LeapMotionGestures::wristFlexedDown, this);
				break;
		}
	}
}


int LeapMotionGestures::wristFlexedUp(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;
	//O nome do skeleton é salvo somente para poder capturar dentro desses metodos
	//o nome do skeleton é o nome do dispostivo = Tracker0 Tracker1 etc
	Skeleton skel = skeleton[skelPart.skeletonName];

	std::map<int, std::vector<double>> points = getPoints(skel.elbowL , skel.wristL, skel.handL );
	if ( points.size() < 3 ) {
		return -1;
	}
	return flexed3d(points, keyMap, UP);
}

int LeapMotionGestures::wristFlexedDown(void * data, KeyMap * keyMap) {
	SkeletonPart skelPart = *(SkeletonPart*)data;
	Skeleton skel = skeleton[skelPart.skeletonName];

	std::map<int, std::vector<double>> points = getPoints(skel.elbowL, skel.wristL, skel.handL);
	if ( points.size() < 3 ) {
		return -1;
	}
	return flexed3d(points, keyMap, DOWN);
}



// Mao fechada
int LeapMotionGestures::closed(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;
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
int LeapMotionGestures::pinch(void * data, KeyMap * keyMap) {
	vrpn_ANALOGCB a = *(vrpn_ANALOGCB *)data;

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
