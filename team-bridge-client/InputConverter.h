#pragma once
#include <vector>
#include "KeyMap.h"
#include "DeviceInfo.h"
#include "GestureRecognizer.h"
#include "CheckerSubject.h"

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

struct DeviceSensorCount {
	int count = 0;
	bool finalized = false;
	int sensorStarted = -1;
};

class InputConverter {
private:
	static std::vector<KeyMap> map;
	static std::vector<DeviceType> devs;

	std::map<std::string, CheckerSubject> checkers;
	 
	static SkeletonPart skelPart;
	GestureRecognizer *gr;
	AbstractAction *act;

	static int lastTimeTrack;
	bool viewOn = false;
	View *view;
	

 #ifdef THERAPY_MODULE
	 Storage *storage;
 #endif
	 

public:

	InputConverter() {
		
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

		for ( size_t i = 0; i < devs.size(); i++ ) {
			checkers[devs.at(i).name] = CheckerSubject();
		}

		gr = new GestureRecognizer();
		gr->assignCheckers(checkers, map);
	#ifdef THERAPY_MODULE
		gr->AbstractGestureRecognizer::setStorage(this->storage);
	#endif

	}

#ifdef THERAPY_MODULE
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, Storage &storage, AbstractAction *act, View &view)
		: InputConverter(map, devs, storage, act) {
		this->storage = &storage;
#else
	InputConverter(std::vector<KeyMap> &map, std::vector<DeviceType> &devs, AbstractAction *act, View &view)
		: InputConverter(map, devs, act) {
#endif

		this->view = &view;
	}

	

	 bool checkTrack(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	 bool checkButton(const char * name, const vrpn_BUTTONCB b);

	 bool checkAnalog(const char *name, const vrpn_ANALOGCB a);

	 void interpretKeyMap(KeyMap *keyMap);

	 bool interpretOnLeave(bool active, KeyMap *keyMap);
	
};


