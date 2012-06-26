#pragma once

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

#include "ImageProc/inc/Picture.h"

class OpenGlWrap
{
	static const int MINX = -5;
	static const int MINY = -5;
	static const int MAXX = 5;
	static const int MAXY = 5;
	int width, height;

	void shiftTo(int x, int y);
public:
	OpenGlWrap();
	~OpenGlWrap();
	void resize(int x, int y, int width, int height);
	void drawPic(Picture pic, int xTop, int yLeft, int xBottom, int yRight, bool BGR = false);
	void startPaint();
	void finishPaint();
};