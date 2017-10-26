#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>


class LeapMotionGestures {
private:

	float closeAngle = 0.5;
	float pinchAngle = 0.5;

	int fistFlexed(const vrpn_TRACKERCB t);
	bool handClosed(float angle);
	bool pinch(float angle);

public:
	
	int leftFistFlexedUp(const vrpn_TRACKERCB t);
	int leftFistFlexedDown(const vrpn_TRACKERCB t);
	bool leftClosed(const vrpn_ANALOGCB a);
	bool leftPinch(const vrpn_ANALOGCB a);

	int rightFistFlexedUp(const vrpn_TRACKERCB t);
	int rightFistFlexedDown(const vrpn_TRACKERCB t);
	bool rightClosed(const vrpn_ANALOGCB a);
	bool rightPinch(const vrpn_ANALOGCB a);

};