#pragma once
#include "KinectGestures.h"
#include "LeapMotionGestures.h"
#include "NEDGloveGestures.h"
#include "ButtonChecker.h"
#include "KeyMap.h"
#include "AbstractGestureRecognizer.h"
#include "CheckerSubject.h"
#include <map>

class GestureRecognizer
	: public KinectGestures
	, public LeapMotionGestures
	, public NEDGloveGestures
	, public ButtonChecker {

	private:
	bool assignChecker(CheckerSubject *checker, KeyMap *keyMap);

	public:
	void assignCheckers(std::map<std::string, CheckerSubject> &checkers, std::vector<KeyMap> &map);

};