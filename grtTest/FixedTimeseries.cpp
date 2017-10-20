/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
GRT DTW Example
This examples demonstrates how to initialize, train, and use the DTW algorithm for classification.

The Dynamic Time Warping (DTW) algorithm is a powerful classifier that works very well for recognizing temporal gestures.

In this example we create an instance of an DTW algorithm and then train the algorithm using some pre-recorded training data.
The trained DTW algorithm is then used to predict the class label of some test data.

This example shows you how to:
- Create an initialize the DTW algorithm
- Load some LabelledTimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
- Trim any periods of non-movement from the start and end of each timeseries recording
- Train the DTW algorithm using the training dataset
- Test the DTW algorithm using the test dataset
- Manually compute the accuracy of the classifier
*/

//You might need to set the specific path of the GRT header relative to your project
#include "GRT.h"
#include <iostream>
#include <windows.h>

using namespace GRT;
using namespace std;


float x = 0;
float y = 0;

HANDLE hStdout;
COORD destCoord;

int tam = 6;
int linha1[] = { 1, 5, 3, 2, 1 };
int linha2[] = { 1, 4, 0, 4, 1 };
int linha3[] = { 0, 0, 0, 0, 1, 5, 3, 2, 1, 0, 0, 0 };

int fixedTimeSeries() {

	MatrixDouble timeseries;


	//Create a new DTW instance, using the default parameters
	DTW dtw;

	


	TimeSeriesClassificationData trainingData;
	UINT trainingClassLabel;
	//GestureRecognitionPipeline pipeline;


	trainingData.setNumDimensions(1);

	//dtw.enableZNormalization(true);
	//dtw.enableTrimTrainingData(true, 0.1, 90);

	//pipeline.setClassifier(dtw);
	
	printf("\nSeq1:");
	//1
	trainingClassLabel = 1;
	
	VectorDouble sample(1);
	
	for ( int i = 0; i < tam; i++ ) {
		sample[0] = linha1[i];
		printf("%.2f ", sample[0]);
		timeseries.push_back(sample);
	}

	trainingData.addSample(trainingClassLabel, timeseries);
	timeseries.clear();

	printf("\nSeq2:");
	//2
	trainingClassLabel = 2;

	for ( int i = 0; i < tam; i++ ) {
		sample[0] = linha2[i];
		printf("%.2f ", sample[0]);
		timeseries.push_back(sample);
	}

	trainingData.addSample(trainingClassLabel, timeseries);

	printf("\n");
	dtw.train(trainingData);
	//pipeline.train(trainingData);
	timeseries.clear();
	//predict


	//Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
	//dtw.enableNullRejection(true);

	//Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
	//You can increase this value if you find that your real-time gestures are not being recognized
	//If you are getting too many false positives then you should decrease this value
	//dtw.setNullRejectionCoeff(50);

	
	

	float read;
	while ( true ) {


		printf("\nDigite um numero: ");

		for ( int i = 0; i < 6; i++ ) {

			scanf("%f", &read);
			sample[0] = read;
			//sample[0] = linha3[i];
			
			if ( timeseries.getSize() < 6 ) {
				timeseries.push_back(sample);
			} else {

				int y;
				for ( y = 0; y < 5; y++ ) {
					VectorFloat row = timeseries.getRow(y+1);
					timeseries.setRowVector(row, y);
				}

				timeseries.setRowVector(sample, y);

				//timeseries.resize(3, 1);
				printf("size:%d\n", timeseries.getSize());


				for ( int y = 0; y < timeseries.getSize(); y++ ) {

					VectorFloat row = timeseries.getRow(y);
					
					for ( std::vector<double>::iterator it = row.begin(); it != row.end(); ++it ) {
						printf("%.2f ", (*it));
					}
				}
			}
		

			//if ( !pipeline.predict(timeseries) ) {
			if ( !dtw.predict(timeseries) ) {
				printf("FALHA");
			}

			UINT predictedClassLabel = dtw.getPredictedClassLabel();
			double maximumLikelihood = dtw.getMaximumLikelihood();
			VectorDouble classLikelihoods = dtw.getClassLikelihoods();
			VectorDouble classDistances = dtw.getClassDistances();

			cout << "\nPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;

			//timeseries.clear();

		}

	}
	
		
	return EXIT_SUCCESS;
}



