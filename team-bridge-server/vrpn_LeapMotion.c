/** @file	vrpn_LeapMotion.C
@brief	Drivers for LeapMotion VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/


#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "vrpn_LeapMotion.h"
#include "Leap.h"

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc


VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()

//faz os dedos ficarem relativos a posicao da mao
bool relative_to_hand = 0;
double hand_pos[3];

Leap::Controller vrpn_LeapMotion::controller;

vrpn_LeapMotion::vrpn_LeapMotion(const char *name, int hand, vrpn_Connection *c)
	: vrpn_Analog(name, c),
	  vrpn_Tracker(name, c) {
	
	this->handId = hand;
	
	controller.addListener(*this);
	
	vrpn_Analog::num_channel = 2;
	vrpn_Tracker::num_sensors = 23;

	memset(channel, 0, sizeof(channel));
	memset(last, 0, sizeof(last));

	vrpn_gettimeofday(&_timestamp, NULL);
}

void vrpn_LeapMotion::mainloop() {

	server_mainloop();
}

vrpn_LeapMotion::~vrpn_LeapMotion() {
	controller.removeListener(*this);
}


void vrpn_LeapMotion::onConnect(const Leap::Controller& controller) {
	std::cout << this->d_servicename << " connected." << std::endl;
}

void vrpn_LeapMotion::reportPose(int sensor, Leap::Vector position) {
	double pos1 = (double)position[0];
	double pos2 = (double)position[1];
	double pos3 = (double)position[2];



	if ( relative_to_hand ) {
		if ( sensor == 0) {
			hand_pos[0] = pos1;
			hand_pos[1] = pos2;
			hand_pos[2] = pos3;
		} else {
			//conversao para manter as juntas relativas a palma 0
			pos1 = pos1 - hand_pos[0];
			pos2 = pos2 - hand_pos[1];
			pos3 = pos3 - hand_pos[2];
		}
	}


	//Seta dados para envio
	d_sensor = sensor;
	pos[0] = pos1;
	pos[1] = pos2;
	pos[2] = pos3;

	d_quat[0] = 0;
	d_quat[1] = 0;
	d_quat[2] = 0;
	d_quat[3] = 0;


	timeval t;
	vrpn_gettimeofday(&t, NULL);

	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
		
	if ( d_connection->pack_message(len, t, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
		
}



void vrpn_LeapMotion::onFrame(const Leap::Controller& controller) {

	const Leap::Frame frame = controller.frame();

	Leap::HandList hands = frame.hands();

	int handCount = hands.count();
	//Se a mão desejada não estiver presente
	if ( handCount < handId + 1 ) {
		return;
	}

	channel[0] = hands[handId].grabAngle();
	channel[1] = hands[handId].pinchDistance();

	vrpn_Analog::report();


	//Tracker Code
	Leap::FingerList fingers;

	Leap::Vector position;
	Leap::Vector quaternion;

	//Não importa qual é a mão, sempre será sensor 0 na palma
	int sensor = 0;

	//0 sao as palmas
	position = hands[handId].palmPosition();
	reportPose(sensor++, position);

	//1 sao os cotovelos
	position = hands[handId].arm().elbowPosition();
	reportPose(sensor++, position);

	//wrist(punho) da mao tem o mesmo valor que o do braco
	position = hands[handId].arm().wristPosition();
	reportPose(sensor++, position);

	fingers = hands[handId].fingers();
	for ( Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++ ) {
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		for ( int b = 0; b < 4; b++ ) {
			boneType = static_cast<Leap::Bone::Type>(b);
			bone = (*fl).bone(boneType);

			position = bone.nextJoint();
			reportPose(sensor++, position);

			// Sleep for 1ms so we don't eat the CPU
			vrpn_SleepMsecs(1);
		}
	}

	

}
