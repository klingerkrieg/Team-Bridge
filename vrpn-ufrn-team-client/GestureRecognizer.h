#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"

class GestureRecognizer {
private:
	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static double lastHeight;
	static bool lastHeightDefined;

	static double lastHeadHeight;
	static bool lastHeadHeightDefined;
	double handTopInterval = 0.10;

public:

	int detectLeftHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectRightHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens);

};


