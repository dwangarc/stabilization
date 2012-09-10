#pragma once

#include "Include/define.h"
#include "Geom/inc/Point.h"
#include "Libs/CxImage/ximage.h"
#include "opencv/highgui.h"

class Picture
{
	sh_ptr_uch pic;
	int colors;
	int width, height;

	void simpleInit(int width = 0, int height = 0, int colors = 1);

	unsigned char* getRow(int row) const;

public:
	Picture(sh_ptr_uch pic, int width, int height, int colors = 1);
	Picture(unsigned char* pic, int width, int height, int colors = 1);
	Picture(int width, int height, int colors = 1);
	static Picture createFromCxImage(CxImage* image);
	static Picture createFromIplImage(IplImage* image);
	Picture();
	void copyPic(unsigned char const* pic);
	void copyPic(Picture pic, int offX, int offY);
	
	int getColors() const;
	int getWidth() const;
	int getHeight() const;

	sh_ptr_uch getPicture();
	unsigned char const* getPictureChar() const;

	unsigned char get(int width, int height, int color = 0) const;
	unsigned char get(Point2D const& p1, int color = 0) const;

	unsigned char& at(int width, int height, int color = 0);
	unsigned char& at(Point2D const& p1, int color = 0);
	unsigned char& at(Point2Di const& p1, int color = 0);
	unsigned char getColorFast(double wd, double ht, int curColor = 0) const;
	double getColor(double wd, double ht, int curColor = 0) const;


	Picture getRect(int x1, int y1, int x2, int y2) const;
	Picture getRectMiddle(int band) const;
	Picture clone() const;

	void overlay(Picture pic);
	void coverBlack(Picture pic, int sz);

	void fill(unsigned char colorVal);
	void fill(unsigned char colorVal, int minW, int minH, int maxW, int maxH);

	Picture operator +(Picture const&) const;
	Picture operator -(Picture const&) const;
	Picture operator &(Picture const&) const;
	Picture operator |(Picture const&) const;

	Picture deinterlace() const;
};

typedef boost::shared_ptr<Picture> sh_ptr_pic;
