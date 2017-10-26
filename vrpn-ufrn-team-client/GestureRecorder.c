#include "GestureRecorder.h"

int GestureRecorder::lastTime = 0;
int GestureRecorder::countMessages = 0;
bool GestureRecorder::recording = false;


void GestureRecorder::record(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {

	if ( recording == false ) {
		int actTime = (int)time(0);

		//Inicia a contagem para gravação
		if ( lastTime == 0 ) {
			countMessages = recordDelay;
			lastTime = actTime;
		}
			

		if ( actTime - lastTime >= countDelay ) {
			lastTime = actTime;
			countMessages--;
			printf("%d\n", countMessages);
			if ( viewOn ) {
				view->showMsg(std::to_string(countMessages));
			}
		}
		
		if ( countMessages <= 0 ) {
			printf("Gravando");
			if ( viewOn ) {
				view->showMsg("Gravando");
			}
			recording = true;
		}

		return;
	} else {
		printf("gravando..\n");
		return;
	}


}