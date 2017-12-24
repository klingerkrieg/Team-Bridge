#include <stdio.h>                      // for fprintf, stderr, printf, etc

#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"
#include "vrpn_Shared.h"                // for vrpn_SleepMsecs
#include "vrpn_Tracker.h"               // for vrpn_TRACKERCB, etc
#include "vrpn_Types.h"                 // for vrpn_float64
#include <vector>                // for vector


using namespace std;

std::string TRACKER_NAME = "Tracker0";
int	CONNECTION_PORT = vrpn_DEFAULT_LISTEN_PORT_NO;	// Port for connection to listen on

vrpn_Tracker_Server	*trackerServer;
vrpn_Tracker_Remote	*trackerRem;
vrpn_Connection		*connection;

bool print = false;

int delay = 0;

class tracker_user_callback {
public:
    char t_name[vrpn_MAX_TEXT_LEN];
    vector<unsigned> t_counts;
};



/*****************************************************************************
 *
   Callback handlers
 *
 *****************************************************************************/

void VRPN_CALLBACK
handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t)
{
    tracker_user_callback *t_data = static_cast<tracker_user_callback *>(userdata);
	
    // Make sure we have a count value for this sensor
    while (t_data->t_counts.size() <= static_cast<unsigned>(t.sensor)) {
        t_data->t_counts.push_back(0);
    }

	printf("Tracker %s, sensor [%d]:\n     pos (%5.2f, %5.2f, %5.2f); "
			"quat (%5.2f, %5.2f, %5.2f, %5.2f)\n",
			t_data->t_name, t.sensor, t.pos[0], t.pos[1], t.pos[2],
			t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
    
}


void usage() {
	printf("Usage:\n\n Obrigatoriamente passe como primeiro parametro o nome do arquivo.txt, o numero de sensores e o endereço do dispositivo, em seguida as demais opcoes\n-p - Print tracking\n-n - no print\nd - Delay");
}




int main (int argc, char * argv []) {

	printf("default port: %d \n", CONNECTION_PORT);
	if (argc < 4) {
		usage();
		return -1;
	}

	for ( int i = 0; i < argc; i++ ) {
		if ( !strcmp(argv[i],"-p") ) {
			print = true;
		}

		if ( !strcmp(argv[i], "-d") ) {
			delay = atoi(argv[i + 1]);
		}
	}
	
	
	TRACKER_NAME = argv[3];
	// explicitly open the connection
	connection = vrpn_create_server_connection(CONNECTION_PORT);


	int sensoresQtd = atoi(argv[2]);
	printf("Sensores:[%d] Arquivo:[%s]\n",atoi(argv[2]), argv[1]);


	// Open the tracker server, using this connection, 2 sensors, update 60 times/sec
	trackerServer = new vrpn_Tracker_Server(TRACKER_NAME.c_str(), connection, sensoresQtd);

	
	
	// Open the tracker remote using this connection
	
	trackerRem = new vrpn_Tracker_Remote (TRACKER_NAME.c_str(), connection);
	tracker_user_callback *tc1 = new tracker_user_callback;
	strncpy(tc1->t_name, TRACKER_NAME.c_str(), sizeof(tc1->t_name));

	fprintf(stderr, "Tracker's name is %s.\n", TRACKER_NAME.c_str());

	

	// Set up the tracker callback handlers
	//printf("Tracker update: '.' = pos, '/' = vel, '~' = acc\n");
	if ( print ){
		trackerRem->register_change_handler(tc1, handle_tracker_pos_quat);
	}
	
	//Le o arquivo e faz o mainloop ao mesmo tempo
	char str[512];
	FILE * file;
	file = fopen( argv[1] , "r");

	

	timeval t;
	
	if (file) {

		while (1) { 

			file = fopen(argv[1], "r");

			while ( fgets (str , 512 , file) != NULL ){

				//Caso seja comentario
				if (str[0] == '#'){
					continue;
				}


				vrpn_gettimeofday(&t, NULL);

				//pula sensor
				char * part = strtok(str, " \t");
				if ( strcmp(part, TRACKER_NAME.c_str()) ) {
					//Caso nao seja do dispositivo esperado
					continue;
				}

				part = strtok (NULL," \t");
				part = strtok (NULL, " \t");
				int sensor = atoi(part);

				//pula timestamp
				part = strtok(NULL, " \t");
				part = strtok(NULL, " \t");
			
				//pula pos
				part = strtok (NULL, " \t");
				part = strtok (NULL, " \t");
				double pos1 = atof(part);

				part = strtok (NULL, " \t");
				double pos2 = atof(part);

				part = strtok (NULL, " \t");
				double pos3 = atof(part);

				//pula quat
				part = strtok (NULL, " \t");
				part = strtok (NULL, " \t");
				double quat1 = atof(part);

				part = strtok (NULL, " \t");
				double quat2 = atof(part);

				part = strtok (NULL, " \t");
				double quat3 = atof(part);

				
				part = strtok (NULL, " \t");
				double quat4 = atof(part);


				// Let the tracker server, client and connection do their things
				trackerServer->mainloop();
				
				
				vrpn_float64 position[3] = {pos1, pos2, pos3};
				vrpn_float64 quaternion[4] = {quat1, quat2, quat3, quat4};
				
				trackerServer->report_pose(sensor,t, position, quaternion,vrpn_CONNECTION_LOW_LATENCY);

				if ( print ){
					trackerRem->mainloop();
				}
				connection->mainloop();

				// Sleep for 1ms so we don't eat the CPU
				vrpn_SleepMsecs(1);

				if ( delay != 0 ) {
					Sleep(delay);
				}
			}
			fclose(file);

			printf("Endfile\n");
		}
		
	}

	return 0;

}   /* main */


