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
#include <vrpn_Text.h>           // for vrpn_Text_Receiver, etc
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vector>                // for vector

#include "vrpn_BaseClass.h" // for vrpn_System_TextPrinter, etc
#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32

#include <iostream>
#include <map>
#include <vector>
#include "ConfigFileReader.h"
#include "KeyMap.h"

//#include "hook.h"

using namespace std;

int done = 0;                // Signals that the program should exit



class device_info {
public:
	char *name;
	vrpn_Tracker_Remote *tkr;
	vrpn_Button_Remote *btn;
	vrpn_Analog_Remote *ana;
};
const unsigned MAX_DEVICES = 50;


class tracker_user_callback {
public:
	char t_name[vrpn_MAX_TEXT_LEN];
	vector<unsigned> t_counts;
};


void VRPN_CALLBACK
handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	tracker_user_callback *t_data = static_cast<tracker_user_callback *>(userdata);

	// Make sure we have a count value for this sensor
	while ( t_data->t_counts.size() <= static_cast<unsigned>(t.sensor) ) {
		t_data->t_counts.push_back(0);
	}

	t_data->t_counts[t.sensor] = 0;
	printf("Tracker %s, sensor %d:\n     pos (%5.2f, %5.2f, %5.2f); "
			"quat (%5.2f, %5.2f, %5.2f, %5.2f)\n",
			t_data->t_name, t.sensor, t.pos[0], t.pos[1], t.pos[2],
			t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
	
}



void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
	const char *name = (const char *)userdata;

	printf("##########################################\r\n"
		   "Button %s, number %d was just %s\n !!!"
		   "##########################################\r\n",
		   name, b.button, b.state ? "pressed" : "released");
}


void VRPN_CALLBACK
handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b) {
	const char *name = (const char *)userdata;

	printf("Button %s has %d buttons with states!!!:", name, b.num_buttons);
	int i;
	for ( i = 0; i < b.num_buttons; i++ ) {
		printf(" %d", b.states[i]);
	}
	printf("\n");
}


void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a) {
	int i;
	const char *name = (const char *)userdata;

	printf("!!!Analog %s:\n         %5.2f", name, a.channel[0]);
	for ( i = 1; i < a.num_channel; i++ ) {
		printf(", %5.2f", a.channel[i]);
	}
	printf(" (%d chans)\n", a.num_channel);
}

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

		

		device_info *dev;

		// Make sure we have enough room for the new device
		if ( devs.size() == MAX_DEVICES ) {
			fprintf(stderr, "Mais dispositivos do que o permitido!\n");
			exit(-1);
		}


		for ( std::vector<std::string>::iterator it = devs.begin(); it != devs.end(); ++it ) {
			

			// Name the device and open it as everything
			dev = &device_list[num_devices];
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
				tracker_user_callback *tc1 = new tracker_user_callback;
				tracker_user_callback *tc2 = new tracker_user_callback;
				tracker_user_callback *tc3 = new tracker_user_callback;

				if ( (tc1 == NULL) || (tc2 == NULL) || (tc3 == NULL) ) {
					fprintf(stderr, "Out of memory\n");
				}
				printf(" Tracker");

				strncpy(tc1->t_name, dev->name, sizeof(tc1->t_name));
				strncpy(tc2->t_name, dev->name, sizeof(tc2->t_name));
				strncpy(tc3->t_name, dev->name, sizeof(tc3->t_name));


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

		return true;

	}

	void start() {
		printf("Press ^C to exit.\n");
		while ( !done ) {
			unsigned i;

			for ( i = 0; i < num_devices; i++ ) {
				device_list[i].tkr->mainloop();
				device_list[i].btn->mainloop();
				device_list[i].ana->mainloop();
			}

			vrpn_SleepMsecs(1);
		}

		unsigned i;
		for ( i = 0; i < num_devices; i++ ) {
			delete device_list[i].tkr;
			delete device_list[i].btn;
			delete device_list[i].ana;
		}
	}

private:

	char *configFileName = "vrpn-client.cfg";
	bool printTracker = true;
	bool printButton = true;
	bool printAnalog = true;

	device_info device_list[MAX_DEVICES];
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

	client.start();

	return 0;
} /* main */
