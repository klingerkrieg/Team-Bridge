/** @file	vrpn_KinectV2.C
@brief	Drivers for KinectV1 VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

#include "vrpn_KinectV2.h"


IKinectSensor*          vrpn_KinectV2::m_pKinectSensor;
ICoordinateMapper*      vrpn_KinectV2::m_pCoordinateMapper;
IBodyFrameReader*       vrpn_KinectV2::m_pBodyFrameReader;
HRESULT					vrpn_KinectV2::hr;
bool vrpn_KinectV2::connected = false;
bool vrpn_KinectV2::status = false;
bool vrpn_KinectV2::skeletonArr[BODY_COUNT];
int vrpn_KinectV2::skeletonIds[BODY_COUNT] = { -1,-1,-1,-1,-1,-1 };

VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()


vrpn_KinectV2::vrpn_KinectV2(const char *name, int skeleton , vrpn_Connection *c) : vrpn_Tracker(name, c), vrpn_Analog(name, c) {
	vrpn_Analog::num_channel = 2;
	vrpn_Tracker::num_sensors = 25;
	this->skeleton = skeleton;
	connect();
}

void vrpn_KinectV2::mainloop() {

	if ( connected ) {
		if ( !onFrame() ) {
			printf("Perda de conexao com o Kinect.\n");
			connected = false;
			connect();
		}
	}
	server_mainloop();
}

vrpn_KinectV2::~vrpn_KinectV2() {
	// done with body frame reader
	SafeRelease(m_pBodyFrameReader);

	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if ( m_pKinectSensor ) {
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}





bool vrpn_KinectV2::connect() {

	while ( connected == false ) {
		hr = GetDefaultKinectSensor(&m_pKinectSensor);
		
		if ( FAILED(hr) ) {
			printf("Nenhum KinectV2 encontrado.\n");
			Sleep(3000);
			continue;
		}

		if ( m_pKinectSensor ) {
			// Initialize the Kinect and get coordinate mapper and the body reader
			IBodyFrameSource* pBodyFrameSource = NULL;
			
			hr = m_pKinectSensor->Open();
			//Espera iniciar
			Sleep(3000);
			if ( SUCCEEDED(hr) ) {
				hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
			}

			if ( SUCCEEDED(hr) ) {
				hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
			}

			if ( SUCCEEDED(hr) ) {
				hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
			}

			SafeRelease(pBodyFrameSource);
		}

		if ( !m_pKinectSensor || FAILED(hr) ) {
			printf("Falha ao se conectar ao Kinect.\n");
			Sleep(3000);
			continue;
		}

		BOOLEAN available = false;
		m_pKinectSensor->get_IsAvailable(&available);
		if ( available == false ) {
			printf("Kinect indisponivel.\n");
			Sleep(3000);
			continue;
		}

		connected = true;
		printf("Kinect conectado.\n");
	}

	return true;
}

void vrpn_KinectV2::reportPose(int sensor, CameraSpacePoint position, int quat) {
	//Seta dados para envio
	d_sensor = sensor;
	pos[0] = position.X;
	pos[1] = position.Y;
	pos[2] = position.Z;

	d_quat[0] = quat;
	d_quat[1] = quat;
	d_quat[2] = quat;
	d_quat[3] = quat;


	timeval t;
	vrpn_gettimeofday(&t, NULL);
	/*if ( sensor == 0 ) {
		printf("Y:%.2f Z:%.2f\n",position.Y, position.Z);
	}*/
	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf,
		vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}


bool vrpn_KinectV2::setKinectSkeletonId(IBody* ppBodies[BODY_COUNT]) {
	bool inUse = false;
	IBody* pBody;
	//Encontra os skeletons na ordem em que aparecem, o primeiro sempre sera 0
	for ( int y = 0; y < BODY_COUNT; y++ ) {

		for ( int x = 0; x < BODY_COUNT; x++ ) {
			if ( skeletonIds[x] == y ) {
				inUse = true;
				break;
			}
		}
		if ( inUse ) {
			continue;
		}


		pBody = ppBodies[y];
		BOOLEAN bTrackedFound = false;
		hr = pBody->get_IsTracked(&bTrackedFound);
		if ( bTrackedFound == (BOOLEAN)true ) {
			//Encontra o primeiro skeletonId livre
			for ( int x = 0; x < BODY_COUNT; x++ ) {
				if ( skeletonIds[x] == -1 ) {
					skeletonIds[x] = y;//insere o kinectId correto
					return true;
				}
			}
		}
	}
	return false;
}

bool vrpn_KinectV2::onFrame() {


	int countSkeletons = 0;

	BOOLEAN available = false;
	m_pKinectSensor->get_IsAvailable(&available);

	if ( !m_pBodyFrameReader || available == false ) {
		return false;
	}


	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if ( SUCCEEDED(hr) ) {
		
		IBody* ppBodies[BODY_COUNT] = { 0 };

		hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		

		if ( SUCCEEDED(hr) ) {
			

			if ( SUCCEEDED(hr) && m_pCoordinateMapper ) {
				IBody* pBody;

				//Quero deixar aqui a minha indigna��o, todo esse c�digo � porque o Kinect V2 n�o controla quem s�o seus skeletons
				//A primeira pessoa que aparece pode receber qualquer id, depende da vontade do Kinect, 0, 2, 5 tanto faz
				//Ent�o tive primeiro que identificar quem � que o kinect t� vendo, pra depois seguir a vida.

				int id = skeletonIds[skeleton];
				//Caso n�o tenha sido encontrado, verifica se h� algum sendo exibido em outra id
				if ( id == -1 ) {
					//ja atualiza o skeletonIds automaticamente
					setKinectSkeletonId(ppBodies);
				}

				id = skeletonIds[skeleton];
				//verifica se encontrou algo
				if ( id != -1 ) {

					//Confirma se ainda est� ativo
					BOOLEAN bTracked = false;
					pBody = ppBodies[id];
					hr = pBody->get_IsTracked(&bTracked);
					if ( !bTracked ) {
						//Se n�o estiver ativo, libera a posi��o no array
						skeletonIds[skeleton] = -1;

					} else {
						//Se estiver ativo

						pBody = ppBodies[id];

						countSkeletons++;
						Joint joints[JointType_Count];

						HandState leftHandState = HandState_Unknown;
						HandState rightHandState = HandState_Unknown;

						pBody->get_HandLeftState(&leftHandState);
						pBody->get_HandRightState(&rightHandState);

						//Status das m�os
						channel[0] = leftHandState;
						channel[1] = rightHandState;
						vrpn_Analog::report();


						hr = pBody->GetJoints(_countof(joints), joints);
						if ( SUCCEEDED(hr) ) {
							countSkeletons++;

							for ( int j = 0; j < _countof(joints); ++j ) {
								int sensor = j;

								//Adapta��o para manter semelhan�a com os dados enviados pelo FAAST
								switch ( j ) {
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
										sensor = j;
										break;
								}

								reportPose(sensor, joints[j].Position, 0);
							}//for
						}//if SUCCEEDED
					}//else
				}//id != -1
			}//if coordinate
				
		} //if hr GetAndRefreshBodyData
			

		//Informar se esta capturando alguem
		if ( countSkeletons > 0 ) {
			skeletonArr[skeleton] = true;
		} else
		if ( countSkeletons == 0 ) {
			skeletonArr[skeleton] = false;
		}

		int has = 0;
		for ( int i = 0; i < BODY_COUNT; i++ ) {
			has += skeletonArr[i];
		}
	
		if ( has == 0 && status == true ) {
			printf("Kinect parado.\n");
			status = false;
		} else
		if ( has > 0 && status == false ) {
			printf("Kinect capturando.\n");
			status = true;
		}

		for ( int i = 0; i < _countof(ppBodies); ++i ) {
			SafeRelease(ppBodies[i]);
		}

	}//if hr AcquireLatestFrame
	
	SafeRelease(pBodyFrame);
	
	return true;
}
