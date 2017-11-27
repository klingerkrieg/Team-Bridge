#pragma once
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include "Config.h"
#include "Skeleton.h"

timeval getTimeVal();

vrpn_TRACKERCB getTrackerCB();

vrpn_ANALOGCB getAnalogCB();

SkeletonPart getSkeletonPart();