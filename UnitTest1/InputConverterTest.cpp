#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "InputConverter.h"
#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <math.h>
#include "utilTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InputConverterTest {

	TrackerUserCallback *tc1 = new TrackerUserCallback;
	vrpn_TRACKERCB t;
	vrpn_BUTTONCB b;
	InputConverter iC;
	char keybd[] = "Keyboard0@localhost";

	TEST_MODULE_INITIALIZE(InputConverter_init) {
		
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));

		t = getTrackerCB();
		double heightSens = 0.15;

		b = vrpn_BUTTONCB();
		b.msg_time = getTimeVal();
		b.state = 1;
		b.button = 10;


		std::vector<KeyMap> map;

		KeyMap m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_ADD=0.15	A");
		KeyMap m2 = KeyMap("Tracker0@localhost", "KINECT_TOP_DEC=0.15	A");
		KeyMap m3 = KeyMap("Keyboard0@localhost", "A	D");
		KeyMap m4 = KeyMap("Keyboard0@localhost", "VK_LEFT	VK_RIGHT");

		map.push_back(m1);
		map.push_back(m2);
		map.push_back(m3);
		map.push_back(m4);

		std::string app = "";

		iC = InputConverter(map, app);
	}


	TEST_CLASS(InputConverterTest) {
public:

	TEST_METHOD(InputConverter_checkTrack) {
		
			t.sensor = 0;
			//Inicia
			Assert::IsFalse(iC.checkTrack(tc1, t));
			
			//Sobe
			t.pos[1] = 1.151;
			Assert::IsTrue(iC.checkTrack(tc1, t));
			//Mantem
			Assert::IsFalse(iC.checkTrack(tc1, t));

			//Desce
			t.pos[1] = 1.0;
			Assert::IsTrue(iC.checkTrack(tc1, t));
		

	}

	TEST_METHOD(InputConverter_checkButton) {

		//Inicia
		b.button = 31;//S
		Assert::IsFalse(iC.checkButton(keybd, b));
		b.button = 30;//A mapeada
		Assert::IsTrue(iC.checkButton(keybd, b));
		b.button = 77;//seta direita
		Assert::IsFalse(iC.checkButton(keybd, b));
		b.button = 75;//seta esquerda mapeada
		Assert::IsTrue(iC.checkButton(keybd, b));

	}


	};
}