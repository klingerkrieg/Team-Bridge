#pragma once
#include <vrpn_Button.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include "KeyMap.h"

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class ButtonChecker : public AbstractGestureRecognizer {

	public:
	int pressed(void *data, KeyMap *keyMap);

	protected:
	void assignChecker(std::vector<KeyMap> &map);



};