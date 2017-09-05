#include <stdio.h>                      // for fprintf, stderr, printf, etc

#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"
#include "vrpn_Shared.h"                // for vrpn_SleepMsecs
#include "vrpn_Tracker.h"               // for vrpn_TRACKERCB, etc
#include "vrpn_analog.h"
#include "vrpn_Types.h"                 // for vrpn_float64
#include <vector>                // for vector
#include "Leap.h"

#include <vrpn_Analog.h>
#include "vrpn_Analog_Output.h"



using namespace Leap;
using namespace std;

const char	*TRACKER_NAME = "Tracker0";
const char	*ANALOG_NAME = "LeapMotion0";
int	CONNECTION_PORT = vrpn_DEFAULT_LISTEN_PORT_NO;	// Port for connection to listen on
bool printTracker = false;
bool printAnalog = false;


vrpn_Analog_Server	*analogServer;
vrpn_Analog_Remote	*analogRem;

vrpn_Tracker_Server	*trackerServer;
vrpn_Tracker_Remote	*trackerRem;
vrpn_Connection		*connection;

class tracker_user_callback {
	public:
		char t_name[vrpn_MAX_TEXT_LEN];
		vector<unsigned> t_counts;
};


//faz os dedos ficarem relativos a posicao da mao
double hand_pos[2][3];


/*****************************************************************************
*
Callback handlers
*
*****************************************************************************/

void VRPN_CALLBACK
handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	tracker_user_callback *t_data = static_cast<tracker_user_callback *>(userdata);

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
	
	printf("sensor\t%d\tpos\t%.2f\t%.2f\t%.2f\tquat\t%.2f\t%.2f\t%.2f\t%.2f\n",
		   t.sensor, pos[0], pos[1], pos[2],
		   t.quat[0], t.quat[1], t.quat[2], t.quat[3]);

}

void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a) {
	int i;
	const char *name = (const char *)userdata;

	printf("Analog %s:\n         %5.2f", name, a.channel[0]);
	for ( i = 1; i < a.num_channel; i++ ) {
		printf(", %5.2f", a.channel[i]);
	}
	printf(" (%d chans)\n", a.num_channel);
}

//Apenas um vetor com 4 0.0f para poder enviar essa informacao nao e importante para o trabalho
Vector toQuat(Vector position) {
	Vector v_pos = Vector(0.0, 0.0, 0.0);
	return v_pos;
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

	trackerServer->report_pose(sensor, t, positions2, quaternion2, vrpn_CONNECTION_LOW_LATENCY);
}


void mainloop() {
	trackerServer->mainloop();
	if (printTracker) {
		trackerRem->mainloop();
	}
	connection->mainloop();
}

class LeapListener : public Listener {
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};

void LeapListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();
	//Consertar o time
	timeval t;
	vrpn_gettimeofday(&t, NULL);


	HandList hands = frame.hands();


	if ( hands.count() == 0 ) {
		return;
	}


	//Analog Code
	//primeira mao
	analogServer->channels()[0] = hands[0].grabAngle();
	analogServer->channels()[1] = hands[0].pinchDistance();
	//segunda mao
	analogServer->channels()[2] = hands[1].grabAngle();
	analogServer->channels()[3] = hands[1].pinchDistance();


	analogServer->report();
	analogServer->mainloop();
	analogRem->mainloop();


	//Tracker Code
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

				
				mainloop();

				// Sleep for 1ms so we don't eat the CPU
				vrpn_SleepMsecs(1);

			}
		}

	}

}

/*
caso seja necessario um dia
#include <math.h> //hypot
double groupData[6][7];

void preencheData(int sensor,Vector pos, Vector rot) {
groupData[sensor][0] = pos[0] - groupData[0][0];
groupData[sensor][1] = pos[1] - groupData[0][1];
groupData[sensor][2] = pos[2] - groupData[0][2];
groupData[sensor][3] = rot[0];
groupData[sensor][4] = rot[1];
groupData[sensor][5] = rot[2];
groupData[sensor][6] = rot[3];
}

void pegarDistanciaEntreDoisPontos(Hand hand) {
	preencheData(0, hand.palmPosition(),
				 hand.direction());


	FingerList fingers = hand.fingers();
	int u = 1;
	for ( Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); fl++ ) {
		Leap::Bone bone;
		Leap::Bone::Type boneType;

		bone = (*fl).bone(Bone::Type::TYPE_DISTAL);
		preencheData(u++, bone.nextJoint(), bone.direction());
	}


	double dist[3];
	dist[0] = double(groupData[1][0] - groupData[2][0]) * (groupData[1][0] - groupData[2][0]);
	dist[1] = double(groupData[1][1] - groupData[2][1]) * (groupData[1][1] - groupData[2][1]);
	dist[2] = double(groupData[1][2] - groupData[2][2]) * (groupData[1][2] - groupData[2][2]);


	//printf("%.2f\t-\t%.2f\t-\t%.2f\n", dist[0], dist[1], dist[2]);
	double distance = hypot(hypot(groupData[1][0] - groupData[2][0],
								  groupData[1][1] - groupData[2][1]),
							groupData[1][2] - groupData[2][2]);

	printf("%.2f\n", distance);
}



*/


int main(int argc, char * argv[]) {

	printf("default port: %d \n", CONNECTION_PORT);
	
	fprintf(stderr, "Usage: %s \nt - Print tracking\na - print analog", argv[0]);
		

	for ( int i = 1; i < argc; i++ ) {
		if ( tolower(argv[i][0]) == 't' ) {
			printTracker = true;
		} else
		if ( tolower(argv[i][0]) == 'a' ) {
			printAnalog = true;
		}
	}

	

	// explicitly open the connection
	connection = vrpn_create_server_connection(CONNECTION_PORT);


	//Tracker SETUP
	trackerServer = new vrpn_Tracker_Server(TRACKER_NAME, connection, 46);//46 sensores = 2 maos

	//Client SETUP
	trackerRem = new vrpn_Tracker_Remote(TRACKER_NAME, connection);
	tracker_user_callback *tc1 = new tracker_user_callback;
	strncpy(tc1->t_name, TRACKER_NAME, sizeof(tc1->t_name));
	fprintf(stderr, "Tracker's name is %s.\n", TRACKER_NAME);


	//Analog SETUP
	analogServer = new vrpn_Analog_Server(ANALOG_NAME, connection, 4); //2 maos
	//0/2 - grab
	//1/3 - pinch
	//Client SETUP
	analogRem = new vrpn_Analog_Remote(ANALOG_NAME, connection);
	tracker_user_callback *tc2 = new tracker_user_callback;
	strncpy(tc2->t_name, ANALOG_NAME, sizeof(tc2->t_name));
	fprintf(stderr, "Analog's name is %s.\n", ANALOG_NAME);

	



	// Set up the tracker callback handlers
	if ( printTracker ) {
		trackerRem->register_change_handler(tc1, handle_tracker_pos_quat);
	}
	if ( printAnalog ) {
		analogRem->register_change_handler(tc2, handle_analog);
	}

	LeapListener listener;
	Controller controller;

	controller.addListener(listener);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	// Remove the sample listener when done
	controller.removeListener(listener);

	return 0;

}   /* main */


