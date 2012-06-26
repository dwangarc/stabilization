#include "stdafx.h"
#include "../MediatorParams.h"
#include <fstream>
#include <math.h>
#include <afxwin.h>

MediatorParams MediatorParams::getParams()
{
	MediatorParams params;

	params.pyrSize = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("PyramidSize"),	3);
	params.pyrIter = sh_ptr_int(new int[max(params.pyrSize, 3)]);
	params.defaultIterNum = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber"), 5);
	params.pyrIter.get()[0] = params.defaultIterNum;
	params.pyrIter.get()[1] = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber1"), 10);
	params.pyrIter.get()[2] = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber2"), 10);
	params.bandWidth = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("BandWidth"), 0);
	params.alpha = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("Alpha"), 95);
	params.alpha *= 0.01;
	params.downscale = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("Downscale"), 1);
	params.maxCornerDistance = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("MaxCornerDistance"), 200);

	params.safeShift = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("SafeShift"), 20);
	params.shiftParam = AfxGetApp()->GetProfileInt(_T("StabilizaitonParams"),	_T("ShiftParam"), 200);

	return params;
}

bool MediatorParams::setData(double alp, int pyr, sh_ptr_int pyrIter, int downsc, int cornerDistance, int shiftParam, int safeShift)
{
	int iterN = pyrIter.get()[0];
	maxCornerDistance = cornerDistance;
	this->shiftParam = shiftParam;
	this->safeShift = safeShift;	
	if (!(abs(alp - alpha) > 1e-2 || pyr != pyrSize || iterN != defaultIterNum || downsc != downscale || maxCornerDistance != cornerDistance))
	{
		bool flag = false;
		for (int i = 0; i < max(pyrSize, 3); ++i)
		{
			if (pyrIter.get()[i] != this->pyrIter.get()[i])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			return false;
		}
	}
	if (alp >= 0 && alp < 0.999) alpha = alp;
	if (pyr > 0 && pyr < 6) pyrSize = pyr;
	if (iterN > 0 && iterN < 100) defaultIterNum = iterN;
	if (downsc == 1 || downsc == 2 || downsc == 4) downscale = downsc;
	this->pyrIter = sh_ptr_int(new int[max(pyrSize, 3)]);
	for (int i = 0; i < max(pyrSize, 3); ++i)
	{
		int iterNum = pyrIter.get()[i];
		if (iterNum > 0 && iterNum < 100)
		{
			this->pyrIter.get()[i] = pyrIter.get()[i];
		}
		else
		{
			this->pyrIter.get()[i] = defaultIterNum;
		}
	}
	return true;
}

void MediatorParams::saveParams()
{
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("PyramidSize"),	pyrSize);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber"), defaultIterNum);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber1"), pyrIter.get()[1]);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("IterationNumber2"), pyrIter.get()[2]);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("BandWidth"), bandWidth);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("Alpha"), (int) (alpha * 100.0));
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("Downscale"), downscale);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("MaxCornerDistance"), maxCornerDistance);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("SafeShift"), safeShift);
	AfxGetApp()->WriteProfileInt(_T("StabilizaitonParams"),	_T("ShiftParam"), shiftParam);
}
