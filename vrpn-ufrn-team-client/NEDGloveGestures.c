#include "NEDGloveGestures.h"

int NEDGloveGestures::sum(const vrpn_ANALOGCB a) {
	int sum = 0;
	for ( int i = 0; i < 5; i++ ) {
		sum += a.channel[i];
	}
	return sum / 5;
}

bool NEDGloveGestures::handClosed(const vrpn_ANALOGCB a,int closeCalib) {
	if ( sum(a) <= closeCalib ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handOpen(const vrpn_ANALOGCB a, int closeCalib) {
	if ( sum(a) > closeCalib ) {
		return true;
	} else {
		return false;
	}
}