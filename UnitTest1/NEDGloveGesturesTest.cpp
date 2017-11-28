#include "stdafx.h"
#include "CppUnitTest.h"

#include "NEDGloveGestures.h"
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include "utilTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NEDGloveGesturesTest {


TEST_CLASS(NEDGloveGesturesTest) {
	public:

	TEST_METHOD(NEDGloveGestures_handCloseOpen) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Analog0@localhost", sizeof(tc1->name));
		vrpn_ANALOGCB a = getAnalogCB();

		NEDGloveGestures gr = NEDGloveGestures();
		int closedCalib = 205;

		int val = 205;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		Assert::IsFalse(gr.handOpen(a, closedCalib));
		Assert::IsTrue(gr.handClosed(a, closedCalib));

		val = 210;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		Assert::IsTrue(gr.handOpen(a, closedCalib));
		Assert::IsFalse(gr.handClosed(a, closedCalib));

	}



};
}