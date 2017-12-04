/** @file	vrpn_LeapMotion.C
@brief	Drivers for LeapMotion VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

// Based on the vrpn_Oculus driver

#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "vrpn_OculusDK1.h"
#include "Leap.h"

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc

VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()


vrpn_OculusDK1::vrpn_OculusDK1(const char *name,vrpn_Connection *c)
	: vrpn_Tracker(name, c) {
	while ( connected == false ) {
		connect();
		Sleep(500);
	}
}

void vrpn_OculusDK1::mainloop() {
	if ( connected == true ) {
		onFrame();
	}
	server_mainloop();
}

vrpn_OculusDK1::~vrpn_OculusDK1() {
	ovr_Destroy(session);
	ovr_Shutdown();
}


void vrpn_OculusDK1::connect() {
	printf("Conectando ao OculusDK1.\n");

	result = ovr_Initialize(nullptr);
	if ( OVR_FAILURE(result) ) {
		printf("Falha ao iniciar OculusDK1.\n");
		return;
	}

	
	result = ovr_Create(&session, &luid);
	if ( OVR_FAILURE(result) ) {
		printf("Falha ao iniciar sessao OculusDK1.\n");
		ovr_Shutdown();
		return;
	}

	printf("OculusDK1 Conectado [%s].\n", this->d_servicename);
	connected = true;
	//ovrHmdDesc desc = ovr_GetHmdDesc(session);
	//ovrSizei resolution = desc.Resolution;
}

void vrpn_OculusDK1::onFrame() {
	// Query the HMD for the current tracking state.
	ovrTrackingState ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds(), true);

	if ( ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked) ) {
		//Posef pose = ts.HeadPose;

		
		ovrQuatf ovrQuat = ts.HeadPose.ThePose.Orientation;
		ovrVector3f ovrPos = ts.HeadPose.ThePose.Position;
		//cout << quat.x << "\t" << quat.y << "\t" << quat.z << "\t" << quat.w << "\t\n";
		//cout << ts.HeadPose.ThePose.Position.x << "\t" << ts.HeadPose.ThePose.Position.y << "\t" << ts.HeadPose.ThePose.Position.z << "\n";

		pos[0] = ovrPos.x;
		pos[1] = ovrPos.y;
		pos[2] = ovrPos.z;

		d_quat[0] = ovrQuat.x*-1;
		d_quat[1] = ovrQuat.y*-1;
		d_quat[2] = ovrQuat.z;
		d_quat[3] = ovrQuat.w;

		printf("\r%.2f %.2f %.2f", d_quat[0], d_quat[1], d_quat[2]);

	}

	//Seta dados para envio
	d_sensor = 0;

	timeval t;
	vrpn_gettimeofday(&t, NULL);
	
	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf,
		vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}


