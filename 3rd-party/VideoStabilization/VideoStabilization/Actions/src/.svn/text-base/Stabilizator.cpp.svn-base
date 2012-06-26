#include "stdafx.h"
#include "../inc/Stabilizator.h"
#include "ImageProc/inc/gausian.h"
#include "ImageProc/inc/pictureConverter.h"

#include "Projective/ProjectiveConverter.h"
#include "ImageProc/inc/PicDerivative.h"

#include "Include/capture_log.h"

#include <omp.h>

Stabilizator::Stabilizator(MediatorParams const&params):prevPic()
{
	stabilizator = sh_ptr_PrEst(new ProjectionEstimator(params.pyrSize, params.defaultIterNum, params.alpha));
	stabilizator->setMaxCornerDifference(params.maxCornerDistance / params.downscale);
	stabilizator->setShiftData(params.safeShift / params.downscale, params.shiftParam / params.downscale);
	pyrIter = params.pyrIter;
	downscale = params.downscale;
}

Picture Stabilizator::nextStep(Picture basePic)
{
	if (basePic.getWidth() == 0) return Picture();
	Picture baseSmall;
	if (downscale > 1)
	{
		baseSmall = picCv.downscale(basePic, downscale);
	}
	else
	{
		baseSmall = basePic;
	}

	if (prevPic.getWidth() != baseSmall.getWidth())
	{
		prevPic = baseSmall.clone();
	}
	static int step = 0;

	sh_ptr_db projection = stabilizator->estimateProj(baseSmall, prevPic, pyrIter);

	if (downscale > 1)
	{
		ProjectiveConverter converter;
		converter.resizeProj(projection.get(), downscale);
	}

	Matrix mt(3, 3, projection);
	mt = mt.obratn();
	memcpy(projection.get(), mt.get_const_mem(), 9 * sizeof(double));

	stabilizator->stabilizeStableLight(basePic, projection);
	prevPic = baseSmall;
	return stabilizator->getStabilizedUnion();
}

