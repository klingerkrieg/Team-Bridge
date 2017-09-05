#include <stdio.h>                      // for fprintf, stderr, printf, etc

#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"
#include "vrpn_Shared.h"                // for vrpn_SleepMsecs
#include "vrpn_Tracker.h"               // for vrpn_TRACKERCB, etc
#include "vrpn_Types.h"                 // for vrpn_float64
#include <vector>                // for vector
#include "Leap.h"


using namespace Leap;
using namespace std;

const char	*TRACKER_NAME = "Tracker0";
int	CONNECTION_PORT = vrpn_DEFAULT_LISTEN_PORT_NO;	// Port for connection to listen on
bool print = false;
vrpn_Tracker_Server	*ntkr;
vrpn_Tracker_Remote	*tkr;
vrpn_Connection		*connection;

class t_user_callback {
public:
	char t_name[vrpn_MAX_TEXT_LEN];
	vector<unsigned> t_counts;
};



double hand_pos[2][3];



/*****************************************************************************
*
Callback handlers
*
*****************************************************************************/

void VRPN_CALLBACK
handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	t_user_callback *t_data = static_cast<t_user_callback *>(userdata);

	// Make sure we have a count value for this sensor
	while (t_data->t_counts.size() <= static_cast<unsigned>(t.sensor)) {
		t_data->t_counts.push_back(0);
	}

	double pos[3];

	if (t.sensor == 0 || t.sensor == 23) {
		int i = 0;
		if (t.sensor == 23) {
			i = 1;
		}
		hand_pos[i][0] = t.pos[0];
		hand_pos[i][1] = t.pos[1];
		hand_pos[i][2] = t.pos[2];

		pos[0] = t.pos[0];
		pos[1] = t.pos[1];
		pos[2] = t.pos[2];
	} else {

		//conversao para manter as juntas relativas a palma 0 e 23
		int i = 0;
		if (t.sensor >= 23) {
			i = 1;
		}
		pos[0] = t.pos[0] - hand_pos[i][0];
		pos[1] = t.pos[1] - hand_pos[i][1];
		pos[2] = t.pos[2] - hand_pos[i][2];
	}
	/*
			0
		-5		5


			60
		55		65		*/
	
		
	//if (t.sensor == 6) {


		//printf("orig:%.2f\tconv:%.2f\thand:%.2f\n", t.pos[0], pos[0], hand_pos[0][0]);

		printf("sensor\t%d\tpos\t%.2f\t%.2f\t%.2f\tquat\t%.2f\t%.2f\t%.2f\t%.2f\n",
			t.sensor, pos[0], pos[1], pos[2],
			t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
	//}

}


Vector toQuat(Vector position) {
	//Vector v_pos = Vector(0.0, 0.0, 0.0);
	//return v_pos;

	float scale = 20.0;
	Vector v_pos = Vector(position.x, position.y, -position.z) / scale;
	Vector v_dir = Vector(position.x, position.y, -position.z).normalized();
	Vector ez = Vector(0.0, 0.0, -1.0);
	float angle = ez.angleTo(v_dir);

	Vector axis = ez.cross(v_dir).normalized();
	return axis;
}


void reportPose(int sensor, timeval t, Vector position, Vector quaternion) {
	double pos1 = (double)position[0];
	double pos2 = (double)position[1];
	double pos3 = (double)position[2];


	double quat1 = (double)quaternion[0];
	double quat2 = (double)quaternion[1];
	double quat3 = (double)quaternion[2];
	double quat4 = (double)quaternion[3];


	vrpn_float64 positions2[3] = { pos1, pos2, pos3 };
	vrpn_float64 quaternion2[4] = { quat1, quat2, quat3, quat4 };

	ntkr->report_pose(sensor, t, positions2, quaternion2, vrpn_CONNECTION_LOW_LATENCY);
}


void mainloop() {
	ntkr->mainloop();
	if (print) {
		tkr->mainloop();
	}
	connection->mainloop();
}

class SampleListener : public Listener {
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};

void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();

	timeval t;
	t.tv_sec = 1;
	t.tv_usec = t.tv_sec;

	HandList hands = frame.hands();
	Leap::FingerList fingers;

	Leap::Vector position;
	Leap::Vector quaternion;

	int sensor = 0;


	//limita para pegar somente 2 maos
	int handCount = hands.count();
	if (handCount > 2) {
		handCount = 1;
	}

	for (int i = 0; i < handCount; i++) {


		//0 e 23 sao as palmas
		position = hands[i].palmPosition();
		quaternion = toQuat(position);
		reportPose(sensor++, t, position, quaternion);
		mainloop();

		//1 e 24 sao os cotovelos
		position = hands[i].arm().elbowPosition();
		quaternion = toQuat(position);
		reportPose(sensor++, t, position, quaternion);

		mainloop();

		//wrist(punho) da mao tem o mesmo valor que o do braco
		position = hands[i].arm().wristPosition();
		quaternion = toQuat(position);
		reportPose(sensor++, t, position, quaternion);

		mainloop();


		fingers = hands[i].fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++) {
			Leap::Bone bone;
			Leap::Bone::Type boneType;

			for (int b = 0; b < 4; b++) {
				boneType = static_cast<Leap::Bone::Type>(b);
				bone = (*fl).bone(boneType);

				position = bone.nextJoint();
				quaternion = toQuat(position);
				reportPose(sensor++, t, position, quaternion);

				//Caso seja falange pega a nextJoint
				/*if (boneType == Bone::Type::TYPE_DISTAL) {
					position = bone.nextJoint();
					quaternion = toQuat(position);
					reportPose(sensor++, t, position, quaternion);
				}*/
				
				mainloop();

				// Sleep for 1ms so we don't eat the CPU
				vrpn_SleepMsecs(1);

			}
		}

	}

}



int main(int argc, char * argv[]) {

	printf("default port: %d \n", CONNECTION_PORT);
	if (argc != 2) {
		fprintf(stderr, "Usage: %s\n\n \np - Print tracking\nany - no print", argv[0]);
		return -1;
	}

	if (argv[1][0] == 'p') {
		print = true;
	}

	// explicitly open the connection
	connection = vrpn_create_server_connection(CONNECTION_PORT);

	// Open the tracker server, using this connection, 2 sensors, update 60 times/sec
	ntkr = new vrpn_Tracker_Server(TRACKER_NAME, connection, 46);

	// Open the tracker remote using this connection
	tkr = new vrpn_Tracker_Remote(TRACKER_NAME, connection);
	t_user_callback *tc1 = new t_user_callback;
	strncpy(tc1->t_name, TRACKER_NAME, sizeof(tc1->t_name));

	fprintf(stderr, "Tracker's name is %s.\n", TRACKER_NAME);

	// Set up the tracker callback handlers
	if (print) {
		tkr->register_change_handler(tc1, handle_tracker_pos_quat);
	}

	SampleListener listener;
	Controller controller;

	controller.addListener(listener);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	// Remove the sample listener when done
	controller.removeListener(listener);

	return 0;

}   /* main */


