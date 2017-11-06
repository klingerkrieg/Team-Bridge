#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "util.h"
#include "FlexedMember.h"


class LeapMotionGestures : public FlexedMember {
private:

	float closeAngle = 2.5;
	float pinchAngle = 60;

	static std::map<int, std::vector<double>> lastPositions[10];

	int handClosed(float angle);
	int pinch(float angle);

public:
	
	int leftFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int leftFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);
	int leftClosed(const vrpn_ANALOGCB a);
	int leftPinch(const vrpn_ANALOGCB a);

	int rightFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int rightFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);
	int rightClosed(const vrpn_ANALOGCB a);
	int rightPinch(const vrpn_ANALOGCB a);

};