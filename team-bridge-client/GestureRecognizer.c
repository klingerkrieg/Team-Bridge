#include "GestureRecognizer.h"


bool GestureRecognizer::assignChecker(CheckerSubject *checker, KeyMap *keyMap) {
	return false;
}


void GestureRecognizer::assignCheckers(std::map<std::string, CheckerSubject> &checkers, std::vector<KeyMap> &map) {
	//Cada assigner é chamado
	
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		KeyMap *keyMap = &map.at(keyMapId);

		CheckerSubject *checker = &checkers[keyMap->getDev()];

		if ( this->LeapMotionGestures::assignChecker(checker, keyMap) ) {
			continue;
		} else if ( this->ButtonChecker::assignChecker(checker, keyMap) ) {
			continue;
		} else if ( this->NEDGloveGestures::assignChecker(checker, keyMap) ) {
			continue;
		} else if ( this->KinectGestures::assignChecker(checker, keyMap) ) {
			continue;
		}

		printf("KeyMap checker not assigned\n");

	}
}
