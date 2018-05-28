/** @file	client.C
@brief	Cliente para VRPN do TEAM UFRN

@date 2017
@author Alan Klinger klingerkrieg@gmail.com
@license Standard VRPN license.
*/
#include "Client.h"

using namespace std;

int done = 0;                // Signals that the program should exit

void handle_cntl_c(int) {
	done = 1;
}


bool Client::setup(bool test = false) {
		

		// If we happen to open a file, neither preload nor accumulate the
		// messages in memory, to avoid crashing for huge files.
		vrpn_FILE_CONNECTIONS_SHOULD_PRELOAD = false;
		vrpn_FILE_CONNECTIONS_SHOULD_ACCUMULATE = false;

		ConfigFileReader configFileReader = ConfigFileReader();

		std::vector<DeviceType> devs = {};
		std::vector<KeyMap> map = {};
		Config config = Config();

		if ( !configFileReader.readConfigFile(configFileName, devs, map, config) ) {
			printf("Falha ao ler arquivo de configuracao.\n");
			return false;
		}

		#ifdef ARDUINO_MODULE
		ArduinoAction *tbAction = new ArduinoAction();
		#else
		//Cria acionador de eventos, para essa aplicação será eventos de teclado e mouse
		KeyPressAction *tbAction = new KeyPressAction();
		#endif

		
		

	#ifdef THERAPY_MODULE
		
		//Setando configs no store
		storage = Storage(config, exportDb);

		//So exporta para o banco se for solicitado
		if ( exportDb ) {
			storage.checkSent();
		}

		//Setando InputConverter
		inputConverter = InputConverter(map, devs, storage, tbAction, view);
	#else
		//Setando InputConverter
		inputConverter = InputConverter(map, devs, tbAction, view);
	#endif

		

		DeviceInfo *dev;

		// Make sure we have enough room for the new device
		if ( devs.size() == MAX_DEVICES ) {
			fprintf(stderr, "Mais dispositivos do que o permitido!\n");
			exit(-1);
		}


		for ( std::vector<DeviceType>::iterator it = devs.begin(); it != devs.end(); ++it ) {
			

			// Name the device and open it as everything
			dev = &deviceList[num_devices];
			dev->name = (char *)it->name.c_str();
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

		

		//Apos configurar o start tem que ser automatico e dentro desse metodo
		//Caso ele saia do metodo as variaveis que guardam o nome dos dispositivos sao apagadas
		printf("Press ^C to exit.\n");
		if ( test ) {
			done = true;//para teste unitario
		}
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

	#ifdef THERAPY_MODULE
		storage.close();
	#endif
		return true;

}


/*****************************************************************************
*
Callback handlers
*
*****************************************************************************/
Client client;


void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	TrackerUserCallback *t_data = static_cast<TrackerUserCallback *>(userdata);

#ifdef THERAPY_MODULE
	client.getStorage().saveToFile(t_data, t);
#endif
	client.getInputConverter()->checkTrack(t_data, t);
}



void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
	const char *name = (const char *)userdata;
	
#ifdef THERAPY_MODULE
	client.getStorage().saveToFile(name, b);
#endif

	//Teste para identificar se o mapeamento esta correto
	/*for ( std::map<string,int>::iterator it = KeyMap::configToScanCode.begin(); it != KeyMap::configToScanCode.end(); ++it ) {
		if ( it->second == b.button ) {
			printf("%s\n", it->first.c_str());
		}
	}*/


	client.getInputConverter()->checkButton(name, b);
}


void VRPN_CALLBACK handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b) {
	const char *name = (const char *)userdata;

	printf("Button %s has %d buttons with states!!!:", name, b.num_buttons);
	printf("\n");
}


void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a) {
	const char *name = (const char *)userdata;
#ifdef THERAPY_MODULE
	client.getStorage().saveToFile(name, a);
#endif
	client.getInputConverter()->checkAnalog(name, a);
}


// WARNING: On Windows systems, this handler is called in a separate
// thread from the main program (this differs from Unix).  To avoid all
// sorts of chaos as the main program continues to handle packets, we
// set a done flag here and let the main program shut down in its own
// thread by calling shutdown() to do all of the stuff we used to do in
// this handler.
void Usage() {

	printf("Usage: \n"
		" -notracker:  Don't print tracker reports for following devices\n"
		"  -nobutton:  Don't print button reports for following devices\n"
		"  -noanalog:  Don't print analog reports for following devices\n");

	exit(0);
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	//Devido a utilizacao do WinMain o cmd nao aparecerá
	//os seguintes comandos são para criar uma nova janela do cmd
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);


	View view = View(hInstance);
	view.showMsg("Starting TEAM-VRPN :)");
	client.setView(view);

	TrackerUserCallback *userdata = new TrackerUserCallback;
	vrpn_TRACKERCB t = vrpn_TRACKERCB();


	//Captura de argumentos

	LPWSTR *argv;
	int argc;
	int i;
	char buffer[500];

	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if ( NULL == argv ) {
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	} else for ( i = 0; i < argc; i++ ) {
		printf("%d: %ws\n", i, argv[i]);

		if ( !wcscmp(argv[i], L"-f") ) { // Specify config-file name
			wcstombs(buffer, argv[i+1], 500);
			if ( ++i > argc ) {
				Usage();
			}
			client.setConfigFile(buffer);
		} else
		if ( !wcscmp(argv[i], L"-notracker") ) {
			client.setPrintTracker(false);
		} else if ( !wcscmp(argv[i], L"-nobutton") ) {
			client.setPrintButton(false);
		} else if ( !wcscmp(argv[i], L"-noanalog") ) {
			client.setPrintAnalog(false);
		} else if ( !wcscmp(argv[i], L"-export") ) {
			client.setExport(true);
		}
	}

	// Free memory allocated for CommandLineToArgvW arguments.
	LocalFree(argv);

	client.setup();


	return 0;
} /* main */
