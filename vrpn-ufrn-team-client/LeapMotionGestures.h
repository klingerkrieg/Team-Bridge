#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "util.h"
#include "FlexedMember.h"


class LeapMotionGestures : public FlexedMember {
private:

	float closeAngle = 0.5;
	float pinchAngle = 0.5;

	static std::map<int, std::vector<double>> lastPositions[10];

	bool handClosed(float angle);
	bool pinch(float angle);

public:
	
	int leftFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int leftFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);
	bool leftClosed(const vrpn_ANALOGCB a);
	bool leftPinch(const vrpn_ANALOGCB a);

	int rightFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int rightFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);
	bool rightClosed(const vrpn_ANALOGCB a);
	bool rightPinch(const vrpn_ANALOGCB a);

};