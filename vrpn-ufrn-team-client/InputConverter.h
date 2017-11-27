#pragma once
#include <vector>
#include "KeyMap.h"
#include "DeviceInfo.h"
#include "GestureRecognizer.h"

#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32
#include "vrpn_BaseClass.h" // for vrpn_System_TextPrinter, etc
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vrpn_Analog.h>         // for vrpn_ANALOGCB, etc
#include <vrpn_Button.h>         // for vrpn_Button_Remote, etc
#include <Tchar.h>
#include "View.h"
#include "util.h"


//#define PERFORMANCE_TEST

struct DeviceSensorCount {
	int count = 0;
	bool finalized = false;
	int sensorStarted = -1;
};

class InputConverter {
private:
	 static std::vector<KeyMap> map;
	 std::string app;
	 GestureRecognizer gr;

	 static std::map<std::string, DeviceSensorCount> devicesSensorsCount;

	 static bool nextDefineCenterPos;

	 bool viewOn = false;
	 View *view;
	 static int lastTimeTrack;
	 static int lastTimeCenterPos;
	 int centerPosDelay = 1;

	 static bool mouseLeftPressed;
	 static bool mouseRightPressed;
	 static bool mouseMiddlePressed;

 #ifdef PERFORMANCE_TEST
	 static double qtdMed;
	 static double usecMed;
	 static double secMed;
 #endif

public:

	InputConverter() {
		init();
	}

	~InputConverter();

	InputConverter(std::vector<KeyMap> map, std::string app) {
		this->map = map;
		this->app = app;
		init();
	}

	InputConverter(std::vector<KeyMap> map, std::string app, View &view) {
		this->map = map;
		this->app = app;
		this->view = &view;
		viewOn = true;
		init();
	}

	void init() {
		gr = GestureRecognizer();
	}
	
	 void press(KeyMap key);

	 void interpretKeyMap(KeyMap &keyMap);

	// void release(char key);
	 bool interpretOnLeave(bool active, KeyMap &keyMap);

	 bool checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	 bool checkButton(const char * name, const vrpn_BUTTONCB b);

	 bool checkAnalog(const char *name, const vrpn_ANALOGCB a);
	
};


