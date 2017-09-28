#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "GestureRecognizer.h"
#include <vrpn_Tracker.h>
#include <math.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GestureRecognizerTest {


	TEST_CLASS(GestureRecognizerTest) {
public:

	TEST_METHOD(GestureRecognizer_detectTopChange) {

		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));

		vrpn_TRACKERCB t = vrpn_TRACKERCB();
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 1;
		t.msg_time = tv;
		float pos[3] = { 1.0, 1.0, 1.0 };
		std::copy(pos, pos + 3, t.pos);
		float quat[4] = { 1.0, 1.0, 1.0, 1.0 };
		std::copy(quat, quat + 4, t.quat);

		t.sensor = 0;
		double heightSens = 0.15;
		

		GestureRecognizer gr = GestureRecognizer();

		//primeira altura = 1.0
		Assert::AreEqual(0, gr.detectTopChange(tc1, t, heightSens));

		//mantem
		t.pos[1] = 1.141;
		Assert::AreEqual(0, gr.detectTopChange(tc1, t, heightSens));

		//subiu (devido a problemas com o float do vrpn ele entende como 1.4999)
		t.pos[1] = 1.151;
		Assert::AreEqual(1, gr.detectTopChange(tc1, t, heightSens));

		//mantem
		Assert::AreEqual(0, gr.detectTopChange(tc1, t, heightSens));
		
		//desceu
		t.pos[1] = 1.0;
		Assert::AreEqual(-1, gr.detectTopChange(tc1, t, heightSens));

	}


	};
}