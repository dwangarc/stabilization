/*
#include <stabilization/actions/Analyzer.h>

Analyzer::Analyzer(MediatorParams const& params) : prevPic(), projectiveAnalyzer()
{
	stabilizator = sh_ptr_PrEst(new ProjectionEstimator(params.pyrSize, params.defaultIterNum, params.alpha));
	stabilizator->setMaxCornerDifference(params.maxCornerDistance / params.downscale);
	pyrIter = params.pyrIter;
}

ProjectiveAnalyzer& Analyzer::getProjectiveAnalyzer()
{
	return projectiveAnalyzer;
}

Picture Analyzer::nextStep(Picture basePic)
{
	if (basePic.getWidth() == 0) return Picture();

	if (prevPic.getWidth() == 0)
	{
		prevPic = basePic.clone();
	}
	sh_ptr_db proj = stabilizator->estimateProj(prevPic, basePic, pyrIter);
	projectiveAnalyzer.nextStep(proj.get());
	prevPic = basePic;

	return basePic;
}
*/
