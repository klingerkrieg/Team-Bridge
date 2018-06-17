#include "Skeleton.h"

SkeletonPart assignSkeletonPart(SkeletonPart &skelPart, const vrpn_TRACKERCB t, int skelConstant, std::string skeletonName) {
	skelPart.x = t.pos[0];
	skelPart.y = t.pos[1];
	skelPart.z = t.pos[2];
	skelPart.quat_x = t.quat[0];
	skelPart.quat_y = t.quat[1];
	skelPart.quat_z = t.quat[2];
	skelPart.quat_w = t.quat[3];
	skelPart.sensor = t.sensor;
	skelPart.defined = true;
	skelPart.skelConstant = skelConstant;
	skelPart.skeletonName = skeletonName;
	return skelPart;
}


SkeletonPart getSkeletonPartByName(Skeleton &skeleton, std::string name) {
	
	if ( name == "head" ) {
		return skeleton.head;
	} else if ( name == "shoulderCenter" ) {
		return skeleton.shoulderCenter;
	} else if ( name == "shoulderR" ) {
		return skeleton.shoulderR;
	} else if ( name == "shoulderL" ) {
		return skeleton.shoulderL;
	} else if ( name == "elbowR" ) {
		return skeleton.elbowR;
	} else if ( name == "elbowL" ) {
		return skeleton.elbowL;
	} else if ( name == "wristR" ) {
		return skeleton.wristR;
	} else if ( name == "wristL" ) {
		return skeleton.wristL;
	} else if ( name == "handR" ) {
		return skeleton.handR;
	} else if ( name == "handL" ) {
		return skeleton.handL;
	} else if ( name == "thumbR" ) {
		return skeleton.thumbR;
	} else if ( name == "thumbL" ) {
		return skeleton.thumbL;
	} else if ( name == "spine" ) {
		return skeleton.spine;
	} else if ( name == "hipCenter" ) {
		return skeleton.hipCenter;
	} else if ( name == "legR" ) {
		return skeleton.legR;
	} else if ( name == "legL" ) {
		return skeleton.legL;
	} else if ( name == "kneeR" ) {
		return skeleton.kneeR;
	} else if ( name == "kneeL" ) {
		return skeleton.kneeL;
	} else if ( name == "ankleR" ) {
		return skeleton.ankleR;
	} else if ( name == "ankleL" ) {
		return skeleton.ankleL;
	} else if ( name == "footR" ) {
		return skeleton.footR;
	} else if ( name == "footL" ) {
		return skeleton.footL;
	}
	SkeletonPart s;
	return s;
}

bool vrpnToSkeleton(Skeleton &skeleton, std::map<int, int> &skeletonMap, const vrpn_TRACKERCB t, SkeletonPart &skelPart, std::string skeletonName) {

	int partConst;
	try {
		partConst = skeletonMap.at(t.sensor);
	} catch ( std::exception ex) {
		return false;
	}
	skeleton.name = skeletonName;
	switch ( partConst ) {
		case SKELETON_HEAD:			skelPart = assignSkeletonPart(skeleton.head,		t, SKELETON_HEAD, skeletonName); break;
		case SKELETON_SHOULDER_CENTER:		skelPart = assignSkeletonPart(skeleton.shoulderCenter,		t, SKELETON_SHOULDER_CENTER, skeletonName); break;
		case SKELETON_SHOULDER_R:	skelPart = assignSkeletonPart(skeleton.shoulderR,	t, SKELETON_SHOULDER_R, skeletonName); break;
		case SKELETON_SHOULDER_L:	skelPart = assignSkeletonPart(skeleton.shoulderL,	t, SKELETON_SHOULDER_L, skeletonName); break;
		case SKELETON_ELBOW_R:		skelPart = assignSkeletonPart(skeleton.elbowR,		t, SKELETON_ELBOW_R, skeletonName); break;
		case SKELETON_ELBOW_L:		skelPart = assignSkeletonPart(skeleton.elbowL,		t, SKELETON_ELBOW_L, skeletonName); break;
		case SKELETON_WRIST_R:		skelPart = assignSkeletonPart(skeleton.wristR,		t, SKELETON_WRIST_R, skeletonName); break;
		case SKELETON_WRIST_L:		skelPart = assignSkeletonPart(skeleton.wristL,		t, SKELETON_WRIST_L, skeletonName); break;
		case SKELETON_HAND_R:		skelPart = assignSkeletonPart(skeleton.handR,		t, SKELETON_HAND_R, skeletonName); break;
		case SKELETON_HAND_L:		skelPart = assignSkeletonPart(skeleton.handL,		t, SKELETON_HAND_L, skeletonName); break;
		case SKELETON_THUMB_R:		skelPart = assignSkeletonPart(skeleton.thumbR,		t, SKELETON_THUMB_R, skeletonName); break;
		case SKELETON_THUMB_L:		skelPart = assignSkeletonPart(skeleton.thumbL,		t, SKELETON_THUMB_L, skeletonName); break;
		case SKELETON_SPINE:		skelPart = assignSkeletonPart(skeleton.spine,		t, SKELETON_SPINE, skeletonName); break;
		case SKELETON_HIP_CENTER:		skelPart = assignSkeletonPart(skeleton.hipCenter,		t, SKELETON_HIP_CENTER, skeletonName); break;
		case SKELETON_HIP_R:		skelPart = assignSkeletonPart(skeleton.legR,		t, SKELETON_HIP_R, skeletonName); break;
		case SKELETON_HIP_L:		skelPart = assignSkeletonPart(skeleton.legL,		t, SKELETON_HIP_L, skeletonName); break;
		case SKELETON_KNEE_R:		skelPart = assignSkeletonPart(skeleton.kneeR,		t, SKELETON_KNEE_R, skeletonName); break;
		case SKELETON_KNEE_L:		skelPart = assignSkeletonPart(skeleton.kneeL,		t, SKELETON_KNEE_L, skeletonName); break;
		case SKELETON_ANKLE_R:		skelPart = assignSkeletonPart(skeleton.ankleR,		t, SKELETON_ANKLE_R, skeletonName); break;
		case SKELETON_ANKLE_L:		skelPart = assignSkeletonPart(skeleton.ankleL,		t, SKELETON_ANKLE_L, skeletonName); break;
		case SKELETON_FOOT_R:		skelPart = assignSkeletonPart(skeleton.footR,		t, SKELETON_FOOT_R, skeletonName); break;
		case SKELETON_FOOT_L:		skelPart = assignSkeletonPart(skeleton.footL,		t, SKELETON_FOOT_L, skeletonName); break;
		default:
			return false;
			break;
	}

	return true;

}
