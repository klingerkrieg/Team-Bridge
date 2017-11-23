#pragma once
#include <stdio.h>  // for printf, fprintf, NULL, etc
#include <stdlib.h> // for exit, atoi

#ifndef _WIN32_WCE
#include <signal.h> // for signal, SIGINT
#endif
#include <string.h>              // for strcmp, strncpy
#include <vrpn_Analog.h>         // for vrpn_ANALOGCB, etc
#include <vrpn_Button.h>         // for vrpn_Button_Remote, etc
#include <vrpn_Dial.h>           // for vrpn_Dial_Remote, etc
#include <vrpn_FileConnection.h> // For preload and accumulate settings
#include <vrpn_Shared.h>         // for vrpn_SleepMsecs
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc

#include "vrpn_BaseClass.h" // for vrpn_System_TextPrinter, etc
#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32

#include <iostream>
#include <map>
#include <vector>
#include "Config.h"
#include "ConfigFileReader.h"
#include "KeyMap.h"
#include "InputConverter.h"
//#include "DeviceInfo.h"
#include "Storage.h"
#include "InputConverter.h"

#include "View.h"
#include "json.hpp"

//GUI

#include <shellapi.h>
#include <windows.h> 
#include <tchar.h>  
#include <io.h>
#include <fcntl.h>


void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t);
void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b);
void VRPN_CALLBACK handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b);
void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a);


class Client {
private:
	Storage storage;
	InputConverter inputConverter;
	View view;

	char *configFileName = "vrpn-client.cfg";
	bool printTracker = true;
	bool printButton = true;
	bool printAnalog = true;
	bool exportDb = false;

	bool recordGesture = false;

	static const unsigned MAX_DEVICES = 50;
	DeviceInfo deviceList[MAX_DEVICES];
	unsigned num_devices = 0;


public:

	Client() {

	}

	View& getView() {
		return view;
	}

	void setView(View &view) {
		this->view = view;
	}

	Storage& getStorage() {
		return storage;
	}

	InputConverter* getInputConverter() {
		return &inputConverter;
	}


	void setConfigFile(char * file_name) {
		configFileName = file_name;
	}

	void setPrintTracker(bool print) {
		printTracker = print;
	}

	void setPrintButton(bool print) {
		printButton = print;
	}

	void setPrintAnalog(bool print) {
		printAnalog = print;
	}

	void setExport(bool exp) {
		exportDb = exp;
	}

	bool setup(bool test);
};