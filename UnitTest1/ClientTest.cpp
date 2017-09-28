#include "stdafx.h"
#include "CppUnitTest.h"

#include <client.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClientTest {

	TEST_CLASS(ClientTest) {
public:

	TEST_METHOD(ClientTest_setup) {
		Client c = Client();
		Assert::IsTrue(c.setup(true));
	}

	};
}