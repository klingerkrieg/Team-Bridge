#include "stdafx.h"
#include "CppUnitTest.h"

#include <client.h>
/*
Caso não consiga realizar o teste por não encontrar o arquivo cliente.~bh
Verifique se o VisualStudio está no modo Debug, o modo Release não está configurado
*/

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