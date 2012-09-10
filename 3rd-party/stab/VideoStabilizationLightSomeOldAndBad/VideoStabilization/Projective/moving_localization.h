#pragma once

#include "projection.h"

#include "KalmanProjective.h"

class MlTrackData
{
	int	width;
	int	height;

	Projection* proj_engine;
	double proj[8];

	KalmanProjective kalmProj;

public:
	MlTrackData(int width, int height, double dispAngle, double dispShift,double dispAngleMeas, double dispShiftMeas);
	~MlTrackData();
	int ml_track_init(unsigned char* im);
	int	ml_track_track(unsigned char* &im);
	double* getStabProj() const;
};

