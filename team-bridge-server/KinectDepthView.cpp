/*#include "KinectDepthView.h"


GLuint KinectDepthView::textureId;
GLubyte KinectDepthView::data[width*height * 4];

// Kinect variables
HANDLE KinectDepthView::depthStream;
INuiSensor * KinectDepthView::sensor;

// Body tracking variables
Vector4 KinectDepthView::skeletonPosition[NUI_SKELETON_POSITION_COUNT];



bool KinectDepthView::configure() {
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
	return sensor;
}


void KinectDepthView::getSkeletalData() {
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	if ( sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame) >= 0 ) {
		sensor->NuiTransformSmooth(&skeletonFrame, NULL);
		// Loop over all sensed skeletons
		for ( int z = 0; z < NUI_SKELETON_COUNT; ++z ) {
			const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[z];
			// Check the state of the skeleton
			if ( skeleton.eTrackingState == NUI_SKELETON_TRACKED ) {
				// Copy the joint positions into our array
				for ( int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i ) {
					skeletonPosition[i] = skeleton.SkeletonPositions[i];
					if ( skeleton.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_NOT_TRACKED ) {
						skeletonPosition[i].w = 0;
					}
				}
				return; // Only take the data for one skeleton
			}
		}
	}
}

void KinectDepthView::getKinectData(GLubyte* dest) {
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

	getSkeletalData();
}

void KinectDepthView::drawKinectData() {
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


void draw() {
	KinectDepthView::drawKinectData();
	glutSwapBuffers();
}

bool KinectDepthView::start() {

	if ( !configure() ) {
		return false;
	}

	//instance = this;

	int argc = 0;
	char *argv[] = { "team-bridge-server.exe", NULL };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Kinect View");
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	// Initialize textures
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// OpenGL setup
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glEnable(GL_TEXTURE_2D);

	// Camera setup
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Main loop
	glutMainLoop();
	return true;
}


/*#include "MainView.h"

// OpenGL Variables
GLuint textureId;
GLubyte data[width*height * 4];

// Kinect variables
HANDLE depthStream;
INuiSensor* sensor;

// Body tracking variables
Vector4 skeletonPosition[NUI_SKELETON_POSITION_COUNT];


bool configure() {
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
	return sensor;
}


void getSkeletalData() {
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	if ( sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame) >= 0 ) {
		sensor->NuiTransformSmooth(&skeletonFrame, NULL);
		// Loop over all sensed skeletons
		for ( int z = 0; z < NUI_SKELETON_COUNT; ++z ) {
			const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[z];
			// Check the state of the skeleton
			if ( skeleton.eTrackingState == NUI_SKELETON_TRACKED ) {
				// Copy the joint positions into our array
				for ( int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i ) {
					skeletonPosition[i] = skeleton.SkeletonPositions[i];
					if ( skeleton.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_NOT_TRACKED ) {
						skeletonPosition[i].w = 0;
					}
				}
				return; // Only take the data for one skeleton
			}
		}
	}
}

void getKinectData(GLubyte* dest) {
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

	getSkeletalData();
}

void drawKinectData() {
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


void draw() {
	drawKinectData();
	glutSwapBuffers();
	MainView::writeln("Draw.");
}

bool start() {

	if ( !configure() ) {
		return false;
	}

	int argc = 1;
	char *argv[] = { "team-bridge-server.exe", NULL };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Kinect View");
	glutDisplayFunc(draw);
	glutIdleFunc(draw);

	// Initialize textures
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// OpenGL setup
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glEnable(GL_TEXTURE_2D);

	// Camera setup
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Main loop
	glutMainLoop();
	return true;
}*/