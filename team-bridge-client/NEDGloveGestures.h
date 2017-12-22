#pragma once
#include <vrpn_Analog.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"
#include "CheckerSubject.h"

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class NEDGloveGestures : public AbstractGestureRecognizer {
	
	int strengthNormal = 250;

	public:
	int closed(vrpn_ANALOGCB a, KeyMap *keyMap);
	int pinch(vrpn_ANALOGCB a, KeyMap *keyMap);


	protected:
	bool assignChecker(CheckerSubject * checker, KeyMap * keyMap);

	

};