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

		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));

		t = getTrackerCB();
		//Nao captura
		t.sensor = 5;
		flex.getPoints(t, 6, 7, 8, positions);
		
		Assert::AreEqual(0, (int)positions.size());
		
		t.sensor = 6;
		double pos1[] = { 0, 0, 0 };
		std::copy(pos1, pos1 + 3, t.pos);
		flex.getPoints(t, 6, 7, 8, positions);
		
		t.sensor = 7;
		double pos2[] = { 10, 0, 0 };
		std::copy(pos2, pos2 + 3, t.pos);
		flex.getPoints(t, 6, 7, 8, positions);

		t.sensor = 8;
		double pos3[] = { 12, 5, 0 };
		std::copy(pos3, pos3 + 3, t.pos);
		flex.getPoints(t, 6, 7, 8, positions);

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

		Assert::AreEqual(1, flex.flexed3d(positionsWithValues, 110, 1, UP));//maior que 110
		Assert::AreEqual(1, flex.flexed3d(positionsWithValues, 115, -1, UP));//menor que 115

	}
};
}