#pragma once
#include <map>
#include <string>
#include <vrpn_Tracker.h>


const int SKELETON_HEAD			= 1;
const int SKELETON_NECK			= 23;
const int SKELETON_SHOULDER_CENTER= 2;
const int SKELETON_SHOULDER_R	= 3;
const int SKELETON_SHOULDER_L	= 4;
const int SKELETON_ELBOW_R		= 5;
const int SKELETON_ELBOW_L		= 6;
const int SKELETON_WRIST_R		= 7;
const int SKELETON_WRIST_L		= 8;
const int SKELETON_HAND_R		= 9;
const int SKELETON_HAND_L		= 10;
const int SKELETON_HAND_TIP_R	= 24;
const int SKELETON_HAND_TIP_L	= 25;
const int SKELETON_THUMB_R		= 11;
const int SKELETON_THUMB_L		= 12;
const int SKELETON_SPINE		= 13;
const int SKELETON_HIP_CENTER	= 14;
const int SKELETON_HIP_R		= 15;
const int SKELETON_HIP_L		= 16;
const int SKELETON_KNEE_R		= 17;
const int SKELETON_KNEE_L		= 18;
const int SKELETON_ANKLE_R		= 19;
const int SKELETON_ANKLE_L		= 20;
const int SKELETON_FOOT_R		= 21;
const int SKELETON_FOOT_L		= 22;

struct SkeletonPart {
	double x;
	double y;
	double z;
	int sensor;
	int skelConstant;
	bool defined = false;

	double quat_x;
	double quat_y;
	double quat_z;
	double quat_w;
	std::string skeletonName;
};


struct Skeleton {
	std::string name;
	SkeletonPart head;
	SkeletonPart shoulderCenter;
	SkeletonPart shoulderR;
	SkeletonPart shoulderL;
	SkeletonPart elbowR;
	SkeletonPart elbowL;
	SkeletonPart wristR;
	SkeletonPart wristL;
	SkeletonPart handR;
	SkeletonPart handL;
	SkeletonPart thumbR;
	SkeletonPart thumbL;
	SkeletonPart spine;
	SkeletonPart hipCenter;
	SkeletonPart legR;
	SkeletonPart legL;
	SkeletonPart kneeR;
	SkeletonPart kneeL;
	SkeletonPart ankleR;
	SkeletonPart ankleL;
	SkeletonPart footR;
	SkeletonPart footL;
};


SkeletonPart assignSkeletonPart(SkeletonPart &skelPart, const vrpn_TRACKERCB t, int skelConstant, std::string skeletonName);

bool vrpnToSkeleton(Skeleton &skeleton, std::map<int, int> &skeletonMap, const vrpn_TRACKERCB t, SkeletonPart &skelPart, std::string skeletonName);
