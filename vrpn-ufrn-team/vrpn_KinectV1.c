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
	connected = connect();
	if ( connected ) {
		printf("Kinect conectado.\n");
	}
	//vrpn_gettimeofday(&_timestamp, NULL);

	/*#ifdef PERFORMANCE_TEST
		fileOutput = std::ofstream("kinect-server-performance-test.txt");
		fileOutput << "STARTED\n";
		printf("Performance test\n");
	#endif*/
}

void vrpn_KinectV1::mainloop() {
	if ( connected ) {
		onFrame();
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

	/*#ifdef PERFORMANCE_TEST
		fileOutput.close();
	#endif*/
}


bool vrpn_KinectV1::connect() {
	INuiSensor * pNuiSensor;

	int iSensorCount = 0;
	HRESULT hr = NuiGetSensorCount(&iSensorCount);
	if ( FAILED(hr) ) {
		return false;
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
		printf("No ready Kinect found!");
		return false;
	}

	return true;
}

void vrpn_KinectV1::reportPose(int sensor, timeval t,Vector4 position) {
	//Seta dados para envio
	d_sensor = sensor;
	pos[0] = position.x;
	pos[1] = position.y;
	pos[2] = position.z;
	
	d_quat[0] = 0;
	d_quat[1] = 0;
	d_quat[2] = 0;
	d_quat[3] = 0;


	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	//if (sensor == 0 )
	//	printf("sensor: %d %.2f %.2f %.2f \n", sensor, position.x, position.y, position.z);
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf,
		vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}

void vrpn_KinectV1::onFrame() {
	NUI_SKELETON_FRAME skeletonFrame = { 0 };

	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if ( FAILED(hr) ) {
		return;
	}

	timeval t;
	vrpn_gettimeofday(&t, NULL);

	/*#ifdef PERFORMANCE_TEST
		gettimeofday(&tp, NULL);
		long int actualTime = tp.tv_usec / 1000;
		//fileOutput << (int)time(0) << "." << actualTime << " < \n";
		t.tv_sec = (long int)time(0);
		t.tv_usec = actualTime;
	#endif*/

	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
	
	int countSkeletons = 0;
	for ( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
		NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
		
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
				reportPose(sensor, t, skeletonFrame.SkeletonData[i].SkeletonPositions[y]);
			}
		}
		/*if ( NUI_SKELETON_TRACKED == trackingState ) {
			// We're tracking the skeleton, draw it
			reportPose(i,t,skeletonFrame.SkeletonData[i].Position);
			//DrawSkeleton(skeletonFrame.SkeletonData[i], width, height);
		} else if ( NUI_SKELETON_POSITION_ONLY == trackingState ) {
			// we've only received the center point of the skeleton, draw that
			/*D2D1_ELLIPSE ellipse = D2D1::Ellipse(
				SkeletonToScreen(skeletonFrame.SkeletonData[i].Position, width, height),
				g_JointThickness,
				g_JointThickness
			);
		}*/

	}

	if ( countSkeletons > lastSkeletonCount ) {
		printf("Kinect Capturando.\n");
		lastSkeletonCount = countSkeletons;
	} else if ( lastSkeletonCount != countSkeletons ){
		printf("Kinect Parado.\n");
		lastSkeletonCount = countSkeletons;
	}

}
