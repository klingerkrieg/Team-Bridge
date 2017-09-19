#include <stdio.h>  // for printf, fprintf, NULL, etc
#include <stdlib.h> // for exit, atoi

#include <vrpn_Shared.h>         // for vrpn_SleepMsecs
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vector>                // for vector

#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32

#include <Windows.h>
#include <Tchar.h>

using namespace std;


char* aplicacao;
double altura;
double ultimaAltura = 0;

class device_info {
public:
	char *name;
	vrpn_Tracker_Remote *tkr;
};
const unsigned MAX_DEVICES = 50;


class tracker_user_callback {
public:
	char t_name[vrpn_MAX_TEXT_LEN];
	vector<unsigned> t_counts;
};



void callKey(char key) {
	//HWND notepad = FindWindow(_T(aplicacao), NULL);
	//HWND edit = FindWindowEx(notepad, NULL, _T("Edit"), NULL);
	//PostMessage(edit, WM_KEWYDOWN, VkKeyScanEx(key, GetKeyboardLayout(0)), 0);

	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}



void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	tracker_user_callback *t_data = static_cast<tracker_user_callback *>(userdata);

	// Make sure we have a count value for this sensor
	while ( t_data->t_counts.size() <= static_cast<unsigned>(t.sensor) ) {
		t_data->t_counts.push_back(0);
	}

	if ( t.sensor == 0 ) {

		printf("Altura: %.2f \n", t.pos[1]);


		if ( ultimaAltura - t.pos[1] > altura ) {
			ultimaAltura = t.pos[1];
			keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
			//callKey('W');
			//keybd_event( 'S' , 0, 0, 0);
			//keybd_event('S', 0, KEYEVENTF_KEYUP, 0);
		} else 
		if ( ultimaAltura - t.pos[1] < (altura * -1) ) {
			ultimaAltura = t.pos[1];
			//callKey(VK_SPACE);
			keybd_event(VK_SPACE, 0, 0, 0);
			//keybd_event('W', 0, 0, 0);
			//keybd_event('W', 0, KEYEVENTF_KEYUP, 0);
			
			
		}
		
	}
	
}





int main(int argc, char *argv[]) {

	if ( argc != 3 ) {
		printf("Passe como primeiro parametro a altura do degrau, um numero de 0.0 a 1.0 e o segundo o nome da aplicacao");
	}


	altura = atof(argv[1]);
	aplicacao = argv[2];

	device_info *dev = new device_info();

	dev->name = "Tracker0@localhost";
	dev->tkr = new vrpn_Tracker_Remote(dev->name);

	if ( (dev->tkr == NULL) ) {
		fprintf(stderr, "Error opening %s\n", dev->name);
		return -1;
	} else {
		printf("Opened %s as:", dev->name);
	}


	tracker_user_callback *tc1 = new tracker_user_callback;


	if ( (tc1 == NULL) ) {
		fprintf(stderr, "Out of memory\n");
	}
				
	strncpy(tc1->t_name, dev->name, sizeof(tc1->t_name));

	dev->tkr->register_change_handler(tc1, handle_tracker_pos_quat);

	vrpn_System_TextPrinter.remove_object(dev->tkr);

	printf(".\n");
	


	printf("Press ^C to exit.\n");
	while ( true ) {
		
		dev->tkr->mainloop();
		
		vrpn_SleepMsecs(1);
	}

	delete dev->tkr;
	
	
	return 0;
} /* main */
