#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <map>
#include "Config.h"
#include "Storage.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vrpn_Tracker.h>
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StorageTest {



std::map<std::string,std::string> configMap = { {"HOST","tcp://127.0.0.1:3306"},
												{"DB","vrpn"},
												{"USER","root" },
												{"PASSWD","" },
												{"PATIENT","PAULO"},
												{"SAVE_DIR", "SAVES"} };

Config config = Config(configMap);
Storage st = Storage(config, true);

	TEST_CLASS(StorageTest) {
public:

	TEST_METHOD(Storage_saveToFile) {
		st.setSaveDir("SAVES");
		//Cria info
		TrackerUserCallback *tc1 = new TrackerUserCallback;
		strncpy(tc1->name, "Tracker0@localhost", sizeof(tc1->name));

		vrpn_TRACKERCB t = getTrackerCB();

		//Salva info
		Assert::IsTrue(st.saveToFile(tc1, t));
		st.closeOut();

		
		//Abre o arquivo recentemente criado
		std::ifstream fileInput = std::ifstream(st.getFileName().c_str());
		std::string line;
		std::string text = "";
		if ( fileInput.is_open() ) {
			while ( getline(fileInput, line) ) {
				text += line + "\n";
			}
		}


		std::string templateText =
			"DEV	Tracker0@localhost\n"
			"DATE	"+st.getDateStr()+"\n"
			"PATIENT	PAULO\n"
			"SENSOR	0	TIMESTAMP	1	POS	1	1	1	QUAT	0	0	0	1\n";


		Assert::AreEqual(templateText.c_str(),text.c_str());

	}

	TEST_METHOD(Storage_sendFileToDb) {
		st.setSaveDir("SAVES");
		std::string fileName = config.getSaveDir() + "/sendToDbTest.txt";
		std::ofstream fileInput = std::ofstream(fileName);
		std::string content = "DEV	Tracker0@localhost\n"
			"DATE	2017-09-27 19:19:02\n"
			"PATIENT	PAULO\n"
			"SENSOR	0	TIMESTAMP	1	POS	1	1	1	QUAT	1	1	1	1\n";
		if ( fileInput.is_open() ) {
			fileInput << content;
			fileInput.close();
			
			try {
				st.sendFileToDb((char *)fileName.c_str());
				Assert::AreEqual(("SYNC\n" + content).c_str(), st.getExpFullContent().c_str());
				Assert::AreEqual("Tracker0@localhost", st.getExpDev().c_str());
				Assert::AreEqual("PAULO", st.getExpPatientName().c_str());
				Assert::AreEqual("2017-09-27 19:19:02", st.getExpDateTime().c_str());
			} catch ( std::exception ex ) {
				const size_t cSize = strlen(ex.what()) + 1;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs(wc, ex.what(), cSize);

				Assert::Fail(wc);
			}


		} else {
			Assert::Fail(L"Falha ao criar arquivo.");
		}
	}

	TEST_METHOD(Storage_checkSent) {
		std::string fileName = "checkSent/checkSent-1.txt";
		std::ofstream fileInput = std::ofstream(fileName);
		std::string content = "DEV	Tracker0@localhost\n"
			"DATE	2017-09-27 19:19:02\n"
			"PATIENT	PAULO\n"
			"SENSOR	0	TIMESTAMP	1	POS	1	1	1	QUAT	1	1	1	1\n";

		if ( fileInput.is_open() ) {
			fileInput << content;
			fileInput.close();
		} else {
			Assert::Fail(L"Falha ao criar arquivo.");
		}

		fileName = "checkSent/checkSent-2.txt";
		fileInput = std::ofstream(fileName);

		if ( fileInput.is_open() ) {
			fileInput << "SYNC\n";
			fileInput << content;
			fileInput.close();
		} else {
			Assert::Fail(L"Falha ao criar arquivo.");
		}

		//seta o diretorio para esse teste
		st.setSaveDir("checkSent");
		
		try {
			//esse teste tambem envolve garantir que o arquivo com SYNC nao foi enviado
			Assert::AreEqual(1, st.checkSent());
		} catch ( std::exception ex ) {
			const size_t cSize = strlen(ex.what()) + 1;
			wchar_t* wc = new wchar_t[cSize];
			mbstowcs(wc, ex.what(), cSize);
		}
	}

	};
}