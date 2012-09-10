#pragma once

#include "Geom/inc/position.h"
#include "boost/shared_ptr.hpp"
#include "SystemState.h"


struct SystemObservation
{
	Position pos;
};

struct SystemObsError
{
	Point3D rot;
	Point3D pos;
};

struct SystemError
{
	Point3D rot;
	double accel;
	SystemError():rot(), accel(0)
	{

	}
};

struct SigmaPoint
{
	SystemState x;
	double weightM, weightC;
};

typedef boost::shared_ptr<SigmaPoint> sh_ptr_SP;

class UKF
{
	Matrix PXX, PXY, PYY;

	SystemState x;

	SystemState f(SystemState x, SystemError er) const;
	SystemObservation h(SystemState x, SystemObsError er) const;

	Rotation getRotationError(Point3D const& rot) const;
	sh_ptr_SP generateSigmaPoints(SystemState const& x, Matrix const& correlation) const;
	sh_ptr_SP samplesStep(sh_ptr_SP samples) const;
	SystemState computeAverage(sh_ptr_SP samples) const;
public:
	UKF();
	SystemState observe(SystemObservation obs);

};