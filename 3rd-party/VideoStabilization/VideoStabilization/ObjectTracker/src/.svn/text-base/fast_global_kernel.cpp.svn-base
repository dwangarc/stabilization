#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../inc/fast_global_kernel.h"

#define BINS_NUM 16 // was 16
#define TOTAL_BINS (BINS_NUM * BINS_NUM * BINS_NUM)
#define CL_MS_STOP 1
#define ALPHA 1.25
#define EPSILON 0.001
#define SPATIAL_BREAK 1
#define NUM_IN_SET 3
#define SET_1 10
#define SET_2 15
#define SET_3 20
#define SET_4 25
#define FOR_SET_1 1
#define FOR_SET_2 0.75
#define FOR_SET_3 0.5
#define FOR_SET_4 0.25

#define N_SCALE 2
#define B_SCALE 1.1
#define PRIOR_SCALE 0.7
#define SCALE_MISTAKE 0.1
#define NUM_ITER_MAX 15
#define H_W_MIN 10
#define H_W_MAX 100
#define SCALE_TO_DO 10


typedef struct fgk_track_data {
	int width;
	int height;
	double double_t_width;
	double double_t_height;

	double sigma;
			
	double hist[TOTAL_BINS];
	track_target target;

} fgk_track_data;

int fgk_extract_hist(unsigned char const*im, int step, int width, int height, int x0, int y0, int hx, int hy, 
			double* hist);
int fgk_m_shift(unsigned char const*im, int step, int x0, int y0, int &x_result, int &y_result, int width, 
			int height, int hx, int hy, double* hist_obj);
int fgk_classic_m_shift(unsigned char const *im, int step, int x0, int y0, int &x_result, int &y_result, 
			int width, int height, int hx, int hy, double* hist_obj);
double fgk_fK(unsigned char const* im, int step, int x, int y, int width, int height, int hx, 
			int hy, double* hist_obj);
//int fgk_mean_shift_scale(fgk_track_data *data, unsigned char *im, int step);


void*	fgk_track_create(int width, int height)
{
	fgk_track_data* data = new fgk_track_data;
	memset(data, 0, sizeof(fgk_track_data));
	data->height = height;
	data->width = width;
	return data;
}
int fgk_track_init(fgk_track_data *data, unsigned char const*im, int step, track_target *target)
{
	data->target = *target;
	data->double_t_width = (double)data->target.w;
	data->double_t_height = (double)data->target.h;
	data->sigma = PRIOR_SCALE;
		
	fgk_extract_hist(im, step, data->width, data->height, target->x, target->y, target->w / 2, 
		target->h / 2, data->hist);
	return 1;
}

int fgk_track_track(fgk_track_data *data, unsigned char const *im, int step, track_target *target)
{
	int x_new, y_new;
	int x0 = data->target.x;
	int y0 = data->target.y;
	int hx = (int)(data->double_t_width / 2);
	int hy = (int)(data->double_t_height / 2);
	double set_num = 1;
	
	if(hx < SET_1 && hy < SET_1)
		set_num = FOR_SET_1;
	else if(hx < SET_2 && hy < SET_2)
		set_num = FOR_SET_2;
	else if(hx < SET_3 && hy < SET_3)
		set_num = FOR_SET_3;
	else if(hx < SET_4 && hy < SET_4)
		set_num = FOR_SET_4;
	else
		set_num = 0;
	for(int k = NUM_IN_SET; k >= 0; k--)
	{	
		int new_hx = (int)((1 + set_num * k) * hx);
		int new_hy = (int)((1 + set_num * k) * hy);
		if(x0 - new_hx <= 0 || x0 + new_hx >= data->width || 
			y0 - new_hy <= 0 || y0 + new_hy >= data->height)
			continue;
		fgk_m_shift(im, step, x0, y0, x_new, y_new, data->width, data->height, 
			new_hx, new_hy, data->hist);
		x0 = x_new;
		y0 = y_new;
	}

	//if(abs(data->target.x - x0) < SCALE_TO_DO && abs(data->target.y - y0) < SCALE_TO_DO)
	//	fgk_mean_shift_scale(data, im, step);
	
	data->target.x = x0;
	data->target.y = y0;
	data->target.w = (int)data->double_t_width;
	data->target.h = (int)data->double_t_height;
	
	target->x = data->target.x;
	target->y = data->target.y;
	target->w = data->target.w;
	target->h = data->target.h;

	return 1;
}
void fgk_track_release(fgk_track_data *data)
{
	if(!data) return;

	delete data;
}
int fgk_extract_hist(unsigned char const *im, int step, int width, int height, int x0, int y0, int hx, int hy, 
			 double* hist)
{
	if(x0 - hx <= 0 || x0 + hx >= width || y0 - hy <= 0 || y0 + hy >= height)
		return 0;
	double norm = 0;
	double norm_i = 1.0 / hx / hx;
	double norm_j = 1.0 / hy / hy;
		
	memset(hist, 0, TOTAL_BINS * sizeof(double));

	unsigned char const* row = im + (y0 - hy) * step + 3 * x0;
	for(int j = -hy; j <= hy; j++){
		for(int i = -hx; i <= hx; i++){
			int R = row[3 * i + 0] / (256 / BINS_NUM);
			int G = row[3 * i + 1] / (256 / BINS_NUM);
			int B = row[3 * i + 2] / (256 / BINS_NUM);
			double argument = i * i * norm_i + j * j * norm_j;
			hist[R * BINS_NUM * BINS_NUM + G * BINS_NUM + B] += exp(- argument / 2);
		}
		row += step;
	}
	for(int i = 0; i < TOTAL_BINS; i++)
		norm += hist[i];
	if(norm)
		norm = 1.0 / norm;
	for(int i = 0; i < TOTAL_BINS; i++)
		hist[i] *= norm;
	
	return 1;
}

int fgk_m_shift(unsigned char const *im, int step, int x0, int y0, int &x_result, int &y_result, int width, 
					int height, int hx, int hy, double* hist_obj)
{
	double beta = 1;
	int xk = x0;
	int yk = y0;
	int xk1, yk1, xk1t, yk1t;
	double fxk, fxk1;
	do{
		if(!fgk_classic_m_shift(im, step, xk, yk, xk1t, yk1t, width, height, hx, hy, hist_obj)){
			x_result = x0;
			y_result = y0;
			return 0;
		}
		xk1 = xk + (int)(beta * (xk1t - xk));
		yk1 = yk + (int)(beta * (yk1t - yk));
		fxk = fgk_fK(im, step, xk, yk, width, height, hx, hy, hist_obj);
		fxk1 = fgk_fK(im, step, xk1, yk1, width, height, hx, hy, hist_obj);
		if(fxk1 > fxk){
			xk = xk1;
			yk = yk1;
			beta = ALPHA * beta;
		}else{
			xk = xk1t;
			yk = yk1t;
			beta = 1;
		}
		if(fxk == 0)
			continue;
	}while(fxk1 / fxk - 1.0 > EPSILON);
	if(xk - hx <= 0 || xk + hx >= width || yk - hy <= 0 || yk + hy >= height){
		x_result = x0;
		y_result = y0;
		return 0;
	}
	x_result = xk;
	y_result = yk;
	return 1;
}
int fgk_classic_m_shift(unsigned char const *im, int step, int x0, int y0, int &x_result, int &y_result, 
					int width, int height, int hx, int hy, double* hist_obj)
{
	double cur_hist[TOTAL_BINS];
	double x_shift = 0;
	double y_shift = 0;
	double norm = 0;
	double norm_i = 1.0 / hx / hx;
	double norm_j = 1.0 / hy / hy;

	do{	
		x_shift = 0;
		y_shift = 0;
		norm = 0;
		if(!fgk_extract_hist(im, step, width, height, x0, y0, hx, hy, cur_hist))
			return 0;
		for(int i = 0; i < TOTAL_BINS; i++)
			if(cur_hist[i]) cur_hist[i] = sqrt(hist_obj[i] / cur_hist[i]);

		unsigned char const* row = im + (y0 - hy) * step + 3 * x0;

		for(int j = -hy; j <= hy; j++){
			for(int i = - hx; i <= hx; i++){
				int R = row[3 * i + 0] / (256 / BINS_NUM);
				int G = row[3 * i + 1] / (256 / BINS_NUM);
				int B = row[3 * i + 2] / (256 / BINS_NUM);
				double wgt = cur_hist[R * BINS_NUM * BINS_NUM + G * BINS_NUM + B];
				double argument = i * i * norm_i + j * j * norm_j;
				x_shift += wgt * i * exp(-argument / 2);
				y_shift += wgt * j * exp(-argument / 2);
				norm += wgt * exp(-argument / 2);
			}
			row += step;
		}

		if(norm)
			norm = 1.0 / norm;
		x_shift *= norm;
		y_shift *= norm;

		if(x0 - (int)x_shift <= 0 || x0 + (int)x_shift >= width)
			x_shift = 0;
		if(y0 - (int)y_shift <= 0 || y0 + (int)y_shift >= height)
			y_shift = 0;
		x0 += (int)x_shift;
		y0 += (int)y_shift;
	}while(x_shift > SPATIAL_BREAK || y_shift > SPATIAL_BREAK);
	x_result = x0;
	y_result = y0;

	return 1;
}
double fgk_fK(unsigned char const* im, int step, int x, int y, int width, int height, int hx, 
		  int hy, double* hist_obj)
{
	double cur_hist[TOTAL_BINS];
	double norm_i = 1.0 / hx / hx;
	double norm_j = 1.0 / hy / hy;
	double result = 0;
	
	if(!fgk_extract_hist(im, step, width, height, x, y, hx, hy, cur_hist))
		return 0;
	for(int i = 0; i < TOTAL_BINS; i++)
		if(cur_hist[i]) cur_hist[i] = sqrt(hist_obj[i] / cur_hist[i]);

	unsigned char const* row = im + (y - hy) * step + 3 * x;

	for(int j = -hy; j <= hy; j++){
		for(int i = - hx; i <= hx; i++){
			int R = row[3 * i + 0] / (256 / BINS_NUM);
			int G = row[3 * i + 1] / (256 / BINS_NUM);
			int B = row[3 * i + 2] / (256 / BINS_NUM);
			double wgt = cur_hist[R * BINS_NUM * BINS_NUM + G * BINS_NUM + B];
			double argument = i * i * norm_i + j * j * norm_j;
			result += wgt * exp(-argument / 2);
		}
		row += step;
	}
	return result;
}