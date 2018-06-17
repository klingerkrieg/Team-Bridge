#pragma once

#ifdef ARDUINO_MODULE

#include "AbstractAction.h"
#include <iostream>

//seconds
#define ARDUINO_DELAY_TO_MSG 2
//milli
#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 8

struct COMDevice {
	int COM;
	HANDLE handler;
	COMSTAT status;
	DWORD errors;
	int bauds;
	bool connected = false;
	int lastMessage = 0;
};

class ArduinoAction : public AbstractAction {

	private:
	// COM / ComDevice
	std::map<int, COMDevice> connections;

	static bool liga;

	COMDevice ArduinoAction::connect(KeyMap key);

	public:
	ArduinoAction();
	~ArduinoAction();
	void run(KeyMap key);

};

#endif