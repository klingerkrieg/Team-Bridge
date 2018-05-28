#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

#include <Ole2.h>


const int width = 640;
const int height = 480;


class GLView {

	protected:

	// OpenGL Variables
	static GLuint textureId;
	static GLubyte data[width*height * 4];

	virtual bool configure() = 0;

	public:
	static void(*drawDataFunc)();
	bool start();

};


void draw();
