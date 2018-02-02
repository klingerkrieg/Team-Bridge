#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "KinectGestures.h"
#include <vrpn_Tracker.h>
#include <math.h>
#include "utilTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KinectRecognizerTest {


	TEST_CLASS(KinectGesturesTest) {
public:

	TEST_METHOD(KinectRecognizer_detectTopChange) {

		SkeletonPart t = getSkeletonPart();
		

		KinectGestures gr = KinectGestures();
		
		json js = {
			{ "divClass", "kinectStep" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_STEP_UP" },
			{"sensivity" , 0.15},
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap *mUp = new KeyMap(js);
		js = {
			{ "divClass", "kinectStep" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_STEP_DOWN" },
			{ "sensivity" , 0.15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap *mDown = new KeyMap(js);
		js = {
			{ "divClass", "kinectStep" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_STEP_NORMAL" },
			{ "sensivity" , 0.15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap *mNormal = new KeyMap(js);

		//primeira altura = 1.0 ira definir o STEP_NORMAL
		Assert::AreEqual(-1, gr.detectTopChangeUp(t, mUp));


		//mantem
		t.y = 1.141;
		Assert::AreEqual(0, gr.detectTopChangeUp(t, mUp));
		Assert::AreEqual(1, gr.detectTopChangeNormal(t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown(t, mDown));

		//subiu (devido a problemas com o float do vrpn ele entende como 1.4999)
		t.y = 1.151;
		Assert::AreEqual(1, gr.detectTopChangeUp(t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal(t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown(t, mDown));

		//mantem
		Assert::AreEqual(1, gr.detectTopChangeUp(t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal(t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown(t, mDown));

		//altura normal
		t.y = 1.0;
		Assert::AreEqual(0, gr.detectTopChangeUp(t, mUp));
		Assert::AreEqual(1, gr.detectTopChangeNormal(t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown(t, mDown));

		//dobrou os joelhos, esta um degrau abaixo do normal
		t.y = 0.74;
		Assert::AreEqual(0, gr.detectTopChangeUp(t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal(t, mNormal));
		Assert::AreEqual(1, gr.detectTopChangeDown(t, mDown));
	}

	TEST_METHOD(KinectRecognizer_detectHandTop) {
		SkeletonPart t = getSkeletonPart();
		KinectGestures gr = KinectGestures();

		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca
		trackData.sensor = 0;
		vrpnToSkeleton(gr.skeleton[t.skeletonName], gr.skeletonMap1, trackData, skelPart, t.skeletonName);

		json js = {
			{ "divClass", "handTop" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_RIGHT_HAND_TOP" },
			{ "x" , 0 },
			{ "coordinateMod" , "=" },
			{ "y" , 3 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap *m1 = new KeyMap(js);


		t.skelConstant = SKELETON_HAND_R; //right hand
		t.y = 1.301;
		js["y"] = 5;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		t.y = 1.151;
		js["y"] = 4;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		t.y = 1.05;
		js["y"] = 3;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		t.y = 0.80;
		js["y"] = 2;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		t.y = 0.50;
		js["y"] = 1;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));

		//Acima de
		t.y = 1.301;
		js["y"] = 5;
		js["coordinateMod"] = ">=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		//Abaixo de
		t.y = 0.50;
		js["y"] = 1;
		js["coordinateMod"] = "<=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));


		//Abaixo de
		t.y = 1.151;
		js["y"] = 5;
		js["coordinateMod"] = "<=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		//Acima de
		t.y = 0.80;
		js["y"] = 1;
		js["coordinateMod"] = ">=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop(t, m1));


	}


	TEST_METHOD(KinectRecognizer_detectHandXPos) {
		SkeletonPart t = getSkeletonPart();
		KinectGestures gr = KinectGestures();

		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca e da bacia
		trackData.sensor = 0;
		vrpnToSkeleton(gr.skeleton[t.skeletonName], gr.skeletonMap1, trackData, skelPart, t.skeletonName);
		trackData.sensor = 3;
		vrpnToSkeleton(gr.skeleton[t.skeletonName], gr.skeletonMap1, trackData, skelPart, t.skeletonName);


		json js = {
			{ "divClass", "handTop" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_RIGHT_HAND_TOP" },
			{ "x" , 0 },
			{ "coordinateMod" , "=" },
			{ "y" , 3 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap *m1 = new KeyMap(js);

		
		
		t.skelConstant = SKELETON_HAND_R;
		t.x = 1.0;
		Assert::IsTrue(gr.detectRightHandTop(t, m1));
		

		js["x"] = 1;
		m1 = new KeyMap(js);
		t.x = 1.401;
		Assert::IsTrue(gr.detectRightHandTop(t, m1));

		js["x"] = 0;
		m1 = new KeyMap(js);
		t.skelConstant = SKELETON_HAND_L;
		t.x = 0.61;
		Assert::IsTrue(gr.detectLeftHandTop(t, m1));

		js["x"] = -1;
		m1 = new KeyMap(js);
		t.x = 0.59;
		Assert::IsTrue(gr.detectLeftHandTop(t, m1));



	}



	TEST_METHOD(KinectRecognizer_detectWalk) {
		SkeletonPart t = getSkeletonPart();

		json js = {
			{ "divClass", "kinectWalking" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_WALK" },
			{ "delay" , 1000 },
			{"sensivity" , 0.15},
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};

		KeyMap *m1 = new KeyMap(js);
		
		int delay = 1000;
		double sens = 0.15;
		KinectGestures gr = KinectGestures();
		
		t.skelConstant = SKELETON_KNEE_R; // usa joelho direito
		t.y = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t, m1));
		t.y = 1.14; //NAO esta suficientemente levantado
		Assert::IsFalse(gr.detectWalk(t, m1));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk(t, m1));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk(t, m1));

		t.skelConstant = SKELETON_KNEE_L; // usa joelho esquerdo
		t.y = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk(t, m1));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk(t, m1));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk(t, m1));
	}



	TEST_METHOD(KinectRecognizer_detectBody) {
		SkeletonPart t = getSkeletonPart();

		json js = {
			{ "divClass", "kinectBody" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_BODY_FRONT" },
			{ "angleMin" , 15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};

		KeyMap *m1 = new KeyMap(js);
		
		KinectGestures gr = KinectGestures();
		int angle = 15;
		//Inicia
		t.skelConstant = SKELETON_HIP_CENTER;
		gr.detectBodyFront( t, m1);
		t.y = 1.4;
		t.skelConstant = SKELETON_HEAD;
		gr.detectBodyFront(t, m1);
		

		t.z = 1; //NAO esta suficientemente inclinado para frente
		Assert::IsFalse(gr.detectBodyFront(t, m1));

		t.z = 0; //inclinado para frente
		Assert::IsTrue(gr.detectBodyFront(t, m1));
		t.z = 2; //inclinado para trás
		Assert::IsTrue(gr.detectBodyBack(t, m1));

		t.x = 2; //inclinado para direita
		Assert::IsTrue(gr.detectBodyRight(t, m1));
		t.x = 0; //inclinado para esquerda
		Assert::IsTrue(gr.detectBodyLeft(t, m1));

	}

	TEST_METHOD(KinectRecognizer_detectTurn) {

		KinectGestures gr = KinectGestures();
		SkeletonPart t = getSkeletonPart();
		t.skelConstant = SKELETON_SPINE;

		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		trackData.quat[0] = -0.01;
		trackData.quat[1] = 1.00;
		trackData.quat[2] = 0.03;
		trackData.quat[3] = 0.04;
		//Essa etapa é necessária para o centerPos
		vrpnToSkeleton(gr.skeleton[t.skeletonName], gr.skeletonMap1, trackData, skelPart, t.skeletonName);

		//Esses métodos não requerem nenhuma informação do KeyMap
		json js = {};
		KeyMap *m1 = new KeyMap(js);
		
		gr.setCenterPos(t, m1);

		//girou para direita
		t.quat_y = 0.93;
		t.quat_z = 0.00;
		t.quat_w = 0.38;
		Assert::IsTrue(gr.detectTurnRight(t, m1));

		//girou para esquerda
		t.quat_x = 0.01;
		t.quat_y = 0.95;
		t.quat_z = 0.01;
		t.quat_w = 0.30;
		Assert::IsTrue(gr.detectTurnLeft(t, m1));

	}

	TEST_METHOD(KinectRecognizer_setCenterPos) {

		std::string skeletonName = "Tracker0@localhost";
		KinectGestures gr = KinectGestures();
		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca e da bacia
		trackData.sensor = 2;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		SkeletonPart t = getSkeletonPart();

		//Esses métodos não requerem nenhuma informação do KeyMap
		json js = {};
		KeyMap *m1 = new KeyMap(js);

		Assert::IsTrue(gr.setCenterPos(t, m1));


		Assert::IsTrue(gr.getKinectDetection()[skeletonName].centerPosDefined);
		Assert::AreEqual(skelPart.x, gr.getKinectDetection()[skeletonName].spine.x);
		Assert::AreEqual(skelPart.y, gr.getKinectDetection()[skeletonName].spine.y);
		Assert::AreEqual(skelPart.z, gr.getKinectDetection()[skeletonName].spine.z);
		Assert::AreEqual(skelPart.quat_x, gr.getKinectDetection()[skeletonName].spine.quat_x);
		Assert::AreEqual(skelPart.quat_y, gr.getKinectDetection()[skeletonName].spine.quat_y);
		Assert::AreEqual(skelPart.quat_z, gr.getKinectDetection()[skeletonName].spine.quat_z);
		Assert::AreEqual(skelPart.quat_w, gr.getKinectDetection()[skeletonName].spine.quat_w);


	}

	TEST_METHOD(KinectRecognizer_wristFlex) {

		std::string skeletonName = "Tracker0@localhost";
		KinectGestures gr = KinectGestures();
		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;

		//Esse método requer pelo menos o angulo de flexão
		json js = {
			{ "angleMin" , 35 }
		};
		KeyMap *m1 = new KeyMap(js);

		//punho esquerdo

		trackData.sensor = 5;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 6;//wrist
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 7;//hand
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		Assert::IsFalse(gr.leftWristFlexedUp(skelPart, m1));



		trackData.sensor = 5;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 6;//wrist
		trackData.pos[0] += 10;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);


		trackData.sensor = 7;//hand
		trackData.pos[0] += 2;
		trackData.pos[1] += 5;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		Assert::IsTrue(gr.leftWristFlexedUp(skelPart, m1));

		/* Flexão esquerda para baixo */


		trackData = getTrackerCB();
		trackData.sensor = 5;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 6;//wrist
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 7;//hand
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		//Flexionado para baixo
		Assert::IsFalse(gr.leftWristFlexedDown(skelPart, m1));



		trackData = getTrackerCB();
		trackData.sensor = 5;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 6;//wrist
		trackData.pos[0] += 10;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 7;//hand
		trackData.pos[0] += 2;
		trackData.pos[1] -= 5;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		//Flexionado para baixo
		Assert::IsTrue(gr.leftWristFlexedDown(skelPart, m1));


		//punho direito
		trackData = getTrackerCB();
		trackData.sensor = 9;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 10;//wrist
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 11;//hand
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		Assert::IsFalse(gr.rightWristFlexedUp(skelPart, m1));




		trackData = getTrackerCB();
		trackData.sensor = 9;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 10;//wrist
		trackData.pos[0] += 10;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);


		trackData.sensor = 11;//hand
		trackData.pos[0] += 2;
		trackData.pos[1] += 5;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		Assert::IsTrue(gr.rightWristFlexedUp(skelPart, m1));


		/* Flexão direito para baixo */

		trackData = getTrackerCB();
		trackData.sensor = 9;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 10;//wrist
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		trackData.sensor = 11;//hand
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		//Flexionado para baixo
		Assert::IsFalse(gr.rightWristFlexedDown(skelPart, m1));


		trackData = getTrackerCB();
		trackData.sensor = 9;//elbow
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 10;//wrist
		trackData.pos[0] += 10;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);

		trackData.sensor = 11;//hand
		trackData.pos[0] += 2;
		trackData.pos[1] -= 5;
		vrpnToSkeleton(gr.skeleton[skeletonName], gr.skeletonMap1, trackData, skelPart, skeletonName);
		//Flexionado para baixo
		Assert::IsTrue(gr.rightWristFlexedDown(skelPart, m1));

	}


	TEST_METHOD(KinectRecognizer_CloseAndLasso) {

		std::string skeletonName = "Tracker0@localhost";
		KinectGestures gr = KinectGestures();
		vrpn_ANALOGCB a = getAnalogCB();
		
		//Esses métodos não requerem nenhuma informação do KeyMap
		json js = {};
		KeyMap *m1 = new KeyMap(js);

		//esquerda e direita fechada
		a.channel[0] = 3;
		a.channel[1] = 3;

		Assert::IsTrue(gr.leftHandClosed(a, m1));
		Assert::IsTrue(gr.rightHandClosed(a, m1));
		Assert::IsFalse(gr.leftHandLasso(a, m1));
		Assert::IsFalse(gr.rightHandLasso(a, m1));

		//esquerda e direita lasso
		a.channel[0] = 4;
		a.channel[1] = 4;

		Assert::IsFalse(gr.leftHandClosed(a, m1));
		Assert::IsFalse(gr.rightHandClosed(a, m1));
		Assert::IsTrue(gr.leftHandLasso(a, m1));
		Assert::IsTrue(gr.rightHandLasso(a, m1));

		//esquerda e direita aberta
		a.channel[0] = 2;
		a.channel[1] = 2;

		Assert::IsFalse(gr.leftHandClosed(a, m1));
		Assert::IsFalse(gr.rightHandClosed(a, m1));
		Assert::IsFalse(gr.leftHandLasso(a, m1));
		Assert::IsFalse(gr.rightHandLasso(a, m1));


	}

	};
}