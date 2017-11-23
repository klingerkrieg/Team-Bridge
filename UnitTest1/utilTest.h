#pragma once
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include "Config.h"


timeval getTimeVal();

vrpn_TRACKERCB getTrackerCB();

vrpn_ANALOGCB getAnalogCB();
