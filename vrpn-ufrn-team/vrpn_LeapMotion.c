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

	//Analog Code
	//primeira mao
	channel[0] = hands[0].grabAngle();
	channel[1] = hands[0].pinchDistance();
	//segunda mao
	channel[2] = hands[1].grabAngle();
	channel[3] = hands[1].pinchDistance();


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




/*
vrpn_LeapMotion::vrpn_LeapMotion(const char *name, vrpn_Connection *c)
	: vrpn_Analog(name, c) {

	


	controller.addListener(listener);
}




void vrpn_LeapMotion::parse_message_type_1(std::size_t bytes, vrpn_uint8 *buffer) {
	size_t num_reports = buffer[1];
	if ( num_reports > 3 ) {
		num_reports = 3;
	}

	// Skip past the report type and num_reports bytes and
	// start parsing there.
	vrpn_uint8 *bufptr = &buffer[2];

	// The next two bytes are an increasing counter that changes by 1 for
	// every report.
	vrpn_uint16 report_index =
		vrpn_unbuffer_from_little_endian<vrpn_uint16, vrpn_uint8>(bufptr);
	channel[1] = report_index;

	// The next two bytes are zero, so we skip them
	vrpn_uint16 skip =
		vrpn_unbuffer_from_little_endian<vrpn_uint16, vrpn_uint8>(bufptr);

	// The next entry is temperature, and it may be in hundredths of a degree C
	const double temperature_scale = 0.01;
	vrpn_uint16 temperature =
		vrpn_unbuffer_from_little_endian<vrpn_uint16, vrpn_uint8>(bufptr);
	channel[0] = temperature * temperature_scale;

	// The magnetometer data comes after the space to store three
	// reports.
	vrpn_uint8 *magnetometer_ptr = &buffer[56];
	vrpn_int16 magnetometer_raw[3];
	for ( size_t i = 0; i < 3; i++ ) {
		magnetometer_raw[i] =
			vrpn_unbuffer_from_little_endian<vrpn_int16, vrpn_uint8>(
				magnetometer_ptr);
	}
	// Invert all axes to make the magnetometer direction match
	// the sign of the gravity vector.
	const double magnetometer_scale = 0.0001;
	channel[8] = -magnetometer_raw[0] * magnetometer_scale;
	channel[9] = -magnetometer_raw[1] * magnetometer_scale;
	channel[10] = -magnetometer_raw[2] * magnetometer_scale;

	// Unpack a 16-byte accelerometer/gyro report using the routines from
	// Oliver's code.
	for ( size_t i = 0; i < num_reports; i++ ) {
		vrpn_int32 accelerometer_raw[3];
		vrpn_int32 gyroscope_raw[3];
		unpackVector(bufptr, accelerometer_raw);
		bufptr += 8;
		unpackVector(bufptr, gyroscope_raw);
		bufptr += 8;

		// Compute the double values using default calibration.
		// The accelerometer data goes into analogs 0,1,2.
		// The gyro data goes into analogs 3,4,5.
		// The magnetomoter data goes into analogs 6,7,8.
		const double accelerometer_scale = 0.0001;
		const double gyroscope_scale = 0.0001;
		channel[2] =
			static_cast<double>(accelerometer_raw[0]) * accelerometer_scale;
		channel[3] =
			static_cast<double>(accelerometer_raw[1]) * accelerometer_scale;
		channel[4] =
			static_cast<double>(accelerometer_raw[2]) * accelerometer_scale;

		channel[5] = static_cast<double>(gyroscope_raw[0]) * gyroscope_scale;
		channel[6] = static_cast<double>(gyroscope_raw[1]) * gyroscope_scale;
		channel[7] = static_cast<double>(gyroscope_raw[2]) * gyroscope_scale;

		vrpn_Analog::report_changes();
	}
}

void vrpn_LeapMotion::mainloop() {
	
	server_mainloop();
}

bool vrpn_LeapMotion::parse_message(std::size_t bytes, vrpn_uint8 *buffer) {
	return false;
}

void vrpn_LeapMotion::on_data_received(std::size_t bytes, vrpn_uint8 *buffer) {
	/*   // For debugging
	printf("Got %d bytes:\n", static_cast<int>(bytes));
	for (size_t i = 0; i < bytes; i++) {
	printf("%02X ", buffer[i]);
	}
	printf("\n");
	
	// Set the timestamp for all reports
	vrpn_gettimeofday(&d_timestamp, NULL);

	// Make sure the message length and type is what we expect.
	// We get 64-byte responses on Windows and 62-byte responses on the mac.
	if ( (bytes != 62) && (bytes != 64) ) {
		fprintf(stderr, "vrpn_LeapMotion::on_data_received(): Unexpected message "
				"length %d, ignoring\n",
				static_cast<int>(bytes));
		return;
	}

	switch ( buffer[0] ) {
	case 1:
		parse_message_type_1(bytes, buffer);
		break;
	default:
		// Delegate message type to child
		if ( !parse_message(bytes, buffer) ) {
			fprintf(stderr, "vrpn_LeapMotion::on_data_received(): Unexpected "
					"message type %d, ignoring\n",
					buffer[0]);
		}
		break;
	}
}
*/