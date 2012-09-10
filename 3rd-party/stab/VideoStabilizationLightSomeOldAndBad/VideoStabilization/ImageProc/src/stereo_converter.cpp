#include "stdafx.h"
#include <math.h>
#include "Libs/abacus.h"
#include "./../inc/stereo_converter.h"
#include "Geom/inc/vector3D.h"
#include "Include/capture_log.h"

StereoConverter::StereoConverter()
{
	init = false;
}


void StereoConverter::setParams(const double* ic_, bool downscale, int width, int height)
{
	frame_height = height;
	frame_width = width;
	for (int i = 0; i < param_num; ++i)
	{
		ic[i] = ic_[i];
	}

	// for downscaled images :
	if (downscale)
	{
		ic[0]	*= 0.5;
		ic[1]	*= 0.5;
		ic[2]	*= 0.5;
		ic[3]	*= 0.5;
	}
	fx	= ic[0]; // new / 2
	fy	= ic[1]; // New / 2
	cx	= ic[2];
	cy	= ic[3];

	init = true;
}
/*
sh_prt_uch StereoConverter::convert(sh_prt_uch image, int width, int height) const
{
	return convert(image.get(), width, height);
}

sh_prt_uch StereoConverter::convert(const unsigned char* image, int width, int height) const
{

}
*/
Point3D StereoConverter::convert(Point3D const& point) 
{
	assert(init);
	Point3D res(point);

	res.z = res.z ? 1./res.z : 1;
	res.x *= res.z;
	res.y *= res.z;

	// compensate distortions
	if (ic) 
	{
		res.x = (res.x - cx) / fx;
		res.y = (res.y - cy) / fy;

		r2 = res.x * res.x + res.y * res.y;
		r4 = r2 * r2;
		a1 = 2 * res.x * res.y;
		a2 = r2 + 2 * res.x * res.x;
		a3 = r2 + 2 * res.y * res.y;
		cdist = 1 + ic[4] * r2 + ic[5] * r4;
		res.x = res.x * cdist + ic[6] * a1 + ic[7] * a2;
		res.y = res.y * cdist + ic[6] * a3 + ic[7] * a1;

		res.x = res.x * fx + cx;
		res.y = res.y * fy + cy;
	}
	return res;

}

void StereoConverter::normalize(Point2D& point) const
{
	point.x -= fx / 2;
	point.y -= fy / 2;
	point.y *= -1;
	// y to the top
	// x to the right
	// center is at the image center.
}

/*
y - forward
x - left
z - top
*/

Point3D StereoConverter::get3DPointRect(Point2D p_l, Point2D p_r) const
{
	static double focus = 1066;
	normalize(p_l);
	normalize(p_r);
	if (p_l.x - p_r.x < 10)
	{
		p_l.y += 0.1;
	}
	Point3D p(0);
	double d = p_l.x - p_r.x;
	p.y = focus * base / d;     // y is distance
	p.x = p.y * p_l.x / focus;  // x is to the right
	p.z = p.y * p_l.y / focus;  // z is to the top
	return p;
}

Point3D StereoConverter::get3DPoint(Point2D p_l, Point2D p_r) const
{
//	static double focus = 1066;
	normalize(p_l);
	normalize(p_r);
	Vector3D vl(Point3D(), Point3D(p_l.x, fx, p_l.y).normalize(0.1));
	Vector3D vr(Point3D(), Point3D(p_r.x, fx, p_r.y).normalize(0.1));
	vr = camShift.apply(vr);
	Point3D inter = vl.intersectM(vr);
/*	Point3D control = vl.intersectIterative(vr);
	double dist = control.dist(inter);*/
	return inter;
}


void StereoConverter::setBase(double bs)
{
	base = bs;
}




