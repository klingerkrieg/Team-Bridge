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
	a.num_channel = 4;
	return a;
}