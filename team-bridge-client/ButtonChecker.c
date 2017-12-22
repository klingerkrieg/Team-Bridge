#include "ButtonChecker.h"

bool ButtonChecker::assignChecker(CheckerSubject *checker, KeyMap *keyMap) {
		
		//Caso seja mouse ou teclado
		if ( keyMap->getIdDevType() == DEVTYPE_KEYBOARD || keyMap->getIdDevType() == DEVTYPE_MOUSE ) {
			checker->attach((ButtonCheckerMethod)&ButtonChecker::pressed, keyMap, this);
			//keyMap->assignGestureChecker(BUTTON_TYPE, (KeyMap::gestureCheckerMethod)&ButtonChecker::pressed, this);
			return true;
		}
		return false;
	}



int ButtonChecker::pressed(vrpn_BUTTONCB b, KeyMap * keyMap) {
	if ( keyMap->getKey() == b.button && b.state == 1 ) {
		return 1;
	} else {
		return 0;
	}
}