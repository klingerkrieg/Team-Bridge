#include "Leap.h"
#include <Windows.h>

using namespace Leap;

bool reported = false;

class LeapListener : public Listener {
	public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};

void LeapListener::onConnect(const Controller& controller) {
}

double getAngle(Vector points[3]) {
	double v1[3] = { points[0].x - points[1].x, points[0].y - points[1].y, points[0].z - points[1].z };
	double v2[3] = { points[2].x - points[1].x, points[2].y - points[1].y, points[2].z - points[1].z };

	double v1mag = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
	std::vector<double> v1norm = { v1[0] / v1mag, v1[1] / v1mag, v1[2] / v1mag };

	double v2mag = sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
	std::vector<double> v2norm = { v2[0] / v2mag, v2[1] / v2mag, v2[2] / v2mag };


	double res = v1norm[0] * v2norm[0] + v1norm[1] * v2norm[1] + v1norm[2] * v2norm[2];

	return acos(res) * 180.0 / 3.14159265;
}

void LeapListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();
	//Consertar o time
	
	HandList hands = frame.hands();
	if ( reported == true ) {
		return;
	}
	reported = true;
	if ( hands.count() == 0 ) {
		printf("{'msg':'Nenhuma mão encontrada.'}");
	} else {
		Vector points[3] = { hands[0].arm().elbowPosition(), hands[0].arm().wristPosition(), hands[0].palmPosition() };
		double angle = getAngle(points);

		printf("{'pinch':%.2f, 'close':%.2f, 'wrist':%.2f}", hands[0].pinchDistance(), hands[0].grabAngle(), angle);
	}
}

int main() {
	LeapListener listener;
	Controller controller;

	controller.addListener(listener);
	
	while ( reported == false ) {
		Sleep(200);
	}

	// Remove the sample listener when done
	controller.removeListener(listener);

	return 0;
}