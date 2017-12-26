/** @file	vrpn_KinectV2.C
@brief	Drivers for KinectV1 VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/


#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "vrpn_KinectV2.h"

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc

#include <math.h>

VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()


vrpn_KinectV2::vrpn_KinectV2(const char *name, vrpn_Connection *c) : vrpn_Tracker(name, c) {
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

	//printf("sensor:%d %.2f %.2f %.2f\n", sensor, pos[0], pos[1], pos[2]);

	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	//if (sensor == 0 )
	//	printf("sensor: %d %.2f %.2f %.2f \n", sensor, position.x, position.y, position.z);
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf,
		vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}

bool vrpn_KinectV2::onFrame() {

	bool ret = true;
	int countSkeletons = -1;

	BOOLEAN available = false;
	m_pKinectSensor->get_IsAvailable(&available);

	if ( !m_pBodyFrameReader || available == false ) {
		return false;
	}


	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if ( SUCCEEDED(hr) ) {
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if ( SUCCEEDED(hr) ) {
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if ( SUCCEEDED(hr) ) {
			//ProcessBody(nTime, BODY_COUNT, ppBodies);

			if ( SUCCEEDED(hr) && m_pCoordinateMapper ) {
				
				countSkeletons = 0;
				for ( int i = 0; i < BODY_COUNT; ++i ) {
					IBody* pBody = ppBodies[i];

					if ( pBody ) {
						BOOLEAN bTracked = false;
						hr = pBody->get_IsTracked(&bTracked);
						
						if ( SUCCEEDED(hr) && bTracked ) {
							
							countSkeletons++;
							Joint joints[JointType_Count];
							
							HandState leftHandState = HandState_Unknown;
							HandState rightHandState = HandState_Unknown;

							pBody->get_HandLeftState(&leftHandState);
							pBody->get_HandRightState(&rightHandState);

							hr = pBody->GetJoints(_countof(joints), joints);
							if ( SUCCEEDED(hr) ) {
								
								for ( int j = 0; j < _countof(joints); ++j ) {
									int sensor = j;

									//Adaptação para manter semelhança com os dados enviados pelo FAAST
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
							}//if success
						}//if tracked
					}// if pBody
				}// for body
				ret = true;
			} //if coordinate
			
		}

		for ( int i = 0; i < _countof(ppBodies); ++i ) {
			SafeRelease(ppBodies[i]);
		}

	}
	SafeRelease(pBodyFrame);
	

	//printf("%d %d\n", countSkeletons, lastSkeletonCount);
	if ( countSkeletons != -1 ) {
		if ( countSkeletons > 0 && lastSkeletonCount != countSkeletons ) {
			printf("Kinect Capturando.\n");
			lastSkeletonCount = countSkeletons;
		} else if ( countSkeletons == 0 && lastSkeletonCount != 0 ) {
			printf("Kinect Parado.\n");
			lastSkeletonCount = 0;
		}
	}
	

	return ret;
	


	//Verifica conexao
	/*hr = m_pNuiSensor->NuiStatus();
	if ( S_OK != hr ) {
	return false;
	}*/



	/*
	NUI_SKELETON_FRAME skeletonFrame = { 0 };

	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if ( FAILED(hr) ) {
		//printf("Falha ao capturar frame.\n");
		return true;//Mesmo tendo essa falha ainda pode estar conectado
	}



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


				reportPose(sensor, skeletonFrame.SkeletonData[i].SkeletonPositions[h],
						   orientation.absoluteRotation.rotationQuaternion);
			}
		}

	}

	if ( countSkeletons > lastSkeletonCount ) {
		printf("Kinect Capturando.\n");
		lastSkeletonCount = countSkeletons;
	} else if ( lastSkeletonCount != countSkeletons ) {
		printf("Kinect Parado.\n");
		lastSkeletonCount = countSkeletons;
	}

	return true;*/
}
