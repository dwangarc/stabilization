#include "stdafx.h"
#include "../inc/Picture.h"

Picture::Picture(sh_ptr_uch pic, int width, int height, int colors)
{
	this->pic = pic;
	this->width = width;
	this->height = height;
	this->colors = colors;
}

Picture::Picture(unsigned char* pic, int width, int height, int colors)
{
	this->pic = sh_ptr_uch(pic);
	this->width = width;
	this->height = height;
	this->colors = colors;
}

int Picture::getColors() const
{
	return colors;
}

int Picture::getWidth() const
{
	return width;
}

int Picture::getHeight() const
{
	return height;
}

sh_ptr_uch Picture::getPicture()
{
	return pic;
}

unsigned char const* Picture::getPictureChar() const
{
	return pic.get();
}

Picture Picture::clone() const
{
	unsigned char* data = new unsigned char[width * height * colors];
	for (int i = 0; i < width * height * colors; ++i)
	{
		data[i] = pic.get()[i];
	}
	return Picture(data, width, height, colors);
}

unsigned char& Picture::at(int wd, int ht, int color)
{
	assert(color < colors);
	return pic.get()[ht * width * colors + wd * colors + color];
}

unsigned char Picture::get(int wd, int ht, int color) const
{
	assert(color < colors);
	return pic.get()[ht * width * colors + wd * colors + color];
}

unsigned char Picture::get(Point2D const& p1, int color) const
{
	return get((int)p1.x, (int)p1.y, color);
}

unsigned char& Picture::at(Point2D const& p1, int color)
{
	return at((int)p1.x, (int)p1.y, color);
}



Picture::Picture(int width, int height, int colors)
{
	this->width = width;
	this->height = height;
	this->colors = colors;
	pic = sh_ptr_uch(new unsigned char[width * height * colors]);
}


double Picture::getColor(double wd, double ht, int curColor) const
{
	assert(curColor < colors);
	int ix = (int)wd;
	int iy = (int)ht;
	double x = wd;
	double y = ht;
	double a3, a0, a1, a2;

	if (ix >= 0 && ix < width - 1 && iy >= 0 && iy < height - 1) 
	{
		x -= ix;
		y -= iy;
		a3 = x * y;
		a0 = 1.f - x - y + a3;//(1.f - x) * (1.f - y);
		a1 = x - a3;//x * (1.f - y);
		a2 = y - a3;//y * (1.f - x);
		return a0 * get(ix, iy, curColor) + a1 * get(ix + 1, iy, curColor) + a2 * get(ix, iy + 1, curColor) + a3 * get(ix + 1, iy + 1, curColor);
	}
	return 0;
}

void Picture::copyPic(unsigned char const* pic1)
{
	for (int i = 0; i < width * height * colors; ++i)
	{
		pic.get()[i] = pic1[i];
	}
}

Picture Picture::getRect(int x1, int y1, int x2, int y2) const
{
	x1 = max(0, x1);
	y1 = max(0, y1);
	x2 = min(width, x2);
	y2 = min(height, y2);
	x1 = x1 > x2 ? x2 : x1;
	y1 = y1 > y2 ? y2 : y1;
	Picture res(x2 - x1, y2 - y1, colors);
	for (int i = x1; i < x2; ++i)
	{
		for (int j = y1; j < y2; ++j)
		{
			for (int k = 0; k < colors; ++k)
			{
				res.at(i - x1, j - y1, k) = get(i, j, k);
			}
		}
	}
	return res;
}

Picture Picture::getRectMiddle(int band) const
{
	return getRect(band, band, width - band, height - band);
}

Picture Picture::operator +(Picture const& pic) const
{
	if (pic.getColors() != this->getColors() || pic.getHeight() != this->getHeight() || pic.getWidth() != this->getWidth()) return Picture(0, 0);
	Picture res = pic.clone();
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				int sum = pic.get(i, j, c);
				sum += get(i, j, c);
				res.at(i, j, c) = (unsigned char) (sum % 256);
			}
		}
	}
	return res;
}

Picture Picture::operator -(Picture const& pic) const
{
	if (pic.getColors() != getColors() || pic.getHeight() != getHeight() || pic.getWidth() != getWidth()) return Picture(0, 0);
	Picture res = pic.clone();
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				int sum = pic.get(i, j, c);
				sum -= get(i, j, c);
				sum += 256;
				res.at(i, j, c) = (unsigned char) (sum % 256);
			}
		}
	}
	return res;
}
