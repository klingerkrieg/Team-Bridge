#pragma once
#include <string>
#include <fstream>
#include <iostream>

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
		// Ignore comments and empty lines.  Skip white space before comment
		// mark (#).
		if ( line.length() < 3 ) {
			return true;
		}
		bool ignore = false;
		for ( int j = 0; line[j] != '\0'; j++ ) {
			if ( line[j] == ' ' || line[j] == '\t' ) {
				return true;
			}
			break;
		}
		return ignore;
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