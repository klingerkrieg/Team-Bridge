/** @file	vrpn_KinectV2.h
@brief	Header for KinectV2 VR devices.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#define __STRUCT__ struct
#define interface __STRUCT__


#include "Kinect.h"



class VRPN_API vrpn_KinectV2 : public  vrpn_Tracker {

	private:
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	IBodyFrameReader*       m_pBodyFrameReader;
	int lastSkeletonCount	= -1;
	HRESULT hr;

	bool connected = false;

	public:

	vrpn_KinectV2(const char *name, vrpn_Connection *c = NULL);
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
};

