/** @file	vrpn_LeapMotion.h
@brief	Header for LeapMotion VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#define __STRUCT__ struct
#define interface __STRUCT__
/*
#include <fstream>
#include <iostream>
#include <time.h>*/

#include "NuiApi.h"



class VRPN_API vrpn_KinectV1 : public  vrpn_Tracker {
	
	private:
	INuiSensor * pNuiSensor;
	INuiSensor* m_pNuiSensor;
	HANDLE      m_pSkeletonStreamHandle;
	HANDLE      m_hNextSkeletonEvent;
	int iSensorCount;
	HRESULT hr;
	
	int lastSkeletonCount = 0;
	bool connected = false;
	
	public:

	vrpn_KinectV1(const char *name, vrpn_Connection *c = NULL);
	virtual ~vrpn_KinectV1();
	virtual void mainloop();

	struct timeval _timestamp;

	protected:
	bool connect();
	bool onFrame();
	void reportPose(int sensor, Vector4 position, Vector4 quat);

};
