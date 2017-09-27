#pragma once
#include <string>
#include <fstream>

class FileParser {

protected:
	std::ofstream fileOutput;
	std::ifstream fileInput;
	bool isOpenOut = false;
	bool isOpenIn = false;


public:


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
			if ( line[j] == '#' ) {
				ignore = true;
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