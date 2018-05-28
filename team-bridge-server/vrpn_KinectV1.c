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
int  vrpn_KinectV1::skeletonIds[NUI_SKELETON_COUNT] = { -1,-1,-1,-1,-1,-1 };

vrpn_KinectV1::vrpn_KinectV1(const char *name, int skeleton, vrpn_Connection *c) : vrpn_Tracker(name, c) {
	vrpn_Tracker::num_sensors = 20;
	this->skeleton = skeleton;
	connect();
	MainView::startDeviceView(MAIN_VIEW_KINECT_V1);
}

void vrpn_KinectV1::mainloop() {
	if ( connected ) {
		if ( !onFrame() ) {
			MainView::writeln("Perda de conexao com o KinectV1.");
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
	
	while (connected == false && tentativas < TENTATIVAS_MAX) {
		tentativas++;

		MainView::writeln("Conectando-se ao KinectV1...");

		iSensorCount = 0;
		hr = NuiGetSensorCount(&iSensorCount);
		if ( FAILED(hr) || iSensorCount == 0 ) {
			MainView::writeln("Nenhum KinectV1 encontrado.\n");
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
			MainView::writeln("Falha ao se conectar ao KinectV1.\n");
			Sleep(3000);
			continue;
		}

		
		connected = true;
		MainView::writeln("KinectV1 conectado.\n");
		tentativas = 0;

		return true;
	}
	
	return false;
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
		MainView::writeln("vrpn_KinectV1: cannot write message: tossing");
	}
}


void vrpn_KinectV1::setKinectSkeletonId(NUI_SKELETON_DATA ppBodies[]) {
	bool inUse = false;
	NUI_SKELETON_DATA pBody;

	//Encontra os skeletons na ordem em que aparecem, o primeiro sempre sera 0
	//Talvez trocar esse for externo do y pelo int skeleton
	for ( int y = 0; y < NUI_SKELETON_COUNT; y++ ) {

		for ( int x = 0; x < NUI_SKELETON_COUNT; x++ ) {
			if ( skeletonIds[x] == y ) {
				inUse = true;
				break;
			}
		}
		if ( inUse ) {
			continue;
		}
		
		pBody = ppBodies[y];
		if ( pBody.eTrackingState == NUI_SKELETON_TRACKED ) {
			//Encontra o primeiro skeletonId livre
			for ( int x = 0; x < NUI_SKELETON_COUNT; x++ ) {
				if ( skeletonIds[x] == -1 ) {
					skeletonIds[x] = y;//insere o kinectId correto
					return;
				}
			}
		}
	}
	return;
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
		//printf("2\n");
		return true;//Mesmo tendo essa falha ainda pode estar conectado
	}


	// smooth out the skeleton data
	m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
	
	
	NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];


	if ( skeleton > NUI_SKELETON_COUNT ) {
		MainView::writeln("Skeleton id maior do que o permitido:");
		MainView::write(std::to_string(skeleton));
		return true;
	}
		
	

	int id = skeletonIds[skeleton];
	//Caso não tenha sido encontrado, verifica se há algum sendo exibido em outra id
	if ( id == -1 ) {
		//ja atualiza o skeletonIds automaticamente
		setKinectSkeletonId(skeletonFrame.SkeletonData);
	}

	id = skeletonIds[skeleton];

	//verifica se encontrou algo
	if ( id != -1 ) {

		NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[id].eTrackingState;
		NuiSkeletonCalculateBoneOrientations(&skeletonFrame.SkeletonData[id], boneOrientations);

		int countSkeletons = 0;
		int sensor = 0;
		if ( NUI_SKELETON_TRACKED != trackingState ) {
			//Se não estiver ativo, libera a posição no array
			skeletonIds[skeleton] = -1;
		} else {

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

				reportPose(countSkeletons, sensor, skeletonFrame.SkeletonData[id].SkeletonPositions[h],
						   orientation.absoluteRotation.rotationQuaternion);
			}
		}



		if ( countSkeletons > 0 ) {
			skeletonArr[id] = true;
		} else
		if ( countSkeletons == 0 ) {
			skeletonArr[id] = false;
		}
	}

	int has = 0;
	for ( int i = 0; i < NUI_SKELETON_COUNT; i++ ) {
		has += skeletonArr[i];
	}

	if ( has == 0 && status == true) {
		MainView::writeln("KinectV1 parado.");
		status = false;
	} else 
	if ( has > 0 && status == false){
		MainView::writeln("KinectV1 capturando.");
		status = true;
	}

	return true;
}
