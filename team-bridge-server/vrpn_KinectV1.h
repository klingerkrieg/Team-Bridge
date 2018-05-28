/** @file	vrpn_KinectV1.h
@brief	Header for KinectV1 VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#define __STRUCT__ struct
#define interface __STRUCT__

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc
#include <math.h>
#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "NuiApi.h"
#include <string>
#include "MainView.h"
#include "configs.h"


class VRPN_API vrpn_KinectV1 : public  vrpn_Tracker {
	
	private:
	static INuiSensor * pNuiSensor;
	static INuiSensor* m_pNuiSensor;
	static HANDLE      m_pSkeletonStreamHandle;
	static HANDLE      m_hNextSkeletonEvent;
	static int iSensorCount;
	static HRESULT hr;
	static bool connected;
	static bool skeletonArr[NUI_SKELETON_COUNT];
	static bool status;
	static int skeletonIds[NUI_SKELETON_COUNT];
	int tentativas = 0;

	int skeleton;
	
	public:

	
	vrpn_KinectV1(const char *name, int skeleton, vrpn_Connection *c = NULL);
	virtual ~vrpn_KinectV1();
	virtual void mainloop();

	struct timeval _timestamp;

	protected:
	bool connect();
	bool onFrame();
	void reportPose(int skeleton, int sensor, Vector4 position, Vector4 quat);

	void setKinectSkeletonId(NUI_SKELETON_DATA ppBodies[]);

};

