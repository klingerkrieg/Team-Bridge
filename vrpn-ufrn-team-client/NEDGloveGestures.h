#pragma once
#include <vrpn_Analog.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"

class NEDGloveGestures {
	
	int sum(const vrpn_ANALOGCB a);

	public:
	bool handClosed(const vrpn_ANALOGCB a, KeyMap *keyMap);
	bool handOpen(const vrpn_ANALOGCB a, KeyMap *keyMap);

	bool handOpenMax(const vrpn_ANALOGCB a, KeyMap *keyMap);
	bool handSemiOpen(const vrpn_ANALOGCB a, KeyMap *keyMap);

	bool handClosedMax(const vrpn_ANALOGCB a, KeyMap *keyMap);
	bool handSemiClose(const vrpn_ANALOGCB a, KeyMap *keyMap);

};