/** @file	vrpn_NEDGlove.C
@brief	Drivers for NED Team Glove device.

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/

// Based on the vrpn_Oculus driver

#include "vrpn_NEDGlove.h"
#include "vrpn_BaseClass.h" // for ::vrpn_TEXT_NORMAL, etc

VRPN_SUPPRESS_EMPTY_OBJECT_WARNING()


vrpn_NEDGlove::vrpn_NEDGlove(const char *name, int port, int bauds, vrpn_Connection *c)
	: vrpn_Analog(name, c) {

	vrpn_Analog::num_channel = 5;
	this->port = port;
	this->bauds = bauds;

	memset(channel, 0, sizeof(channel));
	memset(last, 0, sizeof(last));

	vrpn_gettimeofday(&_timestamp, NULL);
	
	connect();
}

void vrpn_NEDGlove::mainloop() {
	if ( connected ) {
		if ( !frame() ) {
			std::cout << "Perda de conexao com a NED Glove.\n";
			CloseHandle(hPort);
			connected = false;
			connect();
		}
	}
	server_mainloop();
}

vrpn_NEDGlove::~vrpn_NEDGlove() {
	CloseHandle(hPort);
}


bool vrpn_NEDGlove::connect() {
	
	while ( connected == false ) {
		wchar_t comConf[5];
		wsprintfW(comConf, L"COM%d", port);

		hPort = CreateFileW(
			comConf,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if ( !GetCommState(hPort, &dcb) ) {
			std::cout << "Falha ao se conectar ao NED Glove.\n";
			Sleep(1000);
			continue; //ERROR
		}

		dcb.BaudRate = bauds;          //9600 Baud 
		dcb.ByteSize = 8;                 //8 data bits 
		dcb.Parity = NOPARITY;            //no parity 
		dcb.StopBits = ONESTOPBIT;        //1 stop

		if ( !SetCommState(hPort, &dcb) ) {
			std::cout << "Falha ao usar configurar a NED Glove.\n";
			Sleep(1000);
			continue; //ERROR
		}

		SetCommMask(hPort, EV_RXCHAR | EV_TXEMPTY);       //receive character event  
		connected = true;
		std::cout << "NED Glove conectado.\n";
	}
	
	return true;
}


bool vrpn_NEDGlove::frame() {
	
	WaitCommEvent(hPort, &dwCommModemStatus, 0);  //wait for character 

	if ( dwCommModemStatus & EV_RXCHAR )
		ReadFile(hPort, &Byte, numBytes, &dwBytesTransferred, 0);  //read 1 
	else if ( dwCommModemStatus & EV_ERR ) {
		std::cout << "Erro na leitura";
		return false;
	}

	if ( !GetCommState(hPort, &dcb) ) {
		return false; //ERROR
	}
	
	
	std::string data = ((char *)Byte);
	
	data = data.substr(0, data.find("\r"));
	char * pch;
	data += ";";

	std::cout << data << "\n";

	pch = strtok((char *)data.c_str(), ";");
	for (int i = 0; i < 5; i++){
		if ( strlen(pch) > 0 ) {
			channel[i] = atoi(pch);//err
			pch = strtok(NULL, ";");
		}
		
	}

	vrpn_Analog::report();
	return true;
}
