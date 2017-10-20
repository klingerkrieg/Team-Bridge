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


void getMousePos() {
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	x = cursorPos.x;
	y = cursorPos.y;


	//printf("x: %.2f y : %.2f\n", x, y);

	

}

int main (int argc, const char * argv[]) {

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
	dtw.enableNullRejection(true);

	//Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
	//You can increase this value if you find that your real-time gestures are not being recognized
	//If you are getting too many false positives then you should decrease this value
	dtw.setNullRejectionCoeff(3);
    
	//Load some training data to train the classifier - the DTW uses LabelledTimeSeriesClassificationData
	LabelledTimeSeriesClassificationData trainingData;
    
	if( !trainingData.loadDatasetFromFile("DTWTrainingData.txt") ){
		cout << "Failed to load training data!\n";
		return EXIT_FAILURE;
	}
    
	//Use 20% of the training dataset to create a test dataset
	LabelledTimeSeriesClassificationData testData = trainingData.split( 80 );

	//Trim the training data for any sections of non-movement at the start or end of the recordings
	dtw.enableTrimTrainingData(true,0.1,90);
    
	//Train the classifier
	if( !dtw.train( trainingData ) ){
		cout << "Failed to train classifier!\n";
		return EXIT_FAILURE;
	}	
    
	//Save the DTW model to a file
	if( !dtw.save("DTWModel.txt") ){
		cout << "Failed to save the classifier model!\n";
		return EXIT_FAILURE;
	}
    
	//Load the DTW model from a file
	if( !dtw.load("DTWModel.txt") ){
		cout << "Failed to load the classifier model!\n";
		return EXIT_FAILURE;
	}
    
	//Use the test dataset to test the DTW model
	double accuracy = 0;
	for(UINT i=0; i<testData.getNumSamples(); i++){
		//Get the i'th test sample - this is a timeseries
		UINT classLabel = testData[i].getClassLabel();
		MatrixDouble timeseries = testData[i].getData();
        
		//Perform a prediction using the classifier
		if( !dtw.predict( timeseries ) ){
			cout << "Failed to perform prediction for test sampel: " << i <<"\n";
			return EXIT_FAILURE;
		}
        
		//Get the predicted class label
		UINT predictedClassLabel = dtw.getPredictedClassLabel();
		double maximumLikelihood = dtw.getMaximumLikelihood();
		VectorDouble classLikelihoods = dtw.getClassLikelihoods();
		VectorDouble classDistances = dtw.getClassDistances();
        
		//Update the accuracy
		if( classLabel == predictedClassLabel ) accuracy++;
        
        cout << "TestSample: " << i <<  "\tClassLabel: " << classLabel << "\tPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;
	}
    
	cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;

	int y = 0;
	while ( true ) {

		getMousePos();
		y++;
		if (y % 5000 == 0){

			VectorDouble sample(2);
			sample[0] = x;
			sample[1] = y;
			timeseries.push_back(sample);
			y = 0;

			printf(".");

		}
		

		if ( timeseries.getSize() > 5000 ) {
			if ( !dtw.predict(timeseries) ) {
				printf("FALHA");
			}


			UINT predictedClassLabel = dtw.getPredictedClassLabel();
			double maximumLikelihood = dtw.getMaximumLikelihood();
			VectorDouble classLikelihoods = dtw.getClassLikelihoods();
			VectorDouble classDistances = dtw.getClassDistances();

			cout << "\tPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;

			timeseries.clear();
		}
	}
    
	return EXIT_SUCCESS;
}



