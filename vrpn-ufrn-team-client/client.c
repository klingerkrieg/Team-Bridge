/** @file	client.C
@brief	Cliente para VRPN do TEAM UFRN

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/


#include <stdio.h>  // for printf, fprintf, NULL, etc
#include <stdlib.h> // for exit, atoi
#ifndef _WIN32_WCE
#include <signal.h> // for signal, SIGINT
#endif
#include <string.h>              // for strcmp, strncpy
#include <vrpn_Analog.h>         // for vrpn_ANALOGCB, etc
#include <vrpn_Button.h>         // for vrpn_Button_Remote, etc
#include <vrpn_Dial.h>           // for vrpn_Dial_Remote, etc
#include <vrpn_FileConnection.h> // For preload and accumulate settings
#include <vrpn_Shared.h>         // for vrpn_SleepMsecs
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc

#include "vrpn_BaseClass.h" // for vrpn_System_TextPrinter, etc
#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32

#include <iostream>
#include <map>
#include <vector>
#include "ConfigFileReader.h"
#include "KeyMap.h"
#include "callbacks.h"
#include "Hook.h"
//#include "DeviceInfo.h"
#include "Storage.h"

using namespace std;

int done = 0;                // Signals that the program should exit




const unsigned MAX_DEVICES = 50;



void handle_cntl_c(int) {
	done = 1;
}

class Client {
public:

	void setConfigFile(char * file_name) {
		configFileName = file_name;
	}

	void setPrintTracker(bool print) {
		printTracker = print;
	}

	void setPrintButton(bool print) {
		printButton = print;
	}

	void setPrintAnalog(bool print) {
		printAnalog = print;
	}
	
	bool setup() {
		

		// If we happen to open a file, neither preload nor accumulate the
		// messages in memory, to avoid crashing for huge files.
		vrpn_FILE_CONNECTIONS_SHOULD_PRELOAD = false;
		vrpn_FILE_CONNECTIONS_SHOULD_ACCUMULATE = false;

		

		std::vector<string> devs = {};
		std::vector<KeyMap> map = {};
		std::map<string, string> config = {};

		if ( !ConfigFileReader::readConfigFile(configFileName, devs, map, config) ) {
			printf("Falha ao ler arquivo de configuracao.\n");
			return false;
		}

		

		DeviceInfo *dev;

		// Make sure we have enough room for the new device
		if ( devs.size() == MAX_DEVICES ) {
			fprintf(stderr, "Mais dispositivos do que o permitido!\n");
			exit(-1);
		}


		for ( std::vector<std::string>::iterator it = devs.begin(); it != devs.end(); ++it ) {
			

			// Name the device and open it as everything
			dev = &deviceList[num_devices];
			dev->name = (char *)it->c_str();
			dev->tkr = new vrpn_Tracker_Remote(dev->name);
			dev->ana = new vrpn_Analog_Remote(dev->name);
			dev->btn = new vrpn_Button_Remote(dev->name);

			if ( (dev->ana == NULL) || (dev->btn == NULL) || (dev->tkr == NULL) ) {
				fprintf(stderr, "Error opening %s\n", dev->name);
				return false;
			} else {
				printf("Opened %s as:", dev->name);
			}


			if ( printTracker ) {
				vrpn_Tracker_Remote *tkr = dev->tkr;
				TrackerUserCallback *tc1 = new TrackerUserCallback;

				if ( (tc1 == NULL) ) {
					fprintf(stderr, "Out of memory\n");
				}
				printf(" Tracker");

				strncpy(tc1->name, dev->name, sizeof(tc1->name));


				dev->tkr->register_change_handler(tc1, handle_tracker_pos_quat);
			}

			if ( printButton ) {
				printf(" Button");
				dev->btn->register_change_handler(dev->name, handle_button);
				dev->btn->register_states_handler(dev->name, handle_button_states);
			}

			if ( printAnalog ) {
				printf(" Analog");
				dev->ana->register_change_handler(dev->name, handle_analog);
			} else {
				vrpn_System_TextPrinter.remove_object(dev->tkr);
				vrpn_System_TextPrinter.remove_object(dev->btn);
				vrpn_System_TextPrinter.remove_object(dev->ana);
			}

			printf(".\n");
			num_devices++;
		}
			


		if ( num_devices == 0 ) {
			printf("Nao foi encontrado nenhum dispositivo no arquivo de configuracao.");
		}

		#ifndef _WIN32_WCE
			signal(SIGINT, handle_cntl_c);
		#endif

		//Setando hook
		Hook::setMap(map);
		
		std::map<string, string>::iterator it = config.find("APP");
		if ( it != config.end() ) {
			Hook::setApp(it->second);
		}

		//Setando configs no store
		Storage::setConfig(config);
		
		Storage::checkSent();

		return 0;

		//Apos configurar o start tem que ser automatico e dentro desse metodo
		//Caso ele saia do metodo as variaveis que guardam o nome dos dispositivos sao apagadas
		printf("Press ^C to exit.\n");
		while ( !done ) {
			unsigned i;

			for ( i = 0; i < num_devices; i++ ) {
				deviceList[i].tkr->mainloop();
				deviceList[i].btn->mainloop();
				deviceList[i].ana->mainloop();
			}

			vrpn_SleepMsecs(1);
		}

		unsigned i;
		for ( i = 0; i < num_devices; i++ ) {
			delete deviceList[i].tkr;
			delete deviceList[i].btn;
			delete deviceList[i].ana;
		}

		Storage::closeFile();
		return true;

	}


private:
	char *configFileName = "vrpn-client.cfg";
	bool printTracker = true;
	bool printButton = true;
	bool printAnalog = true;

	DeviceInfo deviceList[MAX_DEVICES];
	unsigned num_devices = 0;

};

/*****************************************************************************
*
Callback handlers
*
*****************************************************************************/





// WARNING: On Windows systems, this handler is called in a separate
// thread from the main program (this differs from Unix).  To avoid all
// sorts of chaos as the main program continues to handle packets, we
// set a done flag here and let the main program shut down in its own
// thread by calling shutdown() to do all of the stuff we used to do in
// this handler.
void Usage(const char *arg0) {



	fprintf(
		stderr,
		"Usage:  %s [-notracker] [-nobutton] [-noanalog] \n"
		" -notracker:  Don't print tracker reports for following devices\n"
		"  -nobutton:  Don't print button reports for following devices\n"
		"  -noanalog:  Don't print analog reports for following devices\n",
		arg0);

	exit(0);
}

int main(int argc, char *argv[]) {

	TrackerUserCallback *userdata = new TrackerUserCallback;
	vrpn_TRACKERCB t = vrpn_TRACKERCB();

	Client client = Client();
	
	// Parse arguments, creating objects as we go.  Arguments that
	// change the way a device is treated affect all devices that
	// follow on the command line.
	for (int i = 1; i < argc; i++ ) {

		if ( !strcmp(argv[i], "-f") ) { // Specify config-file name
			if ( ++i > argc ) {
				Usage(argv[0]);
			}
			client.setConfigFile(argv[i]);
		} else
		if ( !strcmp(argv[i], "-notracker") ) {
			client.setPrintTracker(false);
		} else if ( !strcmp(argv[i], "-nobutton") ) {
			client.setPrintButton(false);
		} else if ( !strcmp(argv[i], "-noanalog") ) {
			client.setPrintAnalog(false);
		}

	}


	client.setup();

	return 0;
} /* main */
