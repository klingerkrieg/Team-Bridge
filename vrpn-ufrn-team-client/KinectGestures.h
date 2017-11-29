#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "FlexedMember.h"

const int KINECT_UP = 1;
const int KINECT_DOWN = 2;
const int KINECT_NORMAL = 3;
const int KINECT_LEFT = 1;
const int KINECT_RIGHT = 2;
const int KINECT_FRONT = 3;
const int KINECT_BACK = 4;

struct KinectDetection {
	bool centerPosDefined = false;
	double centerPos[3];
	//usado para detectBody
	std::map<int, std::vector<double>> lastMemberPos;
	std::map<long, long> lastMemberTime;

	//deteccao marcha estacionaria
	long int lastWalk;
	double leftKneeLastHeight;
	double rightKneeLastHeight;

	double turnZeroQuat;
};


class KinectGestures : public FlexedMember {
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

	bool detectWalkHeight(double &kneeLastHeight, SkeletonPart skelPart, int delay, double sensitivity);
	int detectBody(SkeletonPart skelPart, int direction, int angle);
	bool detectMemberFast(SkeletonPart skelPart, double maxVelMs);

	int detectHandTop(SkeletonPart skelPart, int topLevel, int handTopMod);

	//Flexao
	static std::map<int, std::vector<double>> lastPositions[20];

public:

	static Skeleton skeleton;

	//flexao
	int leftWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod);
	int leftWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod);
	int rightWristFlexedUp(SkeletonPart skelPart, int angle, int angleMod);
	int rightWristFlexedDown(SkeletonPart skelPart, int angle, int angleMod);

	std::vector<double> getLastMemberPos(int sensor);
	int getLastMemberTime(int sensor);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);

	int detectLeftHandTop(SkeletonPart skelPart, int topLevel, int handTopMod);

	int detectRightHandTop(SkeletonPart skelPart, int topLevel, int handTopMod);


	int detectTopChange(SkeletonPart skelPart, double heightSens, int direction);

	int detectLeftHandFast(SkeletonPart skelPart, double maxVelMs);
	int detectRightHandFast(SkeletonPart skelPart, double maxVelMs);

	


	int detectBodyFront(SkeletonPart skelPart, int angle);
	int detectBodyRight(SkeletonPart skelPart, int angle);
	int detectBodyLeft(SkeletonPart skelPart, int angle);
	int detectBodyBack(SkeletonPart skelPart, int angle);

	int setCenterPos(SkeletonPart skelPart);


	int detectWalk(SkeletonPart skelPart, int delay, double sensitivity);


	int detectTurnLeft(SkeletonPart skelPart);
	int detectTurnRight(SkeletonPart skelPart);



	int detectLeftHandXPos(SkeletonPart skelPart, int xPos);
	int detectRightHandXPos(SkeletonPart skelPart, int xPos);
	int detectHandXPos(SkeletonPart skelPart, int xPos);
	

	int bodyBalance(SkeletonPart skelPart, int angle, int angleMod);

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


