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

class KinectGestures : public FlexedMember {
private:
	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static std::vector<double> KinectGestures::headTopPositions;

	static double normalStepHeight;
	static double lastHeadHeight;
	static bool lastHeadHeightDefined;
	double handTopInterval = 0.10;


	static double lastHeadXPos;
	static bool lastHeadXPosDefined;
	static double handXPosInterval;
	static double lastCenterXPos;
	

	//usado para detectBody
	static bool centerPosDefined;
	static double centerPos[3];
	double bodyCenterDistance = 0.15;

	static std::map<int, std::vector<double>> lastMemberPos;
	static std::map<long, long> lastMemberTime;

	
	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 300ms
	double fastMemberDelay = 250;


	//deteccao marcha estacionaria
	static long int lastWalk;
	static double leftKneeLastHeight;
	static double rightKneeLastHeight;

	
	double turnFactor = 0.07;
	static double turnZeroQuat;

	bool detectWalkHeight(double &kneeLastHeight, const vrpn_TRACKERCB t, int delay, double sensitivity);
	int detectBody(const vrpn_TRACKERCB t, int direction);
	bool detectMemberFast(const vrpn_TRACKERCB t, double maxVelMs);

	int detectHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod);

	//Flexao
	static std::map<int, std::vector<double>> lastPositions[20];

public:

	//flexao
	int leftFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int leftFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);
	int rightFistFlexedUp(const vrpn_TRACKERCB t, int angle, int angleMod);
	int rightFistFlexedDown(const vrpn_TRACKERCB t, int angle, int angleMod);

	std::vector<double> getLastMemberPos(int sensor);
	int getLastMemberTime(int sensor);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);

	int detectLeftHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod);

	int detectRightHandTop(const vrpn_TRACKERCB t, int topLevel, int handTopMod);


	int detectTopChange(const vrpn_TRACKERCB t, double heightSens, int direction);

	int detectLeftHandFast(const vrpn_TRACKERCB t, double maxVelMs);
	int detectRightHandFast(const vrpn_TRACKERCB t, double maxVelMs);

	


	int detectBodyFront(const vrpn_TRACKERCB t);
	int detectBodyRight(const vrpn_TRACKERCB t);
	int detectBodyLeft(const vrpn_TRACKERCB t);
	int detectBodyBack(const vrpn_TRACKERCB t);

	int setCenterPos(const vrpn_TRACKERCB t);


	int detectWalk(const vrpn_TRACKERCB t, int delay, double sensitivity);


	int detectTurnLeft(const vrpn_TRACKERCB t);
	int detectTurnRight(const vrpn_TRACKERCB t);



	int detectLeftHandXPos(const vrpn_TRACKERCB t, int xPos);
	int detectRightHandXPos(const vrpn_TRACKERCB t, int xPos);
	int detectHandXPos(const vrpn_TRACKERCB t, int xPos);
	

	int bodyBalance(const vrpn_TRACKERCB t, int angle, int angleMod);

	static void setKinectXInterval(double interval) {
		handXPosInterval = interval;
	}

	static double getKinectXInterval() {
		return handXPosInterval;
	}
		
};


