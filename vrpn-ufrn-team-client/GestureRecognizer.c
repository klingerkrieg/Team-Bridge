#include "GestureRecognizer.h"

void GestureRecognizer::assignChecker(std::vector<KeyMap> &map) {
	//Cada assigner é chamado
	this->KinectGestures::assignChecker(map);
	this->NEDGloveGestures::assignChecker(map);
	this->LeapMotionGestures::assignChecker(map);
}
