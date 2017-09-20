#pragma once
#include <vector>
#include "KeyMap.h"
#include "DeviceInfo.h"

#include "vrpn_Configure.h" // for VRPN_CALLBACK
#include "vrpn_Types.h"     // for vrpn_float64, vrpn_int32
#include "vrpn_BaseClass.h" // for vrpn_System_TextPrinter, etc
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vrpn_Analog.h>         // for vrpn_ANALOGCB, etc
#include <vrpn_Button.h>         // for vrpn_Button_Remote, etc
#include <Tchar.h>

class Hook {
public:
	
	static std::vector<KeyMap> map;
	static std::string app;
	static int app2;

	static void setMap(std::vector<KeyMap> mapP);

	static void setApp(std::string appP);

	static void Hook::press(char key);

	static void Hook::release(char key);

	static void Hook::checkTrack();

	static void Hook::checkButton(const char * name, const vrpn_BUTTONCB b);

	static void Hook::checkAnalog();
	
};



std::vector<KeyMap> Hook::map;
std::string Hook::app;
int Hook::app2;

void Hook::setMap(std::vector<KeyMap> mapP) {
	map = mapP;
}

void Hook::setApp(std::string appP) {
	app = appP;
}

void Hook::press(char key) {
	printf("Press: %d ", key);
	if ( app != "" ) {

		HWND window = FindWindow(_T(app.c_str()), NULL);
		HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);
		if ( edit != NULL ) {
			printf(" em %s.\n", app.c_str());
			PostMessage(edit, WM_KEYDOWN, VkKeyScanEx(key, GetKeyboardLayout(0)), 0);
			return;
		}

	}
	printf(" no Windows.\n");
	//Caso nenhum app tenha sido configurado ou encontrado lanca evento no windows
	keybd_event(key, 0, 0, 0);
}

void Hook::release(char key) {
	printf("Release: %d\n", key);

	if ( app != "" ) {

		HWND window = FindWindow(_T(app.c_str()), NULL);
		HWND edit = FindWindowEx(window, NULL, _T("Edit"), NULL);

		if ( edit != NULL ) {
			printf(" em %s.\n", app.c_str());
			PostMessage(edit, WM_KEYUP, VkKeyScanEx(key, GetKeyboardLayout(0)), 0);
			return;
		}


	}
	printf(" no Windows.\n");
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);

}


void Hook::checkTrack() {

}

void Hook::checkButton(const char * name, const vrpn_BUTTONCB b) {

	for ( std::vector<KeyMap>::iterator keyMap = map.begin(); keyMap != map.end(); ++keyMap ) {
		printf("%d == %d\n", keyMap->key, b.button);

		if ( keyMap->key == b.button ) {
			if ( b.state == 1 ) {
				press(keyMap->toKey);
			} else {
				//o release esta ocorrendo em duplicacao do evento
				//release(keyMap->toKey);
			}
		}
	}

}

void Hook::checkAnalog() {

}

/*
			Callbacks
*/


void VRPN_CALLBACK
handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	TrackerUserCallback *t_data = static_cast<TrackerUserCallback *>(userdata);

	// Make sure we have a count value for this sensor
	while ( t_data->counts.size() <= static_cast<unsigned>(t.sensor) ) {
		t_data->counts.push_back(0);
	}

	t_data->counts[t.sensor] = 0;
	printf("Tracker %s, sensor %d:\n     pos (%5.2f, %5.2f, %5.2f); "
		   "quat (%5.2f, %5.2f, %5.2f, %5.2f)\n",
		   t_data->name, t.sensor, t.pos[0], t.pos[1], t.pos[2],
		   t.quat[0], t.quat[1], t.quat[2], t.quat[3]);

}



void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
	const char *name = (const char *)userdata;
	printf("Button\n");
	Hook::checkButton(name, b);
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

