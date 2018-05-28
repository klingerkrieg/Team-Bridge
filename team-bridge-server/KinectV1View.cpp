#include "KinectV1View.h"


// Kinect variables
HANDLE KinectV1View::depthStream;
INuiSensor * KinectV1View::sensor;


bool KinectV1View::configure() {
	// Get a working kinect sensor
	int numSensors;
	if ( NuiGetSensorCount(&numSensors) < 0 || numSensors < 1 ) return false;
	if ( NuiCreateSensorByIndex(0, &sensor) < 0 ) return false;

	// Initialize sensor
	sensor->NuiInitialize(
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
		| NUI_INITIALIZE_FLAG_USES_DEPTH
		| NUI_INITIALIZE_FLAG_USES_COLOR
		| NUI_INITIALIZE_FLAG_USES_SKELETON);

	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH, // Depth camera or rgb camera?
		NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
		0,         // Image stream flags, e.g. near mode
		2,        // Number of frames to buffer
		NULL,   // Event handle
		&depthStream);

	drawDataFunc = drawKinectData;

	return sensor;
}



void KinectV1View::getKinectData(GLubyte* dest) {
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if ( sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0 ) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	if ( LockedRect.Pitch != 0 ) {
		const USHORT* curr = (const USHORT*)LockedRect.pBits;
		const USHORT* dataEnd = curr + (width*height);

		while ( curr < dataEnd ) {
			// Get depth in millimeters
			USHORT depth = NuiDepthPixelToDepth(*curr++);

			// Draw a grayscale image of the depth:
			// B,G,R are all set to depth%256, alpha set to 1.
			for ( int i = 0; i < 3; ++i )
				*dest++ = (BYTE)depth % 256;
			*dest++ = 0xff;
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);

}

void KinectV1View::drawKinectData() {
	glBindTexture(GL_TEXTURE_2D, textureId);
	getKinectData(data);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);

	glColor3f(1.f, 1.f, 1.f);
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


