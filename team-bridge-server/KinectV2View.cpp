#include "KinectV2View.h"

#ifdef KINECTV2
// Kinect variables
IKinectSensor* KinectV2View::sensor;         // Kinect sensor
IDepthFrameReader* KinectV2View::reader;     // Kinect depth data source


bool KinectV2View::configure() {
	if ( FAILED(GetDefaultKinectSensor(&sensor)) ) {
		return false;
	}
	if ( sensor ) {
		sensor->Open();
		IDepthFrameSource* framesource = NULL;
		sensor->get_DepthFrameSource(&framesource);
		framesource->OpenReader(&reader);
		if ( framesource ) {
			framesource->Release();
			framesource = NULL;
		}


		drawDataFunc = drawKinectData;

		return true;
	} else {
		return false;
	}
}



void KinectV2View::getKinectData(GLubyte* dest) {
	IDepthFrame* frame = NULL;
	if ( SUCCEEDED(reader->AcquireLatestFrame(&frame)) ) {
		unsigned int sz;
		unsigned short* buf;
		frame->AccessUnderlyingBuffer(&sz, &buf);

		const unsigned short* curr = (const unsigned short*)buf;
		const unsigned short* dataEnd = curr + (width*height);

		while ( curr < dataEnd ) {
			// Get depth in millimeters
			unsigned short depth = (*curr++);

			// Draw a grayscale image of the depth:
			// B,G,R are all set to depth%256, alpha set to 1.
			for ( int i = 0; i < 3; ++i )
				*dest++ = (BYTE)depth % 256;
			*dest++ = 0xff;
		}
	}
	if ( frame ) frame->Release();

}

void KinectV2View::drawKinectData() {
	glBindTexture(GL_TEXTURE_2D, textureId);
	getKinectData(data);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, 0, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, height, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, height, 0.0f);
	glEnd();
}



#endif