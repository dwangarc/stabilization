#include "HistRatio.h"
#include <memory.h>
#include <stdlib.h>

HistRatio::HistRatio()
{
	BinNum = 8;
	TotalBins = BinNum * BinNum * BinNum;
	BorderShift = 10;
	Range = 15;
	L = 3;
	Treshold = 15;

	HistInit = new double[TotalBins];
	HistModel = new double[TotalBins];
	HistDivide = new double[TotalBins];
	ImBackProj = NULL;
	
	memset(HistInit, 0, TotalBins * sizeof(double));
	memset(HistModel, 0, TotalBins * sizeof(double));
	memset(HistDivide, 0, TotalBins * sizeof(double));

	x = 0;
	y = 0;
	w = 0;
	h = 0;

	Initialized = 0;

	Alpha = 0.5;

	return;
}
HistRatio::~HistRatio()
{
	if(HistInit) delete[] HistInit;
	HistInit = NULL;
	if(HistModel) delete[] HistModel;
	HistModel = NULL;
	if(HistDivide) delete[] HistDivide;
	HistDivide = NULL;
	if(ImBackProj) delete[] ImBackProj;
	ImBackProj = NULL;

	return;
}
int HistRatio::init_track(unsigned char* image, int im_width, int im_height, 
						  int in_x, int in_y, int in_w, int in_h)
{
	if(in_x == 0 || in_y == 0 || in_w == 0 || in_h == 0)
		return 0;
	
	if(Initialized)
		return 0;

	x = in_x;
	y = in_y;
	w = in_w;
	h = in_h;

	if(!image || !im_width || !im_height)
		return 0;
	width = im_width;
	height = im_height;
	if(ImBackProj) delete[] ImBackProj;
	ImBackProj = new double[width * height];
	
	if(width != im_width || height != im_height)
		return 0;
	
	int x0 = x;
	int y0 = y;
	int hx = w / 2;
	int hy = h / 2;
	
	if(x0 - hx < 0 || x0 + hx > width || y0 - hy < 0 || y0 + hy > height)
		return 0;

	memset(HistInit, 0, TotalBins * sizeof(double));
	memset(HistModel, 0, TotalBins * sizeof(double));
	memset(HistDivide, 0, TotalBins * sizeof(double));
	
	memset(ImBackProj, 0, width * height * sizeof(double));
	
	unsigned char* row = image + (y0 - hy) * width * 3 + x0 * 3;
	for (int j = -hy; j <= hy; j++){
		for (int i = -hx; i <= hx; i++){		
				int BINR = row[3*i + 0] * BinNum / 256;
				int BING = row[3*i + 1] * BinNum / 256;
				int BINB = row[3*i + 2] * BinNum / 256;
				HistInit[BINR * BinNum * BinNum + BING * BinNum + BINB]++;
		}
		row += width * 3;
	}
		
	hx += BorderShift;
	hy += BorderShift;
	
	if(x0 - hx < 0 || x0 + hx > width || y0 - hy < 0 || y0 + hy > height)
		return 0;

	row = image + (y0 - hy) * width * 3 + x0 * 3;
	for (int j = -hy; j <= hy; j++){	
		for (int i = -hx; i <= hx; i++){								
			int BINR = row[3*i + 0] * BinNum / 256;
			int BING = row[3*i + 1] * BinNum / 256;
			int BINB = row[3*i + 2] * BinNum / 256;
			HistModel[BINR * BinNum * BinNum + BING * BinNum + BINB]++;
		}
		row += width * 3;
	}			

	hist_divide(HistInit, HistModel, HistDivide);
	calc_foreground(image);
	memset(HistDivide, 0, TotalBins * sizeof(double));

	Initialized = 1;
	return 1;
}
int HistRatio::hist_ratio(unsigned char* image, int im_width, int im_height, unsigned char* mask)
{
	if(!Initialized)
		return 0;

	if(x == 0 || y == 0 || w == 0 || h == 0)
		return 0;

	int x0 = x;
	int y0 = y;
	int hx = w / 2;
	int hy = h / 2;
	
	if(!width || !height)
		return 0;
	if(width != im_width || height != im_height)
		return 0;

	count_hist(image);
	hist_divide(HistInit, HistModel, HistDivide);	
	calc_foreground(image);
	shift();
	upgrade_hist(image, mask);

	//memset(HistDivide, 0, TotalBins * sizeof(double));
	
	return 1;
}
int HistRatio::load_data(int in_x, int in_y, int in_width, int in_height)
{
	x = in_x;
	y = in_y;
	w = in_width;
	h = in_width;

	return 1;
}
int HistRatio::read_data(int &out_x, int &out_y, int &out_width, int &out_height)
{
	out_x = x;
	out_y = y;
	out_width = w;
	out_height = h;

	return 1;
}
void HistRatio::hist_divide(double* hist1, double* hist2, double* hist_res)
{
	for(int i = 0; i < TotalBins; i++)
		if(hist2[i] == 0)	hist_res[i] = 0;
		else	hist_res[i] = __max(hist1[i] / hist2[i], 0);
	
	histogram_normalize(hist_res);

	return;
}
void HistRatio::histogram_normalize(double* hist)
{
	double maxVal = 0;
	double minVal = 100000000;

	for(int i = 0; i < TotalBins; i++){
		if(hist[i] > maxVal)
			maxVal = hist[i];
		if(hist[i] > 0 && hist[i] < minVal)
			minVal = hist[i];
	}
	
	for(int i = 0; i < TotalBins; i++)
		if(hist[i] > 0)
			hist[i] = (hist[i] - minVal) * 255.0 / (maxVal - minVal);

	return;
}
void HistRatio::calc_foreground(unsigned char* image)
{
	double	max_val = 0;
	
	int x0 = x;
	int y0 = y;
	int hx = w / 2 + BorderShift;
	int hy = h / 2 + BorderShift;
	
	if(x0 - hx < 0 || x0 + hx > width || y0 - hy < 0 || y0 + hy > height)
		return;

	double* bp_img = calc_bp(image, HistDivide);

	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++)
			if(bp_img[i + j * width] > max_val)
				max_val = bp_img[i + j * width];
	
	if(max_val != 0)
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++)
				bp_img[i + j * width] *= 255.0 / max_val;
	
	memset(ImBackProj, 0, width * height * sizeof(double));
	double* src_img = bp_img + (y0 - hy) * width + x0;
	double* dst_img = ImBackProj + (y0 - hy) * width + x0;
	for(int j = -hy; j <= hy; j++){
		for(int i = -hx; i <= hx; i++){
				dst_img[i] = src_img[i];
		}
		src_img += width;
		dst_img += width;
	}
	delete[] bp_img;

	return;
}
double* HistRatio::calc_bp(unsigned char* image, double* hist)
{
	double* bp_img = new double[width * height];
	memset(bp_img, 0, width * height * sizeof(double));

	unsigned char*	src_img = image;
	double*			dst_img = bp_img;
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++){
			int RBIN = (int)(src_img[0] * BinNum / 256);
			int GBIN = (int)(src_img[1] * BinNum / 256);
			int BBIN = (int)(src_img[2] * BinNum / 256);
			*dst_img = hist[BinNum * BinNum * RBIN + BinNum * GBIN + BBIN];
			src_img += 3;
			dst_img++;
		}
	
	return bp_img;
}
void HistRatio::count_hist(unsigned char* image)
{
	int x0 = x;
	int y0 = y;
	int hx = w / 2 + BorderShift;
	int hy = h / 2 + BorderShift;
	
	memset(HistModel, 0, TotalBins * sizeof(double));
	
	unsigned char* row = image + (y0 - hy) * width * 3 + x0 * 3;
	for(int j = -hy; j <= hy; j++){
		for(int i = -hx; i <= hx; i++){
			int RBIN = (int)(row[i*3 + 0] * BinNum / 256);
			int GBIN = (int)(row[i*3 + 1] * BinNum / 256);
			int BBIN = (int)(row[i*3 + 2] * BinNum / 256);
			HistModel[BinNum * BinNum * RBIN + BinNum * GBIN + BBIN]++;
		}
		row += width * 3;
	}

	return;
}
void HistRatio::shift()
{	
	double	sumRow, sumCol, sumPix;
	int		dx, dy;
	int		loopCount;

	int hx = w / 2;
	int hy = h / 2;
	double* img = ImBackProj;

	int x0 = x;
	int y0 = y;

	if(x0 - hx < 0 || x0 + hx > width || y0 - hy < 0 || y0 + hy > height)
		return;

	loopCount=0;
	dx=1;
	dy=1;
	while (abs(dx)+abs(dy)>=1 && loopCount<10){		
		sumRow = 0;
		sumCol = 0;
		sumPix = 0;
		double* row = img + (y - hy) * width + x;
		for (int j = -hy; j <= hy; j++){
			for (int i = -hx; i <= hx; i++){
				sumCol += row[i]*i;
				sumRow += row[i]*j;
				sumPix += row[i];
			}
			row += width;
		}
		dx = (int)(sumCol/sumPix + 0.5);
		dy = (int)(sumRow/sumPix + 0.5);

		if(x + dx - hx > BorderShift && x + dx + hx < width - BorderShift)
			x += dx;
		if(y + dy - hy > BorderShift && y + dy + hy < height - BorderShift)
			y += dy;

		loopCount++;
	}
	dx = x - x0;
	dy = y - y0;
	if (dx < -Range)	
		dx = -Range;
	if (dx > Range)		
		dx = Range;
	if (dy < -Range)	
		dy = -Range;
	if (dy > Range)		
		dy = Range;

	x	+= dx;
	y	+= dy;

	return;
}
int HistRatio::non_zero_data()
{
	if(x && y && w && h)
		return 1;
	return 0;
}
int HistRatio::reset_initialized()
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	if(ImBackProj) delete[] ImBackProj;
	ImBackProj = NULL;

	width = 0;
	height = 0;

	Initialized = 0;
	return 1;
}
void HistRatio::upgrade_hist(unsigned char* image, unsigned char* mask)
{
	if(!x || !y || !w || !h || !width || !height)
		return;
	double* HistTemp = new double[TotalBins];
	memset(HistTemp, 0, TotalBins * sizeof(double));
	
	int x0 = x;
	int y0 = y;
	int hx = w / 2;
	int hy = h / 2;
	
	if(x0 - hx < 0 || x0 + hx > width || y0 - hy < 0 || y0 + hy > height)
		return;

	unsigned char* row = image + (y0 - hy) * width * 3 + x0 * 3;
	unsigned char* mk = mask + (y0 - hy) * width + x0;
	for (int j = -hy; j <= hy; j++){
		for (int i = -hx; i <= hx; i++){		
				if(!mk[i])
					continue;
				int BINR = row[3*i + 0] * BinNum / 256;
				int BING = row[3*i + 1] * BinNum / 256;
				int BINB = row[3*i + 2] * BinNum / 256;
				HistTemp[BINR * BinNum * BinNum + BING * BinNum + BINB]++;
		}
		row += width * 3;
		mk += width;
	}
	histogram_normalize(HistTemp);

	for(int i = 0; i < TotalBins; i++)
		HistInit[i] = (1 - Alpha) * HistInit[i] + Alpha * HistTemp[i];

	histogram_normalize(HistInit);//No action  
	delete[] HistTemp;

	return;
}
