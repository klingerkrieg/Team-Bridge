#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include "util.h"
#include "Skeleton.h"

#define PI 3.14159265;

const int UP = 1;
const int DOWN = 2;

class FlexedMember {
	public:
	int flexed2d(std::map<int, std::vector<double>> points, int angle);

	int flexed3d(std::map<int, std::vector<double>> points, int paramAngle, int angleMod);
	int flexed3d(std::map<int, std::vector<double>> points, int angle, int angleMod, int direction);

	std::map<int, std::vector<double>> getPoints(SkeletonPart skelPart, int sensor1, int sensor2, int sensor3, std::map<int, std::vector<double>> &positions);
	std::map<int, std::vector<double>> getPoints(const vrpn_TRACKERCB t, int sensor1, int sensor2, int sensor3, std::map<int, std::vector<double>> &positions);
};