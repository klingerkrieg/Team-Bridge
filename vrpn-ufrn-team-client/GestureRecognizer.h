#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"

class GestureRecognizer {
private:
	static double lastHeight;
	static bool lastHeightDefined;

	static double lastHeadHeight;
	static bool lastHeadHeightDefined;
	double handTopInterval = 0.15;

public:

	int detectLeftHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectRightHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens);

};


