#include "VariabilitiesConfig.h"

#ifdef ARDUINO_MODULE

#include "ArduinoAction.h"

bool ArduinoAction::liga = true;

/*
Para melhor performance utilize leitura de bytes no arduino
input = Serial.read();
*/

ArduinoAction::ArduinoAction() {
	
}

COMDevice ArduinoAction::connect(KeyMap key) {
	
	COMDevice comDev;
	comDev.COM = key.getCOM();

	char portName[14];
	sprintf(portName, "\\\\.\\COM%d", key.getCOM());
	

	comDev.handler = CreateFileA(static_cast<LPCSTR>(portName),
								GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if ( comDev.handler == INVALID_HANDLE_VALUE ) {
		if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
			printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		} else {
			printf("ERROR!!!");
		}
	} else {
		DCB dcbSerialParameters = { 0 };

		if ( !GetCommState(comDev.handler, &dcbSerialParameters) ) {
			printf("failed to get current serial parameters");
		} else {
			dcbSerialParameters.BaudRate = key.getBauds();
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if ( !SetCommState(comDev.handler, &dcbSerialParameters) ) {
				printf("ALERT: could not set Serial port parameters\n");
			} else {
				comDev.connected = true;
				PurgeComm(comDev.handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}

	connections.insert_or_assign(key.getCOM(), comDev);
	return comDev;
}

ArduinoAction::~ArduinoAction() {
	//Desconecta todos os dispositivos
	for ( std::map<int, COMDevice>::iterator it = connections.begin(); it != connections.end(); ++it ) {
		if ( it->second.connected ) {
			it->second.connected = false;
			CloseHandle(it->second.handler);
		}
	}
}


void ArduinoAction::run(KeyMap key) {

	COMDevice conn;

	std::map<int, COMDevice>::iterator it = connections.find(key.getCOM());
	
	
	if ( it != connections.end() ) {
		//Usa conexao ja existente
		conn = it->second;
	} else {
		//Conecta
		conn = connect(key);
	}

	int actualTime = (int)time(0);

	//Realiza um delay para não sobrecarregar o arduino
	if ( actualTime - conn.lastMessage < ARDUINO_DELAY_TO_MSG ) {
		return;
	}

	//Se tiver sido um pressionamento de tecla
	if ( key.isKeyUpEvent() == false ) {

		DWORD bytesSend;
		//Um valor muito alto aqui pode causar falha.
		unsigned int buf_size = 10;//MAX_DATA_LENGTH;

		char writing[10] = "";

		if ( key.getKey() == JOINT_ANGLE ) {
			
			char tmpStr[10];
			sprintf(tmpStr, "%d=%d;", key.getEngine(), key.getAngle());
			strcpy(writing, tmpStr);
			/*if ( liga ) {
				sprintf(tmpStr, "%d=%d;", 1, 10);
				strcpy(toKey, tmpStr);
				liga = false;
			} else {
				sprintf(tmpStr, "%d=%d;", 1, 100);
				strcpy(toKey, tmpStr);
				liga = true;
			}*/
			
		} else {
			strcpy(writing, (char*)key.getToKey());
		}
		
		std::cout << "Writing to Arduino" << conn.COM << ":" << writing << "\n";

		if ( !WriteFile(conn.handler, (void*)writing, buf_size, &bytesSend, 0) ) {
			ClearCommError(conn.handler, &conn.errors, &conn.status);
		}

		conn.lastMessage = actualTime;
		connections.insert_or_assign(conn.COM,conn);

		
	}

}

#endif