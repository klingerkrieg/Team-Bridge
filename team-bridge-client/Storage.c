#include "Storage.h"

// Get current date/time, format is YYYY-MM-DD_HH-mm
const std::string currentDateTime(std::string format) {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), format.c_str(), &tstruct);

	return buf;
}

bool Storage::startFile(const char * name) {
	if ( isOpenOut == false ) {
		if ( fileName == "" ) {
			dateStr = currentDateTime("%Y-%m-%d %H:%M:%S");
			fileName = Storage::saveDir + "/" + currentDateTime("%Y-%m-%d_%H-%M-%S");
			if ( csvMode ) {
				fileName += ".csv";
			} else {
				fileName += ".txt";
			}
		} else {
			fileName = Storage::saveDir + "/" + fileName;
			if ( csvMode ) {
				fileName += ".csv";
			} else {
				fileName += ".txt";
			}
		}

		if ( !openOut(fileName) ) {
			std::cout << "Nao foi possivel abrir arquivo: " << fileName << "\n";
			return false;
		} else {
			if ( infoData ) {
				if ( csvMode ) {
					fileOutput << "DEV;" << name << ";\n";
					fileOutput << "DATE;" << dateStr << ";\n";
					fileOutput << "PATIENT;" << patient << ";\n";
					fileOutput << "DEV;Sensor;PosX;posY;posZ;\n";
				} else {
					fileOutput << "DEV\t" << name << "\n";
					fileOutput << "DATE\t" << dateStr << "\n";
					fileOutput << "PATIENT\t" << patient << "\n";
				}
			}
			std::cout << fileName << " Criado.\n";

		}
	}
	return true;
}


bool Storage::saveToFile(const char * dev, std::string actionName, double value) {
	//Cria cabeçalho no arquivo
	startFile(dev);
	if ( fileOutput.is_open() ) {
		if ( csvMode ) {
			//fileOutput << dev << ";" << actionName << ";" << value << ";\n";
			if ( value ) {
				fileOutput << "---------------------- " << actionName << " " << value << " ----------------------------\n";
			}
		} else {
			fileOutput << dev << "\tACTION\t" << actionName << "\tVALUE\t" << value << "\n";
		}
		return true;
	} else {
		return false;
	}
}

bool Storage::saveToFile(const char * name, const vrpn_ANALOGCB a) {
	//Cria cabeçalho no arquivo
	startFile(name);
	if ( fileOutput.is_open() ) {

		if ( csvMode ) {
			fileOutput << name;
			for ( int i = 0; i < a.num_channel; i++ ) {
				fileOutput << ";" << a.channel[i];
			}
			fileOutput << ";\n";
		} else {
			fileOutput << name << "\tTIMESTAMP\t" << a.msg_time.tv_usec << "\tCHANNEL_COUNT\t" << a.num_channel << "\tCHANNELS";
			for ( int i = 0; i < a.num_channel; i++ ) {
				fileOutput << "\t" << a.channel[i];
			}
			fileOutput << "\n";
		}
		return true;
	} else {
		return false;
	}
}

bool Storage::saveToFile(const char * name, const vrpn_BUTTONCB b) {
	//Cria cabeçalho no arquivo
	startFile(name);
	if ( fileOutput.is_open() ) {

		if ( csvMode ) {
			fileOutput << name << ";" << b.button << ";" << b.state << ";\n";
		} else {
			fileOutput << name << "\tTIMESTAMP\t" << b.msg_time.tv_usec << "\tBUTTON\t" << b.button << "\tSTATE\t" << b.state << "\n";
		}

		return true;
	} else {
		return false;
	}
}

bool Storage::saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	//Cria cabeçalho no arquivo
	startFile(userdata->name);

	if ( csvMode ) {
		//Tracker0	sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
		if ( fileOutput.is_open() ) {
			fileOutput << userdata->name << ";" << t.sensor
				<< ";" << t.pos[0] << ";" << t.pos[1] << ";" << t.pos[2] << ";\n";
				//<< "\tQUAT\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
			return true;
		} else {
			return false;
		}
	} else {
		//Tracker0	sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
		if ( fileOutput.is_open() ) {
			fileOutput << userdata->name << "\tSENSOR\t" << t.sensor << "\tTIMESTAMP\t" << t.msg_time.tv_usec
				<< "\tPOS\t" << t.pos[0] << "\t" << t.pos[1] << "\t" << t.pos[2]
				<< "\tQUAT\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
			return true;
		} else {
			return false;
		}
	}

	

}


int Storage::checkSent() {
	HANDLE hFind;
	WIN32_FIND_DATA data;
	int filesSent = 0;

	hFind = FindFirstFile((saveDir + "/*.txt").c_str(), &data);
	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			try {
				if ( !sendFileToDb((char *)(Storage::saveDir + "/" + data.cFileName).c_str()) ) {
					printf("Envio interrompido, %s.\n", data.cFileName);
				} else {
					filesSent++;
				}
			} catch ( AlreadySent ex ) {
				//nao sinaliza erro, apenas nao conta
			}
		} while ( FindNextFile(hFind, &data) );
		FindClose(hFind);
	}

	return filesSent;
}

bool Storage::sendFileToDb(char * fileName) {


	
	expFullContent = "SYNC\n";
	std::string line;
	openIn(fileName);
	char *pch;

	char scrap[LINESIZE];


	int idPatient = -1;
	int idDevice = -1;
	int idSession = -1;

	int timestamp, channelCount, sensor, buttonNumber, buttonState, idAction;
	char dev[512], channels[512], action[512];
	double pos[3], quat[4], value;
	char s1[LINESIZE], s2[LINESIZE];


	std::map<std::string, std::string> values;
	int processedLines = 0;
	bool oneExported = false;
	std::vector<std::string> channelsStr;

	//Conecta
	if ( !db.connect() ) {
		throw std::exception("Nao pode conectar ao banco de dados.");
		return false;
	}


	if ( fileInput.is_open() ) {

		int lines = (int)std::count(std::istreambuf_iterator<char>(fileInput),
									std::istreambuf_iterator<char>(), '\n');

		if ( lines < 3 ) {
			return false;
		}

		printf("Enviando: %s\n", fileName);
		fileInput.seekg(0);

		while ( getline(fileInput, line) ) {



			expFullContent += line + "\n";

			//Ignora linhas que iniciam com #
			if ( ignoreLine((char *)line.c_str()) ) {
				continue;
			}

			strncpy(scrap, line.c_str(), LINESIZE - 1);
			scrap[sizeof(scrap) - 1] = '\0';


			//Identifica o tipo de configuracao que sera lido
			if ( !strcmp(pch = strtok(scrap, " \t"), "SYNC") ) {
				//Se ja tiver sido enviado ao banco
				printf("Ja enviado.\n");
				throw AlreadySent();
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "DEV") ) {
				if ( sscanf(line.c_str(), "%s\t%[^\t\n]", s1, s2) != 2 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					throw std::exception("Falha ao ler linha.");
					return false;
				}
				expDev = s2;
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "PATIENT") ) {
				if (sscanf(line.c_str(), "%s\t%[^\t\n]", s1, s2) != 2){
					fprintf(stderr, "Falha ao ler paciente %s: %s\n", fileName, line.c_str());
					expPatientName = "";
				} else {
					expPatientName = s2;
				}
				
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "DATE") ) {
				if ( sscanf(line.c_str(), "%s\t%[^\t\n]", s1, s2) != 2 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					throw std::exception("Falha ao ler linha.");
					return false;
				}
				expDateTime = s2;
			} else {
				//Caso não seja nenhum acima é porque o cabeçalho já acabou

				if ( idSession == -1 ) {
					//Se nao tem a id da sessao
					if ( expPatientName.compare("") ) {
						values.clear();
						values["name"] = expPatientName;
						idPatient = db.findOrCreate("patients", "idpatient", values);
						if ( idPatient == -1 ) {
							throw std::exception("Falha inserir paciente.");
							return false;
						}
					}

					

					values.clear();
					values["date_time"] = expDateTime;
					if (idPatient != -1)
						values["idpatient"] = std::to_string(idPatient);
					idSession = db.findOrCreate("sessions", "idsession", values);
					if ( idSession == -1 ) {
						throw std::exception("Falha inserir sessao.");
						return false;
					}
				}

				bool trackDev = false;
				bool analogDev = false;
				bool buttonDev = false;
				bool saveAction = false;
				bool readSuccess = false;


				//Tracker0 SENSOR	0	TIME	0	POS	-0.16	 0.51	 1.02	QUAT	-0.19	 0.21	 0.12	 0.85
				if ( sscanf(line.c_str(), "%s\tSENSOR\t%d\tTIMESTAMP\t%d\tPOS\t%lf\t%lf\t%lf\tQUAT\t%lf\t%lf\t%lf\t%lf",
							dev,
							&sensor,
							&timestamp,
							&pos[0], &pos[1], &pos[2],
							&quat[0], &quat[1], &quat[2], &quat[3]) == 10 ) {
					trackDev = true;
					readSuccess = true;
				}

				//Se leitura do tracker não deu certo tenta outro dispositivo
				//Analog0@localhost	TIMESTAMP	277557	CHANNEL_COUNT	5	CHANNELS	245	231	227	214	202
				if ( readSuccess == false && sscanf(line.c_str(), "%s\tTIMESTAMP\t%d\tCHANNEL_COUNT\t%d\tCHANNELS\t%[^\n]",
					dev, &timestamp, &channelCount, channels) == 4 ) {
					analogDev = true;
					readSuccess = true;
					channelsStr = split(channels, "\t");
				}

				//Mouse0@localhost	TIMESTAMP	41790	BUTTON:	0	STATE:	1
				if ( readSuccess == false && sscanf(line.c_str(), "%s\tTIMESTAMP\t%d\tBUTTON\t%d\tSTATE\t%d",
					dev, &timestamp, &buttonNumber, &buttonState) == 4 ) {
					buttonDev = true;
					readSuccess = true;
				}

				//Analog0@localhost	ACTION	strength	VALUE	42
				if ( readSuccess == false && sscanf(line.c_str(), "%s\tACTION\t%s\tVALUE\t%lf",
					dev, action, &value) == 3 ) {
					saveAction = true;
					readSuccess = true;

					values.clear();
					values["action_name"] = action;
					idAction = db.findOrCreate("actions", "idaction", values);
					if ( idAction == -1 ) {
						throw std::exception("Falha inserir action.");
						return false;
					}
				}

				values.clear();
				values["device_name"] = dev;
				idDevice = db.findOrCreate("devices", "iddevice", values);
				if ( idDevice == -1 ) {
					throw std::exception("Falha inserir dispositivo.");
					return false;
				}


				if ( trackDev ) {
					//uint = 4294967295
					//meia hora = 540.000 registros
					//cabem 7953 sessoes
					values.clear();
					values["idsession"] = std::to_string(idSession);
					values["iddevice"] = std::to_string(idDevice);
					values["sensor"] = std::to_string(sensor);
					values["pos_0"] = std::to_string(pos[0]);
					values["pos_1"] = std::to_string(pos[1]);
					values["pos_2"] = std::to_string(pos[2]);
					values["quat_0"] = std::to_string(quat[0]);
					values["quat_1"] = std::to_string(quat[1]);
					values["quat_2"] = std::to_string(quat[2]);
					values["quat_3"] = std::to_string(quat[3]);
					

					if ( db.insert("trackers", values, false) == 0 ) {
						oneExported = true;
					} else {
						throw std::exception("Falha ao inserir no banco linha.");
						return false;
					}
				} else 
				if ( analogDev ) {
					values.clear();
					values["idsession"] = std::to_string(idSession);
					values["iddevice"] = std::to_string(idDevice);
					
					int idAnalog = db.insert("analogs", values, true);

					if ( idAnalog == -1 ) {
						throw std::exception("Falha ao inserir no banco linha.");
						return false;
					}
					values.clear();
					values["idanalog"] = std::to_string(idAnalog);
					for ( size_t i = 0; i < channelsStr.size(); i++ ) {
						values["num"] = std::to_string(i);
						values["value"] = channelsStr[i];

						if ( db.insert("channels", values, false) ) {
							throw std::exception("Falha ao inserir no banco linha.");
							return false;
						}
					}
					
					oneExported = true;
				} else 
				if ( buttonDev ) {
					values.clear();
					values["idsession"] = std::to_string(idSession);
					values["iddevice"] = std::to_string(idDevice);
					values["button"] = std::to_string(buttonNumber);
					values["pressed"] = std::to_string(buttonState);
					
					if ( db.insert("buttons", values, false) ) {
						throw std::exception("Falha ao inserir no banco linha.");
						return false;
					}
					oneExported = true;
				} else 
				if ( saveAction ) {
					values.clear();
					values["idsession"] = std::to_string(idSession);
					values["iddevice"] = std::to_string(idDevice);
					values["idaction"] = std::to_string(idAction);
					values["value"] = std::to_string(value);
					
					if ( db.insert("action_history", values, false) ) {
						throw std::exception("Falha ao inserir no banco linha.");
						return false;
					}
					oneExported = true;
				}
			}


			//Mostra a porcentagem de envio
			int perc = (processedLines * 100) / lines;
			std::cout << "\r " << perc << "%\r";
			processedLines++;

		}

		std::cout << "\r 100%\n";

		//Grava no arquivo a palavra chave SYNC para identificar que ja foi enviado ao banco
		openOut(fileName);
		if ( fileOutput.is_open() ) {
			fileOutput << expFullContent;
		} else {
			throw std::exception("Nao foi possivel escrever SYNC no arquivo.");
			return false;
		}
		//Fecha todos os arquivos
		closeOut();
		closeIn();

		//So retorna true se pelo menos um dado foi inserido
		if ( oneExported == 0 ) {
			throw std::exception("Nenhum registro inserido.");
		}
		return oneExported;
	} else {

		throw std::exception("Nao pode ler o arquivo a ser enviado.");
		return false;
	}
	 

	
}

