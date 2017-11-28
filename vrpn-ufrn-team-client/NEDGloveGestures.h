#pragma once
#include <vrpn_Analog.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"

class NEDGloveGestures {
	
	int strengthNormal = 250;

	public:
	bool closed(const vrpn_ANALOGCB a, KeyMap *keyMap);
	bool pinch(const vrpn_ANALOGCB a, KeyMap *keyMap);

};