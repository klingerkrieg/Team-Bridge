#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "FlexedMember.h"
#include "KeyMap.h"
#include "util.h"
#include "CheckerSubject.h"

const int KINECT_UP = 1;
const int KINECT_DOWN = 2;
const int KINECT_NORMAL = 3;
const int KINECT_LEFT = 1;
const int KINECT_RIGHT = 2;
const int KINECT_FRONT = 3;
const int KINECT_BACK = 4;


struct KinectDetection {
	bool centerPosDefined = false;
	SkeletonPart spine;
	
	std::map<int, std::vector<double>> lastMemberPos;
	std::map<long, long> lastMemberTime;

	//deteccao marcha estacionaria
	long int lastWalk;
	double leftKneeLastHeight;
	double rightKneeLastHeight;
};

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class KinectGestures : public FlexedMember {
private:

	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static std::vector<double> KinectGestures::headTopPositions;

	static double normalStepHeight;
	double handTopInterval = 0.10;


	static std::map<std::string,KinectDetection> kinectDetection;

	//usado para detectBody
	static std::map<int, std::vector<double>> bodyDirectionPoints;
	double bodyCenterDistance = 0.15;

	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 300ms
	double fastMemberDelay = 250;


	bool detectWalkHeight(SkeletonPart skelPart, KeyMap * keyMap, double &kneeLastHeight);
	int detectBody(SkeletonPart skelPart, KeyMap * keyMap ,int direction);
	bool detectMemberFast(SkeletonPart skelPart, KeyMap * keyMap);

	int detectHandTop(SkeletonPart skelPart, KeyMap * keyMap);
	int detectTopChange(SkeletonPart skelPart, KeyMap * keyMap, int direction);
	int detectHandXPos(SkeletonPart skelPart, KeyMap * keyMap);

	//Flexao
	static std::map<int, std::vector<double>> lastPositions[20];

protected:
	bool assignChecker( CheckerSubject *checker, KeyMap *keyMap);

public:

	static std::map<std::string, Skeleton> skeleton;

	std::map<std::string, KinectDetection> getKinectDetection() {
		return kinectDetection;
	}

	//step
	int detectTopChangeUp(SkeletonPart skelPart, KeyMap * keyMap);
	int detectTopChangeDown(SkeletonPart skelPart, KeyMap * keyMap);
	int detectTopChangeNormal(SkeletonPart skelPart, KeyMap * keyMap);


	//flexao
	int leftWristFlexedUp(SkeletonPart skelPart, KeyMap * keyMap);
	int leftWristFlexedDown(SkeletonPart skelPart, KeyMap * keyMap);
	int rightWristFlexedUp(SkeletonPart skelPart, KeyMap * keyMap);
	int rightWristFlexedDown(SkeletonPart skelPart, KeyMap * keyMap);

	std::vector<double> getLastMemberPos(SkeletonPart skelPart);
	int getLastMemberTime(SkeletonPart skelPart);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);



	int detectLeftHandFast(SkeletonPart skelPart, KeyMap * keyMap);
	int detectRightHandFast(SkeletonPart skelPart, KeyMap * keyMap);

	


	int detectBodyFront(SkeletonPart skelPart, KeyMap * keyMap);
	int detectBodyRight(SkeletonPart skelPart, KeyMap * keyMap);
	int detectBodyLeft(SkeletonPart skelPart, KeyMap * keyMap);
	int detectBodyBack(SkeletonPart skelPart, KeyMap * keyMap);

	int setCenterPos(SkeletonPart skelPart, KeyMap * keyMap);


	int detectWalk(SkeletonPart skelPart, KeyMap * keyMap);

	int detectTurnBody(SkeletonPart skelPart, KeyMap * keyMap, int direction);
	int detectTurnLeft(SkeletonPart skelPart, KeyMap * keyMap);
	int detectTurnRight(SkeletonPart skelPart, KeyMap * keyMap);



	int detectLeftHandTop(SkeletonPart skelPart, KeyMap * keyMap);
	int detectRightHandTop(SkeletonPart skelPart, KeyMap * keyMap);
	
	int bodyBalance(SkeletonPart skelPart, KeyMap * keyMap);


	static std::map<int, int> skeletonMap1;
	static std::map<int, int> create_SkeletonMap1() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( skeletonMap1.size() > 0 ) {
			return skeletonMap1;
		}

		std::map<int, int> map;
		map[0]	= SKELETON_HEAD;
		map[1]	= SKELETON_SHOULDER_CENTER;
		map[8]	= SKELETON_SHOULDER_R;
		map[4]	= SKELETON_SHOULDER_L;
		map[9]	= SKELETON_ELBOW_R;
		map[5]	= SKELETON_ELBOW_L;
		map[10] = SKELETON_WRIST_R;
		map[6]	= SKELETON_WRIST_L;
		map[11] = SKELETON_HAND_R;
		map[7]	= SKELETON_HAND_L;
		map[2]	= SKELETON_SPINE;
		map[3]	= SKELETON_HIP_CENTER;
		map[16] = SKELETON_HIP_R;
		map[12] = SKELETON_HIP_L;
		map[17] = SKELETON_KNEE_R;
		map[13] = SKELETON_KNEE_L;
		map[18] = SKELETON_ANKLE_R;
		map[14] = SKELETON_ANKLE_L;
		map[19] = SKELETON_FOOT_R;
		map[15] = SKELETON_FOOT_L;
		
		return map;
	}

	static std::map<int, int> skeletonMapV2;
	static std::map<int, int> create_SkeletonMapV2() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( skeletonMapV2.size() > 0 ) {
			return skeletonMapV2;
		}

		std::map<int, int> map = skeletonMap1;

		map[20] = SKELETON_NECK;
		map[21] = SKELETON_HAND_TIP_L;
		map[22] = SKELETON_THUMB_L;

		map[23] = SKELETON_HAND_TIP_R;
		map[24] = SKELETON_THUMB_R;

		return map;
	}
		
};


