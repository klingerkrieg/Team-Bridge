#define GRT_STATIC_LIB 1

#include <stdio.h>                      // for fprintf, stderr, printf, etc

#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"
#include "vrpn_Shared.h"                // for vrpn_SleepMsecs
#include "vrpn_Tracker.h"               // for vrpn_TRACKERCB, etc
#include "vrpn_Types.h"                 // for vrpn_float64
#include <vector>                // for vector

#include "GRT/GRT.h"
#include <iostream>
#include <string>


using namespace GRT;
using namespace std;

float rand(float min, float max) {
	return (max - min) * ((((float)rand()) / (float)RAND_MAX)) + min;
}


const char	*TRACKER_NAME = "Tracker0@localhost";
int	CONNECTION_PORT = vrpn_DEFAULT_LISTEN_PORT_NO;	// Port for connection to listen on

vrpn_Tracker_Server	*trackerServer;
vrpn_Tracker_Remote	*trackerRem;
vrpn_Connection		*connection;
unsigned tracker_stride = 1; // Every nth report will be printed
GestureRecognitionPipeline pipeline;

bool modoCodigo = false;

//Usar somente esses sensores
const int sensoresLength = 6;
int sensores[] = { 0, 6, 10, 14, 18, 22 };

double groupData[sensoresLength][4];
int groupSize = 0;
double hand_pos[2][3];


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

void VRPN_CALLBACK handle_tracker_pos_quat(void *userdata, const vrpn_TRACKERCB t) {
	tracker_user_callback *t_data = static_cast<tracker_user_callback *>(userdata);

	// Make sure we have a count value for this sensor
	while (t_data->t_counts.size() <= static_cast<unsigned>(t.sensor)) {
		t_data->t_counts.push_back(0);
	}

	if (++t_data->t_counts[t.sensor] >= tracker_stride) {

		t_data->t_counts[t.sensor] = 0;
		/*printf("Tracker %s, sensor %d:\n     pos (%5.2f, %5.2f, %5.2f); "
			"quat (%5.2f, %5.2f, %5.2f, %5.2f)\n",
			t_data->t_name, t.sensor, t.pos[0], t.pos[1], t.pos[2],
			t.quat[0], t.quat[1], t.quat[2], t.quat[3]);*/


		//ignora os sensores nao usados
		bool  canUse = false;
		int key;
		for (key = 0; key < sensoresLength; key++) {
			if (t.sensor == sensores[key]) {
				canUse = true;
				break;
			}
		}
		if (canUse == false) {
			return;
		}


		

		//vai preenchendo os grupos - dados brutos
		groupData[key][0] = t.sensor;
		groupData[key][1] = t.pos[0];
		groupData[key][2] = t.pos[1];
		groupData[key][3] = t.pos[2];
		groupSize++;
		

		//quando todo o grupo rastreado for preenchido
		if (groupSize == sensoresLength) {
			groupSize = 0;


			//correcao da posicao dos dedos para estarem relativos a mao
			for ( int i = 0; i < sensoresLength; i++ ) {

				//conversao para manter as juntas relativas a palma 0 ou 23
				int y = 0;
				if ( i >= 23 ) {
					y = 1;
				}

				if ( i == 0 || i == 23 ) {
					//trainLine inicia do 1 porque o 0 é o sensor
					hand_pos[y][0] = groupData[i][1];
					hand_pos[y][1] = groupData[i][2];
					hand_pos[y][2] = groupData[i][3];
				} else {
					groupData[i][1] = groupData[i][1] - hand_pos[y][0];
					groupData[i][2] = groupData[i][2] - hand_pos[y][1];
					groupData[i][3] = groupData[i][3] - hand_pos[y][2];
				}
			}



			if ( modoCodigo ) {

				double dist[3];

				dist[0] = groupData[1][1] - groupData[2][1];
				dist[1] = groupData[1][2] - groupData[2][2];
				dist[2] = groupData[1][3] - groupData[2][3];


				if ( dist[0] > -15 && dist[0] > 0 && dist[0] < 15 ) {
					printf("Pinca\n");
				}
				//printf("%.2f\t-\t%.2f\t=\t%.2f\n", groupData[1][1], groupData[2][1], dist[0]);
				//Utiliza codigo
				printf("%.2f\t%.2f\t%.2f\n", dist[0], dist[1], dist[2]);


			
			} else {
				//Utiliza aprendizado de maquina
				//verifica se esta batendo com algum gesto
				VectorDouble inputVector(sensoresLength * 4);


				//preenche o array para predicao
				int x = 0;
				for ( int i = 0; i < sensoresLength; i++ ) {
					for ( int y = 0; y < 4; y++ ) {
						inputVector[x] = groupData[i][y];
						x++;
					}
				}

				pipeline.reset();

				bool predictionSuccess = pipeline.predict(inputVector);
				if ( predictionSuccess ) {

					UINT predictedClassLabel = pipeline.getPredictedClassLabel();

					cout << "GESTO " << predictedClassLabel << "\n";

					cout << pipeline.getLastErrorMessage();


				} else {
					cout << "Fail\n";
					cout << pipeline.getInfo();
					cout << pipeline.getLastInfoMessage();
					cout << pipeline.getLastErrorMessage();

				}

			}
			
		}

	}
}






int main(int argc, char * argv[]) {


	if (strcmp(argv[1], "code") == 0) {
		printf("Usando codigo\n");
		modoCodigo = true;
	} else {

		ClassificationData trainingData;
		if ( !trainingData.load("TrainingData.grt") ) {
			cout << "ERROR: Failed to load dataset from file!\n";
			return EXIT_FAILURE;
		}



		pipeline.addPreProcessingModule(MovingAverageFilter(5, (4 * sensoresLength)));


		if ( strcmp(argv[1], "knn") == 0 ) {
			pipeline.setClassifier(KNN());
			printf("Usando KNN\n");
		} else
		if ( strcmp(argv[1], "anbc") == 0 ) {
			pipeline.setClassifier(ANBC());
			printf("Usando ANBC\n");
		} else {
			pipeline.setClassifier(SVM());
			printf("Usando SVM\n");
		}


		struct timeval tp;
		gettimeofday(&tp, NULL);
		float ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

		//pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(1000));
		printf("Training...\n");

		bool trainSuccess = pipeline.train(trainingData);
		if ( trainSuccess ) {
			gettimeofday(&tp, NULL);
			float ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			printf("Trained. %.2f s\n", ((ms2 - ms) / 1000));


			double accuracy = pipeline.getCrossValidationAccuracy();
			printf("CrossValidationAccuracy: %.2f\n", accuracy);

			ClassificationData testData = trainingData.partition(80);
			if ( !pipeline.test(testData) ) {
				cout << "ERROR: Failed to test the pipeline!\n";
				return EXIT_FAILURE;
			}
			cout << "Test Accuracy: " << pipeline.getTestAccuracy() << endl;

			cout << "Precision: ";
			for ( UINT k = 0; k < pipeline.getNumClassesInModel(); k++ ) {
				UINT classLabel = pipeline.getClassLabels()[k];
				cout << "\t" << pipeline.getTestPrecision(classLabel);
			}
			cout << endl;
		} else {
			return -1;
		}

	}

	trackerRem = new vrpn_Tracker_Remote(TRACKER_NAME);
	tracker_user_callback *tc1 = new tracker_user_callback;
	strncpy(tc1->t_name, TRACKER_NAME, sizeof(tc1->t_name));

	fprintf(stderr, "Tracker's name is %s.\n", TRACKER_NAME);

	
	trackerRem->register_change_handler(tc1, handle_tracker_pos_quat);
	
	
	while (1) {
		trackerRem->mainloop();
		
		vrpn_SleepMsecs(1);
	}
		
	return 0;

}   /* main */
