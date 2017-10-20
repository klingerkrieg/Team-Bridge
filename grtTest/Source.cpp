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
#include <conio.h>

using namespace GRT;
using namespace std;


float x = 0;
float y = 0;

HANDLE hStdout;
COORD destCoord;


void getMousePos() {
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	x = cursorPos.x;
	y = cursorPos.y;


	//printf("x: %.2f y : %.2f\n", x, y);



}

int main(int argc, const char * argv[]) {

	MatrixDouble timeseries;

	/*

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	//position cursor at start of window
	destCoord.X = 10;
	destCoord.Y = 10;
	SetConsoleCursorPosition(hStdout, destCoord);
	printf(".");




	int i = 0;
	while ( true ) {

	getMousePos();
	i++;
	if ( i % 1000 == 0 ) {
	destCoord.X = x;
	destCoord.Y = y;
	SetConsoleCursorPosition(hStdout, destCoord);
	printf("x");
	}
	}
	*/


	//Create a new DTW instance, using the default parameters
	DTW dtw;

	//Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
	//dtw.enableNullRejection(true);

	//Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
	//You can increase this value if you find that your real-time gestures are not being recognized
	//If you are getting too many false positives then you should decrease this value
	//dtw.setNullRejectionCoeff(3);


	TimeSeriesClassificationData trainingData;
	UINT trainingClassLabel;
	GestureRecognitionPipeline pipeline;

	int movDelay = 100;
	int trainSize = 15000;
	int predictSize = 2000;
	
	trainingData.setNumDimensions(2);

	pipeline.setClassifier(dtw);
	trainingClassLabel = 1;
	printf("Faca uma letra com o mouse, aperte uma tecla:");
	Sleep(3000);
	printf("já");

	int y = 0;
	while ( timeseries.getSize() < trainSize ) {

		getMousePos();
		y++;
		if ( y % movDelay == 0 ) {

			VectorDouble sample(2);
			sample[0] = x;
			sample[1] = y;
			timeseries.push_back(sample);
			y = 0;

			printf(".");
			
		}
	}

	trainingData.addSample(trainingClassLabel, timeseries);
	pipeline.train(trainingData);
	timeseries.clear();


	trainingClassLabel = 2;
	printf("Faca outra letra com o mouse, aperte uma tecla:");
	Sleep(3000);
	printf("ja");

	y = 0;
	while ( timeseries.getSize() < trainSize ) {

		getMousePos();
		y++;
		if ( y % movDelay == 0 ) {

			VectorDouble sample(2);
			sample[0] = x;
			sample[1] = y;
			timeseries.push_back(sample);
			y = 0;

			printf(".");

		}
	}

	trainingData.addSample(trainingClassLabel, timeseries);
	pipeline.train(trainingData);
	timeseries.clear();
	

	printf("Repita o movimento:");

	y = 0;

	
	while ( true ) {

		getMousePos();
		y++;
		if ( y % movDelay == 0 ) {

			VectorDouble sample(2);
			sample[0] = x;
			sample[1] = y;
			timeseries.push_back(sample);
			y = 0;

			printf(".");

		}


		if ( timeseries.getSize() > predictSize ) {
			if ( !pipeline.predict(timeseries) ) {
				printf("FALHA");
			}


			UINT predictedClassLabel = dtw.getPredictedClassLabel();
			double maximumLikelihood = dtw.getMaximumLikelihood();
			VectorDouble classLikelihoods = dtw.getClassLikelihoods();
			VectorDouble classDistances = dtw.getClassDistances();

			cout << "\nPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;

			timeseries.clear();
		}
	}

	/*
	while ( true ) {

		getMousePos();
		
		VectorDouble sample(2);
		sample[0] = x;
		sample[1] = y;
		timeseries.push_back(sample);
			

		if ( !pipeline.predict(timeseries) ) {
			printf("FALHA");
		}


		UINT predictedClassLabel = dtw.getPredictedClassLabel();
		double maximumLikelihood = dtw.getMaximumLikelihood();
		VectorDouble classLikelihoods = dtw.getClassLikelihoods();
		VectorDouble classDistances = dtw.getClassDistances();

		cout << "\nPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;

		timeseries.clear();
		
	}
	*/
	return EXIT_SUCCESS;
}



