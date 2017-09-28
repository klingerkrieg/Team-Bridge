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

	TEST_METHOD(KeyMapTest_init) {
		//O teste do KeyMap deve ficar nesse arquivo por conta do VS2017 que nao permite
		KeyMap m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_ADD", "A", "0.15");
		Assert::AreEqual("Tracker0@localhost", m1.getDev().c_str());
		Assert::AreEqual(KINECT_TOP_ADD, m1.getKey());
		Assert::AreEqual('A', m1.getToKey());
		Assert::AreEqual(0.15, m1.getHeightSens());
		Assert::AreEqual(false, m1.getToKeyIsConstant());


		m1 = KeyMap("Keyboard0@localhost", "VK_LEFT", "VK_RIGHT");
		Assert::AreEqual("Keyboard0@localhost", m1.getDev().c_str());
		Assert::AreEqual(VK_LEFT, m1.getKey());
		Assert::AreEqual(VK_RIGHT, (int)m1.getToKey());
		Assert::AreEqual(true, m1.getToKeyIsConstant());
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
		
		KeyMap m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_ADD", "A", "0.15");
		Assert::AreEqual(m1.toString(), map.at(0).toString());
		m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_DEC", "D", "0.15");
		Assert::AreEqual(m1.toString(), map.at(1).toString());

		m1 = KeyMap("Mouse0@localhost", "0", "A");
		Assert::AreEqual(m1.toString(), map.at(2).toString());
		m1 = KeyMap("Mouse0@localhost", "1", "VK_LEFT");
		Assert::AreEqual(m1.toString(), map.at(3).toString());
		m1 = KeyMap("Mouse0@localhost", "2", "B");
		Assert::AreEqual(m1.toString(), map.at(4).toString());

		m1 = KeyMap("Keyboard0@localhost", "2", "U");
		Assert::AreEqual(m1.toString(), map.at(5).toString());
		m1 = KeyMap("Keyboard0@localhost", "3", "U");
		Assert::AreEqual(m1.toString(), map.at(6).toString());
	}

	
	};
}