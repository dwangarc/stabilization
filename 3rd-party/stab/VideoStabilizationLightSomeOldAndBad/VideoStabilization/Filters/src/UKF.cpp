#include "stdafx.h"
#include "../inc/UKF.h"
#include "Include/different_libs.h"

SystemState UKF::f(SystemState x, SystemError er) const
{
	double timeStep = 0.3;
	SystemState res = x;
	Point3D speed(x.speed, 0, 0);
	speed = x.pos.rot.rotate(speed);
	speed *= timeStep;
	res.pos.pos += speed;
	Point3D accel(er.accel, 0, 0);
	accel = x.pos.rot.rotate(accel);
	accel *= sqr(timeStep) / 2;
	res.pos.pos += accel;
	
	res.speed += er.accel * timeStep;

	res.pos.rot += getRotationError(er.rot);

	return res;
}

Rotation UKF::getRotationError(Point3D const& rot) const
{
	Rotation erRot(Point3D(1), rot.x);
	erRot += Rotation(Point3D(0, 1), rot.y);
	erRot += Rotation(Point3D(0, 0, 1), rot.z);
	return erRot;
}


SystemObservation UKF::h(SystemState x, SystemObsError er) const
{
	SystemObservation res;
	res.pos = x.pos;
	res.pos.pos += er.pos;
	res.pos.rot += getRotationError(er.rot);
	return res;
}

sh_ptr_SP UKF::generateSigmaPoints(SystemState const& x, Matrix const& correlation) const
{
	double k = 0; //2We usually set k to 0 for state-estimation and to 3-	L for parameter estimation
	double alpha = 1e-3; //from 1e-4 to 1
	double beta = 2; // is used to incorporate prior knowledge of the distribution of x 
	// (for Gaussian distributions, beta = 2 is optimal)
	size_t dim = SystemState::dim;
	double lambda = sqr(alpha) * (dim + k) - dim;

	SigmaPoint* points = new SigmaPoint[2 * dim + 1];
	Matrix corL(correlation);
	corL = corL.mult(dim + lambda).square_root();
	Matrix corLMin(corL);
	corLMin.mult(-1);

	points[0].x = x;
	points[0].weightM = lambda / (dim + lambda);	
	points[0].weightC = points[0].weightM + (1.0 - sqr(alpha) + beta);

	for (size_t i = 0; i < dim; ++i)
	{
		points[1 + i].x = x + corL[i];
		points[1 + dim + i].x = x + corLMin[i];
		points[1 + i].weightC = points[1 + i].weightM = 0.5 / (dim + lambda);
		points[1 + dim + i].weightC = points[1 + dim + i].weightM = points[1 + i].weightC;
	}
	return sh_ptr_SP(points);
}

sh_ptr_SP UKF::samplesStep(sh_ptr_SP samples) const
{
	size_t dim = SystemState::dim;
	SigmaPoint* points = new SigmaPoint[2 * dim + 1];
	for (size_t i = 0; i < 2 * dim + 1; ++i)
	{
		points[i].x = f(samples.get()[i].x, SystemError());
		points[i].weightC = samples.get()[i].weightC;
		points[i].weightM = samples.get()[i].weightM;
	}
	return sh_ptr_SP(points);
}

SystemState UKF::computeAverage(sh_ptr_SP samples) const
{
	size_t dim = SystemState::dim;
	SystemState avrg = SystemState();
	for (size_t i = 0; i < 2 * dim + 1; ++i)
	{
		SigmaPoint* point = samples.get() + i;
		avrg.simpleAdd(point->x * point->weightM);
	}
	return avrg;
}



SystemState UKF::observe(SystemObservation obs)
{
	sh_ptr_SP pointsK1K1 = generateSigmaPoints(x, PXX);
	sh_ptr_SP pointsKK1 = samplesStep(pointsK1K1);
	SystemState xAv = computeAverage(pointsKK1);

	return x;
}

