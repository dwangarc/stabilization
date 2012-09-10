#pragma once

#include <boost/shared_ptr.hpp>
#include "Geom/inc/Point.h"
#include "Geom/inc/rotation.h"
#include "Include/capture_log.h"
#include "Geom/inc/position.h"

typedef boost::shared_ptr<unsigned char> sh_prt_uch;

class StereoConverter
{

	static const int param_num = 8;
	double ic[param_num];
	bool init;
	int frame_width, frame_height;
	Position camShift;


public:
	StereoConverter();

	void setParams( const double* ic_, bool downscale, int width, int height);

	// Base in meters.
	void setBase(double bs);

/*	sh_prt_uch convert(sh_prt_uch image, int width, int height) const;
	sh_prt_uch convert(const unsigned char* image, int width, int height) const;*/
	Point3D convert(Point3D const& point);

	Point3D get3DPointRect(Point2D p1, Point2D p2) const;
	Point3D get3DPoint(Point2D p1, Point2D p2) const;

private:
	double fx, fy, cx, cy;
	double r2, r4, a1, a2, a3, cdist;
	double base;

	void normalize(Point2D& point) const;

};