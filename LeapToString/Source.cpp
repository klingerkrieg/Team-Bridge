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
		printf("{'pinch':%.2f, 'close':%.2f}", hands[0].pinchDistance(), hands[0].grabAngle());
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