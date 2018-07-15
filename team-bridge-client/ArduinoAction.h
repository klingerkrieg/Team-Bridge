#pragma once

#ifdef ARDUINO_MODULE

#include "AbstractAction.h"
#include <iostream>
#include <chrono>

using namespace  std::chrono;

//mili
#define ARDUINO_DELAY_TO_MSG 40
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
	long long lastMessage = 0;
	int lastEngine = -1;
	bool uniqueEngine = true;
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