#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "util.h"
#include "FlexedMember.h"
#include "KeyMap.h"


//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class LeapMotionGestures : public FlexedMember  {
private:
	
	//As medidas de angulo são diferentes para mão e pinça
	float handAngleMax = 20.0;
	float pinchDistanceMax = 200.0;


protected:
	void assignChecker(std::vector<KeyMap> &map);

public:

	static std::map<std::string, Skeleton> skeleton;

	int wristFlexedUp(void * data, KeyMap * keyMap);
	int wristFlexedDown(void * data, KeyMap * keyMap);
	int closed(void * data, KeyMap * keyMap);
	int pinch(void * data, KeyMap * keyMap);

	static std::map<int, int> handSkeletonMap1;
	static std::map<int, int> create_handSkeletonMap1() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( handSkeletonMap1.size() > 0 ) {
			return handSkeletonMap1;
		}
		//deve-se usar somente as constantes para a mão esquerda porque ele não faz diferenciação de mãos
		//apenas diferencia a ordem que as mãos aparecem
		std::map<int, int> map;
		map[0] = SKELETON_HAND_L;
		map[2] = SKELETON_WRIST_L;
		map[1] = SKELETON_ELBOW_L;

		return map;
	}


};