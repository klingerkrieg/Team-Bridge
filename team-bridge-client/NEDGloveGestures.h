#pragma once
#include <vrpn_Analog.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class NEDGloveGestures : public AbstractGestureRecognizer {
	
	int strengthNormal = 250;

	public:
	int closed(void *data, KeyMap *keyMap);
	int pinch(void *data, KeyMap *keyMap);


	protected:
	void assignChecker(std::vector<KeyMap> &map);

	

};