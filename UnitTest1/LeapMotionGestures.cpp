#include "stdafx.h"
#include "CppUnitTest.h"

#include "LeapMotionGestures.h"
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include "utilTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LeapMotionGesturesTest {


TEST_CLASS(LeapMotionGesturesTest) {
	public:

	TEST_METHOD(LeapMotionGestures_handClosedAndPinch) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Analog0@localhost", sizeof(tc1->name));
		vrpn_ANALOGCB a = getAnalogCB();
		
		LeapMotionGestures gr = LeapMotionGestures();
		
		a.channel[0] = 0.4;
		Assert::IsTrue(gr.leftClosed(a));
		a.channel[1] = 0.6;
		Assert::IsFalse(gr.leftPinch(a));


		a.channel[1] = 0.4;
		Assert::IsTrue(gr.leftPinch(a));
		a.channel[1] = 0.6;
		Assert::IsFalse(gr.leftPinch(a));
	}



};
}