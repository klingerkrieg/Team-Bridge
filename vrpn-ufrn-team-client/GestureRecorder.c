#include "GestureRecorder.h"

std::map<int,Sensor> GestureRecorder::writing;
int GestureRecorder::lastTime = 0;
int GestureRecorder::countMessages = 0;
bool GestureRecorder::recording = false;
std::string GestureRecorder::recordName = "";
std::ofstream GestureRecorder::fileOutput;

std::vector<int> GestureRecorder::sensors;

void GestureRecorder::record(TrackerUserCallback *userdata, const vrpn_TRACKERCB t) {
	int actTime = (int)time(0);

	if ( recording == false ) {
		
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
			fileOutput = std::ofstream(saveDir + "/" + recordName + ".txt");
			lastTime = actTime;
		}

		return;
	} else {

		if ( actTime - lastTime >= recordDuration ) {
			fileOutput.close();
			exit(0);
		}

		for ( int i = 0; i < sensors.size(); i++ ) {
			if ( t.sensor == sensors[i] ) {

				Sensor s;
				s.id = t.sensor;
				s.x = t.pos[0];
				s.y = t.pos[1];
				s.z = t.pos[2];
				//Adiciona para ser salvo em uma linha
				writing.insert_or_assign(t.sensor, s);

				//Salva todos os pontos em uma unica linha
				if ( writing.size() == sensors.size() ) {
					for ( int y = 0; y < sensors.size(); y++ ) {
						Sensor s = writing.at(sensors[y]);
						fileOutput << std::to_string(s.id) + "\t" + std::to_string(s.x) + "\t" + std::to_string(s.y) + "\t" + std::to_string(s.z) + "\t";
					}
					fileOutput << "\n";
					//reseta o que esta pra ser salvo
					writing.clear();
				}
				break;
			}
		}

		printf("gravando.. %d\n", actTime - lastTime);
		return;
	}


}