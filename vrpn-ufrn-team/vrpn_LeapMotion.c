/** @file	vrpn_LeapMotion.C
@brief	Drivers for LeapMotion VR devices.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

// Based on the vrpn_Oculus driver

#include "vrpn_Tracker.h"               // for vrpn_Tracker
#include "vrpn_LeapMotion.h"
#include "Leap.h"

#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc

VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()



//faz os dedos ficarem relativos a posicao da mao
bool relative_to_hand = 0;
double hand_pos[2][3];

 

vrpn_LeapMotion::vrpn_LeapMotion(const char *name, vrpn_Connection *c)
	: vrpn_Analog(name, c),
	  vrpn_Tracker(name, c) {

	controller.addListener(*this);

	vrpn_Analog::num_channel = 4;
	vrpn_Tracker::num_sensors = 46;

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
	std::cout << "Connected" << std::endl;
}

void vrpn_LeapMotion::reportPose(int sensor, timeval t, Leap::Vector position) {
	double pos1 = (double)position[0];
	double pos2 = (double)position[1];
	double pos3 = (double)position[2];



	if ( relative_to_hand ) {

		if ( sensor == 0 || sensor == 23 ) {
			int i = 0;
			if ( sensor == 23 ) {
				i = 1;
			}
			hand_pos[i][0] = pos1;
			hand_pos[i][1] = pos2;
			hand_pos[i][2] = pos3;
		} else {

			//conversao para manter as juntas relativas a palma 0 e 23
			int i = 0;
			if ( sensor >= 23 ) {
				i = 1;
			}
			pos1 = pos1 - hand_pos[i][0];
			pos2 = pos2 - hand_pos[i][1];
			pos3 = pos3 - hand_pos[i][2];
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


	char msgbuf[512];
	int len = vrpn_Tracker::encode_to(msgbuf);
	if ( d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, msgbuf,
									vrpn_CONNECTION_LOW_LATENCY) ) {
		fprintf(stderr, "vrpn_LeapMotion: cannot write message: tossing\n");
	}
}



void vrpn_LeapMotion::onFrame(const Leap::Controller& controller) {
	const Leap::Frame frame = controller.frame();
	

	timeval t;
	vrpn_gettimeofday(&t, NULL);


	Leap::HandList hands = frame.hands();

	if ( hands.count() == 0 ) {
		return;
	}
	
	channel[0] = 0;
	channel[1] = 0;
	//segunda mao
	channel[2] = 0;
	channel[3] = 0;
	bool left = false;
	bool right = false;

	for ( int i = 0; i < hands.count(); i++ ) {

		if ( hands[i].isLeft() && left == false ) {
			//mao esquerda
			channel[0] = hands[0].grabAngle();
			channel[1] = hands[0].pinchDistance();
			left = true;
		}
		if ( hands[i].isRight() && right == false ) {
			//segunda mao
			channel[2] = hands[1].grabAngle();
			channel[3] = hands[1].pinchDistance();
			right = true;
		}
	}


	vrpn_Analog::report_changes();


	//Tracker Code
	Leap::FingerList fingers;

	Leap::Vector position;
	Leap::Vector quaternion;

	int sensor = 0;


	//limita para pegar somente 2 maos
	int handCount = hands.count();
	if ( handCount > 2 ) {
		handCount = 1;
	}

	for ( int i = 0; i < handCount; i++ ) {


		//0 e 23 sao as palmas
		position = hands[i].palmPosition();
		reportPose(sensor++, t, position);

		//1 e 24 sao os cotovelos
		position = hands[i].arm().elbowPosition();
		reportPose(sensor++, t, position);


		//wrist(punho) da mao tem o mesmo valor que o do braco
		position = hands[i].arm().wristPosition();
		reportPose(sensor++, t, position);



		fingers = hands[i].fingers();
		for ( Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++ ) {
			Leap::Bone bone;
			Leap::Bone::Type boneType;

			for ( int b = 0; b < 4; b++ ) {
				boneType = static_cast<Leap::Bone::Type>(b);
				bone = (*fl).bone(boneType);

				position = bone.nextJoint();
				reportPose(sensor++, t, position);


				// Sleep for 1ms so we don't eat the CPU
				vrpn_SleepMsecs(1);

			}
		}

	}

}
