#pragma warning(suppress : 4996)
#define GRT_STATIC_LIB 1
#include <stdio.h>                      // for fprintf, stderr, printf, etc
#include "GRT/GRT.h"
#include <iostream>
#include <string>

using namespace GRT;
using namespace std;


#define LEAPMOTION (1)
#define HANDS_LEN (1)

#if LEAPMOTION == 1
	//usar somente
	#if (HANDS_LEN == 2)
		const int sensoresLength = 12;//44;
	#else 
		const int sensoresLength = 6;//22
	#endif
	//int sensores[sensoresLength];//22
	int sensores[] = {0, 6, 10, 14, 18, 22};
#else
	//kinect
	//usar somente
	const int sensoresLength = 22;
	int sensores[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21 };
#endif



double hand_pos[2][3];
ClassificationData trainingData;

void train(int gesto, double trainLine[][4]) {
	UINT gestureLabel = gesto;
	VectorDouble sample(sensoresLength*4);

	int x = 0;
	for (int i = 0; i < sensoresLength; i++) {

		if (i == 0 || i == 23) {
			//trainLine inicia do 1 porque o 0 é o sensor
			hand_pos[i][0] = trainLine[i][1];
			hand_pos[i][1] = trainLine[i][2];
			hand_pos[i][2] = trainLine[i][3];
		} else {
			//conversao para manter as juntas relativas a palma 0 ou 23
			int y = 0;
			if (i >= 23) {
				y = 1;
			}

			//printf("Convertido de %.2f ", trainLine[i][1]);
			trainLine[i][1] = trainLine[i][1] - hand_pos[y][0];
			//printf(" para %.2f\n", trainLine[i][1]);
			trainLine[i][2] = trainLine[i][2] - hand_pos[y][1];
			trainLine[i][3] = trainLine[i][3] - hand_pos[y][2];
		}

		for (int y = 0; y < 4; y++) {
			sample[x] = trainLine[i][y];
			x++;
		}
	}

	trainingData.addSample(gestureLabel, sample);
}



int main(int argc, char * argv[]) {

	/*#if LEAPMOTION == 1
		int y = 0;
		for (int i = 0; i <= sensoresLength; i++) {
			//pula o eixo do cotovelo
			if (i == 1)
				continue;
			sensores[y] = i;
			y++;
		}
	#endif*/


	if (argc != 3) {
		cout << "Passe o nome do arquivo.txt como primeiro parametro em seguida um int para a id do gesto.";
	}

	//Carrega ou cria arquivo de treino
	if (!trainingData.load("TrainingData.grt")) {
		cout << "Criando arquivo de treino..\n";
		trainingData.setNumDimensions( sensoresLength*4 );
		trainingData.setDatasetName("DummyData");
		trainingData.setInfoText("This data contains some dummy data");
	} else {
		cout << "Adicionando a arquivo ja existente\n";
		trainingData.setDatasetName("DummyData");
		trainingData.setInfoText("This data contains some dummy data");
	}

	//cada linha sera composta por todos os sensores, tera 1 numero do sensor + 3 posicoes
	double trainLine[sensoresLength][4];
	int trainKey = 0;
	int trainLineFull = 0;

	int idGesto = atoi(argv[2]);

	cout << "GESTO ID:" << idGesto << "\n";

	//Le o arquivo e faz o mainloop ao mesmo tempo
	char str[512];
	FILE * file;
	file = fopen(argv[1], "r");
	

	if (file) {
		while (fgets(str, 100, file) != NULL) {
			//Caso seja comentario
			if (str[0] != 's') {
				continue;
			}


			//pula sensor
			char * part = strtok(str, "\t");
			part = strtok(NULL, " \t");
			int sensor = atoi(part);

			//pula pos
			part = strtok(NULL, " \t");
			part = strtok(NULL, " \t");
			double pos1 = atof(part);

			part = strtok(NULL, " \t");
			double pos2 = atof(part);

			part = strtok(NULL, " \t");
			double pos3 = atof(part);

			

			//pula quat
			/*part = strtok(NULL, " \t");
			part = strtok(NULL, " \t");
			double quat1 = atof(part);

			part = strtok(NULL, " \t");
			double quat2 = atof(part);

			part = strtok(NULL, " \t");
			double quat3 = atof(part);

			part = strtok(NULL, " \n");
			double quat4 = atof(part);*/

			//ignora os sensores nao usados
			bool  canTrain = false;
			for (trainKey = 0; trainKey < sensoresLength; trainKey++) {
				if (sensor == sensores[trainKey]) {
					canTrain = true;
					break;
				}
			}

			if (canTrain) {
				//cout << pos1 << "\t" << pos2 << "\t" << pos3 << "\n";
				trainLine[trainKey][0] = sensor;
				trainLine[trainKey][1] = pos1;
				trainLine[trainKey][2] = pos2;
				trainLine[trainKey][3] = pos3;
				trainLineFull++;

				if (trainLineFull == sensoresLength) {
					train(idGesto, trainLine);
					trainLineFull = 0;
				}
				
			}
		}

		fclose(file);
	}



	//After recording your training data you can then save it to a file
	if (!trainingData.save("TrainingData.grt")) {
		cout << "ERROR: Failed to save dataset to file!\n";
		return EXIT_FAILURE;
	}

	//This can then be loaded later
	if (!trainingData.load("TrainingData.grt")) {
		cout << "ERROR: Failed to load dataset from file!\n";
		return EXIT_FAILURE;
	}

	//You can also save and load the training data to a CSV file
	//Each row will contain a sample, with the first column containing the class label and the remaining columns containing the data
	if (!trainingData.save("TrainingData.csv")) {
		cout << "ERROR: Failed to save dataset to csv file!\n";
		return EXIT_FAILURE;
	}

	if (!trainingData.load("TrainingData.csv")) {
		cout << "ERROR: Failed to load dataset from csv file!\n";
		return EXIT_FAILURE;
	}

	//This is how you can get some stats from the training data
	UINT numSamples = trainingData.getNumSamples();
	UINT numDimensions = trainingData.getNumDimensions();
	UINT numClasses = trainingData.getNumClasses();


	cout << "NumberOfSamples: " << numSamples << endl;
	cout << "NumberOfDimensions: " << numDimensions << endl;
	cout << "NumberOfClasses: " << numClasses << endl;

	//You can also get the minimum and maximum ranges of the data
	/*vector< MinMax > ranges = trainingData.getRanges();

	cout << "The ranges of the dataset are: \n";
	for (UINT j = 0; j<ranges.size(); j++) {
		//cout << "Dimension: " << j << " Min: " << ranges[j].minValue << " Max: " << ranges[j].maxValue << endl;
	}*/

	//If you want to partition the dataset into a training dataset and a test dataset then you can use the partition function
	//A value of 80 means that 80% of the original data will remain in the training dataset and 20% will be returned as the test dataset
	/*ClassificationData testData = trainingData.split(80);

	//If you have multiple datasets that you want to merge together then use the merge function
	if (!trainingData.merge(testData)) {
		cout << "ERROR: Failed to save merge datasets!\n";
		return EXIT_FAILURE;
	}

	//If you want to run K-Fold cross validation using the dataset then you should first spilt the dataset into K-Folds
	//A value of 10 splits the dataset into 10 folds and the true parameter signals that stratified sampling should be used
	if (!trainingData.spiltDataIntoKFolds(10, true)) {
		cout << "ERROR: Failed to spiltDataIntoKFolds!\n";
		return EXIT_FAILURE;
	}

	//After you have called the spilt function you can then get the training and test sets for each fold
	for (UINT foldIndex = 0; foldIndex<10; foldIndex++) {
		ClassificationData foldTrainingData = trainingData.getTrainingFoldData(foldIndex);
		ClassificationData foldTestingData = trainingData.getTestFoldData(foldIndex);
	}*/

	return 0;

}   /* main */


