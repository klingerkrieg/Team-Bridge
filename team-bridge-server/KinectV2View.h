#pragma once
#ifdef KINECTV2
#include "GLView.h"
#include <Kinect.h>


class KinectV2View : public GLView {

	private:

	// Kinect V2 variables
	static IKinectSensor* sensor;         // Kinect sensor
	static IDepthFrameReader* reader;     // Kinect depth data source

	static void getKinectData(GLubyte* dest);

	bool configure();

	public:
	static void drawKinectData();

};

#endif