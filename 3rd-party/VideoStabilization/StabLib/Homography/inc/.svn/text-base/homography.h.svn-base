#pragma once

#include "Geom/inc/Point.h"
#include "Matrix/matrix.h"

class Homography
{
	static const unsigned int HomoDim = 6;
	sh_ptr_db homo;
public:
	Homography();
	Homography(Matrix const& matr);
	Homography(Homography const& homo);
	double dist() const;
	Homography clone() const;
	static unsigned int getHomoDim();

	Homography const& operator +=(Homography const& homoGraphy);
	Homography operator+(Homography const& homo) const;
	Homography operator*(double val) const;

	void applyDownscale(double downscale);

	Point2D applyHomo(Point2D const&) const;
	void printLog() const;

	sh_ptr_db getHomo() const;
	Matrix getHomoMatrix3x3() const;

};