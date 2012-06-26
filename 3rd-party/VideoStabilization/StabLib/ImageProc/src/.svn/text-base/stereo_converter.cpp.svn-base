#include "stdafx.h"
#include <math.h>
#include <tchar.h>
//#include "Libs/abacus.h"
#include "./../inc/stereo_converter.h"
#include "Geom/inc/vector3D.h"
#include "Include/RegistryParamsSaver.h"
//#include "Include/capture_log.h"


StereoConverter::StereoConverter(double distDownscale1): distDownscale(distDownscale1)
{
	mapData = 0;
	mapDataInt = 0;
	goodMapData = false;
	exactness = 10000.0;
	RegistryParamsSaver sf;
	ic[0] = sf.loadDouble(_T("DistortionParams"), _T("Fx"), 804.9);
	ic[1] = sf.loadDouble(_T("DistortionParams"), _T("Fy"), 804.46);
	ic[2] = sf.loadDouble(_T("DistortionParams"), _T("Cx"), 346.08);
	ic[3] = sf.loadDouble(_T("DistortionParams"), _T("Cy"), 229.2);
	ic[4] = sf.loadDouble(_T("DistortionParams"), _T("K1"), -0.504);
	ic[5] = sf.loadDouble(_T("DistortionParams"), _T("K2"), 0.252);
	ic[6] = sf.loadDouble(_T("DistortionParams"), _T("P1"), -0.014);
	ic[7] = sf.loadDouble(_T("DistortionParams"), _T("P2"), -0.173);
	init = true;
	//ic[0] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("Fx"), (int)(exactness * 804.9)) / exactness;
	//ic[1] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("Fy"), (int)(exactness * 804.46)) / exactness;
	//ic[2] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("Cx"), (int)(exactness * 346.08)) / exactness;
	//ic[3] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("Cy"), (int)(exactness * 229.2)) / exactness;
	//ic[4] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("K1"), (int)(exactness * -0.504)) / exactness;
	//ic[5] = AfxGetApp()->GetProfileInt(_T("DistortionParams"),	_T("K2"), (int)(exactness * 0.252)) / exactness;
}

StereoConverter::~StereoConverter()
{
	delete[] mapData;
	delete[] mapDataInt;
	mapData = 0;
	mapDataInt = 0;
	RegistryParamsSaver sf;
	sf.saveDouble(_T("DistortionParams"), _T("Fx"), ic[0]);
	sf.saveDouble(_T("DistortionParams"), _T("Fy"), ic[1]);
	sf.saveDouble(_T("DistortionParams"), _T("Cx"), ic[2]);
	sf.saveDouble(_T("DistortionParams"), _T("Cy"), ic[3]);
	sf.saveDouble(_T("DistortionParams"), _T("K1"), ic[4]);
	sf.saveDouble(_T("DistortionParams"), _T("K2"), ic[5]);
	sf.saveDouble(_T("DistortionParams"), _T("P1"), ic[6]);
	sf.saveDouble(_T("DistortionParams"), _T("P2"), ic[7]);

	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("Fx"), (int)(exactness * ic[0]));
	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("Fy"), (int)(exactness * ic[1]));
	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("Cx"), (int)(exactness * ic[2]));
	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("Cy"), (int)(exactness * ic[3]));
	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("K1"), (int)(exactness * ic[4]));
	//AfxGetApp()->WriteProfileInt(_T("DistortionParams"),	_T("K2"), (int)(exactness * ic[5]));
}


void StereoConverter::setWidthHeight(int width, int height)
{
	if (width != frame_width || height != frame_height)
	{
		goodMapData = false;
	}
	frame_height = height;
	frame_width = width;
	if (!goodMapData)
	{
		initMapData();
	}
}

void StereoConverter::setDistortionParams(const double* ic_)
{
	for (int i = 0; i < param_num; ++i)
	{
		ic[i] = ic_[i];
	}
	init = true;
}

sh_ptr_db StereoConverter::getDistortionParams() const
{
	sh_ptr_db params = sh_ptr_db(new double[param_num]);
	for (int i = 0; i < param_num; ++i)
	{
		params.get()[i] = ic[i];
	}
	return params;
}

void StereoConverter::initMapData()
{
	if (mapData) delete[] mapData;
	if (mapDataInt)	delete[] mapDataInt;
	mapData = new Point3D[frame_height * frame_width];//sh_ptr_pt3D(new Point3D[frame_height * frame_width]);
	mapDataInt = new Point2Di[frame_height * frame_width];//sh_ptr_pt3D(new Point3D[frame_height * frame_width]);
	int place = 0;

	for (int i = 0; i < frame_width; ++i )
	{
		for (int j = 0; j < frame_height; ++j)
		{
			Point3D pt(i, j, 1);
			if (place < frame_height * frame_width) 
			{
				mapData[place] = convert(pt, distDownscale);
				mapDataInt[place] = Point2Di((int)mapData[place].x, (int)mapData[place].y);
				place++;				
			}
		}
	}
	goodMapData = true;
}



void StereoConverter::setParams(const double* ic_, bool downscale)
{
	setDistortionParams(ic_);
	// for downscaled images :
	if (downscale)
	{
		ic[0]	*= 0.5;
		ic[1]	*= 0.5;
		ic[2]	*= 0.5;
		ic[3]	*= 0.5;
	}
	initMapData();
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
Point3D const& StereoConverter::convertFast(int place) const
{
	return mapData[place];
}

Point2Di const& StereoConverter::convertFastInt(int place) const
{
	return mapDataInt[place];
}



Point3D StereoConverter::convert(Point3D const& point, double downscale) const
{
	assert(init);
	Point3D res(point);

	res.z = res.z ? 1./res.z : 1;
	res.x *= res.z;
	res.y *= res.z;

	// compensate distortions
	if (ic) 
	{
		double r2, r4, a1, a2, a3, cdist;
		res.x = (res.x - ic[2]) / ic[0];
		res.y = (res.y - ic[3]) / ic[1];

		res.x *= downscale;
		res.y *= downscale;

		r2 = res.x * res.x + res.y * res.y;
		r4 = r2 * r2;
		a1 = 2 * res.x * res.y;
		a2 = r2 + 2 * res.x * res.x;
		a3 = r2 + 2 * res.y * res.y;
		cdist = 1 + ic[4] * r2 + ic[5] * r4;
		res.x = res.x * cdist + ic[6] * a1 + ic[7] * a2;
		res.y = res.y * cdist + ic[6] * a3 + ic[7] * a1;

		res.x = res.x * ic[0] + ic[2];
		res.y = res.y * ic[1] + ic[3];
	}
	return res;

}

void StereoConverter::normalize(Point2D& point) const
{
	point.x -= ic[0] / 2;
	point.y -= ic[1] / 2;
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
	Vector3D vl(Point3D(), Point3D(p_l.x, ic[0], p_l.y).normalize(0.1));
	Vector3D vr(Point3D(), Point3D(p_r.x, ic[0], p_r.y).normalize(0.1));
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




