/** @file	vrpn_KinectV2.h
@brief	Header for KinectV2 VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once

#ifdef KINECTV2

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc
#include "vrpn_Analog.h"
#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "Kinect.h"
#include "configs.h"


class VRPN_API vrpn_KinectV2 : public  vrpn_Tracker, public vrpn_Analog {

	private:
	static IKinectSensor*          m_pKinectSensor;
	static ICoordinateMapper*      m_pCoordinateMapper;
	static IBodyFrameReader*       m_pBodyFrameReader;
	static HRESULT hr;
	static bool connected;
	static bool skeletonArr[BODY_COUNT];
	static bool status;
	static int skeletonIds[BODY_COUNT];
	int tentativas = 0;

	int skeleton;
	

	public:

	vrpn_KinectV2(const char *name, int skeleton, vrpn_Connection *c = NULL);
	virtual ~vrpn_KinectV2();
	virtual void mainloop();

	struct timeval _timestamp;

	protected:
	bool connect();
	bool onFrame();
	void reportPose(int sensor, CameraSpacePoint position, int quat);

	template<class Interface>
	void  SafeRelease(Interface *& pInterfaceToRelease) {
		if ( pInterfaceToRelease != NULL ) {
			pInterfaceToRelease->Release();
			pInterfaceToRelease = NULL;
		}
	}

	void setKinectSkeletonId(IBody* ppBodies[BODY_COUNT]);
};

#endif