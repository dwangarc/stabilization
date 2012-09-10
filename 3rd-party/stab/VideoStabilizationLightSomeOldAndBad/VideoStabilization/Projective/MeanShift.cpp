#include "MeanShift.h"
#include <memory.h>
#include <stdlib.h>
#include <math.h>

MeanShift::MeanShift()
{
	BinNum = 8;
	TotalBins = BinNum * BinNum * BinNum;
	HistInit = new double[TotalBins];
	Initialized = 0;

	x = 0;
	y = 0;
	w = 0;
	h = 0;
	width = 0;
	height = 0;

	mode = 1;
	upgrate_mode = 0;
	alpha = 0.1;
}
MeanShift::~MeanShift()
{
	if(HistInit) delete[] HistInit;
	HistInit = NULL;
}
int MeanShift::init_track(unsigned char* image, int im_width, int im_height, 
		int in_x, int in_y, int in_w, int in_h)
{
	if(Initialized)
		return 0;
	if(!image || !im_width || !im_height)
		return 0;
	
	if(!in_x || !in_y || !in_w || !in_h)
		return 0;

	width = im_width;
	height = im_height;
	x = in_x;
	y = in_y;
	w = in_w;
	h = in_h;

	extract_histogram(image, width, height, x, y, w / 2, h / 2, HistInit);

	Initialized = 1;

	return 1;
}
int MeanShift::extract_histogram(unsigned char *im, int w, int h, int x0, int y0, 
							int hx, int hy, double *hist)
{
	
	if (x0 < hx || x0 > width - hx || y0 < hy || y0 > height - hy) return 0;

	double norm_i = 1.0 / hx / hx;
	double norm_j = 1.0 / hy / hy;
	double norm;
	int R, G, B;
	int i, j;

	memset(hist, 0, TotalBins * sizeof(double));
	unsigned char *row = im + (y0 - hy) * 3 * width + x0 * 3;
	for (j = -hy; j <= hy; j++) {
		for (i = -hx; i <= hx; i++) {
			R = row[3*i + 0] / (256 / BinNum);
			G = row[3*i + 1] / (256 / BinNum);
			B = row[3*i + 2] / (256 / BinNum);
			norm = 1 - sqrt(i * i * norm_i + j * j * norm_j);
			hist[R * BinNum * BinNum + G * BinNum + B] += __max(norm, 0);
		}
		row += 3 * width;
	}

	norm = 0;
	for (i = 0; i < TotalBins; i++)
		norm += hist[i];
	norm = 1.0 / norm;
	for (i = 0; i < TotalBins; i++)
		hist[i] *= norm;

	return 1;
}
int MeanShift::mean_shift(unsigned char* image, int im_width, int im_height, unsigned char* mask)
{
	if(!mask && mode != 1)
		return 0;
	if(!image || !im_width || !im_height)
		return 0;
	if(!x || !y || !w || !h)
		return 0;
	
	int old_x, old_y, new_x, new_y;
	double hx, hy;
	double* current = new double[TotalBins];
	memset(current, 0, TotalBins * sizeof(double));
	int cur_new_x = x;
	int cur_new_y = y;

	hx = w / 2;
	hy = h / 2;

	new_x = x;
	new_y = y;

	int num = 0;

	do {
			old_x = new_x;
			old_y = new_y;

			if (!extract_histogram(image, width, height, old_x, old_y, 
				( int ) hx, ( int ) hy, current))
				return 0;
			
			for (int i = 0; i < TotalBins; i++)
				current[i] = (current[i] == 0 ? 0 : sqrt(HistInit[i] / current[i]));
					
			shift(image, width * 3, width, height, old_x, old_y, 
				( int ) hx, ( int ) hy, current, new_x, new_y, mask);
			
			num++;
	} while ((old_x != new_x || old_y != new_y) && (num < 20));
	
	
	x = new_x;
	y = new_y;

	if(upgrate_mode)
		upgrate_hist(image, mask);
	delete[] current;
	return 1;
}
int MeanShift::shift(unsigned char *im, int step, int width, int height, int x0, int y0, 
		  int hx, int hy, double *w, int &x1, int &y1, unsigned char* mask)
{
	if (x0 < hx || x0 > width - hx || y0 < hy || y0 > height - hy) return 0;

	int R, G, B;
	int i, j;

	double sum_x = 0;
	double sum_y = 0;
	double sum = 0;
	double val;

	unsigned char* row = im + (y0 - hy) * step + x0 * 3;
	unsigned char* mk = mask + (y0 - hy) * width + x0;
	for (j = -hy; j <= hy; j++) {
		for (i = -hx; i <= hx; i++) {
			R = row[3*i + 0] / (256 / BinNum);
			G = row[3*i + 1] / (256 / BinNum);
			B = row[3*i + 2] / (256 / BinNum);
			val = w[R * BinNum * BinNum + G * BinNum + B];
			if(mode == 3)
				val = val * mk[i];
			if(mode == 2)
				val = mk[i];
			sum_x += i * val;
			sum_y += j * val;
			sum += val;
		}
		row += step;
		mk += width;
	}

	x1 = x0 + (int)(sum_x / sum);
	y1 = y0 + (int)(sum_y / sum);

	return 1;
}
int MeanShift::non_zero_data()
{
	if(x && y && w && h)
		return 1;
	return 0;
}
int MeanShift::reset_initialized()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	memset(HistInit, 0, TotalBins * sizeof(double));

	width = 0;
	height = 0;

	Initialized = 0;
	return 1;
}
int MeanShift::load_data(int in_x, int in_y, int in_width, int in_height)
{
	x = in_x;
	y = in_y;
	w = in_width;
	h = in_width;

	return 1;
}
int MeanShift::read_data(int &out_x, int &out_y, int &out_width, int &out_height)
{
	out_x = x;
	out_y = y;
	out_width = w;
	out_height = h;

	return 1;
}
int MeanShift::upgrate_hist(unsigned char* im, unsigned char* mask)
{
	int x0 = x;
	int y0 = y;
	int hx = w / 2;
	int hy = h / 2;

	double* NewHist = new double[TotalBins];
	memset(NewHist, 0, TotalBins * sizeof(double));

	if (x0 < hx || x0 > width - hx || y0 < hy || y0 > height - hy) return 0;

	double norm_i = 1.0 / hx / hx;
	double norm_j = 1.0 / hy / hy;
	double norm;
	int R, G, B;
	int i, j;

	unsigned char *row = im + (y0 - hy) * 3 * width + x0 * 3;
	unsigned char* mk = mask + (y0 - hy) * width + x0;
	for (j = -hy; j <= hy; j++) {
		for (i = -hx; i <= hx; i++) {
			R = row[3*i + 0] / (256 / BinNum);
			G = row[3*i + 1] / (256 / BinNum);
			B = row[3*i + 2] / (256 / BinNum);
			norm = 1 - sqrt(i * i * norm_i + j * j * norm_j);
			NewHist[R * BinNum * BinNum + G * BinNum + B] += __max(norm, 0);
		}
		row += 3 * width;
		mk += width;
	}

	norm = 0;
	for (i = 0; i < TotalBins; i++)
		norm += NewHist[i];
	norm = 1.0 / norm;
	for (i = 0; i < TotalBins; i++)
		NewHist[i] *= norm;

	for(int i = 0; i < TotalBins; i++)
		HistInit[i] = (1 - alpha) * HistInit[i] + alpha * NewHist[i];

	norm = 0;
	for (i = 0; i < TotalBins; i++)
		norm += HistInit[i];
	norm = 1.0 / norm;
	for (i = 0; i < TotalBins; i++)
		HistInit[i] *= norm;
	
	delete[] NewHist;

	return 1;
}
int MeanShift::change_mode(int new_mode)
{
	if(new_mode < 1 || new_mode > 3)
		return 0;
	mode = new_mode;
	return 1;
}
