#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

bool debug = false;

string ReadByte(int port, int bauds) {
	const int length = 32;
	DCB       dcb;
	int       retVal;
	BYTE      Byte[length];
	DWORD     dwBytesTransferred;
	DWORD     dwCommModemStatus;

	wchar_t portStr[5];
	wsprintfW(portStr, L"COM%d", port);

	HANDLE hPort = CreateFileW(
		portStr,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if ( !GetCommState(hPort, &dcb) ) {
		return "\nFalha ao conectar na COM" + port; //ERROR
	}

	dcb.BaudRate = bauds;          //9600 Baud 
	dcb.ByteSize = 8;                 //8 data bits 
	dcb.Parity = NOPARITY;            //no parity 
	dcb.StopBits = ONESTOPBIT;        //1 stop

	if ( !SetCommState(hPort, &dcb) ) return "Falha ao configurar bauds"; //ERROR
	
	SetCommMask(hPort, EV_RXCHAR | EV_TXEMPTY);       //receive character event  

	WaitCommEvent(hPort, &dwCommModemStatus, 0);  //wait for character 

	if ( dwCommModemStatus & EV_RXCHAR )
		ReadFile(hPort, &Byte, length, &dwBytesTransferred, 0);  //read 1 
	
	else if ( dwCommModemStatus & EV_ERR )
		retVal = 0x101;

	std::string data = ((char *)Byte);
	data = data.substr(0, data.find("\n"));
	
	CloseHandle(hPort);
	return data;
}

int main(int argc, char **argv ) {

	/*if ( argc < 5 ) {
		cout << "Usage:\n -p - Port: 3\n -b - Bauds: 115200\n -d - Debug\n";
		return 0;
	}*/
	
	int port;
	int bauds;
	for ( int i = 0; i < argc; i++ ) {
		if ( !strcmp(argv[i], "-p") ) {
			port = atoi(argv[++i]);
		} else
		if ( !strcmp(argv[i], "-b") ) {
			bauds = atoi(argv[++i]);
		} else
		if ( !strcmp(argv[i], "-d") ) {
			debug = true;
		}
	}

	cout << ReadByte(6,9600);
	if ( debug ) {
		Sleep(5000);
	}
	return 0;
}