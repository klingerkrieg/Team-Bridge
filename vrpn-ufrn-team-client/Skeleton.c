#include "Skeleton.h"

SkeletonPart assignSkeletonPart(SkeletonPart &skelPart, const vrpn_TRACKERCB t, int skelConstant) {
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
	return skelPart;
}


bool vrpnToSkeleton(Skeleton &skeleton, std::map<int, int> &skeletonMap, const vrpn_TRACKERCB t, SkeletonPart &skelPart) {

	int partConst;
	try {
		partConst = skeletonMap.at(t.sensor);
	} catch ( std::exception ex) {
		return false;
	}

	switch ( partConst ) {
		case SKELETON_HEAD:			skelPart = assignSkeletonPart(skeleton.head,		t, SKELETON_HEAD); break;
		case SKELETON_CHEST:		skelPart = assignSkeletonPart(skeleton.chest,		t, SKELETON_CHEST); break;
		case SKELETON_SHOULDER_R:	skelPart = assignSkeletonPart(skeleton.shoulderR,	t, SKELETON_SHOULDER_R); break;
		case SKELETON_SHOULDER_L:	skelPart = assignSkeletonPart(skeleton.shoulderL,	t, SKELETON_SHOULDER_L); break;
		case SKELETON_ELBOW_R:		skelPart = assignSkeletonPart(skeleton.elbowR,		t, SKELETON_ELBOW_R); break;
		case SKELETON_ELBOW_L:		skelPart = assignSkeletonPart(skeleton.elbowL,		t, SKELETON_ELBOW_L); break;
		case SKELETON_FIST_R:		skelPart = assignSkeletonPart(skeleton.fistR,		t, SKELETON_FIST_R); break;
		case SKELETON_FIST_L:		skelPart = assignSkeletonPart(skeleton.fistL,		t, SKELETON_FIST_L); break;
		case SKELETON_HAND_R:		skelPart = assignSkeletonPart(skeleton.handR,		t, SKELETON_HAND_R); break;
		case SKELETON_HAND_L:		skelPart = assignSkeletonPart(skeleton.handL,		t, SKELETON_HAND_L); break;
		case SKELETON_THUMB_R:		skelPart = assignSkeletonPart(skeleton.thumbR,		t, SKELETON_THUMB_R); break;
		case SKELETON_THUMB_L:		skelPart = assignSkeletonPart(skeleton.thumbL,		t, SKELETON_THUMB_L); break;
		case SKELETON_BELLY:		skelPart = assignSkeletonPart(skeleton.belly,		t, SKELETON_BELLY); break;
		case SKELETON_PELVIS:		skelPart = assignSkeletonPart(skeleton.pelvis,		t, SKELETON_PELVIS); break;
		case SKELETON_LEG_R:		skelPart = assignSkeletonPart(skeleton.legR,		t, SKELETON_LEG_R); break;
		case SKELETON_LEG_L:		skelPart = assignSkeletonPart(skeleton.legL,		t, SKELETON_LEG_L); break;
		case SKELETON_KNEE_R:		skelPart = assignSkeletonPart(skeleton.kneeR,		t, SKELETON_KNEE_R); break;
		case SKELETON_KNEE_L:		skelPart = assignSkeletonPart(skeleton.kneeL,		t, SKELETON_KNEE_L); break;
		case SKELETON_HEEL_R:		skelPart = assignSkeletonPart(skeleton.heelR,		t, SKELETON_HEEL_R); break;
		case SKELETON_HEEL_L:		skelPart = assignSkeletonPart(skeleton.heelL,		t, SKELETON_HEEL_L); break;
		case SKELETON_FOOT_R:		skelPart = assignSkeletonPart(skeleton.footR,		t, SKELETON_FOOT_R); break;
		case SKELETON_FOOT_L:		skelPart = assignSkeletonPart(skeleton.footL,		t, SKELETON_FOOT_L); break;
		default:
			return false;
			break;
	}

	return true;

}

bool vrpnToSkeleton(Skeleton &skeleton, std::map<int, int> &skeletonMap, const vrpn_TRACKERCB t) {
	SkeletonPart skelPart;
	return vrpnToSkeleton(skeleton, skeletonMap, t, skelPart);
}
