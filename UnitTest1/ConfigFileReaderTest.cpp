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
std::vector<std::string> devs;
std::vector<KeyMap> map;
ConfigFileReader cf = ConfigFileReader();



	TEST_CLASS(ConfigFileReaderTest) {
public:

	TEST_CLASS_INITIALIZE(init) {
		cf.readConfigFile("vrpn-client.cfg", devs, map, config);
	}


	TEST_METHOD(ConfigFileReader_readConfigFile) {
		Assert::IsTrue(cf.readConfigFile("vrpn-client.cfg", devs, map, config));
	}

	TEST_METHOD(ConfigFileReader_ConfigOk) {
		Assert::AreEqual("Notepad", config.getApp().c_str());
		Assert::AreEqual("Paulo Júnior Pereira Franco", config.getPatient().c_str());
		Assert::AreEqual("db", config.getDb().c_str());
		Assert::AreEqual("host", config.getHost().c_str());
		Assert::AreEqual("passwd", config.getPasswd().c_str());
		Assert::AreEqual("savedir", config.getSaveDir().c_str());
	}

	TEST_METHOD(ConfigFileReader_DevsOk) {
		Assert::AreEqual("Tracker0@localhost", devs.at(0).c_str());
		Assert::AreEqual("Mouse0@localhost", devs.at(1).c_str());
		Assert::AreEqual("Keyboard0@localhost", devs.at(2).c_str());
	}

	TEST_METHOD(ConfigFileReader_MapsOk) {

		int i = 0;
		
		KeyMap m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_ADD=0.15 A");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_DEC=0.15	D");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		m1 = KeyMap("Tracker0@localhost", "KINECT_LEFT_HAND_TOP=5 O");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		m1 = KeyMap("Tracker0@localhost", "KINECT_RIGHT_HAND_TOP=2,3 9");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		m1 = KeyMap("Mouse0@localhost", "VK_LBUTTON	A");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		m1 = KeyMap("Mouse0@localhost", "VK_MBUTTON	VK_LEFT");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		m1 = KeyMap("Mouse0@localhost", "VK_RBUTTON	B");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		m1 = KeyMap("Keyboard0@localhost", "1	1");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		m1 = KeyMap("Keyboard0@localhost", "A	B");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());
		m1 = KeyMap("Keyboard0@localhost", "VK_LEFT	VK_LEFT");
		Assert::AreEqual(m1.toString(), map.at(i++).toString());

		
	}

	
	};
}