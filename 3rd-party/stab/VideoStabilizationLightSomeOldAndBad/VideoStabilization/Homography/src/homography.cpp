#include "stdafx.h"
#include "../inc/homography.h"
#include <math.h>
#include "Include/capture_log.h"
#include "Include/different_libs.h"

Homography::Homography():homo(new double[HomoDim])
{
	for (int i = 0; i < HomoDim; ++i)
	{
		homo.get()[i] = 0;
	}
	homo.get()[0] = 1;
	homo.get()[4] = 1;
}

Homography::Homography(Homography const& homogr)
{
	homo = homogr.homo;
}


Homography::Homography(Matrix const& matr):homo(new double[HomoDim])
{
	if (matr.get_width() >= 3 && matr.get_height() >= 2)
	{
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				homo.get()[3 * i + j] = matr.get_elem(i, j);
			}
		}
	}
	else
	{
		if (matr.get_height() >= 6)
		{
			for (int i = 0; i < 6; ++i)
			{
				homo.get()[i] = matr.get_elem(i, 0);
			}
		}
	}

}


double Homography::dist() const
{
	double size = 0;
	for (int i = 0; i < HomoDim; ++i)
	{
		size += sqr(homo.get()[i]) * (i % 3 == 2 ? 1 : 10);
	}
	size = sqrt(size);
	return size;
}

Homography Homography::clone() const
{
	Homography cl;
	for (int i = 0; i < HomoDim; ++i)
	{
		cl.homo.get()[i] = homo.get()[i];
	}
	return cl;
}

Homography const& Homography::operator +=(Homography const& homoGraphy)
{
	for (int i = 0; i < HomoDim; ++i)
	{
		homo.get()[i] += homoGraphy.homo.get()[i];
	}
	return *this;
}

Point2D Homography::applyHomo(Point2D const& xy) const
{
	Point2D x1y1;
	x1y1.x = xy.x * homo.get()[0] + xy.y * homo.get()[1] + homo.get()[2];
	x1y1.y = xy.x * homo.get()[3] + xy.y * homo.get()[4] + homo.get()[5];
	return x1y1;
}

void Homography::applyDownscale(double downscale)
{
	homo.get()[2] /= downscale;
	homo.get()[5] /= downscale;
}


void Homography::printLog() const
{
	Capture_Log::getInstance()->log_write("Homography is:\n");
	for (int j = 0; j < 2; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			Capture_Log::getInstance()->log_write("%.6f ", homo.get()[j * 3 + i]);
		}
		Capture_Log::getInstance()->log_write("\n");
	}
	Capture_Log::getInstance()->log_write("\n");
}

Homography Homography::operator+(Homography const& homo) const
{
	Homography res = clone();
	res += homo;
	return res;
}

Homography Homography::operator*(double val) const
{
	Homography res = clone();
	for (int i = 0; i < HomoDim; ++i)
	{
		res.homo.get()[i] *= val;
	}
	return res;
}

unsigned int Homography::getHomoDim()
{
	return HomoDim;
}

sh_ptr_db Homography::getHomo() const
{
	sh_ptr_db data(new double[HomoDim]);
	for (unsigned int i = 0; i < HomoDim; ++i)
	{
		data.get()[i] = homo.get()[i];
	}
	return data;
}

Matrix Homography::getHomoMatrix3x3() const
{
	Matrix res(3, 3);
	for (unsigned int i = 0; i < 2; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			res.at(i, j) = homo.get()[i * 3 + j];
		}
	}
	res.at(2, 2) = 1;
	return res;
}
