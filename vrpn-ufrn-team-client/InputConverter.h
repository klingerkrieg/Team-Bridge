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
#include "ConfigFileReader.h"
#include "AbstractAction.h"

#include "KinectGestures.h"
#include "LeapMotionGestures.h"
#include "NEDGloveGestures.h"

#include "VariabilitiesConfig.h"
#ifdef THERAPY_MODULE
#include "Storage.h"
#endif

//#define PERFORMANCE_TEST

struct DeviceSensorCount {
	int count = 0;
	bool finalized = false;
	int sensorStarted = -1;
};

class InputConverter {
private:
	 static std::vector<KeyMap> map;
	 static std::vector<DeviceType> devs;
	 GestureRecognizer gr;
 #ifdef THERAPY_MODULE
	 Storage *storage;
 #endif
	 static std::map<std::string, DeviceSensorCount> devicesSensorsCount;

	 static bool nextDefineCenterPos;

	 AbstractAction *act;

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

#ifdef THERAPY_MODULE
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, Storage &storage, AbstractAction *act) {
		this->storage = &storage;
#else
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, AbstractAction *act) {
#endif
		this->map = map;
		this->devs = devs;
		this->act = act;

		init();
	}

#ifdef THERAPY_MODULE
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, Storage &storage, AbstractAction *act, View &view) {
		this->storage = &storage;
#else
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, AbstractAction *act, View &view) {
#endif
		this->map = map;
		this->devs = devs;
		this->view = &view;
		this->act = act;
		viewOn = true;
		init();
	}

	void init() {
		gr = GestureRecognizer();
	#ifdef THERAPY_MODULE
		gr.AbstractGestureRecognizer::setStorage(storage);
	#endif

		//Aqui foi aplicado algo semelhante ao padrão Observer
		//Chama o assign geral, dentro desse sera chamado individualmente o assign de cada GestureRecognizer
		gr.GestureRecognizer::assignChecker(map);
	}
	
	 void interpretKeyMap(KeyMap &keyMap);

	// void release(char key);
	 bool interpretOnLeave(bool active, KeyMap &keyMap);

	 bool checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	 bool checkButton(const char * name, const vrpn_BUTTONCB b);

	 bool checkAnalog(const char *name, const vrpn_ANALOGCB a);
	
};


