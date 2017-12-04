/** @file	vrpn_OculusDK1.h
@brief	Header for LeapMotion VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#include "Leap.h"
#include "vrpn_Tracker.h"
#include <iostream>
#include <string>
#include <OVR_CAPI.h>

class VRPN_API vrpn_OculusDK1 : public  vrpn_Tracker {
	private:
	bool connected = false;
	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result;


	public:

	vrpn_OculusDK1(const char *name, vrpn_Connection *c = NULL);
	virtual ~vrpn_OculusDK1();
	virtual void mainloop();

	protected:
	virtual void connect();
	virtual void onFrame();

};

