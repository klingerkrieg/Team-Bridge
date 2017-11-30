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
		//Esses m�todos n�o usam KeyMap mas est� convencionado que todos os m�todos devem receber por par�metro
		KeyMap * km = new KeyMap();
		
		a.channel[0] = 0.4;
		Assert::IsFalse(gr.leftClosed((void *)&a, km));
		a.channel[0] = 2.6;
		Assert::IsTrue(gr.leftClosed((void *)&a, km));
		a.channel[1] = 65;
		Assert::IsFalse(gr.leftPinch((void *)&a, km));
		a.channel[1] = 55;
		Assert::IsTrue(gr.leftPinch((void *)&a, km));

		//Fazer teste de flex�o
		
	}



};
}