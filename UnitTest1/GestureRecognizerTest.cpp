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
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens));

		//mantem
		t.pos[1] = 1.141;
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens));

		//subiu (devido a problemas com o float do vrpn ele entende como 1.4999)
		t.pos[1] = 1.151;
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens));

		//mantem
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens));
		
		//desceu
		t.pos[1] = 1.0;
		Assert::AreEqual(-1, gr.detectTopChange(t, heightSens));

	}

	TEST_METHOD(GestureRecognizer_detectHandTop) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		t.sensor = 0; //head 1.0

		GestureRecognizer gr = GestureRecognizer();
		gr.detectRightHandTop(t, 5);

		t.sensor = 11; //right hand
		t.pos[1] = 1.301;
		Assert::IsTrue(gr.detectRightHandTop(t, 5));
		t.pos[1] = 1.151;
		Assert::IsTrue(gr.detectRightHandTop(t, 4));
		t.pos[1] = 1.05;
		Assert::IsTrue(gr.detectRightHandTop(t, 3));
		t.pos[1] = 0.80;
		Assert::IsTrue(gr.detectRightHandTop(t, 2));
		t.pos[1] = 0.50;
		Assert::IsTrue(gr.detectRightHandTop(t, 1));


	}


	TEST_METHOD(GestureRecognizer_detectHandXPos) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		t.sensor = 0; //head 1.0, 1.0

		GestureRecognizer gr = GestureRecognizer();
		gr.detectRightHandXPos(t, 0);

		t.sensor = 11; //right hand
		t.pos[0] = 1.0;
		Assert::IsTrue(gr.detectRightHandXPos(t, 0));
		
		t.pos[0] = 1.101;
		Assert::IsTrue(gr.detectRightHandXPos(t, 1));

		t.pos[0] = 1.201;
		Assert::IsTrue(gr.detectRightHandXPos(t, 2));

		t.pos[0] = 1.201;
		Assert::IsFalse(gr.detectLeftHandXPos(t, 2));

		t.sensor = 7; //left hand
		t.pos[0] = 0.95;
		Assert::IsTrue(gr.detectLeftHandXPos(t, 0));

		t.pos[0] = 0.89;
		Assert::IsTrue(gr.detectLeftHandXPos(t, -1));

		t.pos[0] = 0.79;
		Assert::IsTrue(gr.detectLeftHandXPos(t, -2));


	}



	TEST_METHOD(GestureRecognizer_detectWalk) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		

		GestureRecognizer gr = GestureRecognizer();
		
		t.sensor = 13; // usa joelho direito
		t.pos[1] = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t));
		t.pos[1] = 1.14; //NAO esta suficientemente levantado
		Assert::IsFalse(gr.detectWalk(t));
		t.pos[1] = 1.151;
		Assert::IsTrue(gr.detectWalk(t));
		t.pos[1] = 1.0;
		Assert::IsTrue(gr.detectWalk(t));

		t.sensor = 17; // usa joelho esquerdo
		t.pos[1] = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t));
		t.pos[1] = 1.151;
		Assert::IsTrue(gr.detectWalk(t));
		t.pos[1] = 1.0;
		Assert::IsTrue(gr.detectWalk(t));
	}



	TEST_METHOD(GestureRecognizer_detectBody) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		t.sensor = 3; // usa bacia

		GestureRecognizer gr = GestureRecognizer();
		t.pos[0] = 1.0;
		t.pos[2] = 1.0;
		gr.setCenterPos(t);


		t.pos[2] = 1.0; //NAO esta suficientemente inclinado para frente
		Assert::IsFalse(gr.detectBodyFront(t));

		t.pos[2] = 1.16; //inclinado para frente
		Assert::IsTrue(gr.detectBodyFront(t));
		t.pos[2] = 0.84; //inclinado para trás
		Assert::IsTrue(gr.detectBodyBack(t));

		t.pos[0] = 1.16; //inclinado para direita
		Assert::IsTrue(gr.detectBodyRight(t));
		t.pos[0] = 0.84; //inclinado para esquerda
		Assert::IsTrue(gr.detectBodyLeft(t));

		t.pos[0] = 0.86; //NAO esta suficientemente inclinado para esquerda
		Assert::IsFalse(gr.detectBodyLeft(t));

	}

	TEST_METHOD(GestureRecognizer_detectTurn) {
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));
		vrpn_TRACKERCB t = getTrackerCB();
		t.sensor = 3; // usa bacia

		GestureRecognizer gr = GestureRecognizer();
		t.quat[2] = 0;
		gr.setCenterPos(t);


		t.quat[2] = 0.14; //NAO esta suficientemente virado
		Assert::IsFalse(gr.detectTurnRight(t));

		t.quat[2] = 0.151; //girou para direita
		Assert::IsTrue(gr.detectTurnRight(t));
		t.quat[2] = -0.151; //girou para esquerda
		Assert::IsTrue(gr.detectTurnLeft(t));

	}


	};
}