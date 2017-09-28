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
		KeyMap m1 = KeyMap("Tracker0@localhost", "KINECT_TOP_ADD", "A", "0.15");
		Assert::AreEqual("Tracker0@localhost", m1.getDev().c_str());
		Assert::AreEqual(KINECT_TOP_ADD, m1.getKey());
		Assert::AreEqual('A', m1.getToKey());
		Assert::AreEqual(0.15, m1.getHeightSens());
		Assert::AreEqual(false, m1.getToKeyIsConstant());

		m1 = KeyMap("Mouse0@localhost", "VK_LBUTTON", "VK_RIGHT");
		Assert::AreEqual("Mouse0@localhost", m1.getDev().c_str());
		Assert::AreEqual(0, m1.getKey());
		Assert::AreEqual(VK_RIGHT, (int)m1.getToKey());
		Assert::AreEqual(true, m1.getToKeyIsConstant());

		m1 = KeyMap("Keyboard0@localhost", "A", "B");
		Assert::AreEqual("Keyboard0@localhost", m1.getDev().c_str());
		Assert::AreEqual(30, m1.getKey());
		Assert::AreEqual((int)'B', (int)m1.getToKey());
		Assert::AreEqual(false, m1.getToKeyIsConstant());
	}
};
}