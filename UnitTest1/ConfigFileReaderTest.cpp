#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "KeyMap.h"
#include "Config.h"
#include "ConfigFileReader.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConfigFileReaderTest {

Config config;
std::vector<DeviceType> devs;
std::vector<KeyMap> map;
ConfigFileReader cf = ConfigFileReader();


	TEST_CLASS(ConfigFileReaderTest) {
public:

	TEST_CLASS_INITIALIZE(init) {
		cf.readConfigFile("config.json", devs, map, config);
	}


	TEST_METHOD(ConfigFileReader_readConfigFile) {
		Assert::IsTrue(cf.readConfigFile("config.json", devs, map, config));
	}

	TEST_METHOD(ConfigFileReader_ConfigOk) {
		Assert::AreEqual("app", config.getApp().c_str());
		Assert::AreEqual("Paulo Júnior", config.getPatient().c_str());
		Assert::AreEqual("db", config.getDb().c_str());
		Assert::AreEqual("host", config.getHost().c_str());
		Assert::AreEqual("user", config.getUser().c_str());
		Assert::AreEqual("passwd", config.getPasswd().c_str());
		Assert::AreEqual("./SAVES/", config.getSaveDir().c_str());
	}

	TEST_METHOD(ConfigFileReader_DevsOk) {
		Assert::AreEqual("Tracker0@localhost", devs.at(0).name.c_str());
		Assert::AreEqual(DEVTYPE_KINECT, devs.at(0).type);
		Assert::AreEqual("LeapMotion0@localhost", devs.at(1).name.c_str());
		Assert::AreEqual(DEVTYPE_LEAPMOTION, devs.at(1).type);
		Assert::AreEqual("Keyboard0@localhost", devs.at(2).name.c_str());
		Assert::AreEqual(DEVTYPE_KEYBOARD, devs.at(2).type);
		Assert::AreEqual("Mouse0@localhost", devs.at(3).name.c_str());
		Assert::AreEqual(DEVTYPE_MOUSE, devs.at(3).type);
		Assert::AreEqual("Analog0@localhost", devs.at(4).name.c_str());
		Assert::AreEqual(DEVTYPE_NEDGLOVE, devs.at(4).type);
	}

	TEST_METHOD(ConfigFileReader_MapsOk) {
		//Para esse teste o arquivo config.json deve seguir a exata sequencia.
		int i = 0;
		json js = {
			{"divClass", "handTop"},
			{"dev" , "Tracker0@localhost"},
			{"key" , "KINECT_LEFT_HAND_TOP"},
			{"x" , -2},
			{"coordinateMod" , "<="},
			{"y" , 5},
			{"toKeyDown" , "A"},
			{"toKeyUp" , "A"}
		};
		KeyMap m1 = KeyMap(js);
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		js = {
			{"divClass", "kinectWalking"},
			{"dev" , "Tracker0@localhost"},
			{"key" , "KINECT_WALK"},
			{"delay" , 1000},
			{"sensivity" , 0.1},
			{ "toKeyWhile" , "A" }
		};

		m1 = KeyMap(js);
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		js = {
			{"divClass","kinectStep"},
			{"dev" ,"Tracker0@localhost"},
			{"key" ,"KINECT_STEP_UP"},
			{"sensivity" ,0.1},
			{"toKeyDown" ,"KINECT_DETERMINE_CENTER_POS"},
			{"toKeyUp" ,"KINECT_DETERMINE_CENTER_POS"}
		};
		m1 = KeyMap(js);
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		js = {
			{"divClass","kinectBalance"},
			{"dev" ,"Tracker0@localhost"},
			{"key" ,"KINECT_BALANCE"},
			{"angleMod" ,">"},
			{"angle" ,35},
			{"msg" ,"Mensagem"},
			{"toKeyDown" ,"ALERT"},
			{"toKeyUp" ,"ALERT"}
		};
		m1 = KeyMap(js);
		Assert::AreEqual(m1.toString(), map.at(i++).toString());


		js = {
			{ "divClass","nedGlove" },
			{ "dev" ,"Analog0@localhost" },
			{ "key" ,"NEDGLOVE_PINCH" },
			{ "strengthMin" ,40 },
			{ "strengthMax" ,0 },
			{ "toKeyDown" ,"A" },
			{ "toKeyUp" ,"A" }
		};
		m1 = KeyMap(js);
		Assert::AreEqual(m1.toString(), map.at(i+9).toString());

		
	}

	
	};
}