#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "GestureRecognizer.h"
#include <vrpn_Tracker.h>
#include <math.h>
#include "util.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GestureRecognizerTest {


	TEST_CLASS(GestureRecognizerTest) {
public:

	TEST_METHOD(GestureRecognizer_detectTopChange) {

		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		

		GestureRecognizer gr = GestureRecognizer();
		double heightSens = 0.15;

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

	TEST_METHOD(GestureRecognizer_detectHandTop) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		t.sensor = 0; //head 1.0

		GestureRecognizer gr = GestureRecognizer();
		gr.detectLeftHandTop(tc1, t, 5);

		t.sensor = 11; //left hand
		t.pos[1] = 1.301;
		Assert::IsTrue(gr.detectHandTop(tc1, t, 5));
		t.pos[1] = 1.151;
		Assert::IsTrue(gr.detectHandTop(tc1, t, 4));
		t.pos[1] = 1.05;
		Assert::IsTrue(gr.detectHandTop(tc1, t, 3));
		t.pos[1] = 0.80;
		Assert::IsTrue(gr.detectHandTop(tc1, t, 2));
		t.pos[1] = 0.50;
		Assert::IsTrue(gr.detectHandTop(tc1, t, 1));


	}


	};
}