#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>


const int GEST_LEFT = 1;
const int GEST_RIGHT = 2;
const int GEST_FRONT = 3;
const int GEST_BACK = 4;

class KinectGestures {
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
	double handXPosInterval = 0.25;
	

	//usado para detectBody
	static bool centerPosDefined;
	static double centerPos[2];
	double bodyCenterDistance = 0.15;

	static std::map<int, std::vector<double>> lastMemberPos;
	static std::map<long, long> lastMemberTime;

	//Maximo de distancia percorrida considerada normal para o intervalo abaixo
	double fastMemberFator = 0.003f;
	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 290ms
	int fastMemberDelay = 250;
	int maxFastMemberDelay = 290;


	//deteccao marcha estacionaria
	static double leftKneeLastHeight;
	static double rightKneeLastHeight;
	double kneeHeightFactor = 0.15;

	
	double turnFactor = 0.07;
	static double turnZeroQuat;

	bool detectWalkHeight(double &kneeLastHeight, const vrpn_TRACKERCB t);
	bool detectBody(const vrpn_TRACKERCB t, int direction);
	bool detectMemberFast(const vrpn_TRACKERCB t);

	bool detectHandTop(const vrpn_TRACKERCB t, int topLevel);

public:

	std::vector<double> getLastMemberPos(int sensor);
	int getLastMemberTime(int sensor);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);

	bool detectLeftHandTop(const vrpn_TRACKERCB t, int topLevel);

	bool detectRightHandTop(const vrpn_TRACKERCB t, int topLevel);


	int detectTopChange(const vrpn_TRACKERCB t, double heightSens);

	bool detectLeftHandFast(const vrpn_TRACKERCB t);

	bool detectRightHandFast(const vrpn_TRACKERCB t);

	


	bool detectBodyFront(const vrpn_TRACKERCB t);
	bool detectBodyRight(const vrpn_TRACKERCB t);
	bool detectBodyLeft(const vrpn_TRACKERCB t);
	bool detectBodyBack(const vrpn_TRACKERCB t);

	bool setCenterPos(const vrpn_TRACKERCB t);


	bool detectWalk(const vrpn_TRACKERCB t);


	bool detectTurnLeft(const vrpn_TRACKERCB t);
	bool detectTurnRight(const vrpn_TRACKERCB t);



	bool detectLeftHandXPos(const vrpn_TRACKERCB t, int xPos);
	bool detectRightHandXPos(const vrpn_TRACKERCB t, int xPos);
	bool detectHandXPos(const vrpn_TRACKERCB t, int xPos);
	

};


