#pragma once
#include "KinectGestures.h"
#include "LeapMotionGestures.h"
#include "NEDGloveGestures.h"

class GestureRecognizer
	: public KinectGestures
	, public LeapMotionGestures
	, public NEDGloveGestures {

};