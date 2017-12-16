/** @file	vrpn_LeapMotion.h
@brief	Header for LeapMotion VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#include "Leap.h"
#include "vrpn_Analog.h"
#include <iostream>
#include <string>

class VRPN_API vrpn_LeapMotion : public Leap::Listener, public vrpn_Analog, public  vrpn_Tracker {
private:
	int handId;
public:
	
	vrpn_LeapMotion(const char *name, int hand, vrpn_Connection *c = NULL);
	virtual ~vrpn_LeapMotion();
	virtual void mainloop();

	struct timeval _timestamp;
	static Leap::Controller controller;


protected:
	virtual void onConnect(const Leap::Controller&);
	virtual void onFrame(const Leap::Controller&);
	virtual void reportPose(int sensor, Leap::Vector position);

};

