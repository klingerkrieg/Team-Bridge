#pragma once
#include <vrpn_Button.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"
#include "CheckerSubject.h"

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class ButtonChecker : public AbstractGestureRecognizer {

	public:
	
	int pressed(vrpn_BUTTONCB b, KeyMap *keyMap);

	protected:
	bool assignChecker(CheckerSubject * checker, KeyMap * keyMap);



};