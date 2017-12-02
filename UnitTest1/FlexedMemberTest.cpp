#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "KeyMap.h"
#include "Config.h"
#include "FlexedMember.h"
#include "utilTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FlexedMemberTest {

TrackerUserCallback *tc1 = new TrackerUserCallback;
vrpn_TRACKERCB t;
vrpn_BUTTONCB b;
FlexedMember flex;
char keybd[] = "Keyboard0@localhost";
std::map<int, std::vector<double>> positions;




TEST_CLASS(FlexedMemberTest) {
	public:

	TEST_METHOD(FlexedMember_getPoints) {

		SkeletonPart skelPart1 = getSkeletonPart();
		SkeletonPart skelPart2 = getSkeletonPart();
		SkeletonPart skelPart3 = getSkeletonPart();

		positions = flex.getPoints(skelPart1, skelPart2, skelPart3);

		Assert::AreEqual(3, (int)positions.size() );

	}

	TEST_METHOD(FlexedMember_flexed3d) {
		
		std::map<int, std::vector<double>> positionsWithValues;

		std::vector<double> p1 = { 0,0,0 };
		std::vector<double> p2 = { 10,0,0 };
		std::vector<double> p3 = { 12,5,0 };

		positionsWithValues.insert_or_assign(0, p1);
		positionsWithValues.insert_or_assign(1, p2);
		positionsWithValues.insert_or_assign(2, p3);

		json js = {
			{ "divClass", "leapWrist" },
			{ "devType" , "leapMotion" },
			{ "dev" , "LeapMotion0@localhost" },
			{ "key" , "LEAP_WRIST_UP" },
			{ "angleMod" , "<" },
			{ "angle" , 115 },
			{ "toKeyDown" , "C" },
			{ "toKeyUp" , "C" }
		};
		KeyMap * km = new KeyMap(js);
		
		Assert::AreEqual(1, flex.flexed3d(positionsWithValues, km, UP));//maior que 110
		Assert::AreEqual(1, flex.flexed3d(positionsWithValues, km, UP));//menor que 115

	}
};
}