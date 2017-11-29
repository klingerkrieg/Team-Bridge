#include "NEDGloveGestures.h"


bool NEDGloveGestures::closed(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	int sum = 0;
	for ( int i = 0; i < 5; i++ ) {
		sum += (int)a.channel[i];
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


bool NEDGloveGestures::pinch(const vrpn_ANALOGCB a, KeyMap *keyMap) {
	int str = strengthNormal - (((int)a.channel[keyMap->getThumb()] + (int)a.channel[keyMap->getIndex()]) / 2);

	int maxStr = keyMap->getStrengthMax() == 0 ? strengthNormal : keyMap->getStrengthMax();
	int minStr = keyMap->getStrengthMin() == 0 ? 0 : keyMap->getStrengthMin();

	//printf("%d %d %d\n", minStr, str, maxStr);
	if ( minStr <= str && str < maxStr ) {
		return true;
	} else {
		return false;
	}

}
