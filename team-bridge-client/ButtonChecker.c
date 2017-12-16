#include "ButtonChecker.h"

void ButtonChecker::assignChecker(std::vector<KeyMap> &map) {
		for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
			KeyMap *keyMap = &map.at(keyMapId);

			//Caso seja mouse ou teclado
			if ( keyMap->getIdDevType() == DEVTYPE_KEYBOARD || keyMap->getIdDevType() == DEVTYPE_MOUSE ) {
				keyMap->assignGestureChecker(BUTTON_TYPE, (KeyMap::gestureCheckerMethod)&ButtonChecker::pressed, this);
			}
		}
	}



int ButtonChecker::pressed(void * data, KeyMap * keyMap) {
	vrpn_BUTTONCB * b = (vrpn_BUTTONCB*)data;
	if ( keyMap->getKey() == b->button && b->state == 1 ) {
		return 1;
	} else {
		return 0;
	}
}