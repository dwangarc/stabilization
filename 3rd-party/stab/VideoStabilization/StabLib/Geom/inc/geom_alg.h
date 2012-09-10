#pragma once
#include "position.h"

class GeomAlg
{
public:
	Point3D getGlobal(Point3D const& posit, Rotation, Point3D const& local) const;
	double shiftDistance(Point3D const** p1, Point3D const** p2, int size, Position const& shift, bool* use = 0) const;

	Position getShift( Point3D const** p1, Point3D const** p2, int size, bool* use = 0, int useCount = 0) const;
	Position getShiftHull( Point3D const** p1, Point3D const** p2, int size, bool* use = 0, int useCount = 0) const;
	
	// covariance is used.
	Position getShift6D( Point3D const** p1, Point3D const** p2, int size, bool* use = 0, int useCount = 0) const;

	Position massCenterRotation(Point3D const** p1, Point3D const** p2, int size, bool* use, Position cur) const;
	Point3D massCenterEstimate(Point3D const** p1, size_t size, bool* use = 0) const;
	Position RANSAC_shift(Point3D const** p1, Point3D const** p2, int size, bool* bst) const;
	Position exclude_shift(Point3D const** p1, Point3D const** p2, int size, ParamGetter const* getter) const;
	int exclude_new(Point3D** p1, Point3D** p2, int size, ParamGetter const* getter) const;
	int exclude_by_time(Point3D** p1, Point3D** p2, int size, ParamGetter const* getter, int& tm) const;

	Point3D vectorMult(Point3D const& p1, Point3D const& p2) const;

private:
	int similarity_3d_estimate(double *H, const double *m0, const double *m1, int count, bool *mask) const;

	// covariance is used.
	int similarity_6d_estimate(double *H, const double *m0, const double *m1, int count, double &scale, bool *mask) const;

	void hull(Point3D** p1, Point3D** p2, size_t sz, size_t maxSize) const;
};