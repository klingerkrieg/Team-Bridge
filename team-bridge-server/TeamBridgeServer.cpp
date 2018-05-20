
#include "TeamBridgeServer.h"

bool TeamBridgeServer::verbose				= false;
volatile int TeamBridgeServer::done			= 0;
char * TeamBridgeServer::g_NICname			= NULL;
const char * TeamBridgeServer::g_inLogName	= NULL;
const char * TeamBridgeServer::g_outLogName	= NULL;

TeamBridgeServer *tbServer;

int VRPN_CALLBACK handle_dlc(void *, vrpn_HANDLERPARAM /*p*/) {
	tbServer->stop();
	return 0;
}


void TeamBridgeServer::start() {

	tbServer = this;
	const char *config_file_name = "vrpn.cfg";

	MainView::write("Lendo arquivo:");
	MainView::write(config_file_name);
	

	bool bail_on_error = true;
	bool auto_quit = false;
	int realparams = 0;
	int port = vrpn_DEFAULT_LISTEN_PORT_NO;
#ifdef _WIN32
	// On Windows, the millisleep with 0 option frees up the CPU time slice for
	// other jobs
	// but does not sleep for a specific time.  On Unix, this returns
	// immediately and does
	// not do anything but waste cycles.
	milli_sleep_time = 0; // How long to sleep each iteration (default: free
							  // the timeslice but be runnable again
							  // immediately)
#else
	milli_sleep_time = 1; // How long to sleep each iteration (default: 1ms)
#endif

	// Need to have a global pointer to the connection so we can shut it down
	// in the signal handler (so we can close any open logfiles.)
	// Form the name based on the type of connection requested.  For a standard
	// VRPN UDP/TCP port, we give it the name "NIC:port" if there is a NIC name,
	// otherwise just ":port" for the default NIC.
	std::stringstream con_name;
	if ( g_NICname ) {
		con_name << g_NICname;
	}
	con_name << ":" << port;
	connection =
		vrpn_create_server_connection(con_name.str().c_str(), g_inLogName, g_outLogName);

	// Create the generic server object and make sure it is doing okay.
	generic_server = new vrpn_Generic_Server_Object(
		connection, config_file_name, verbose, bail_on_error);
	if ( (generic_server == NULL) || !generic_server->doing_okay() ) {
		MessageBox(NULL, L"VRPN não pôde ser inicializado", L"Error", MB_ICONHAND | MB_OK);
		shutDown();
	}

	// Open the Forwarder Server
	forwarderServer = new vrpn_Forwarder_Server(connection);

	// If we're set to auto-quit, then register a handler for the last
	// connection
	// dropped that will cause a callback which will exit.
	if ( auto_quit ) {
		int dlc_m_id =
			connection->register_message_type(vrpn_dropped_last_connection);
		connection->register_handler(dlc_m_id, handle_dlc, NULL);
	}

	MainView::writeln("VRPN Iniciado.");
}


void TeamBridgeServer::mainloop() {
	while ( done == 0 ) {
		bool flush_continuously = false;

		// Let the generic object server do its thing.
		if ( generic_server ) {
			generic_server->mainloop();
		}

		// Send and receive all messages.
		connection->mainloop();

		// Save all log messages that are pending so that they are on disk
		// in case we end up exiting improperly.  This may slow down the
		// server waiting for disk writes to complete, but will more reliably
		// log messages.
		if ( flush_continuously ) {
			connection->save_log_so_far();
		}

		// Bail if the connection is in trouble.
		if ( !connection->doing_okay() ) {
			shutDown();
		}

		// Handle forwarding requests;  send messages
		// on auxiliary connections.
		forwarderServer->mainloop();

		// Sleep so we don't eat the CPU
	#if defined(_WIN32)
		if ( milli_sleep_time >= 0 ) {
		#else
		if ( milli_sleep_time > 0 ) {
		#endif
			vrpn_SleepMsecs(milli_sleep_time);
		}
	}

	shutDown();
}


void TeamBridgeServer::stop() {
	done = 1;
	MainView::writeln("Desligando VRPN...");
}

void TeamBridgeServer::shutDown(void) {
	if ( forwarderServer ) {
		delete forwarderServer;
		forwarderServer = NULL;
	}
	if ( generic_server ) {
		delete generic_server;
		generic_server = NULL;
	}
	if ( connection ) {
		connection->removeReference();
		connection = NULL;
	}
	
}





