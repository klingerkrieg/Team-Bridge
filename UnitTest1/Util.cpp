#include "util.h"

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