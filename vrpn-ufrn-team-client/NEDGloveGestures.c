#include "NEDGloveGestures.h"

void NEDGloveGestures::assignChecker(std::vector<KeyMap> &map) {
	for ( size_t keyMapId = 0; keyMapId < map.size(); keyMapId++ ) {
		KeyMap *keyMap = &map.at(keyMapId);

		//cada metodo que será utilizado é alocado em seus respecitvos keyMaps

		switch ( keyMap->getKey() ) {
			case NEDGLOVE_GRAB:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&NEDGloveGestures::closed, this);
				break;
			case NEDGLOVE_PINCH:
				keyMap->assignGestureChecker(ANALOG_TYPE, (KeyMap::gestureCheckerMethod)&NEDGloveGestures::pinch, this);
				break;
		}
	}
}


int NEDGloveGestures::closed(void * data, KeyMap *keyMap) {
	vrpn_ANALOGCB * a = (vrpn_ANALOGCB*)data;
	int sum = 0;
	for ( int i = 0; i < 5; i++ ) {
		sum += (int)a->channel[i];
	}
	sum /= 5;

	int str = strengthNormal - sum;

	int maxStr = keyMap->getStrengthMax() == 0 ? strengthNormal : keyMap->getStrengthMax();
	int minStr = keyMap->getStrengthMin() == 0 ? 0 : keyMap->getStrengthMin();

	//printf("%d %d %d\n", minStr, str, maxStr);
	if ( minStr <= str && str < maxStr ) {
		return true;
	} else {
		return false;
	}
}


int NEDGloveGestures::pinch(void *data, KeyMap *keyMap) {
	vrpn_ANALOGCB * a = (vrpn_ANALOGCB*)data;
	int str = strengthNormal - (((int)a->channel[keyMap->getThumb()] + (int)a->channel[keyMap->getIndex()]) / 2);

	int maxStr = keyMap->getStrengthMax() == 0 ? strengthNormal : keyMap->getStrengthMax();
	int minStr = keyMap->getStrengthMin() == 0 ? 0 : keyMap->getStrengthMin();

	//printf("%d %d %d\n", minStr, str, maxStr);
	if ( minStr <= str && str < maxStr ) {
		return true;
	} else {
		return false;
	}

}
