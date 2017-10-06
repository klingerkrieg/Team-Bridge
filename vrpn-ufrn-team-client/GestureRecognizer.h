#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include <time.h>

class GestureRecognizer {
private:
	//nao pode usar o mesmo headHeight para detectTopChange porque se nao ele so ira detectar mudancas bruscas
	//porque dentro do detectHandTop o headHeight é atualizado a todo momento
	static double lastHeight;
	static bool lastHeightDefined;

	static double lastHeadHeight;
	static bool lastHeadHeightDefined;
	double handTopInterval = 0.10;

	static std::map<int, std::vector<double>> lastMemberPos;
	static std::map<long, long> lastMemberTime;

	//Maximo de distancia percorrida considerada normal para o intervalo abaixo
	double fastMemberFator = 0.003f;
	//Garanto que ele ira calcular a distancia em intervalo de tempo de 250 a 290ms
	int fastMemberDelay = 250;
	int maxFastMemberDelay = 290;

public:

	std::vector<double> getLastMemberPos(int sensor);
	int getLastMemberTime(int sensor);

	double euclidianDistance(std::vector<double> pos1, std::vector<double> pos2);

	bool detectLeftHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	bool detectRightHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	bool detectHandTop(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, int topLevel);

	int detectTopChange(TrackerUserCallback *userdata, const vrpn_TRACKERCB t, double heightSens);

	bool detectLeftHandFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	bool detectRightHandFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

	bool detectMemberFast(TrackerUserCallback *userdata, const vrpn_TRACKERCB t);

};


