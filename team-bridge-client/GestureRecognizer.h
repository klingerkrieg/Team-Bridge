#pragma once
#include "KinectGestures.h"
#include "LeapMotionGestures.h"
#include "NEDGloveGestures.h"
#include "ButtonChecker.h"
#include "KeyMap.h"
#include "AbstractGestureRecognizer.h"

class GestureRecognizer
	: public KinectGestures
	, public LeapMotionGestures
	, public NEDGloveGestures
	, public ButtonChecker {

	public:
	void assignChecker(std::vector<KeyMap> &map);

};