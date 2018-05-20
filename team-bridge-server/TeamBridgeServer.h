#pragma once


#include <stdio.h>  // for fprintf, stderr, NULL, etc
#include <stdlib.h> // for atoi, exit
#include <string.h> // for strcmp
#include <string>
#include <sstream>
#include "MainView.h"

#include "vrpn_BaseClass.h"             // for ::vrpn_TEXT_NORMAL, etc
#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"            // for vrpn_Connection, etc
#include "vrpn_ForwarderController.h"   // for vrpn_Forwarder_Server
#include "vrpn_Generic_server_object.h" // for vrpn_Generic_Server_Object
#include "vrpn_Shared.h"                // for vrpn_SleepMsecs

class TeamBridgeServer {

	private:
	static volatile int done; // Done and should exit?

	vrpn_Connection *connection;
	vrpn_Generic_Server_Object *generic_server = NULL;

	static char *g_NICname;

	static const char *g_inLogName;
	static const char *g_outLogName;
	int milli_sleep_time;

	// TCH October 1998
	// Use Forwarder as remote-controlled multiple connections.
	vrpn_Forwarder_Server *forwarderServer;

	static bool verbose;

	void shutDown(void);


	public:

	void start();

	void stop();

	void mainloop();


};