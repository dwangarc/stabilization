#include "stdafx.h"
#include "../inc/OpenGlWrap.h"
#include "ImageProc/inc/PictureConverter.h"

OpenGlWrap::OpenGlWrap()
{
}

OpenGlWrap::~OpenGlWrap()
{

}

void OpenGlWrap::resize(int x, int y, int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(x, y, width,height);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(MINX, MAXX, MINY, MAXY, 2,12);   
	gluLookAt( 0,0,5, 0,0,0, 0,1,0 );
	glMatrixMode( GL_MODELVIEW );
}

void OpenGlWrap::shiftTo(int x, int y)
{
	double xKoeff = MAXX - MINX;
	double yKoeff = MAXY - MINY;
	xKoeff /= width;
	yKoeff /= height;
	xKoeff *= x;
	yKoeff *= y;
	glRasterPos2d(MINX + xKoeff, MINY + yKoeff);
}


void OpenGlWrap::drawPic(Picture pic, int xTop, int yLeft, int xBottom, int yRight, bool BGR)
{
//	resize(x, y, width, height);

	int wd = xBottom - xTop;
	int ht = yRight - yLeft;
	shiftTo(xTop, yLeft);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	double zoomX = wd;
	double zoomY = ht;
	zoomX /= pic.getWidth();
	zoomY /= pic.getHeight();

//	PictureConverter conv;
//	Picture pic1 = pic;//conv.toBGR(pic);
//	glPixelZoom(1.0, 1.0);
	glPixelZoom((float)zoomX, (float)zoomY);
	if (BGR)
	{
		glDrawPixels(pic.getWidth(), pic.getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, pic.getPictureChar());
	}
	else
	{
		glDrawPixels(pic.getWidth(), pic.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, pic.getPictureChar());
	}

//	auxSwapBuffers();
}

void OpenGlWrap::startPaint()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void OpenGlWrap::finishPaint()
{
	glFinish();  
	SwapBuffers(wglGetCurrentDC());
}
