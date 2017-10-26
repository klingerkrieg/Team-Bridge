#pragma once
#include "KinectGestures.h"
#include "LeapMotionGestures.h"

class GestureRecognizer
	: public KinectGestures
	, public LeapMotionGestures {

};