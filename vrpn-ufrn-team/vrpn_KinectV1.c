/** @file	vrpn_KinectV1.C
@brief	Drivers for LeapMotion VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

// Based on the vrpn_Oculus driver

#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "vrpn_KinectV1.h"

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc


VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()


vrpn_KinectV1::vrpn_KinectV1(const char *name, vrpn_Connection *c) : vrpn_Tracker(name, c) {
	connect();
}

void vrpn_KinectV1::mainloop() {
	if ( connected ) {
		if ( !onFrame() ) {
			printf("Perda de conexao com o Kinect.\n");
			connected = false;
			connect();
		}
	}
	server_mainloop();
}

vrpn_KinectV1::~vrpn_KinectV1() {
	if ( m_pNuiSensor ) {
		m_pNuiSensor->NuiShutdown();
	}

	if ( m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE) ) {
		CloseHandle(m_hNextSkeletonEvent);
	}
}


bool vrpn_KinectV1::connect() {
	
	while (connected == false) {

		printf("Conectando-se ao Kinect...\n");

		iSensorCount = 0;
		hr = NuiGetSensorCount(&iSensorCount);
		if ( FAILED(hr) || iSensorCount == 0 ) {
			printf("Nenhum Kinect encontrado.\n");
			Sleep(3000);
			continue;
		}

		m_pNuiSensor = NULL;

		// Look at each Kinect sensor
		for ( int i = 0; i < iSensorCount; ++i ) {
			// Create the sensor so we can check status, if we can't create it, move on to the next
			hr = NuiCreateSensorByIndex(i, &pNuiSensor);
			if ( FAILED(hr) ) {
				continue;
			}

			// Get the status of the sensor, and if connected, then we can initialize it
			hr = pNuiSensor->NuiStatus();
			if ( S_OK == hr ) {
				m_pNuiSensor = pNuiSensor;
				break;
			}

			// This sensor wasn't OK, so release it since we're not using it
			pNuiSensor->Release();
		}

		if ( NULL != m_pNuiSensor) {
		
			// Initialize the Kinect and specify that we'll be using skeleton
			hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
		
			if ( SUCCEEDED(hr) ) {
				// Create an event that will be signaled when skeleton data is available
				m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

				// Open a skeleton stream to receive skeleton data
				hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
			}
		}

		if ( NULL == m_pNuiSensor || FAILED(hr) ) {
			printf("Falha ao se conectar ao Kinect.\n");
			Sleep(3000);
			continue;
		}

		connected = true;
		printf("Kinect conectado.\n");
	}
	
	return true;
}

void vrpn_KinectV1::reportPose(int sensor, timeval t,Vector4 position, Vector4 quat) {
	//Seta dados para envio
	d_sensor = sensor;
	pos[0] = position.x;
	pos[1] = position.y;
	pos[2] = position.z;
	
	d_quat[0] = quat.x;
	d_quat[1] = quat.y;
	d_quat[2] = quat.z;
	d_quat[3] = quat.w;

	
	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	//if (sensor == 0 )
	//	printf("sensor: %d %.2f %.2f %.2f \n", sensor, position.x, position.y, position.z);
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf,
		vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}

bool vrpn_KinectV1::onFrame() {

	//Verifica conexao
	hr = m_pNuiSensor->NuiStatus();
	if ( S_OK != hr ) {
		return false;
	}


	NUI_SKELETON_FRAME skeletonFrame = { 0 };

	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if ( FAILED(hr) ) {
		//printf("Falha ao capturar frame.\n");
		return true;//Mesmo tendo essa falha ainda pode estar conectado
	}

	timeval t;
	vrpn_gettimeofday(&t, NULL);

	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
	
	NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];
	int countSkeletons = 0;
	for ( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
		NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
		NuiSkeletonCalculateBoneOrientations(&skeletonFrame.SkeletonData[i], boneOrientations);

		int sensor = 0;
		if ( NUI_SKELETON_TRACKED == trackingState ) {
			countSkeletons++;
			for ( int y = 0; y < 20; y++ ) {
				//Essa adaptação é necessária para enviar os mesmos pontos do FAAST
				switch ( y ) {
					case 3:
						sensor = 0;
						break;
					case 2:
						sensor = 1;
						break;
					case 1:
						sensor = 2;
						break;
					case 0:
						sensor = 3;
						break;
					default:
						sensor = y;
					break;
				}

				NUI_SKELETON_BONE_ORIENTATION & orientation = boneOrientations[y];
				
				reportPose(sensor, t, skeletonFrame.SkeletonData[i].SkeletonPositions[y],
						   orientation.absoluteRotation.rotationQuaternion);
			}
		}

	}

	if ( countSkeletons > lastSkeletonCount ) {
		printf("Kinect Capturando.\n");
		lastSkeletonCount = countSkeletons;
	} else if ( lastSkeletonCount != countSkeletons ){
		printf("Kinect Parado.\n");
		lastSkeletonCount = countSkeletons;
	}
	
	return true;
}
