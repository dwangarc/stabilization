#pragma once

#include <boost/shared_ptr.hpp>
#include "Geom/inc/Point.h"
#include "Geom/inc/rotation.h"
//#include "Include/capture_log.h"
#include "Geom/inc/position.h"
#include <WTypes.h>

typedef boost::shared_ptr<unsigned char> sh_prt_uch;
typedef boost::shared_ptr<Point3D> sh_ptr_pt3D;

class StereoConverter
{
	double distDownscale;
	Point3D* mapData;
	Point2Di* mapDataInt;
//	sh_ptr_pt3D mapData;

	static const int param_num = 8;
	double ic[param_num];
	bool init;
	bool goodMapData;
	int frame_width, frame_height;
	Position camShift;
	double exactness;

	void initMapData();

public:
	StereoConverter(double distDownscale1);
	~StereoConverter();

	void setParams( const double* ic_, bool downscale);
	void setWidthHeight(int width, int height);

	void setDistortionParams(const double* ic);
	sh_ptr_db getDistortionParams() const;

	// Base in meters.
	void setBase(double bs);

/*	sh_prt_uch convert(sh_prt_uch image, int width, int height) const;
	sh_prt_uch convert(const unsigned char* image, int width, int height) const;*/
	Point3D convert(Point3D const& point, double downscale = 1.0) const;
	Point3D const& convertFast(int place) const;
	Point2Di const& convertFastInt(int place) const;

	Point3D get3DPointRect(Point2D p1, Point2D p2) const;
	Point3D get3DPoint(Point2D p1, Point2D p2) const;

private:
	double base;

	void normalize(Point2D& point) const;

};