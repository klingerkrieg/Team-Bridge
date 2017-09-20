#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vrpn_Analog.h>         // for vrpn_ANALOGCB, etc
#include <vrpn_Button.h>         // for vrpn_Button_Remote, etc


class DeviceInfo {
public:
	char *name;
	vrpn_Tracker_Remote *tkr;
	vrpn_Button_Remote *btn;
	vrpn_Analog_Remote *ana;
};


class TrackerUserCallback {
public:
	char name[vrpn_MAX_TEXT_LEN];
	std::vector<unsigned> counts;
};