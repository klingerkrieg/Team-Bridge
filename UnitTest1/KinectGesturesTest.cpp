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
		Assert::AreEqual(-1, gr.detectTopChangeUp((void *)&t, mUp));


		//mantem
		t.y = 1.141;
		Assert::AreEqual(0, gr.detectTopChangeUp((void *)&t, mUp));
		Assert::AreEqual(1, gr.detectTopChangeNormal((void *)&t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown((void *)&t, mDown));

		//subiu (devido a problemas com o float do vrpn ele entende como 1.4999)
		t.y = 1.151;
		Assert::AreEqual(1, gr.detectTopChangeUp((void *)&t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal((void *)&t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown((void *)&t, mDown));

		//mantem
		Assert::AreEqual(1, gr.detectTopChangeUp((void *)&t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal((void *)&t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown((void *)&t, mDown));

		//altura normal
		t.y = 1.0;
		Assert::AreEqual(0, gr.detectTopChangeUp((void *)&t, mUp));
		Assert::AreEqual(1, gr.detectTopChangeNormal((void *)&t, mNormal));
		Assert::AreEqual(0, gr.detectTopChangeDown((void *)&t, mDown));

		//dobrou os joelhos, esta um degrau abaixo do normal
		t.y = 0.74;
		Assert::AreEqual(0, gr.detectTopChangeUp((void *)&t, mUp));
		Assert::AreEqual(0, gr.detectTopChangeNormal((void *)&t, mNormal));
		Assert::AreEqual(1, gr.detectTopChangeDown((void *)&t, mDown));
	}

	TEST_METHOD(GestureRecognizer_detectHandTop) {
		SkeletonPart t = getSkeletonPart();
		KinectGestures gr = KinectGestures();

		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca
		trackData.sensor = 0;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.skeletonMap1, trackData, skelPart, "Tracker0");

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
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		t.y = 1.151;
		js["y"] = 4;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		t.y = 1.05;
		js["y"] = 3;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		t.y = 0.80;
		js["y"] = 2;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		t.y = 0.50;
		js["y"] = 1;
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));

		//Acima de
		t.y = 1.301;
		js["y"] = 5;
		js["coordinateMod"] = ">=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		//Abaixo de
		t.y = 0.50;
		js["y"] = 1;
		js["coordinateMod"] = "<=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));


		//Abaixo de
		t.y = 1.151;
		js["y"] = 5;
		js["coordinateMod"] = "<=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));
		//Acima de
		t.y = 0.80;
		js["y"] = 1;
		js["coordinateMod"] = ">=";
		m1 = new KeyMap(js);
		Assert::IsTrue(gr.detectRightHandTop((void *)&t, m1));


	}


	TEST_METHOD(GestureRecognizer_detectHandXPos) {
		SkeletonPart t = getSkeletonPart();
		KinectGestures gr = KinectGestures();

		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca e da bacia
		trackData.sensor = 0;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.skeletonMap1, trackData, skelPart, "Tracker0");
		trackData.sensor = 3;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.skeletonMap1, trackData, skelPart, "Tracker0");


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
		Assert::IsTrue(gr.detectRightHandXPos((void *)&t, m1));
		

		js["x"] = 1;
		m1 = new KeyMap(js);
		t.x = 1.401;
		Assert::IsTrue(gr.detectRightHandXPos((void *)&t, m1));

		js["x"] = 0;
		m1 = new KeyMap(js);
		t.skelConstant = SKELETON_HAND_L;
		t.x = 0.61;
		Assert::IsTrue(gr.detectLeftHandXPos((void *)&t, m1));

		js["x"] = -1;
		m1 = new KeyMap(js);
		t.x = 0.59;
		Assert::IsTrue(gr.detectLeftHandXPos((void *)&t, m1));



	}



	TEST_METHOD(GestureRecognizer_detectWalk) {
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
		Assert::IsFalse(gr.detectWalk((void *)&t, m1));
		t.y = 1.14; //NAO esta suficientemente levantado
		Assert::IsFalse(gr.detectWalk((void *)&t, m1));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk((void *)&t, m1));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk((void *)&t, m1));

		t.skelConstant = SKELETON_KNEE_L; // usa joelho esquerdo
		t.y = 1.0; //inicia
		Assert::IsFalse(gr.detectWalk((void *)&t, m1));
		t.y = 1.151;
		Assert::IsTrue(gr.detectWalk((void *)&t, m1));
		t.y = 1.0;
		Assert::IsTrue(gr.detectWalk((void *)&t, m1));
	}



	TEST_METHOD(GestureRecognizer_detectBody) {
		SkeletonPart t = getSkeletonPart();

		json js = {
			{ "divClass", "kinectBody" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_BODY_FRONT" },
			{ "angle" , 15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};

		KeyMap *m1 = new KeyMap(js);
		
		KinectGestures gr = KinectGestures();
		int angle = 15;
		//Inicia
		t.skelConstant = SKELETON_HIP_CENTER;
		gr.detectBodyFront( (void *)&t, m1);
		t.y = 1.4;
		t.skelConstant = SKELETON_HEAD;
		gr.detectBodyFront((void *)&t, m1);
		

		t.z = 1; //NAO esta suficientemente inclinado para frente
		Assert::IsFalse(gr.detectBodyFront((void *)&t, m1));

		t.z = 0; //inclinado para frente
		Assert::IsTrue(gr.detectBodyFront((void *)&t, m1));
		t.z = 2; //inclinado para trás
		Assert::IsTrue(gr.detectBodyBack((void *)&t, m1));

		t.x = 2; //inclinado para direita
		Assert::IsTrue(gr.detectBodyRight((void *)&t, m1));
		t.x = 0; //inclinado para esquerda
		Assert::IsTrue(gr.detectBodyLeft((void *)&t, m1));

	}

	TEST_METHOD(GestureRecognizer_detectTurn) {
		SkeletonPart t = getSkeletonPart();
		t.skelConstant = SKELETON_HIP_CENTER;

		//Esses métodos não requerem nenhuma informação do KeyMap
		json js = {};
		KeyMap *m1 = new KeyMap(js);

		KinectGestures gr = KinectGestures();
		t.quat_z = 0;
		gr.setCenterPos((void *)&t, m1);

		t.quat_z = 0.05; //NAO esta suficientemente virado
		Assert::IsFalse(gr.detectTurnRight((void *)&t, m1));

		t.quat_z = 0.071; //girou para direita
		Assert::IsTrue(gr.detectTurnRight((void *)&t, m1));
		t.quat_z = -0.071; //girou para esquerda
		Assert::IsTrue(gr.detectTurnLeft((void *)&t, m1));

	}

	TEST_METHOD(GestureRecognizer_setCenterPos) {
		
		KinectGestures gr = KinectGestures();
		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da cabeca e da bacia
		trackData.sensor = 3;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.skeletonMap1, trackData, skelPart, "Tracker0");

		SkeletonPart t = getSkeletonPart();
		t.skeletonName = "Tracker0";
		//Esses métodos não requerem nenhuma informação do KeyMap
		json js = {};
		KeyMap *m1 = new KeyMap(js);

		Assert::IsTrue(gr.setCenterPos((void *)&t, m1));


		Assert::IsTrue(gr.getKinectDetection()["Tracker0"].centerPosDefined);
		Assert::AreEqual(skelPart.x, gr.getKinectDetection()["Tracker0"].hipCenter.x);
		Assert::AreEqual(skelPart.y, gr.getKinectDetection()["Tracker0"].hipCenter.y);
		Assert::AreEqual(skelPart.z, gr.getKinectDetection()["Tracker0"].hipCenter.z);
		Assert::AreEqual(skelPart.quat_x, gr.getKinectDetection()["Tracker0"].hipCenter.quat_x);
		Assert::AreEqual(skelPart.quat_y, gr.getKinectDetection()["Tracker0"].hipCenter.quat_y);
		Assert::AreEqual(skelPart.quat_z, gr.getKinectDetection()["Tracker0"].hipCenter.quat_z);
		Assert::AreEqual(skelPart.quat_w, gr.getKinectDetection()["Tracker0"].hipCenter.quat_w);
		

	}



	};
}