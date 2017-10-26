#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "util.h"

class FileParser {

protected:
	std::ofstream fileOutput;
	std::ifstream fileInput;
	bool isOpenOut = false;
	bool isOpenIn = false;


public:

	std::string removeComments(std::string source) {

		while ( source.find("/*") != std::string::npos ) {
			size_t Beg = source.find("/*");
			source.erase(Beg, (source.find("*/", Beg) - Beg) + 2);
		}
		while ( source.find("//") != std::string::npos ) {
			size_t Beg = source.find("//");
			source.erase(Beg, source.find("\n", Beg) - Beg);
		}
		while ( source.find("#") != std::string::npos ) {
			size_t Beg = source.find("#");
			source.erase(Beg, source.find("\n", Beg) - Beg);
		}
		return source;
	}
	

	bool ignoreLine(std::string line) {
		
		while ( contains(line, " ") ) {
			replace_all(line, " ", "");
		}
		while ( contains(line, "\t") ) {
			replace_all(line, "\t", "");
		}

		if (line.size() == 0) {
			return true;
		}
		return false;
	}

	bool openOut(std::string fileName) {
		fileOutput = std::ofstream(fileName);
		isOpenOut = true;
		return true;
	}

	bool openIn(std::string fileName) {
		fileInput = std::ifstream(fileName);
		isOpenIn = true;
		return true;
	}

	void closeIn() {
		if ( isOpenIn ) {
			fileInput.close();
			isOpenIn = false;
		}
	}

	void closeOut() {
		if ( isOpenOut ) {
			isOpenOut = false;
			fileOutput.close();
		}
	}

	void close() {
		closeIn();
		closeOut();
	}

};