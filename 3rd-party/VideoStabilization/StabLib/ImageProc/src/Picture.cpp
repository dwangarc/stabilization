#include "stdafx.h"

#include "../inc/Picture.h"
#include <Windows.h>


Picture::Picture(sh_ptr_uch pic, int width, int height, int colors)
{
	this->pic = pic;
	simpleInit(width, height, colors);
}

void Picture::simpleInit(int width, int height, int colors)
{
	this->width = width;
	this->height = height;
	this->colors = colors;
}


Picture::Picture(unsigned char* pic, int width, int height, int colors)
{
	this->pic = sh_ptr_uch(pic);
	simpleInit(width, height, colors);
}

Picture Picture::createFromCxImage(CxImage* image)
{
	if (!image)
	{
		return Picture();
	}
	Picture picture(image->GetWidth(), image->GetHeight(), 3); //image->GetNumColors(); does not work.
	for (int i = 0; i < picture.height; ++i)
	{
		unsigned char* data = image->GetBits(i);
		memcpy(picture.pic.get() + i * picture.width * picture.colors, data, picture.width * picture.colors);
	}
	return picture;
}

Picture Picture::createFromIplImage(IplImage* image)
{
	if (!image) 
	{
		return Picture();
	}
	Picture picture(image->width, image->height, image->nChannels );
	unsigned char* data = (unsigned char*)image->imageData;
	for (int i = 0; i < picture.height; ++i)
	{
		memcpy(picture.pic.get() + (picture.height - i - 1) * picture.width * picture.colors, data, picture.width * picture.colors);
		data += image->widthStep;
	}
	return picture;
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

void Picture::fill(unsigned char colorVal)
{
	memset(pic.get(), colorVal, (size_t)(width * height * colors * sizeof(unsigned char)));
}

unsigned char* Picture::getRow(int row) const
{
	return pic.get() + width * colors * row;
}


void Picture::fill(unsigned char colorVal, int minW, int minH, int maxW, int maxH)
{
	if (maxW <= width && maxH <= height && minW >= 0 && minH >= 0)
	{
		for (int i = minW; i < maxW; ++i)
		{
			for (int j = minH; j < maxH; ++j)
			{
				for (int c = 0; c < colors; ++c)
				{
					at(i, j, c) = colorVal;
				}
			}
		}
	}
}



Picture Picture::clone() const
{
	unsigned char* data = new unsigned char[width * height * colors];
	memcpy(data, pic.get(), sizeof(unsigned char) * width * height * colors);
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
	if (wd < 0 || wd >= width || ht < 0 || ht >= height)
	{
		return 0;
	}
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

unsigned char& Picture::at(Point2Di const& p1, int color)
{
	return at(p1.x, p1.y, color);
}




Picture::Picture(int width, int height, int colors)
{
	this->width = width;
	this->height = height;
	this->colors = colors;
	pic = sh_ptr_uch(new unsigned char[width * height * colors]);
}

unsigned char Picture::getColorFast(double x, double y, int curColor) const
{
	assert(curColor < colors);
	int ix = (int)x;
	int iy = (int)y;

	double a3, a0, a1, a2;
	x -= ix;
	y -= iy;
	a3 = x * y;
	a0 = 1.f - x - y + a3;//(1.f - x) * (1.f - y);
	a1 = x - a3;//x * (1.f - y);
	a2 = y - a3;//y * (1.f - x);
	
	int wc = width * colors;
	int iywc = iy * wc;
	int ixcolors = ix * colors;
	int tmpRes = (int)(a0 * pic.get()[iywc + ixcolors + curColor]);
	tmpRes += (int)(a1 * pic.get()[iywc + ixcolors + colors + curColor]);
	tmpRes += (int)(a2 * pic.get()[iywc + wc + ixcolors + curColor]);
	tmpRes += (int)(a3 * pic.get()[iywc + wc + ixcolors + colors + curColor]);
	return (unsigned char)tmpRes;
}


double Picture::getColor(double x, double y, int curColor) const
{
	assert(curColor < colors);
	int ix = (int)x;
	int iy = (int)y;

	if (ix >= 0 && ix < width - 1 && iy >= 0 && iy < height - 1) 
	{
		double a3, a0, a1, a2;
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

void Picture::copyPic(Picture pic, int offX, int offY)
{
	for (int i = 0; i < pic.getWidth(); ++i)
	{
		for (int j = 0; j < pic.getHeight(); ++j)
		{
			for (int c = 0; c < pic.getColors(); ++c)
			{
				this->at(i + offX, j + offY, c) = pic.get(i, j, c);
			}
		}
	}
}


Picture Picture::getRect(int x1, int y1, int x2, int y2) const
{
	x1 = std::max(0, x1);
	y1 = std::max(0, y1);
	x2 = std::min(width, x2);
	y2 = std::min(height, y2);
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

void Picture::coverBlack(Picture pic, int sz)
{
	if (pic.getWidth() != width || pic.getHeight() != height || pic.getColors() != colors)
	{
		return;
	}
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int sum = 0;
			for (int c = 0; c < colors; ++c)
			{
				sum += get(i, j, c);
			}
			if (sum == 0)
			{
				for (int k = -sz; k < sz + 1; ++k)
				{
					if (k + i < 0 || k + i >= width) continue;
					for (int l = -sz; l < sz + 1; ++l)
					{
						if (l + j < 0 || l + j >= height) continue;
						for (int c = 0; c < colors; ++c)
						{
							at(i + k, j + l, c) = pic.at(i + k, j + l, c);
						}
					}
				}
			}
		}
	}
	//for (int i = 0; i < width * height * colors; ++i)
	//{
	//	if (this->pic.get()[i] == 0)
	//	{
	//		this->pic.get()[i] = pic.pic.get()[i];
	//	}
	//}
}


void Picture::overlay(Picture pic)
{
	if (pic.getWidth() != width || pic.getHeight() != height || pic.getColors() != colors)
	{
		return;
	}
	unsigned char* thisPic = this->pic.get();
	unsigned char const* incomePic = pic.pic.get();
#pragma omp parallel for schedule(static)
	for (int i = 0; i < width * height * colors; ++i)
	{
		if (incomePic[i] != 0)
		{
			thisPic[i] = incomePic[i];
		}
	}
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
				sum = abs(sum);
//				sum += 256;
				res.at(i, j, c) = (unsigned char) (sum % 256);
			}
		}
	}
	return res;
}

Picture Picture::operator &(Picture const& pic) const
{
	if (pic.getColors() != getColors() || pic.getHeight() != getHeight() || pic.getWidth() != getWidth()) return Picture(0, 0);
	Picture res = pic.clone();
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				res.at(i, j, c) = std::min(pic.get(i, j, c), get(i, j, c));
			}
		}
	}
	return res;
}

Picture Picture::operator |(Picture const& pic) const
{
	if (pic.getColors() != getColors() || pic.getHeight() != getHeight() || pic.getWidth() != getWidth()) return Picture(0, 0);
	Picture res = pic.clone();
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				res.at(i, j, c) = std::max(pic.get(i, j, c), get(i, j, c));
			}
		}
	}
	return res;
}



Picture::Picture()
{
	width = height = colors = 0;
}

Picture Picture::deinterlace() const
{
	Picture res(width, height, colors);
	for (int i = 0; i < height / 2; ++i)
	{
		memcpy(res.getRow(i * 2 + 1), getRow(i), width * colors * sizeof(unsigned char));
	}
	for (int i = height / 2; i < height; ++i)
	{
		memcpy(res.getRow((i - height / 2) * 2), getRow(i), width * colors * sizeof(unsigned char));
	}
	return res;
}
