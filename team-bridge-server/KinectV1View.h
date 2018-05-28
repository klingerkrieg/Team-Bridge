#pragma once

#include "GLView.h"
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>



class KinectV1View : public GLView {

	private:

	// Kinect V1 variables
	static HANDLE depthStream;
	static INuiSensor* sensor;

	static void getKinectData(GLubyte* dest);

	bool configure();

	public:
	static void drawKinectData();

};

