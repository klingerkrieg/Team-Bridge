#pragma once

#ifdef ARDUINO_MODULE

#include "AbstractAction.h"
#include <iostream>

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 8

struct COMDevice {
	HANDLE handler;
	COMSTAT status;
	DWORD errors;
	int bauds;
	bool connected = false;
};

class ArduinoAction : public AbstractAction {

	private:
	// COM / ComDevice
	std::map<int, COMDevice> connections;

	COMDevice ArduinoAction::connect(KeyMap key);

	public:
	ArduinoAction();
	~ArduinoAction();
	void run(KeyMap key);

};

#endif