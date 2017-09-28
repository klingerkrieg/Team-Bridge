#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"

class GestureRecognizer {
private:
	double lastHeight = 0;
	bool lastHeightDefined = false;

public:

	int detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens) {
		if ( t.sensor == 0 ) {
			printf("Altura: %.2f \n", t.pos[1]);


			if ( lastHeightDefined == false ) {
				lastHeight = t.pos[1];
				lastHeightDefined = true;
				return 0;
			}

			//Desceu
			//last - pos
			//0.57 - 0.55 = 0.02 >= 0.02
			if ( lastHeight - t.pos[1] >= (float)heightSens ) {
				lastHeight = t.pos[1];
				return -1;
			} else
			//Subiu
			//pos  - last
			//0.59 - 0.57 = 0.02 >= 0.02
			if ( t.pos[1] -lastHeight >= (float)heightSens ) {
				lastHeight = t.pos[1];
				return 1;
			}

		}
		return 0;
	}

};
