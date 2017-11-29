#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "util.h"
#include "FlexedMember.h"


class LeapMotionGestures : public FlexedMember {
private:

	float closeAngle = 2.5;
	float pinchAngle = 60;

	static std::map<int, std::vector<double>> lastPositions[10];

	int handClosed(float angle);
	int pinch(float angle);

public:
	
	int leftWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod);
	int leftWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod);
	int leftClosed(const vrpn_ANALOGCB a);
	int leftPinch(const vrpn_ANALOGCB a);

	int rightWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod);
	int rightWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod);
	int rightClosed(const vrpn_ANALOGCB a);
	int rightPinch(const vrpn_ANALOGCB a);

	static std::map<int, int> handSkeletonMap1;
	static std::map<int, int> create_handSkeletonMap1() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( handSkeletonMap1.size() > 0 ) {
			return handSkeletonMap1;
		}

		std::map<int, int> map;
		map[0] = SKELETON_HAND_R;
		map[2] = SKELETON_WRIST_R;
		map[1] = SKELETON_ELBOW_R;

		return map;
	}

	static std::map<int, int> handSkeletonMap2;
	static std::map<int, int> create_handSkeletonMap2() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( handSkeletonMap2.size() > 0 ) {
			return handSkeletonMap2;
		}

		std::map<int, int> map;
		map[23] = SKELETON_HAND_L;
		map[25] = SKELETON_WRIST_L;
		map[24] = SKELETON_ELBOW_L;

		return map;
	}

};