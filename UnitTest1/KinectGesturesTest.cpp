#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "KinectGestures.h"
#include <vrpn_Tracker.h>
#include <math.h>
#include "utilTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GestureRecognizerTest {


	TEST_CLASS(KinectGesturesTest) {
public:

	TEST_METHOD(GestureRecognizer_detectTopChange) {

		SkeletonPart t = getSkeletonPart();
		

		KinectGestures gr = KinectGestures();
		double heightSens = 0.15;

		//primeira altura = 1.0 ira definir o STEP_NORMAL
		Assert::AreEqual(-1, gr.detectTopChange(t, heightSens, KINECT_UP));

		//mantem
		t.y = 1.141;
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_UP));
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens, KINECT_NORMAL));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_DOWN));

		//subiu (devido a problemas com o float do vrpn ele entende como 1.4999)
		t.y = 1.151;
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens, KINECT_UP));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_NORMAL));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_DOWN));

		//mantem
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens, KINECT_UP));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_NORMAL));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_DOWN));

		//altura normal
		t.y = 1.0;
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_UP));
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens, KINECT_NORMAL));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_DOWN));

		//dobrou os joelhos, esta um degrau abaixo do normal
		t.y = 0.74;
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_UP));
		Assert::AreEqual(0, gr.detectTopChange(t, heightSens, KINECT_NORMAL));
		Assert::AreEqual(1, gr.detectTopChange(t, heightSens, KINECT_DOWN));
	}

	TEST_METHOD(GestureRecognizer_detectHandTop) {
		SkeletonPart t = getSkeletonPart();

		KinectGestures gr = KinectGestures();
		gr.detectRightHandTop(t, 5, 0);

		t.skelConstant = SKELETON_HAND_R; //right hand
		t.y = 1.301;
		Assert::IsTrue(gr.detectRightHandTop(t, 5, 0));
		t.y = 1.151;
		Assert::IsTrue(gr.detectRightHandTop(t, 4, 0));
		t.y = 1.05;
		Assert::IsTrue(gr.detectRightHandTop(t, 3, 0));
		t.y = 0.80;
		Assert::IsTrue(gr.detectRightHandTop(t, 2, 0));
		t.y = 0.50;
		Assert::IsTrue(gr.detectRightHandTop(t, 1, 0));

		//Acima de
		t.y = 1.301;
		Assert::IsTrue(gr.detectRightHandTop(t, 3, 1));
		//Abaixo de
		t.y = 0.50;
		Assert::IsTrue(gr.detectRightHandTop(t, 3, -1));


	}


	TEST_METHOD(GestureRecognizer_detectHandXPos) {
		SkeletonPart t = getSkeletonPart();

		KinectGestures gr = KinectGestures();
		gr.detectRightHandTop(t, 1, 0);//handXPos depende do HandTop
		//HandTop deve ser chamado antes

		t.skelConstant = SKELETON_HAND_R;
		t.x = 1.0;
		Assert::IsTrue(gr.detectRightHandXPos(t, 0));
		
		t.x = 1.401;
		Assert::IsTrue(gr.detectRightHandXPos(t, 1));

		t.skelConstant = SKELETON_HAND_L;
		t.x = 0.61;
		Assert::IsTrue(gr.detectLeftHandXPos(t, 0));

		t.x = 0.59;
		Assert::IsTrue(gr.detectLeftHandXPos(t, -1));



	}



	TEST_METHOD(GestureRecognizer_detectWalk) {
		SkeletonPart t = getSkeletonPart();
		
		int delay = 1000;
		double sens = 0.15;
		KinectGestures gr = KinectGestures();
		
		t.skelConstant = SKELETON_KNEE_R; // usa joelho direito
		t.y = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t,delay, sens));
		t.y = 1.14; //NAO esta suficientemente levantado
		Assert::IsFalse(gr.detectWalk(t, delay, sens));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk(t, delay, sens));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk(t, delay, sens));

		t.skelConstant = SKELETON_KNEE_L; // usa joelho esquerdo
		t.y = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t, delay, sens));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk(t, delay, sens));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk(t, delay, sens));
	}



	TEST_METHOD(GestureRecognizer_detectBody) {
		SkeletonPart t = getSkeletonPart();
		
		KinectGestures gr = KinectGestures();
		int angle = 15;
		//Inicia
		t.skelConstant = SKELETON_HIP_CENTER;
		gr.detectBodyFront(t, angle);
		t.y = 1.4;
		t.skelConstant = SKELETON_HEAD;
		gr.detectBodyFront(t, angle);
		

		t.z = 1; //NAO esta suficientemente inclinado para frente
		Assert::IsFalse(gr.detectBodyFront(t, angle));

		t.z = 0; //inclinado para frente
		Assert::IsTrue(gr.detectBodyFront(t, angle));
		t.z = 2; //inclinado para trás
		Assert::IsTrue(gr.detectBodyBack(t, angle));

		t.x = 2; //inclinado para direita
		Assert::IsTrue(gr.detectBodyRight(t, angle));
		t.x = 0; //inclinado para esquerda
		Assert::IsTrue(gr.detectBodyLeft(t, angle));

	}

	TEST_METHOD(GestureRecognizer_detectTurn) {
		SkeletonPart t = getSkeletonPart();
		t.skelConstant = SKELETON_HIP_CENTER;

		KinectGestures gr = KinectGestures();
		t.quat_z = 0;
		gr.setCenterPos(t);

		t.quat_z = 0.05; //NAO esta suficientemente virado
		Assert::IsFalse(gr.detectTurnRight(t));

		t.quat_z = 0.071; //girou para direita
		Assert::IsTrue(gr.detectTurnRight(t));
		t.quat_z = -0.071; //girou para esquerda
		Assert::IsTrue(gr.detectTurnLeft(t));

	}



	};
}