#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include "util.h"

#define PI 3.14159265;

class FlexedMember {
	public:
	int flexed2d(std::map<int, std::vector<double>> points, int angle);

	int flexed3d(std::map<int, std::vector<double>> points, int angle);

	std::map<int, std::vector<double>> getPoints(const vrpn_TRACKERCB t, int sensor1, int sensor2, int sensor3, std::map<int, std::vector<double>> &positions);
};