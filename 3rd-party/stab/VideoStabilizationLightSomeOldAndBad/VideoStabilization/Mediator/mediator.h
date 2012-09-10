#pragma once
#include <string>

#include "Homography/inc/homoEstimator.h"
#include "Geom/inc/gausian.h"
#include "ImageProc/inc/pictureConverter.h"
#include "Include/frames_reader.h"
#include "Test/tester.h"
#include "Homography/inc/KalmanHomography.h"
#include "Projective/moving_localization.h"

struct MediatorParams
{
	int width, height;
	int downscale;
	bool debug;
	int startPic;
	double alpha, beta, sigma;
	double dispAngle, dispShift, koeffStart;
	double dispAngleMeasure, dispShiftMeasure;
	double f1;
	int bandWidth;
	int stepSize;
};


class Mediator
{
	Picture prevImage, curImage, convImage, prevPic;
	Picture laplas;
	int width, height, downscale;
	HomoEstimator homoEstimator;
	FramesReader frameReader;
	PictureConverter picConverter;
	Gaussian gauss;
	Homography curHomo;
	double alpha1, beta1;
	StereoConverter converter;
	int bandWidth;
	int stepSize;

	MediatorParams params;
	Tester tester;

	KalmanHomography kalmanHomo;


	MlTrackData track_data;

public:
	Mediator(std::string filePath, MediatorParams const& params);
	Picture getFrame() const;
	Homography getHomo() const;
	void nextStep();
	void nextProjStep();
	int getWidth() const;
	int getHeight() const;

	static MediatorParams getParams(std::string const& fileName);

};