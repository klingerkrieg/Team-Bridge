#pragma once
#include <vrpn_Analog.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"

class NEDGloveGestures {
	
	int sum(const vrpn_ANALOGCB a);

	public:
	bool handClosed(const vrpn_ANALOGCB a, int closeCalib);
	bool handOpen(const vrpn_ANALOGCB a, int openCalib);

};