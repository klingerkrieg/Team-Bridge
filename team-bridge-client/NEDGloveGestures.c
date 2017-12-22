#include "NEDGloveGestures.h"

bool NEDGloveGestures::assignChecker(CheckerSubject *checker, KeyMap * keyMap) {
	switch ( keyMap->getKey() ) {
		case NEDGLOVE_GRAB:
			checker->attach((AnalogCheckerMethod)&NEDGloveGestures::closed, keyMap, this);
			break;
		case NEDGLOVE_PINCH:
			checker->attach((AnalogCheckerMethod)&NEDGloveGestures::pinch, keyMap, this);
			break;
		default:
			return false;
			break;
	}
	return true;
}


int NEDGloveGestures::closed(vrpn_ANALOGCB a, KeyMap *keyMap) {
	int sum = 0;
	for ( int i = 0; i < 5; i++ ) {
		sum += (int)a.channel[i];
	}
	sum /= 5;

	int str = strengthNormal - sum;


	int maxStr = keyMap->getStrengthMax() == 0 ? strengthNormal : keyMap->getStrengthMax();
	int minStr = keyMap->getStrengthMin() == 0 ? 0 : keyMap->getStrengthMin();
	
	if ( minStr <= str && str < maxStr ) {
		
#ifdef THERAPY_MODULE
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), str);
		}
#endif
		return true;
	} else {
		return false;
	}
}


int NEDGloveGestures::pinch(vrpn_ANALOGCB a, KeyMap *keyMap) {
	int str = strengthNormal - (((int)a.channel[keyMap->getThumb()] + (int)a.channel[keyMap->getIndex()]) / 2);


	int maxStr = keyMap->getStrengthMax() == 0 ? strengthNormal : keyMap->getStrengthMax();
	int minStr = keyMap->getStrengthMin() == 0 ? 0 : keyMap->getStrengthMin();

	//printf("%d %d %d\n", minStr, str, maxStr);
	if ( minStr <= str && str < maxStr ) {

#ifdef THERAPY_MODULE
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), str);
		}
#endif

		return true;
	} else {
		return false;
	}

}
