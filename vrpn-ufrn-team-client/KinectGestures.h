#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>
#include "FlexedMember.h"

const int GEST_LEFT = 1;
const int GEST_RIGHT = 2;
const int GEST_FRONT = 3;
const int GEST_BACK = 4;

class KinectGestures : public FlexedMember {
private:
	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static double lastHeight;
	static bool lastHeightDefined;

	static double lastHeadHeight;
	static bool lastHeadHeightDefined;
	double handTopInterval = 0.10;


	static double lastHeadXPos;
	static bool lastHeadXPosDefined;
	double handXPosInterval = 0.40;
	

	//usado para detectBody
	static bool centerPosDefined;
	static double centerPos[3];
	double bodyCenterDistance = 0.15;

	static std::map<int, std::vector<double>> lastMemberPos;
	static std::map<long, long> lastMemberTime;

	//Maximo de distancia percorrida considerada normal para o intervalo abaixo
	double fastMemberFator = 0.003f;
	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 290ms
	int fastMemberDelay = 250;
	int maxFastMemberDelay = 290;


	//deteccao marcha estacionaria
	int walkDelay = 500; //ms;
	static long int lastWalk;
	static double leftKneeLastHeight;
	static double rightKneeLastHeight;
	double kneeHeightFactor = 0.15;

	
	double turnFactor = 0.07;
	static double turnZeroQuat;

	bool detectWalkHeight(double &kneeLastHeight, const vrpn_TRACKERCB t);
	int detectBody(const vrpn_TRACKERCB t, int direction);
	bool detectMemberFast(const vrpn_TRACKERCB t);

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


	int detectTopChange(const vrpn_TRACKERCB t, double heightSens);

	int detectLeftHandFast(const vrpn_TRACKERCB t);

	int detectRightHandFast(const vrpn_TRACKERCB t);

	


	int detectBodyFront(const vrpn_TRACKERCB t);
	int detectBodyRight(const vrpn_TRACKERCB t);
	int detectBodyLeft(const vrpn_TRACKERCB t);
	int detectBodyBack(const vrpn_TRACKERCB t);

	int setCenterPos(const vrpn_TRACKERCB t);


	int detectWalk(const vrpn_TRACKERCB t);


	int detectTurnLeft(const vrpn_TRACKERCB t);
	int detectTurnRight(const vrpn_TRACKERCB t);



	int detectLeftHandXPos(const vrpn_TRACKERCB t, int xPos);
	int detectRightHandXPos(const vrpn_TRACKERCB t, int xPos);
	int detectHandXPos(const vrpn_TRACKERCB t, int xPos);
	

	int bodyBalance(const vrpn_TRACKERCB t, int angleMod, int angle);
		
};


