#pragma once
#include "KinectGestures.h"
#include "LeapMotionGestures.h"
#include "NEDGloveGestures.h"
#include "KeyMap.h"
#include "AbstractGestureRecognizer.h"

class GestureRecognizer
	: public KinectGestures
	, public LeapMotionGestures
	, public NEDGloveGestures {

	public:
	void assignChecker(std::vector<KeyMap> &map);

};