#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "FlexedMember.h"
#include "KeyMap.h"

const int KINECT_UP = 1;
const int KINECT_DOWN = 2;
const int KINECT_NORMAL = 3;
const int KINECT_LEFT = 1;
const int KINECT_RIGHT = 2;
const int KINECT_FRONT = 3;
const int KINECT_BACK = 4;

struct KinectDetection {
	bool centerPosDefined = false;
	SkeletonPart hipCenter;
	
	std::map<int, std::vector<double>> lastMemberPos;
	std::map<long, long> lastMemberTime;

	//deteccao marcha estacionaria
	long int lastWalk;
	double leftKneeLastHeight;
	double rightKneeLastHeight;
};

//A primeira classe obrigatoriamente precisa ser AbstractGestureRecognizer
class KinectGestures : public AbstractGestureRecognizer, public FlexedMember {
private:

	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static std::vector<double> KinectGestures::headTopPositions;

	static double normalStepHeight;
	double handTopInterval = 0.10;

	static double handXPosInterval;
	static KinectDetection kinectDetection;

	//usado para detectBody
	static std::map<int, std::vector<double>> bodyDirectionPoints;
	double bodyCenterDistance = 0.15;

	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 300ms
	double fastMemberDelay = 250;

	double turnFactor = 0.07;

	bool detectWalkHeight(SkeletonPart skelPart, KeyMap * keyMap, double &kneeLastHeight);
	int detectBody(SkeletonPart skelPart, KeyMap * keyMap ,int direction);
	bool detectMemberFast(SkeletonPart skelPart, KeyMap * keyMap);

	int detectHandTop(SkeletonPart skelPart, KeyMap * keyMap);
	int detectTopChange(SkeletonPart skelPart, KeyMap * keyMap, int direction);
	int detectHandXPos(SkeletonPart skelPart, KeyMap * keyMap);

	//Flexao
	static std::map<int, std::vector<double>> lastPositions[20];

protected:
	void assignChecker(std::vector<KeyMap> &map);

public:

	static Skeleton skeleton;
	KinectDetection getKinectDetection() {
		return kinectDetection;
	}

	//step
	int detectTopChangeUp(void * data, KeyMap * keyMap);
	int detectTopChangeDown(void * data, KeyMap * keyMap);
	int detectTopChangeNormal(void * data, KeyMap * keyMap);


	//flexao
	int leftWristFlexedUp(void * data, KeyMap * keyMap);
	int leftWristFlexedDown(void * data, KeyMap * keyMap);
	int rightWristFlexedUp(void * data, KeyMap * keyMap);
	int rightWristFlexedDown(void * data, KeyMap * keyMap);

	std::vector<double> getLastMemberPos(int sensor);
	int getLastMemberTime(int sensor);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);

	int detectLeftHandTop(void * data, KeyMap * keyMap);

	int detectRightHandTop(void * data, KeyMap * keyMap);



	int detectLeftHandFast(void * data, KeyMap * keyMap);
	int detectRightHandFast(void * data, KeyMap * keyMap);

	


	int detectBodyFront(void * data, KeyMap * keyMap);
	int detectBodyRight(void * data, KeyMap * keyMap);
	int detectBodyLeft(void * data, KeyMap * keyMap);
	int detectBodyBack(void * data, KeyMap * keyMap);

	int setCenterPos(void * data, KeyMap * keyMap);


	int detectWalk(void * data, KeyMap * keyMap);


	int detectTurnLeft(void * data, KeyMap * keyMap);
	int detectTurnRight(void * data, KeyMap * keyMap);



	int detectLeftHandXPos(void * data, KeyMap * keyMap);
	int detectRightHandXPos(void * data, KeyMap * keyMap);
	
	int bodyBalance(void * data, KeyMap * keyMap);

	static void setKinectXInterval(double interval) {
		handXPosInterval = interval;
	}

	static double getKinectXInterval() {
		return handXPosInterval;
	}


	static std::map<int, int> skeletonMap1;
	static std::map<int, int> create_SkeletonMap1() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( skeletonMap1.size() > 0 ) {
			return skeletonMap1;
		}

		std::map<int, int> map;
		map[0] = SKELETON_HEAD;
		map[1] = SKELETON_SHOULDER_CENTER;
		map[8] = SKELETON_SHOULDER_R;
		map[4] = SKELETON_SHOULDER_L;
		map[9] = SKELETON_ELBOW_R;
		map[5] = SKELETON_ELBOW_L;
		map[10] = SKELETON_WRIST_R;
		map[6] = SKELETON_WRIST_L;
		map[11] = SKELETON_HAND_R;
		map[7] = SKELETON_HAND_L;
		map[2] = SKELETON_SPINE;
		map[3] = SKELETON_HIP_CENTER;
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
		
};


