#pragma once
#include "AbstractGestureRecognizer.h"
#include "KeyMap.h"
#include <vector>
#include <map>
#include <string>

typedef int (AbstractGestureRecognizer::*AnalogCheckerMethod)(vrpn_ANALOGCB data, KeyMap *key);
typedef int (AbstractGestureRecognizer::*TrackerCheckerMethod)(SkeletonPart data, KeyMap *key);
typedef int (AbstractGestureRecognizer::*ButtonCheckerMethod)(vrpn_BUTTONCB data, KeyMap *key);

class InputConverter;

struct AnalogObserver {
	AnalogCheckerMethod method;
	KeyMap *keyMap;
	AbstractGestureRecognizer *context;
};

struct TrackerObserver {
	TrackerCheckerMethod method;
	KeyMap *keyMap;
	AbstractGestureRecognizer *context;
};

struct ButtonObserver {
	ButtonCheckerMethod method;
	KeyMap *keyMap;
	AbstractGestureRecognizer *context;
};

class CheckerSubject {

	std::vector<AnalogObserver> analogObservers;
	std::map<int,std::vector<TrackerObserver>> trackerObservers;
	std::vector<ButtonObserver> buttonObservers;

	public:
		CheckerSubject() {
	}

	void attach(AnalogCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context);
	void attach(int sensor, TrackerCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context);
	void attach(ButtonCheckerMethod method, KeyMap *keyMap, AbstractGestureRecognizer *context);

	bool changeState(SkeletonPart skelPart, InputConverter *conv);
	bool changeState(vrpn_ANALOGCB analog, InputConverter *conv);
	bool changeState(vrpn_BUTTONCB button, InputConverter *conv);

	
};