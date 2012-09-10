#pragma once

#include "Include/define.h"
#include "Geom/inc/Point.h"

class Picture
{
	sh_ptr_uch pic;
	int colors;
	int width, height;

public:
	Picture(sh_ptr_uch pic, int width, int height, int colors = 1);
	Picture(unsigned char* pic, int width, int height, int colors = 1);
	Picture(int width, int height, int colors = 1);
	void copyPic(unsigned char const* pic);
	
	int getColors() const;
	int getWidth() const;
	int getHeight() const;

	sh_ptr_uch getPicture();
	unsigned char const* getPictureChar() const;

	unsigned char get(int width, int height, int color = 0) const;
	unsigned char get(Point2D const& p1, int color = 0) const;

	unsigned char& at(int width, int height, int color = 0);
	unsigned char& at(Point2D const& p1, int color = 0);
	double getColor(double wd, double ht, int curColor = 0) const;


	Picture getRect(int x1, int y1, int x2, int y2) const;
	Picture getRectMiddle(int band) const;
	Picture clone() const;

	Picture operator +(Picture const&) const;
	Picture operator -(Picture const&) const;
};

typedef boost::shared_ptr<Picture> sh_ptr_pic;
