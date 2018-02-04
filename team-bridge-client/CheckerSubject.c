#include "CheckerSubject.h"
#include "InputConverter.h"

void CheckerSubject::attach(AnalogCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context) {
	AnalogObserver obs = { method, keyMap, context };
	analogObservers.push_back(obs);
}

void CheckerSubject::attach(int sensor, TrackerCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context) {
	TrackerObserver obs = { method, keyMap, context };
	trackerObservers[sensor].push_back(obs);
}

void CheckerSubject::attach(ButtonCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context) {
	ButtonObserver obs = {method, keyMap, context};
	buttonObservers.push_back(obs);
}

bool CheckerSubject::changeState(SkeletonPart skelPart, InputConverter *conv) {
	bool pressed = false;
	bool ret = false;
	int active;

	for ( size_t i = 0; i < trackerObservers[skelPart.skelConstant].size(); i++ ) {
		TrackerObserver obs = trackerObservers[skelPart.skelConstant].at(i);

		active = (obs.context->*obs.method)(skelPart, obs.keyMap);

		if ( active != -1 ) {
			pressed = conv->interpretOnLeave(active, obs.keyMap);
			
			ret = ret || pressed;
		}
	}

	return ret;
}

bool CheckerSubject::changeState(vrpn_ANALOGCB analog, InputConverter *conv) {
	bool pressed = false;
	bool ret = false;
	int active;
	
	for ( size_t i = 0; i < analogObservers.size(); i++ ) {
		AnalogObserver obs = analogObservers.at(i);
		
		active = (obs.context->*obs.method)(analog, obs.keyMap);

		if ( active != -1 ) {
			pressed = conv->interpretOnLeave(active, obs.keyMap);
			ret = ret || pressed;
		}
	}

	return ret;
}

bool CheckerSubject::changeState(vrpn_BUTTONCB button, InputConverter *conv) {
	bool pressed = false;
	bool ret = false;
	int active;

	for ( size_t i = 0; i < buttonObservers.size(); i++ ) {
		ButtonObserver obs = buttonObservers.at(i);

		active = (obs.context->*obs.method)(button, obs.keyMap);

		if ( active != -1 ) {
			pressed = conv->interpretOnLeave(active, obs.keyMap);
			ret = ret || pressed;
		}
	}

	return ret;
}