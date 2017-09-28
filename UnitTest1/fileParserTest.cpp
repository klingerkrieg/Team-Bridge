#include "stdafx.h"
#include "CppUnitTest.h"
#include "FileParser.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FileParserTest {


class FileParserT : public FileParser {
public:

	FileParserT() {

	}

	void write(std::string str) {
		fileOutput << "test";
	}

	void read(std::string &readText) {
		getline(fileInput, readText);
	}

	bool ignoreLine(std::string line) {
		return FileParser::ignoreLine(line);
	}

	bool getIsOpenOut() {
		return isOpenOut;
	}

	bool getIsOpenIn() {
		return isOpenIn;
	}
};


	TEST_CLASS(FileParserTest) {
public:

	TEST_METHOD(FileParser_ignoreLine) {
		FileParserT f = FileParserT();
		Assert::IsFalse(f.ignoreLine("DEV 1"));
		Assert::IsTrue(f.ignoreLine("#DEV 1"));
		Assert::IsTrue(f.ignoreLine(" # DEV 1"));
	}

	TEST_METHOD(FileParser_openWriteAndRead) {
		FileParserT f = FileParserT();
		std::string testText = "test";
		Assert::IsTrue(f.openOut("test.txt"));
		Assert::IsTrue(f.getIsOpenOut());
		f.write(testText);
		f.closeOut();
		Assert::IsFalse(f.getIsOpenOut());

		Assert::IsTrue(f.openIn("test.txt"));
		std::string readText;
		f.read(readText);
		Assert::AreEqual(readText, testText);
		f.closeIn();
		Assert::IsFalse(f.getIsOpenOut());


	}
};
}