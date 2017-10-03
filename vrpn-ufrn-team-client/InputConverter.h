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

class InputConverter {
private:
	 std::vector<KeyMap> map;
	 std::string app;
	 GestureRecognizer gr;
	 View view;
	 static int lastTimeTrack;

public:

	InputConverter() {
		init();
	}

	InputConverter(std::vector<KeyMap> map, std::string app) {
		this->map = map;
		this->app = app;
		init();
	}

	void init() {
		gr = GestureRecognizer();
		view = View();
	}
	
	 void InputConverter::press(char key, bool isConstant);

	// void InputConverter::release(char key);

	 bool InputConverter::checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	 bool InputConverter::checkButton(const char * name, const vrpn_BUTTONCB b);

	 bool InputConverter::checkAnalog();
	
};


