#pragma once
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include "DeviceInfo.h"
#include <map>
#include "util.h"
#include "Skeleton.h"
#include "KeyMap.h"

#define PI 3.14159265;

const int UP = 1;
const int DOWN = 2;


class FlexedMember : public AbstractGestureRecognizer {
	private:


	bool assignChecker(CheckerSubject *checker, KeyMap *keyMap);

	public:
	int flexed2d(std::map<int, std::vector<double>> points, KeyMap * keyMap);


	double getAngle2d(std::map<int, std::vector<double>> points);
	double getAngle3d(std::map<int, std::vector<double>> points);

	int flexed3d(std::map<int, std::vector<double>> points, KeyMap * keyMap);
	int flexed3d(std::map<int, std::vector<double>> points, KeyMap * keyMap, int direction);

	std::map<int, std::vector<double>> getPoints(SkeletonPart skelPart0, SkeletonPart skelPart1, SkeletonPart skelPart2);
};