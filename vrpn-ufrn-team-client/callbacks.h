#pragma once
#include "DeviceInfo.h"
#include <vrpn_Tracker.h>        // for vrpn_TRACKERACCCB, etc
#include <vrpn_Button.h>
#include <vrpn_Analog.h>
#include "Hook.h"
#include "Storage.h"

/*
			Callbacks
*/


void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	TrackerUserCallback *t_data = static_cast<TrackerUserCallback *>(userdata);

	Storage::saveToFile(t_data, t);
	Hook::checkTrack(t_data,t);
	// Make sure we have a count value for this sensor
	/*while ( t_data->counts.size() <= static_cast<unsigned>(t.sensor) ) {
		t_data->counts.push_back(0);
	}

	t_data->counts[t.sensor] = 0;
	printf("Tracker %s, sensor %d:\n     pos (%5.2f, %5.2f, %5.2f); "
		   "quat (%5.2f, %5.2f, %5.2f, %5.2f)\n",
		   t_data->name, t.sensor, t.pos[0], t.pos[1], t.pos[2],
		   t.quat[0], t.quat[1], t.quat[2], t.quat[3]);*/

}



void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
	const char *name = (const char *)userdata;
	printf("Button\n");
	Hook::checkButton(name, b);
}


void VRPN_CALLBACK handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b) {
	const char *name = (const char *)userdata;

	printf("Button %s has %d buttons with states!!!:", name, b.num_buttons);
	/*int i;
	for ( i = 0; i < b.num_buttons; i++ ) {
		printf(" %d", b.states[i]);
	}*/
	printf("\n");

}


void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a) {
	//int i;
	const char *name = (const char *)userdata;

	/*printf("!!!Analog %s:\n         %5.2f", name, a.channel[0]);
	for ( i = 1; i < a.num_channel; i++ ) {
		printf(", %5.2f", a.channel[i]);
	}
	printf(" (%d chans)\n", a.num_channel);*/
}

