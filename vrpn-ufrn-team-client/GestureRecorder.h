#pragma once
#include "View.h"
#include "DeviceInfo.h"
#include <vrpn_Tracker.h>
#include <string>

class GestureRecorder {


	bool viewOn = false;
	View *view;
	int recordDelay = 6;
	int countDelay = 2;

	static int countMessages;
	static int lastTime;
	static bool recording;

	public:

	GestureRecorder() {

	}

	GestureRecorder( View &view) {
		this->view = &view;
		viewOn = true;
	}

	void record(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);


};