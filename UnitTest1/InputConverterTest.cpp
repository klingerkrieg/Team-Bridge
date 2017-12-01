#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include "InputConverter.h"
#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <math.h>
#include "utilTest.h"
#include "KeyPressAction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InputConverterTest {

	TrackerUserCallback *tc1 = new TrackerUserCallback;
	vrpn_TRACKERCB t;
	vrpn_BUTTONCB b;
	InputConverter iC;
	char keybd[] = "Keyboard0@localhost";
	char analog[] = "Analog0@localhost";

	TEST_CLASS(InputConverterTest) {

	TEST_CLASS_INITIALIZE(InputConverter_init) {
		
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));

		t = getTrackerCB();
		double heightSens = 0.15;

		b = vrpn_BUTTONCB();
		b.msg_time = getTimeVal();
		b.state = 1;
		b.button = 10;


		std::vector<KeyMap> map;
		json js = {
			{"divClass", "kinectStep"},
			{"dev" , "Tracker0@localhost"},
			{"key" , "KINECT_STEP_UP"},
			{"sensivity" , 0.15},
			{"toKeyDown" , "A"},
			{"toKeyUp" , "A"}
		};
		KeyMap m1 = KeyMap(js);
		js = {
			{ "divClass", "kinectStep" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_STEP_NORMAL" },
			{ "sensivity" , 0.15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap m2 = KeyMap(js);
		js = {
			{ "divClass", "kinectStep" },
			{ "dev" , "Tracker0@localhost" },
			{ "key" , "KINECT_STEP_DOWN" },
			{ "sensivity" , 0.15 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap m3 = KeyMap(js);

		js = {
			{ "divClass", "keyboard" },
			{ "dev" , "Keyboard0@localhost" },
			{ "key" , "A" },
			{ "toKeyDown" , "B" },
			{ "toKeyUp" , "B" }
		};
		KeyMap m4 = KeyMap(js);
		js = {
			{ "divClass","nedGlove" },
			{ "dev" ,"Analog0@localhost" },
			{ "key" ,"NEDGLOVE_PINCH" },
			{ "thumb" , 0 },
			{ "index" , 4 },
			{ "strengthMin" ,40 },
			{ "strengthMax" ,0 },
			{ "toKeyUp" ,"A" }
		};
		KeyMap m5 = KeyMap(js);

		map.push_back(m1);
		map.push_back(m2);
		map.push_back(m3);
		map.push_back(m4);
		map.push_back(m5);

		std::string app = "";

		std::vector<DeviceType> devs;
		DeviceType dt;
		dt.name = "Tracker0@localhost";
		dt.type = DEVTYPE_KINECT;
		dt.type_str = DEVTYPE_STR[dt.type];
		devs.push_back(dt);

		dt.name = "Keyboard0@localhost";
		dt.type = DEVTYPE_KEYBOARD;
		dt.type_str = DEVTYPE_STR[dt.type];
		devs.push_back(dt);

		KeyPressAction *keyPresser = new KeyPressAction();

		iC = InputConverter(map, devs, keyPresser);
	}


	
public:

	TEST_METHOD(InputConverter_checkTrack) {
	
			t.sensor = 0;
			t.pos[1] = 1.0;
			//Inicia
			Assert::IsTrue(iC.checkTrack(tc1, t));
			
			//Sobe
			t.pos[1] = 1.151;
			Assert::IsTrue(iC.checkTrack(tc1, t));
			//Mantem
			Assert::IsTrue(iC.checkTrack(tc1, t));

			//normal
			t.pos[1] = 1.0;
			Assert::IsTrue(iC.checkTrack(tc1, t));

			//Desce
			t.pos[1] = 0.75;
			Assert::IsTrue(iC.checkTrack(tc1, t));


	}

	TEST_METHOD(InputConverter_checkAnalog) {

		vrpn_ANALOGCB a = getAnalogCB();

		NEDGloveGestures gr = NEDGloveGestures();
		int closedCalib = 205;

		int val = 205;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		//Mapeada
		Assert::IsTrue(iC.checkAnalog(analog, a));

		val = 240;
		a.channel[0] = val;
		a.channel[1] = val;
		a.channel[2] = val;
		a.channel[3] = val;
		a.channel[4] = val;
		//Nao mapeada
		Assert::IsTrue(iC.checkAnalog(analog, a));

	}

	TEST_METHOD(InputConverter_checkButton) {

		//Inicia
		b.button = 31;//S
		Assert::IsFalse(iC.checkButton(keybd, b));
		b.button = 30;//A mapeada
		Assert::IsTrue(iC.checkButton(keybd, b));

	}




	};
}