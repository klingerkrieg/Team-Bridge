/** @file	vrpn_KinectV1.C
@brief	Drivers for KinectV1 VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

#include "vrpn_KinectV1.h"


VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()

INuiSensor * vrpn_KinectV1::pNuiSensor;
INuiSensor* vrpn_KinectV1::m_pNuiSensor;
HANDLE      vrpn_KinectV1::m_pSkeletonStreamHandle;
HANDLE      vrpn_KinectV1::m_hNextSkeletonEvent;
int vrpn_KinectV1::iSensorCount;
HRESULT vrpn_KinectV1::hr;
bool vrpn_KinectV1::connected = false;
bool vrpn_KinectV1::status = false;
bool vrpn_KinectV1::skeletonArr[NUI_SKELETON_COUNT];

vrpn_KinectV1::vrpn_KinectV1(const char *name, int skeleton, vrpn_Connection *c) : vrpn_Tracker(name, c) {
	vrpn_Tracker::num_sensors = 20;
	this->skeleton = skeleton;
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
			printf("Nenhum KinectV1 encontrado.\n");
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
				//Sentado
				//hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);
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

void vrpn_KinectV1::reportPose(int skeleton,int sensor, Vector4 position, Vector4 quat) {
	//Seta dados para envio
	d_sensor = sensor;
	pos[0] = position.x;
	pos[1] = position.y;
	pos[2] = position.z;
	
	d_quat[0] = quat.x;
	d_quat[1] = quat.y;
	d_quat[2] = quat.z;
	d_quat[3] = quat.w;

	timeval t;
	vrpn_gettimeofday(&t, NULL);
	
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

	

	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
	
	
	NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];


	if ( skeleton > NUI_SKELETON_COUNT ) {
		printf("Skeleton id maior do que o permitido: %d\n", skeleton);
		return true;
	}
		


	NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[skeleton].eTrackingState;
	NuiSkeletonCalculateBoneOrientations(&skeletonFrame.SkeletonData[skeleton], boneOrientations);
		
	int countSkeletons = 0;
	int sensor = 0;
	if ( NUI_SKELETON_TRACKED == trackingState ) {
		countSkeletons++;
		for ( int h = 0; h < 20; h++ ) {
			//Essa adaptação é necessária para enviar os mesmos pontos do FAAST
			switch ( h ) {
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
					sensor = h;
				break;
			}

			NUI_SKELETON_BONE_ORIENTATION & orientation = boneOrientations[h];

			reportPose(countSkeletons, sensor, skeletonFrame.SkeletonData[skeleton].SkeletonPositions[h],
						orientation.absoluteRotation.rotationQuaternion);
		}
	}

	
	if ( countSkeletons > 0) {
		skeletonArr[skeleton] = true;
	} else
	if ( countSkeletons == 0 ) {
		skeletonArr[skeleton] = false;
	}

	int has = 0;
	for ( int i = 0; i < NUI_SKELETON_COUNT; i++ ) {
		has += skeletonArr[i];
	}

	if ( has == 0 && status == true) {
		printf("Kinect parado.\n");
		status = false;
	} else 
	if ( has > 0 && status == false){
		printf("Kinect capturando.\n");
		status = true;
	}

	return true;
}
