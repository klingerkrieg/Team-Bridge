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

	TEST_METHOD(NEDGloveGestures_CloseAndPinch) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Analog0@localhost", sizeof(tc1->name));
		vrpn_ANALOGCB a = getAnalogCB();

		NEDGloveGestures gr = NEDGloveGestures();
		int closedCalib = 205;

		json js = {
			{ "divClass","nedGlove" },
			{ "dev" ,"Analog0@localhost" },
			{ "key" ,"NEDGLOVE_PINCH" },
			{ "thumb" , 0 },
			{ "index" , 4 },
			{ "strengthMin" ,40 },
			{ "strengthMax" ,0 },
			{ "toKeyUp" ,"A" }
		};
		KeyMap m1 = KeyMap(js);

		int val = 205;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		Assert::IsTrue(gr.closed(a, &m1));
		Assert::IsTrue(gr.pinch(a, &m1));

		val = 220;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		Assert::IsFalse(gr.closed(a, &m1));
		Assert::IsFalse(gr.pinch(a, &m1));

	}



};
}