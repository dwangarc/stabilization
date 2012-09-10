#include "stdafx.h"
#include <stdio.h>
#include "libjpeg\jpeglib.h"
#include "libjpeg\jpegutils.h"
//#include "MainFrm.h"
//#include "Tracking 2.1.h"
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <atlstr.h>

#include "moving_localization.h"

#include "Include/frames_reader.h"
#include "ImageProc/inc/pictureConverter.h"
#include "ProjectiveConverter.h"

static FramesReader framesSaver("proj_stab", ".jpeg", 5);


int mat3x3_invert(double *m, double *m_inv)
{
	double res[9];
	double d = m[0]*(m[4]*m[8] - m[5]*m[7]) - m[1]*(m[3]*m[8] - m[5]*m[6]) + m[2]*(m[3]*m[7] - m[4]*m[6]);
	
	if (d == 0) return 0;
	
	d = 1 / d;

	res[0] = (m[4] * m[8] - m[5] * m[7]) * d;
	res[1] = (m[2] * m[7] - m[1] * m[8]) * d;
	res[2] = (m[1] * m[5] - m[2] * m[4]) * d;
                                  
	res[3] = (m[5] * m[6] - m[3] * m[8]) * d;
	res[4] = (m[0] * m[8] - m[2] * m[6]) * d;
	res[5] = (m[2] * m[3] - m[0] * m[5]) * d;
                                  
	res[6] = (m[3] * m[7] - m[4] * m[6]) * d;
	res[7] = (m[1] * m[6] - m[0] * m[7]) * d;
	res[8] = (m[0] * m[4] - m[1] * m[3]) * d;

	memcpy(m_inv, res, 9 * sizeof(double));

	return 1;
}

unsigned char*	ml_convert_to_grayscale(unsigned char* im, int width, int height);
int ml_save_jpeg_result(unsigned char* im, int width, int height, const char* name);
int ml_save_color_jpeg_result(unsigned char* im, int width, int height, const char* name);
int StabRGB(unsigned char* inputRGB, unsigned char* outputRGB, int width, 
			int height, double* st_pr, double im_inc, double frame_inc);
int split_two_RGB(unsigned char* split_left, unsigned char* split_right, 
				  unsigned char* split_result, int width, int height);
int convert_RGB_BGR(unsigned char* input, unsigned char* output, 
					int width, int height, double im_inc, double frame_inc);
int split_two_RGB_four(unsigned char* im_big, unsigned char* stab_big,
						unsigned char* im_same, unsigned char* stab_same, 
						unsigned char* im_cut, unsigned char* stab_cut, 
						int width, int height, unsigned char* split_image);


MlTrackData::MlTrackData(int width, int height, double dispAngle, double dispShift,double dispAngleMeas, double dispShiftMeas) :
kalmProj(dispAngle, dispShift, dispAngleMeas, dispShiftMeas)
{
	memset(proj, 0, 8 * sizeof(double));
	proj_engine = 0;
	this->width	 = width;
	this->height = height;
}

int	MlTrackData::ml_track_init(unsigned char *im)
{
	unsigned char* ImGray = ml_convert_to_grayscale(im, width, height);

	proj_engine = new Projection;
	proj_engine->load_im(ImGray, width, height);
	
	proj_engine->change_iterNum(30);//5, 10
	
	memset(proj, 0, 8 * sizeof(double));
	proj[0] = 1; 
	proj[4] = 1;
	
	
	delete[] ImGray;
	return 1;
}
int	MlTrackData::ml_track_track(unsigned char* &im)
{
	
	unsigned char* ImGray = ml_convert_to_grayscale(im, width, height);
	proj_engine->load_im(ImGray, width, height);
	proj_engine->multiply_projection(proj_engine->proj, proj, proj);
		
	double proec[9];
	memcpy(proec, proj, 8 * sizeof(double));
	proec[8] = 1.0;
	mat3x3_invert(proec, proec);

	double* newProec = kalmProj.observe(proec);

	Picture pic(width, height, 3);
	PictureConverter converter;
	ProjectiveConverter projConv;

	pic.copyPic(im);
	pic = converter.downscale(pic, 2);
	double* pr1 = projConv.resizeProj(proec, 0.5);
	double* pr2 = projConv.resizeProj(newProec, 0.5);

	unsigned char* small_res_image = projConv.applyProj2(pic.getPicture().get(), width / 2, height / 2, 3, pr1, pr2);
	Picture picRes(small_res_image, width / 2, height / 2, 3);
	picRes = converter.stretch(picRes, 2);


	delete[] pr1;
	delete[] pr2;

	unsigned char* new_result_image = projConv.applyProj2(im, width, height, 3, proec, newProec);
	memcpy(im, new_result_image, 3 * width * height * sizeof(unsigned char));
//	memcpy(im, picRes.getPicture().get(), 3 * width * height * sizeof(unsigned char));

	delete[] new_result_image;
	delete[] newProec;
	delete[] ImGray;
	
	return 1;
}
MlTrackData::~MlTrackData()
{
	delete proj_engine;
}
unsigned char* ml_convert_to_grayscale(unsigned char* im, int width, int height)
{
	unsigned char* ImGray = new unsigned char[width*height];
	memset(ImGray, 0, width*height*sizeof(unsigned char));
	unsigned char* src = im;
	unsigned char* dst = ImGray;
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++){
			dst[0] = (src[0] + src[1] + src[2]) / 3;
			src += 3;
			dst++;
		}
	return ImGray;
}

int StabRGB(unsigned char* inputRGB, unsigned char* outputRGB, int width, 
			int height, double* st_pr, double im_inc, double frame_inc)
{
	if(!inputRGB || !outputRGB || !width || !height) return 0;

	if(frame_inc < im_inc) return 0;
	
	memset(outputRGB, 0, 3 * int((1.0 + 2.0 * frame_inc) * width) * int((1.0 + 2.0 * frame_inc) * height) * sizeof(unsigned char));

	double pr[9];
	pr[8] = 1;
	memcpy(pr, st_pr, 8 * sizeof(double));
	mat3x3_invert(pr, pr);

	#pragma omp parallel
	{
		#pragma omp for
		for(int j = -int(im_inc * height); j < int((1.0 + im_inc) * height); j++)
			for(int i = -int(im_inc * width); i < int((1.0 + im_inc) * width); i++){
				double ip = i - width * 0.5;
				double jp = j - height * 0.5;
				double norm = 1 / (pr[6] * ip + pr[7] * jp + pr[8]);
				double x = (pr[0] * ip + pr[1] * jp + pr[2]) * norm;
				double y = (pr[3] * ip + pr[4] * jp + pr[5]) * norm;

				x += width  * 0.5;
				y += height * 0.5;

				//x += int(0.2 * width);
				//y += int(0.2 * height);
				if (x < 0 || x > width - 1 || y < 0 || y > height - 1) {
					outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 0] = 0;
					outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 1] = 0;
					outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 2] = 0;
					continue;
				}
				int ix = int(x);
				int iy = int(y);
				double dx = x - ix;
				double dy = y - iy;
				outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 2] = 
					unsigned char((1 - dx) * (1 - dy) * inputRGB[3 * (ix + iy * width) + 0] + 
					(1 - dx) * dy * inputRGB[3 * (ix + (iy + 1) * width) + 0] + 
					dx * (1 - dy) * inputRGB[3 * (ix + 1 + iy * width) + 0] + 
					dx * dy * inputRGB[3 * (ix + 1 + (iy + 1) * width) + 0] + 0.5);
				outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 1] = 
					unsigned char((1 - dx) * (1 - dy) * inputRGB[3 * (ix + iy * width) + 1] + 
					(1 - dx) * dy * inputRGB[3 * (ix + (iy + 1) * width) + 1] + 
					dx * (1 - dy) * inputRGB[3 * (ix + 1 + iy * width) + 1] + 
					dx * dy * inputRGB[3 * (ix + 1 + (iy + 1) * width) + 1] + 0.5);
				outputRGB[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 0] = 
					unsigned char((1 - dx) * (1 - dy) * inputRGB[3 * (ix + iy * width) + 2] + 
					(1 - dx) * dy * inputRGB[3 * (ix + (iy + 1) * width) + 2] + 
					dx * (1 - dy) * inputRGB[3 * (ix + 1 + iy * width) + 2] + 
					dx * dy * inputRGB[3 * (ix + 1 + (iy + 1) * width) + 2] + 0.5);
			}
	}
	return 1;
}
int split_two_RGB(unsigned char* split_left, unsigned char* split_right, 
				  unsigned char* split_result, int width, int height)
{
	if(!split_left || !split_right || !split_result || !width || !height)
		return 0;
	memset(split_result, 0, 3 * (2 * width) * height * sizeof(unsigned char));
	unsigned char* dst = split_result;
	unsigned char* s_left = split_left;
	unsigned char* s_right = split_right;
	for(int j = 0; j < height; j++){
		memcpy(dst, s_left, 3 * width * sizeof(unsigned char));
		dst += 3 * width;
		s_left += 3 * width;
		memcpy(dst, s_right, 3 * width * sizeof(unsigned char));
		s_right += 3 * width;
		dst+= 3 * width;
	}
	return 1;
}
int convert_RGB_BGR(unsigned char* input, unsigned char* output, 
					int width, int height, double im_inc, double frame_inc)
{
	if(!input || !output || !width || !height) return 0;

	if(frame_inc < im_inc) return 0;
	
	memset(output, 0, 3 * int((1.0 + 2.0 * frame_inc) * width) * int((1.0 + 2.0 * frame_inc) * height) * sizeof(unsigned char));

	#pragma omp parallel
	{
		#pragma omp for
		for(int j = -int(im_inc * height); j < int((1.0 + im_inc) * height); j++)
			for(int i = -int(im_inc * width); i < int((1.0 + im_inc) * width); i++){
				
				if (i < 0 || i > width - 1 || j < 0 || j > height - 1) {
					output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 0] = 0;
					output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 1] = 0;
					output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 2] = 0;
					continue;
				}
				output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 2] = 
					input[3 * (i + j * width) + 0];
				output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 1] = 
					input[3 * (i + j * width) + 1];
				output[3 * (i + int(frame_inc * width)+ (j + int(frame_inc * height)) * int((1.0 + 2.0 * frame_inc) * width)) + 0] = 
					input[3 * (i + j * width) + 2];
			}
	}
		return 1;
}
int split_two_RGB_four(unsigned char* im_big, unsigned char* stab_big,
						unsigned char* im_same, unsigned char* stab_same, 
						unsigned char* im_cut, unsigned char* stab_cut, 
						int width, int height, unsigned char* split_image)
{
	if(!im_big || !stab_big || !im_same || !stab_same || !im_cut || !stab_cut || 
		!width || !height || !split_image)
		return 0;
	memset(split_image, 0, 3 * (2 * width) * (3 * height) * sizeof(unsigned char));
	unsigned char* dst = split_image;
	unsigned char* s_left = im_big;
	unsigned char* s_right = stab_big;
	for(int j = 0; j < height; j++){
		memcpy(dst, s_left, 3 * width * sizeof(unsigned char));
		dst += 3 * width;
		s_left += 3 * width;
		memcpy(dst, s_right, 3 * width * sizeof(unsigned char));
		s_right += 3 * width;
		dst+= 3 * width;
	}
	s_left = im_same;
	s_right = stab_same;
	for(int j = 0; j < height; j++){
		memcpy(dst, s_left, 3 * width * sizeof(unsigned char));
		dst += 3 * width;
		s_left += 3 * width;
		memcpy(dst, s_right, 3 * width * sizeof(unsigned char));
		s_right += 3 * width;
		dst+= 3 * width;
	}

	s_left = im_cut;
	s_right = stab_cut;
	for(int j = 0; j < height; j++){
		memcpy(dst, s_left, 3 * width * sizeof(unsigned char));
		dst += 3 * width;
		s_left += 3 * width;
		memcpy(dst, s_right, 3 * width * sizeof(unsigned char));
		s_right += 3 * width;
		dst+= 3 * width;
	}
	return 1;
}

double* MlTrackData::getStabProj() const
{
	return 0;
}
