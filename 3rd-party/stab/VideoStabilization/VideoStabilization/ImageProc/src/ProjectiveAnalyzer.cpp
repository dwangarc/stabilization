#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../inc/ProjectiveAnalyzer.h"
#include "Include/capture_log.h"
#include "Include/different_libs.h"

ProjectiveAnalyzer::ProjectiveAnalyzer()
{
	reset();
}


void ProjectiveAnalyzer::reset()
{
	analyzeIter = 1;
	medX = 0;
	medY = 0;
	medAngle = 0;
}

void ProjectiveAnalyzer::nextStep(double const* proj)
{

	LogInterface* log = Capture_Log::getInstance();
	log->log_write("Projection number %d is: \n", analyzeIter);
	for (int i = 0; i < 9; ++i)
	{
		log->log_write("%6.3f ", proj[i]);
		if (i % 3 == 2)
		{
			log->log_write("\n");
		}
	}

	medX += sqr(proj[2]);
	medY += sqr(proj[5]);
	double sinA = 0.25 * (sqr(proj[0] - 1.0) + sqr(proj[4] - 1.0) + sqr(proj[1]) + sqr(proj[3]));
	sinA = sqrt(sinA);
	medAngle += sqr(sinA * 180.0 / M_PI);
	++analyzeIter;
}


double ProjectiveAnalyzer::getMedX() const
{
	return sqrt(medX / analyzeIter);
}

double ProjectiveAnalyzer::getMedY() const
{
	return sqrt(medY / analyzeIter);
}

double ProjectiveAnalyzer::getMedAngle() const
{
	return sqrt(medAngle / analyzeIter);
}
