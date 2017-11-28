#include "NEDGloveGestures.h"

int NEDGloveGestures::sum(const vrpn_ANALOGCB a) {
	int sum = 0;
	for ( int i = 0; i < 5; i++ ) {
		sum += (int)a.channel[i];
	}
	//printf("%d\n", sum/5);
	return sum / 5;
}

bool NEDGloveGestures::handClosed(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) <= keyMap->getOpenHand() - (keyMap->getOpenHand() - keyMap->getClosedHand()) / 2 ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handOpen(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) > keyMap->getOpenHand() - (keyMap->getOpenHand() - keyMap->getClosedHand()) / 2 ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handOpenMax(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) >= keyMap->getOpenHand() ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handSemiOpen(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) > keyMap->getOpenHand() - (keyMap->getOpenHand() - keyMap->getClosedHand()) / 2
		&& sum(a) < keyMap->getOpenHand() ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handClosedMax(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) <= keyMap->getClosedHand() ) {
		return true;
	} else {
		return false;
	}
}

bool NEDGloveGestures::handSemiClose(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	if ( sum(a) < keyMap->getOpenHand() - (keyMap->getOpenHand() - keyMap->getClosedHand()) / 2
		&& sum(a) > keyMap->getClosedHand() ) {
		return true;
	} else {
		return false;
	}
}