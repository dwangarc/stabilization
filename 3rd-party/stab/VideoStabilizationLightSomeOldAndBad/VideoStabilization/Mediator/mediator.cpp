#include "stdafx.h"
#include "mediator.h"
#include "Include/capture_log.h"
#include "Include/different_libs.h"

#include <fstream>
#include <Windows.h>

Mediator::Mediator(std::string filePath, MediatorParams const& params): downscale(params.downscale), homoEstimator(), 
frameReader(filePath + "/image", "png", 5, 0, 1000, params.startPic), gauss(params.width / downscale, params.height / downscale, params.sigma), picConverter(), 
prevPic(0, 0), prevImage(0, 0), curImage(0, 0), convImage(0, 0), laplas(0, 0),
kalmanHomo(params.dispAngle, params.dispShift, params.dispAngleMeasure, params.dispShiftMeasure, params.koeffStart, params.f1),
track_data(params.width / 2, params.height / 2, params.dispAngle, params.dispShift, params.dispAngleMeasure, params.dispShiftMeasure)
{
	stepSize = params.stepSize;
	bandWidth = params.bandWidth;
	width = params.width;
	height = params.height;
	alpha1 = params.alpha;
	beta1 = params.beta;
	this->params = params;
	tester.setWH(width, height);
	double ic[8];
	ic[0] = 804.9; //fx
	ic[1] = 804.46; // fy
	ic[2] = 346.08; //cx
	ic[3] = 229.2;  // cy
	ic[4] = -0.504; // k1
	ic[5] = 0.252; // k2
	ic[6] = ic[7] = 0;
	converter.setParams(ic, false, width, height);


}

Picture Mediator::getFrame() const
{
	return curImage;
//	return convImage.getRectMiddle(32);
//	return convImage;
}

Homography Mediator::getHomo() const
{
	return curHomo;
}




void Mediator::nextStep()
{
	DWORD tm = GetTickCount();
	prevImage = curImage;
	DWORD tm1 = GetTickCount();

	bitmap *bitMap = frameReader.nextFrame();
	Picture basePic(width, height, 3);
	basePic.copyPic(bitMap->data);

	basePic = picConverter.fixDistortions(converter, basePic);
	Picture smallGreyPic = picConverter.convertColor(basePic);
	if (downscale > 1.05)
	{
		smallGreyPic = picConverter.downscale(smallGreyPic, downscale);
	}
	Picture curImage1 = smallGreyPic;//.getRectMiddle(0);// - gauss.apply(smallGreyPic);/*gauss.apply(smallGreyPic);*/
	Capture_Log::getInstance()->log_write("Preprocess time is %d\n", GetTickCount() - tm1);

	Homography homo = homoEstimator.getHomography(prevImage.getWidth() != 0 ? prevImage : curImage1, curImage1, stepSize);
	homo.applyDownscale(1.0 / (double)downscale);

	Homography clearHomo = kalmanHomo.observe(homo);
	
	
	Matrix clearHomoMatrix = clearHomo.getHomoMatrix3x3();
	Matrix clHomoObr = clearHomoMatrix.obratn();
	Matrix homoMatr = homo.getHomoMatrix3x3();
	Matrix HnoiseMatr = clHomoObr * homoMatr;

	Capture_Log::getInstance()->log_write("\n\n\n");
	Capture_Log::getInstance()->log_write("Estimated homo is\n");
	homo.printLog();

	Capture_Log::getInstance()->log_write("Kalman homo is\n");
	clearHomo.printLog();

	Capture_Log::getInstance()->log_write("Kalman obratn homo is\n");
	dif_lib::print(clHomoObr);

	Capture_Log::getInstance()->log_write("Noise homo is\n");
	dif_lib::print(HnoiseMatr);

//	curHomo = Homography(HnoiseMatr);
	Capture_Log::getInstance()->log_write("Check base homo is\n");
	dif_lib::print(clearHomoMatrix * HnoiseMatr);
	curHomo.printLog();
	Capture_Log::getInstance()->log_write("\n\n\n");
	
	

/*	Homography homoE = Homography();
	curHomo = homoE * beta1 + homo * alpha1 + curHomo * (1 - alpha1 - beta1);*/

//	curImage = curImage1;
	curImage = picConverter.applyHomo(curImage1, curHomo);

	if (prevPic.getWidth() == 0)
	{
		prevPic = basePic.clone();
	}
	Gaussian gs(width, height, 2.0);
	if (0 && params.debug && bitMap->data && prevPic.getWidth() > 0)
	{
//		Capture_Log::getInstance()->log_write("Gaussian full time is %d\n", GetTickCount() - tm);
		static int frameId = 0;
		char name1[11] = "filea.jpeg";
		char name2[12] = "fileaa.jpeg";
		name1[4] += frameId;
		name2[4] += frameId;
		frameId++;
/*		Picture pic0 = gs.apply(prevPic);
		Picture pic1 = picConverter.applyHomo(basePic, homo);
		Picture pic = gs.apply(pic1);
		double dist = tester.imgDistace(pic0.getPictureChar(), pic.getPictureChar(), 3);*/
		frameReader.saveFrame(curImage1.getPictureChar(), smallGreyPic.getWidth(), smallGreyPic.getHeight(), smallGreyPic.getColors(), name1);
/*		frameReader.saveFrame(pic.getPictureChar(), width, height, 3, name2);
		Capture_Log::getInstance()->log_write("Pictures distance is %.2f\n", dist);*/
	}

//	Picture img = gs.apply(basePic);
	tm1 = GetTickCount();
	convImage = picConverter.applyHomo(basePic, curHomo);
	Capture_Log::getInstance()->log_write("Homo time is %d\n", GetTickCount() - tm1);
	tm1 = GetTickCount();
//	convImage = gauss.apply(convImage);
	Capture_Log::getInstance()->log_write("Gauss time is %d\n", GetTickCount() - tm1);

	if (params.debug)
	{
		laplas = basePic.clone();//picConverter.convertGrey(curImage1/* - gs.apply(curImage1)*/, 3);//convImage;// - gs.apply(convImage);
	}
	else
	{
		laplas = convImage;
	}


	tm1 = GetTickCount();
	Picture rect = convImage.getRectMiddle(bandWidth);
	frameReader.saveFrame(rect.getPictureChar(), rect.getWidth(), rect.getHeight(), 3);
	
/*	rect = basePic.getRectMiddle(bandWidth);
	frameReader.saveFrame(rect.getPictureChar(), rect.getWidth(), rect.getHeight(), 3);*/

	Capture_Log::getInstance()->log_write("Save time is %d\n", GetTickCount() - tm1);

//	prevPic = basePic.clone();
	bitmap_release(bitMap);
	Capture_Log::getInstance()->log_write("Step time is %d\n\n\n", GetTickCount() - tm);
}

void Mediator::nextProjStep()
{
	bitmap *bitMap = frameReader.nextFrame();
	Picture basePic(width, height, 3);
	basePic.copyPic(bitMap->data);
	PictureConverter picConv;
	Picture smallPic = picConv.downscale(basePic, 2);
	if (curImage.getWidth() == 0)
	{
		track_data.ml_track_init(smallPic.getPicture().get());
	}
	else
	{
		unsigned char* data = smallPic.getPicture().get();
		track_data.ml_track_track(data);
	}
	curImage = picConv.stretch(smallPic, 2);//basePic.clone();
	Picture rect = curImage.getRectMiddle(bandWidth);
	frameReader.saveFrame(rect.getPictureChar(), rect.getWidth(), rect.getHeight(), 3);
}


int Mediator::getWidth() const
{
	return width;
}

int Mediator::getHeight() const
{
	return height;
}

MediatorParams Mediator::getParams(std::string const& fileName)
{
	MediatorParams params;

	std::ifstream inData;
	inData.open(fileName.c_str());
	if (!inData.is_open())
	{
		params.downscale = 1;
		params.debug = false;
		params.startPic = 1;
	}
	else
	{
		std::string capture;
		inData >> capture;
		inData >> params.width;
		inData >> capture;
		inData >> params.height;
		inData >> capture;
		inData >> params.downscale;
		inData >> capture;
		int debug1;
		inData >> debug1;
		params.debug = debug1 != 0 ? true : false;
		inData >> capture;
		inData >> params.startPic;
		inData >> capture;
		inData >> params.alpha;
		inData >> capture;
		inData >> params.beta;
		inData >> capture;
		inData >> params.sigma;
		inData >> capture;
		inData >> params.dispAngle;
		inData >> capture;
		inData >> params.dispShift;
		inData >> capture;
		inData >> params.dispAngleMeasure;
		inData >> capture;
		inData >> params.dispShiftMeasure;
		inData >> capture;
		inData >> params.koeffStart;
		inData >> capture;
		inData >> params.f1;
		inData >> capture;
		inData >> params.bandWidth;
		inData >> capture;
		inData >> params.stepSize;
		
	}



	return params;
}
