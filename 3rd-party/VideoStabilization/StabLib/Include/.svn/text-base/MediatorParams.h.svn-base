#pragma once

#include <string>
#include "Include/define.h"

struct MediatorParams
{
	int downscale;
	double alpha;
	int bandWidth;
	int pyrSize;
	int defaultIterNum;
	int maxCornerDistance;

	int safeShift;
	int shiftParam;

	sh_ptr_int pyrIter;

	bool setData(double alpha, int pyrSize, sh_ptr_int pyrIter, int downscale, int cornerDistance, int shiftParam, int safeShift);
	void saveParams();

	static MediatorParams getParams();
};
