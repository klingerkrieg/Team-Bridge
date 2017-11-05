#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "util.h"

const int LEFT_HAND = 1;
const int RIGHT_HAND = 2;
const int UP = 1;
const int DOWN = 2;


class LeapMotionGestures {
private:

	float closeAngle = 0.5;
	float pinchAngle = 0.5;

	static std::map<int, std::vector<double>> lastPositions[10];

	int fistFlexed(const vrpn_TRACKERCB t, int side, int direction, int angle);
	bool handClosed(float angle);
	bool pinch(float angle);

public:
	
	int leftFistFlexedUp(const vrpn_TRACKERCB t, int angle);
	int leftFistFlexedDown(const vrpn_TRACKERCB t, int angle);
	bool leftClosed(const vrpn_ANALOGCB a);
	bool leftPinch(const vrpn_ANALOGCB a);

	int rightFistFlexedUp(const vrpn_TRACKERCB t, int angle);
	int rightFistFlexedDown(const vrpn_TRACKERCB t, int angle);
	bool rightClosed(const vrpn_ANALOGCB a);
	bool rightPinch(const vrpn_ANALOGCB a);

};