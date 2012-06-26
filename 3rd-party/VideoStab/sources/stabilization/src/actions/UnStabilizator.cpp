/*
#include "stdafx.h"
#include "../inc/UnStabilizator.h"
#include "StabLib/Include/RegistryParamsSaver.h"
#include "StabLib/Include/different_libs.h"

#define _USE_MATH_DEFINES
#include <math.h>

void UnStabilizator::saveParams() const
{
	RegistryParamsSaver sf;
	sf.saveDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_X"), shiftX);
	sf.saveDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_Y"), shiftY);
	sf.saveDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_ANGLE"), shiftAngle);
}

void UnStabilizator::loadParams()
{
	RegistryParamsSaver sf;
	shiftX = sf.loadDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_X"), 10);
	shiftY = sf.loadDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_Y"), 10);
	shiftAngle = sf.loadDouble(_T("UNSTABLE_VIDEO"), _T("SHIFT_ANGLE"), 0.25);
}


UnStabilizator::UnStabilizator() : picConverter()
{
	loadParams();
}

UnStabilizator::~UnStabilizator()
{
	saveParams();
}

Picture UnStabilizator::nextStep(Picture basePic)
{
	if (basePic.getWidth() == 0) return Picture();

	double angle = dif_lib::randomDoubleSigned(shiftAngle * M_PI / 180.0);
	double* homoData = new double[6];
	homoData[2] = dif_lib::randomDoubleSigned(shiftX);
	homoData[5] = dif_lib::randomDoubleSigned(shiftY);
	homoData[0] = cos(angle);
	homoData[4] = cos(angle);
	homoData[1] = sin(angle);
	homoData[3] = -sin(angle);
	Homography homo = Homography(Matrix(2,3,sh_ptr_db(homoData)));
	basePic = picConverter.applyHomo(basePic, homo);
	return basePic;
}

double UnStabilizator::getShiftX() const
{
	return shiftX;
}

double UnStabilizator::getShiftY() const
{
	return shiftY;
}

double UnStabilizator::getShiftAngle() const
{
	return shiftAngle;
}

void UnStabilizator::setShiftXYAngle(double shX, double shY, double shAngle)
{
	shiftX = shX;
	shiftY = shY;
	shiftAngle = shAngle;
	saveParams();
}
*/
