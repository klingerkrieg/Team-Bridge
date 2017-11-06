#pragma once
#include "View.h"
#include "DeviceInfo.h"
#include <vrpn_Tracker.h>
#include <string>
#include "Storage.h"


struct Sensor {
	int id = -1;
	double x;
	double y;
	double z;
};

class GestureRecorder {


	bool viewOn = false;
	View *view;
	int recordDelay = 3;
	int countDelay = 2;
	int recordDuration = 8;

	std::string saveDir = "RECORDS";

	static std::ofstream fileOutput;

	static int countMessages;
	static int lastTime;
	static bool recording;

	static std::string recordName;

	static std::vector<int> sensors;

	static std::map<int,Sensor> writing;
	

	public:

	GestureRecorder() {
	}

	GestureRecorder( View &view) {
		this->view = &view;
		viewOn = true;
	}

	void setName(std::string name) {
		recordName = name;
	}


	void record(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);


	void printOptions() {
		while ( sensors.size() == 0 ) {
			printf("Escolha qual membro sera gravado: 1 - braco esquerdo\n2 - braco direito\n");
			int i;
			scanf("%d", &i);
			if ( 1 == i ) {
				sensors = { 4,5,6,7 };
			} else
				if ( 2 == i ) {
					sensors = { 8,9,10,11 };
				}
		}
	}

};

