#include "VariabilitiesConfig.h"

#ifdef ARDUINO_MODULE

#include "ArduinoAction.h"


/*
Para melhor performance utilize leitura de bytes no arduino
input = Serial.read();
*/

ArduinoAction::ArduinoAction() {
	
}

COMDevice ArduinoAction::connect(KeyMap key) {
	
	COMDevice comDev;

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


	//Se tiver sido um pressionamento de tecla
	if ( key.isKeyUpEvent() == false ) {

		DWORD bytesSend;
		unsigned int buf_size = MAX_DATA_LENGTH;

		char toKey = (char)key.getToKey();

		std::cout << "Writing:" << (char)key.getToKey() << " code:" << key.getToKey() << "\n";


		if ( !WriteFile(conn.handler, (void*)&toKey, buf_size, &bytesSend, 0) ) {
			ClearCommError(conn.handler, &conn.errors, &conn.status);
		}

	}

}

#endif