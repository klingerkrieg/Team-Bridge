#include "GestureRecognizer.h"


int GestureRecognizer::detectLeftHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 8 || t.sensor == 4 ) {
		return detectHandTop(userdata, t, topLevel);
	}
	return false;
}

int GestureRecognizer::detectRightHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
	if ( t.sensor == 12 || t.sensor == 4 ) {
		return detectHandTop(userdata, t, topLevel);
	}
	return false;
}

int GestureRecognizer::detectHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel) {
	//pega a posicao da cabeca
	if ( t.sensor == 4 ) {
		lastHeadHeight = t.pos[1];
		lastHeadHeightDefined = true;
		return false;
	}

	if ( lastHeadHeightDefined == false ) {
		return false;
	}


	//
	//...  - 5
	//1.30 - 4
	//1.15 - 3
	//1.0  - 3
	//0.85 - 2
	// ... - 1
	/*if (lastHeight)
	printf("top:%.2f head:%.2f [%.2f <= %.2f && %.2f > %.2f %d]\n", lastHeight, lastHeadHeight,
			t.pos[1], lastHeadHeight,
		   t.pos[1], lastHeadHeight - handTopInterval,
		   topLevel);*/

	if ( t.pos[1] > lastHeadHeight + (handTopInterval * 2) && topLevel == 5 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight + (handTopInterval * 2) &&
		t.pos[1] > lastHeadHeight + handTopInterval && topLevel == 4 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight + handTopInterval &&
		t.pos[1] > lastHeadHeight && topLevel == 3 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight &&
		t.pos[1] > lastHeadHeight - handTopInterval && topLevel == 2 ) {
		return true;
	} else
	if ( t.pos[1] <= lastHeadHeight - handTopInterval && topLevel == 1 ) {
		return true;
	}

	return false;
}

int GestureRecognizer::detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens) {
	if ( t.sensor == 0 ) {


		if ( lastHeightDefined == false ) {
			lastHeight = t.pos[1];
			lastHeightDefined = true;
			return 0;
		}
		

		//Desceu
		//last - pos
		//0.57 - 0.55 = 0.02 >= 0.02
		if ( lastHeight - t.pos[1] >= (float)heightSens ) {
			lastHeight = t.pos[1];
			return -1;
		} else
			//Subiu
			//pos  - last
			//0.59 - 0.57 = 0.02 >= 0.02
			if ( t.pos[1] - lastHeight >= (float)heightSens ) {
				lastHeight = t.pos[1];
				return 1;
			}

	}
	return 0;
}

double GestureRecognizer::lastHeight = 0;
bool GestureRecognizer::lastHeightDefined = false;

double GestureRecognizer::lastHeadHeight = 0;
bool GestureRecognizer::lastHeadHeightDefined = false;