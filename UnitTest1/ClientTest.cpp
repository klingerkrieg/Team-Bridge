#include "stdafx.h"
#include "CppUnitTest.h"

#include <client.h>
/*
Caso n�o consiga realizar o teste por n�o encontrar o arquivo cliente.~bh
Verifique se o VisualStudio est� no modo Debug, o modo Release n�o est� configurado
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