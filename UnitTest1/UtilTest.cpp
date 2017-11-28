#include "utilTest.h"

timeval getTimeVal() {
	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 1;
	return tv;
}

vrpn_TRACKERCB getTrackerCB() {
	vrpn_TRACKERCB t = vrpn_TRACKERCB();
	timeval tv = getTimeVal();
	t.msg_time = tv;
	float pos[3] = { 1.0, 1.0, 1.0 };
	std::copy(pos, pos + 3, t.pos);
	float quat[4] = { 0.0, 0.0, 0.0, 1.0 };
	std::copy(quat, quat + 4, t.quat);

	t.sensor = 0;
	return t;
}

vrpn_ANALOGCB getAnalogCB() {
	vrpn_ANALOGCB a = vrpn_ANALOGCB();
	a.channel[0] = 0;
	a.channel[1] = 0;
	a.channel[2] = 0;
	a.channel[3] = 0;
	a.channel[4] = 0;
	a.num_channel = 5;
	return a;
}

SkeletonPart getSkeletonPart() {
	SkeletonPart skelPart;
	skelPart.defined = true;
	skelPart.x = 1;
	skelPart.y = 1;
	skelPart.z = 1;
	skelPart.quat_x = 0;
	skelPart.quat_y = 0;
	skelPart.quat_z = 0;
	skelPart.quat_w = 0;
	skelPart.sensor = 0;
	skelPart.skelConstant = SKELETON_HEAD;
	return skelPart;
}