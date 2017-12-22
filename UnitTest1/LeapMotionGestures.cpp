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
		strncpy(tc1->name, "LeapMotion0@localhost", sizeof(tc1->name));
		vrpn_ANALOGCB a = getAnalogCB();
		
		LeapMotionGestures gr = LeapMotionGestures();
		//Esses métodos não usam KeyMap mas está convencionado que todos os métodos devem receber por parâmetro
		json js {
			{"divClass", "leapCloseHand"},
			{"devType" , "leapMotion"},
			{"key" , "LEAP_CLOSED"},
			{"dev" , "LeapMotion0@localhost"},
			{"angleMin" , 2.5},
			{"angleMax" , 0},
			{"toKeyDown" , "A"},
			{"toKeyUp" , "A"}
		};
		

		KeyMap * km = new KeyMap(js);
		
		a.channel[0] = 2.6;
		Assert::IsTrue(gr.closed(a, km));
		a.channel[0] = 2.4;
		Assert::IsFalse(gr.closed(a, km));


		js = {
			{ "divClass", "leapPinch" },
			{ "devType" , "leapMotion" },
			{ "key" , "LEAP_PINCH" },
			{ "dev" , "LeapMotion1@localhost" },
			{ "distanceMin" , 0 },
			{ "distanceMax" , 30 },
			{ "toKeyDown" , "B" },
			{ "toKeyUp" , "B" }
		};
		km = new KeyMap(js);

		a.channel[1] = 29;
		Assert::IsTrue(gr.pinch(a, km));
		a.channel[1] = 31;
		Assert::IsFalse(gr.pinch(a, km));

		//Fazer teste de flexão
		
	}

	TEST_METHOD(LeapMotionGestures_wristFlex) {

		LeapMotionGestures gr = LeapMotionGestures();
		vrpn_TRACKERCB trackData = getTrackerCB();
		SkeletonPart skelPart;
		//Define a posicao da palma
		trackData.pos[0] = 1;
		trackData.pos[1] = 2;
		trackData.pos[2] = 0;
		trackData.sensor = 0;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.handSkeletonMap1, trackData, skelPart, "Tracker0");
		//cotovelo
		trackData.pos[0] = 1;
		trackData.pos[1] = 1;
		trackData.pos[2] = 1;
		trackData.sensor = 1;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.handSkeletonMap1, trackData, skelPart, "Tracker0");
		//punho
		trackData.pos[0] = 1;
		trackData.pos[1] = 1;
		trackData.pos[2] = 0;
		trackData.sensor = 2;
		vrpnToSkeleton(gr.skeleton["Tracker0"], gr.handSkeletonMap1, trackData, skelPart, "Tracker0");
		
		//Esses métodos não usam KeyMap mas está convencionado que todos os métodos devem receber por parâmetro
		json js = {
			{ "divClass", "leapWrist" },
			{ "devType" , "leapMotion" },
			{"dev" , "LeapMotion0@localhost"},
			{"key" , "LEAP_WRIST_UP"},
			{"angleMod" , "<"},
			{"angle" , 130},
			{"toKeyDown" , "C"},
			{"toKeyUp" , "C"}
		};
		KeyMap * km = new KeyMap(js);

		gr.wristFlexedUp(skelPart,km);

		//Fazer teste de flexão

	}



};
}