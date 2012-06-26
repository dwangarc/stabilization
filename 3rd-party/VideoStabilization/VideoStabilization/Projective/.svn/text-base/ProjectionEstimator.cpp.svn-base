#include "ProjectionEstimator.h"

#include <omp.h>
#include <math.h>
#include <Windows.h>

#include "ImageProc/inc/pictureConverter.h"
#include "Libs/abacus.h"
#include "ProjectiveConverter.h"
#include "Include/capture_log.h"
#include "Include/different_libs.h"

#include "ImageProc/inc/PicturePainter.h"

void toLog(Matrix const& mt)
{
	for (unsigned int i = 0; i < mt.get_height(); ++i)
	{
		for (unsigned int j = 0; j < mt.get_width(); ++j)
		{
			Capture_Log::getInstance()->log_write("%.3f ", mt.get_elem(i, j));
		}
		Capture_Log::getInstance()->log_write("\n");
	}
}




ProjectionEstimator::ProjectionEstimator(int pyrSize1, int iterNumber, double alp, int gaussDepth1):prevPic()
{
	medMatrix = Matrix::identity(3);
	prevMatr = Matrix::identity(3);
	safeShift = 10;
	shiftKoeff = 200;
	alpha = alp;
	alpha1 = alpha;
	initialized = false;
	if (pyrSize1 < 1) pyrSize1 = 1;
	pyrSize = std::min(pyrSize1, MAX_PYR_SIZE);
	gaussDepth = gaussDepth1;
	defaultIterNum = iterNumber;
}

sh_ptr_db ProjectionEstimator::simpleProjection() const
{
	double* projection = new double[9];
	Projection::initProj(projection, true);
	return sh_ptr_db(projection);
}


sh_ptr_db ProjectionEstimator::estimateProj(Picture pic1, Picture pic2, sh_ptr_int pyrIter)
{
	if (pic1.getHeight() != pic2.getHeight() || pic1.getWidth() != pic2.getWidth())
	{
		return simpleProjection();
	}

	createPyr(pic1, pic2);
	if (!initialized || proj_engine[0].getWidth() != im2[0].getWidth() ||
		proj_engine[0].getHeight() != im2[0].getHeight())
	{
		initialized = true;
		for (int i = 0; i < pyrSize; ++i)
		{
			proj_engine[i].initNonPic(im2[i].getWidth(), im2[i].getHeight());
		}
	}
	//	shiftIm();

	double proj[9];
	ProjectiveConverter projConv;
	Projection::initProj(proj, true);
	for (int i = pyrSize - 1; i >= 0; --i)
	{
		int iterNum = defaultIterNum;
		if (pyrIter.get())
		{
			iterNum = pyrIter.get()[i];
		}
		proj_engine[i].estimateProj(im1[i].getPicture().get(), im2[i].getPicture().get(), iterNum, proj);
		memcpy(proj, proj_engine[i].proj, 8 * sizeof(double));
		proj[8] = 1;
		mat3x3_invert(proj, proj);
		if (i > 0)
		{
			projConv.resizeProj(proj, 2);
			mat3x3_invert(proj, proj);		
		}
	}
	double* projection = new double[9];
	for (int i = 0; i < 9; ++i)
	{
		projection[i] = proj[i];
	}
	return sh_ptr_db(projection);
}

void ProjectionEstimator::stabilizeStable(Picture pic, sh_ptr_int pyrIter)
{
	if (pic.getWidth() == 0) return;
	if (pic.getWidth() != stabilized.getWidth())
	{
		initialized = false;
		stabilized = pic.clone();
		stabUnion = pic.clone();
		prevPic = pic.clone();
	}

	sh_ptr_db projection = estimateProj(prevPic, pic, pyrIter);
	stabilizeStableLight(pic, projection);

}

void ProjectionEstimator::stabilizeStableLight(Picture pic, sh_ptr_db projection)
{
	if (pic.getWidth() == 0) return;
	if (pic.getWidth() != stabilized.getWidth())
	{
		stabilized = pic.clone();
		stabUnion = pic.clone();
	}

	Matrix toPrev(3, 3, projection);
	Matrix toStabCheck = prevMatr.mult(toPrev);

	Matrix toStabRes = toStabCheck;

	sh_ptr_db proj = sh_ptr_db(new double[9]);
	memcpy(proj.get(), toStabRes.get_const_mem(), 9 * sizeof(double));
	createStabPicExpModify(pic, proj);
}



void ProjectionEstimator::stabilize(Picture pic, sh_ptr_int pyrIter, int downscale)
{
	if (pic.getWidth() == 0) return;
	Picture bigPic = pic;
	PictureConverter pc;
	if (downscale > 1)
	{
		pic = pc.downscale(pic, downscale);
	}
	if (bigPic.getWidth() != stabilized.getWidth())
	{
		initialized = false;
		stabilized = bigPic.clone();
		stabUnion = bigPic.clone();
	}
		
	stabilizedSmall = downscale > 1 ? pc.downscale(stabilized, downscale) : stabilized;
	
	sh_ptr_db projection = estimateProj(stabilizedSmall, pic, pyrIter);
	
	ProjectiveConverter projConv;
	projConv.resizeProj(projection.get(), downscale);
	createStabPicExpModify(bigPic, projection);
//	createStabPicExpMult(bigPic, projection);
}

Picture ProjectionEstimator::getStabilized() const
{
	return stabilized;
}

Picture ProjectionEstimator::getStabilizedUnion() const
{
	return stabUnion;
}


void ProjectionEstimator::setMaxCornerDifference(int val)
{
	maxCornerDifference = val;
}

void ProjectionEstimator::setShiftData(int safeShift, int shiftKoeff)
{
	this->safeShift = safeShift;
	this->shiftKoeff = shiftKoeff;
}




void ProjectionEstimator::shiftIm()
{
	for (int i = 0; i < pyrSize; ++i)
	{
		im1[i] = im2[i];
	}
}

void ProjectionEstimator::createPyr(Picture pic, Picture* im, int pyramidSize)
{
	im[0] = converter.convertColor(pic);
	for (int i1 = 1; i1 < pyramidSize; ++i1)
	{
		im[i1] = converter.downscale(im[i1 - 1], 2);
	}

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pyramidSize; ++i)
		{
			for (int k = 0; k < gaussDepth; ++k)
			{
				proj_engine[i].gauss_filter(im[i].getPicture().get(), im[i].getWidth(), im[i].getHeight());
			}
		}
	}

}


void ProjectionEstimator::createPyr(Picture pic1, Picture pic2)
{
	PictureConverter converter1, converter2;

	int i1, j1;

#pragma omp parallel num_threads(2)
	{
		if (omp_get_thread_num()) 
		{
			im2[0] = converter1.convertColor(pic2);
			for (i1 = 1; i1 < pyrSize; ++i1)
			{
				im2[i1] = converter1.downscale(im2[i1 - 1], 2);
			}

		}
		else
		{
			im1[0] = converter2.convertColor(pic1);
			for (j1 = 1; j1 < pyrSize; ++j1)
			{
				im1[j1] = converter2.downscale(im1[j1 - 1], 2);
			}
		}
	}

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pyrSize; ++i)
		{
			for (int k = 0; k < gaussDepth; ++k)
			{
				proj_engine[i].gauss_filter(im2[i].getPicture().get(), im2[i].getWidth(), im2[i].getHeight());
			}
		}
	}
}

double ProjectionEstimator::estimateCornerDist(double const* proec, int width, int height) const
{
	double dist = 0;
	ProjectiveConverter projConv;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			Point2D p1 = projConv.applyProj(Point2D(width * i, height * j), proec, width, height);
			dist += sqr(p1.x - i * width);
			dist += sqr(p1.y - j * height);
		}
	}
	dist /= 8;
	dist = sqrt(dist);
	return dist;
}


bool ProjectionEstimator::checkSmall(double const* proec, int width, int height) const
{
	ProjectiveConverter projConv;
	Point2D p1 = projConv.applyProj(Point2D(0, 0), proec, width, height);
	if (abs(p1.x) > maxCornerDifference || abs(p1.y) > maxCornerDifference) return false;
	
	p1 = projConv.applyProj(Point2D(width, 0), proec, width, height);
	if (abs(p1.x - width) > maxCornerDifference || abs(p1.y) > maxCornerDifference) return false;

	p1 = projConv.applyProj(Point2D(width, height), proec, width, height);
	if (abs(p1.x - width) > maxCornerDifference || abs(p1.y - height) > maxCornerDifference) return false;

	p1 = projConv.applyProj(Point2D(0, height), proec, width, height);
	if (abs(p1.x) > maxCornerDifference || abs(p1.y - height) > maxCornerDifference) return false;

	return true;
}

bool ProjectionEstimator::createPowerMt(Matrix& powerMt, Matrix& dopolnMt, sh_ptr_db proec, double alpha2)
{
	Matrix matr(3, 3, proec);
#pragma omp parallel num_threads(2)
	{
		if (omp_get_thread_num()) 
		{
			powerMt = matr.power01(alpha2, 10);
		}
		else
		{
			dopolnMt = matr.power01(1.0 - alpha2, 10).obratn();//.mult(matr.obratn());
		}
	}
	if (abs(dopolnMt.at(2, 2)) < 1e-4 || abs(powerMt.at(2, 2)) < 1e-4)
	{
		return false;
	}
	powerMt.mult(1.0 / powerMt.at(2, 2));
	dopolnMt.mult(1.0 / dopolnMt.at(2, 2));
	return true;

}

void ProjectionEstimator::createSimpleStabPic(Picture pic)
{
	stabilized = pic.clone();
	stabUnion = pic.clone();
	prevMatr = Matrix::identity(3);
}


void ProjectionEstimator::createStabPicExpModify(Picture pic, sh_ptr_db proec)
{
	Matrix powerMt = Matrix::identity(3);//matr.power01(alpha1);
	Matrix dopolnMt = Matrix::identity(3);//matr.power01(1.0 - alpha1).obratn();//.mult(matr.obratn());

	alpha1 = alpha;
	if (!createPowerMt(powerMt, dopolnMt, proec, alpha1))
	{
		createSimpleStabPic(pic);
		return;
	}

	double dist = estimateCornerDist(powerMt.get_const_mem(), pic.getWidth(), pic.getHeight());
	if (dist < maxCornerDifference)
	{
		if (dist > safeShift)
		{
			dist -= safeShift;
			dist /= shiftKoeff;
			alpha1 = (1.0 - sqrt(dist)) * alpha1;
			if (!createPowerMt(powerMt, dopolnMt, proec, alpha1))
			{
				createSimpleStabPic(pic);
				return;
			}
		}
		else
		{
			alpha1 = alpha;
		}

		double dist = estimateCornerDist(powerMt.get_const_mem(), pic.getWidth(), pic.getHeight());
		if (dist > maxCornerDifference)
		{
			createSimpleStabPic(pic);
			return;
		}

		Picture curPic = converter.downscale(stabUnion, 2);
		Picture tmpPic(curPic.getWidth(), curPic.getHeight(), curPic.getColors());
		tmpPic.fill(0);

		//PictureConverter conv;
		//Picture curPic2 = conv.stretch(curPic, 2);
		//Picture pic2 = conv.stretch(pic, 2);
		//projConv.applyProj(curPic.getPictureChar(), curPic.getWidth(), curPic.getHeight(), curPic.getColors(),
		//	dopolnMt.get_const_mem(), stabUnion.getPicture().get(), curPic2.getPictureChar());
		//stabilized.fill(0);
		//projConv.applyProj(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(),
		//	powerMt.get_const_mem(), stabilized.getPicture().get(), pic2.getPictureChar());
		//stabUnion.overlay(stabilized);

		dopolnMt = dopolnMt.obratn();
		sh_ptr_db smallProj = sh_ptr_db(new double[9]);
		memcpy(smallProj.get(), dopolnMt.get_const_mem(), 9 * sizeof(double));
		projConv.resizeProj(smallProj.get(), 0.5);
		dopolnMt = Matrix(3, 3, smallProj);
		dopolnMt = dopolnMt.obratn();

		projConv.applyProj(curPic, dopolnMt.get_const_mem(), tmpPic);
		stabUnion.overlay(converter.stretchNearest2(tmpPic));
		stabilized.fill(0);
		projConv.applyProj(pic, powerMt.get_const_mem(), stabilized);
//		stabUnion = stabilized;
		stabUnion.overlay(stabilized);
		prevMatr = powerMt;
	}
	else
	{
		alpha1 = alpha;
		createSimpleStabPic(pic);
	}


}


void ProjectionEstimator::createStabPicExp(Picture pic, sh_ptr_db proec)
{
	ProjectiveConverter projConv;
	Matrix powerMt = Matrix::identity(3);
	Matrix dopolnMt = Matrix::identity(3);

	if (!createPowerMt(powerMt, dopolnMt, proec, alpha1))
	{
		createSimpleStabPic(pic);
		return;
	}
	
	double dist = estimateCornerDist(powerMt.get_const_mem(), pic.getWidth(), pic.getHeight());
	if (dist < maxCornerDifference)
	{
		if (dist > safeShift)
		{
			dist /= shiftKoeff;
			alpha1 = (1.0 - dist) * alpha;
		}
		else
		{
			alpha1 = alpha;
		}
		Picture curPic = stabUnion.clone();
		projConv.applyProj(curPic, dopolnMt.get_const_mem(), stabUnion);
		stabilized.fill(0);
		projConv.applyProj(pic, powerMt.get_const_mem(), stabilized);
		prevMatr = powerMt;
		stabUnion.overlay(stabilized);
	}
	else
	{
		alpha1 = alpha;
		createSimpleStabPic(pic);
	}
}


void ProjectionEstimator::createStabPicExpMult(Picture pic, sh_ptr_db projection)
{
	ProjectiveConverter projConv;
	double proec[9];
	Projection::initProj(proec, true);

	//LogInterface* log = Capture_Log::getInstance();
	//log->log_write("%.4f %.4f\n", proj[6], proj[7]);

/*	int checkKoeff = hugeProec(proj, width, height);
	double beta = checkKoeff ? alpha / 4 : alpha;
	if (checkKoeff == 2) beta = 0;*/
	double beta = alpha;

	for (int i = 0; i < 9; ++i)
	{
		proec[i] *= (1 - beta);
		proec[i] += projection.get()[i] * beta;
	}

	if (checkSmall(proec, pic.getWidth(), pic.getHeight()))
	{
		stabilized = projConv.applyProj(pic, proec/*,stabilized*/);
	}
	else
	{
		stabilized = pic.clone();
	}
	stabUnion = stabilized;
}

int ProjectionEstimator::hugeProec(double const* proec, int width, int height) const
{
	double dist = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			dist += abs(convertX(proec, i * width, j * height) - i * width);
			dist += abs(convertY(proec, i * width, j * height) - j * height);
		}
	}
	dist /= 8;
	if (dist > 70) return 2;
	if (dist > 25) return 1;
	return 0;	
}

double ProjectionEstimator::convertX(double const* proec, double x, double y) const
{
	double norm = (proec[6] * x + proec[7] * y + proec[8]);
	double x1 = (proec[0] * x + proec[1] * y + proec[2]) / norm;
	return x1;
}

double ProjectionEstimator::convertY(double const* proec, double x, double y) const
{
	double norm = (proec[6] * x + proec[7] * y + proec[8]);
	double y1 = (proec[3] * x + proec[4] * y + proec[5]) / norm;
	return y1;
}

Point2D ProjectionEstimator::convertXY(double const* proec, Point2D p1) const
{
	return Point2D(convertX(proec, p1.x, p1.y), convertY(proec, p1.x, p1.y));
}

/************************************************************************/
/* Does not works properly                                                                     */
/************************************************************************/
void ProjectionEstimator::createStabPicMedian(Picture pic, Matrix toStab, Matrix toPrev)
{
	double koeff = 25;
	medMatrix = medMatrix.mult((koeff - 1) / koeff);
	medMatrix += toPrev.mult(1.0 / koeff);
	Matrix stabProj = medMatrix.obratn().mult(toPrev);//.mult(medMatrix.obratn());
	ProjectiveConverter conv;
	stabilized.fill(0);
	conv.applyProj(pic, stabProj.get_const_mem(), stabilized);
}
