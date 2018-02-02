#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "KeyMap.h"
#include "Config.h"
#include "ConfigFileReader.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KeyMapTest {

	TEST_CLASS(KeyMapTest) {
public:


	TEST_METHOD(KeyMapTest_init) {
		json js = {
			{ "divClass", "handTop" },
			{ "dev" , "Tracker0@localhost" },
			{ "devType" ,"kinect" },
			{ "key" , "KINECT_LEFT_HAND_TOP" },
			{ "x" , -2 },
			{ "coordinateMod" , "<=" },
			{ "y" , 5 },
			{ "toKeyDown" , "A" },
			{ "toKeyUp" , "A" }
		};
		KeyMap m1 = KeyMap(js);
		Assert::AreEqual("Tracker0@localhost", m1.getDev().c_str());
		Assert::AreEqual(KINECT_LEFT_HAND_TOP, m1.getKey());
		Assert::AreEqual((int)'A', m1.getToKey());
		Assert::AreEqual(-2, m1.getX());
		Assert::AreEqual(5, m1.getY());
		Assert::AreEqual(-1, m1.getCoordinateMod());
		Assert::AreEqual(false, m1.getToKeyIsConstant());

		js = {
			{ "divClass", "kinectWalking" },
			{ "dev" , "Tracker0@localhost" },
			{ "devType" ,"kinect" },
			{ "key" , "KINECT_WALK" },
			{ "delay" , 1000 },
			{ "sensivity" , 0.1 },
			{ "toKeyWhile" , "A" }
		};
		m1 = KeyMap(js);
		Assert::AreEqual(1000, m1.getDelay());
		Assert::AreEqual(0.1, m1.getSensivity());
		Assert::AreEqual((int)'A', (int)m1.getToKey());


		js = {
			{ "divClass","kinectBalance" },
			{ "dev" ,"Tracker0@localhost" },
			{ "devType" ,"kinect" },
			{ "key" ,"KINECT_BALANCE_LEFT" },
			{ "angleMax" ,40 },
			{ "angleMin" ,35 },
			{ "msg" ,"Mensagem" },
			{ "toKeyDown" ,"ALERT" },
			{ "toKeyUp" ,"ALERT" }
		};
		m1 = KeyMap(js);
		Assert::AreEqual(40.0f, m1.getAngleMax());
		Assert::AreEqual(35.0f, m1.getAngleMin());
		Assert::AreEqual((std::string)"Mensagem", m1.getMsg());
		Assert::AreEqual(ALERT, m1.getToKey());

		js = {
			{ "divClass", "kinectFastHand" },
			{ "dev" , "Tracker0@localhost" },
			{ "devType" ,"kinect" },
			{ "key" , "KINECT_LEFT_HAND_FAST" },
			{ "maxVelocityMs" , 4 },
			{ "msg" ,"Mensagem" },
			{ "toKeyDown" , "MESSAGE" },
			{ "toKeyUp" , "MESSAGE" }
		};
		m1 = KeyMap(js);
		Assert::AreEqual(4, (int)m1.getMaxVelocityMs());
		Assert::AreEqual((std::string)"Mensagem", m1.getMsg());
		Assert::AreEqual(MESSAGE, m1.getToKey());

	}
};
}