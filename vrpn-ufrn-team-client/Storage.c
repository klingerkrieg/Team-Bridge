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


bool Storage::saveToFile(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	if ( isOpenOut == false ) {
		dateStr = currentDateTime("%Y-%m-%d %H:%M:%S");
		fileName = Storage::saveDir + "/" + currentDateTime("%Y-%m-%d_%H-%M-%S") + ".txt";

		if ( !openOut(fileName) ) {
			std::cout << "Nao foi possivel abrir arquivo: " << fileName << "\n";
			return false;
		} else {
			fileOutput << "DEV\t" << userdata->name << "\n";
			fileOutput << "DATE\t" << dateStr << "\n";
			fileOutput << "PATIENT\t" << patient << "\n";
			std::cout << fileName << " Criado.\n";

		}
	}
	//sensor	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
	if ( fileOutput.is_open() ) {
		fileOutput << "SENSOR\t" << t.sensor << "\tTIMESTAMP\t" << t.msg_time.tv_usec
			<< "\tPOS\t" << t.pos[0] << "\t" << t.pos[1] << "\t" << t.pos[2]
			<< "\tQUAT\t" << t.quat[0] << "\t" << t.quat[1] << "\t" << t.quat[2] << "\t" << t.quat[3] << "\n";
		return true;
	} else {
		return false;
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

	int timestamp;
	int sensor;
	double pos[3];
	double quat[4];
	char s1[LINESIZE], s2[LINESIZE];

	std::map<std::string, std::string> values;
	int processedLines = 0;
	bool oneExported = false;
	

	//Conecta
	if ( !db.connect() ) {
		throw std::exception("Nao pode conectar ao banco de dados.");
		return false;
	}


	if ( fileInput.is_open() ) {

		int lines = (int)std::count(std::istreambuf_iterator<char>(fileInput),
									std::istreambuf_iterator<char>(), '\n');

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
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					throw std::exception("Falha ao ler linha.");
					return false;
				}
				expPatientName = s2;
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "DATE") ) {
				if ( sscanf(line.c_str(), "%s\t%[^\t\n]", s1, s2) != 2 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					throw std::exception("Falha ao ler linha.");
					return false;
				}
				expDateTime = s2;
			} else
			if ( !strcmp(pch = strtok(scrap, " \t"), "SENSOR") ) {
				//Caso chegue nos dados dos sensores e porque o cabecalho com as informacoes ja acabou

				if ( idSession == -1 ) {
					//Se nao tem a id da sessao
					values.clear();
					values["name"] = expPatientName;
					idPatient = db.findOrCreate("patients", "idpatient", values);
					if ( idPatient == -1 ) {
						throw std::exception("Falha inserir paciente.");
						return false;
					}

					values.clear();
					values["device_name"] = expDev;
					idDevice = db.findOrCreate("devices", "iddevice", values);
					if ( idDevice == -1 ) {
						throw std::exception("Falha inserir dispositivo.");
						return false;
					}

					values.clear();
					values["date_time"] = expDateTime;
					values["iddevice"] = std::to_string(idDevice);
					values["idpatient"] = std::to_string(idPatient);
					idSession = db.findOrCreate("sessions", "idsession", values);
					if ( idSession == -1 ) {
						throw std::exception("Falha inserir sessao.");
						return false;
					}
				}


				//sensor	0	time	0	pos	-0.16	 0.51	 1.02	quat	-0.19	 0.21	 0.12	 0.85
				if ( sscanf(line.c_str(), "SENSOR\t%d\tTIMESTAMP\t%d\tPOS\t%lf\t%lf\t%lf\tQUAT\t%lf\t%lf\t%lf\t%lf",
							&sensor,
							&timestamp,
							&pos[0], &pos[1], &pos[2],
							&quat[0], &quat[1], &quat[2], &quat[3]) != 9 ) {
					fprintf(stderr, "Falha ao ler %s linha: %s\n", fileName, line.c_str());
					throw std::exception("Falha ao ler linha.");
					return false;
				}

				//uint = 4294967295
				//meia hora = 540.000 registros
				//cabem 7953 sessoes
				values.clear();
				values["idsession"] = std::to_string(idSession);
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
			}


			//Mostra a porcentagem de envio
			int perc = (processedLines * 100) / lines;
			std::cout << "\r " << perc << "%";
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

