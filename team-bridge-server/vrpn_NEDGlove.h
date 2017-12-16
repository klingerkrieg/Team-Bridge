/** @file	vrpn_NEDGlove.h
@brief	Header for NED Team Glove device.
@date	2017

@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#pragma once
#include "vrpn_Analog.h"
#include <Windows.h>
#include <iostream>
#include <string>

class VRPN_API vrpn_NEDGlove : public vrpn_Analog {
	public:

	vrpn_NEDGlove(const char *name, int port, int bauds, vrpn_Connection *c = NULL);
	virtual ~vrpn_NEDGlove();
	virtual void mainloop();

	struct timeval _timestamp;

	bool connected = false;

	protected:

	int port;
	int bauds;
	static const int numBytes = 21;
	DCB       dcb;
	BYTE      Byte[numBytes];
	DWORD     dwBytesTransferred;
	DWORD     dwCommModemStatus;
	HANDLE hPort;

	virtual bool connect();
	virtual bool frame();


};

