#include "inLine.h"
#include <stdlib.h>
#include <memory.h>


inLine::inLine()
{
	lineNumber = 1;
	treshold = 10;
	course = 0;
	gauss_depth = 1;
	
	width = 0;
	height = 0;
	
	imRGB = new unsigned char*;
	imRGB[0] = NULL;
	im = new unsigned char*;
	im[0] = NULL;
	proj = new double*;
	proj[0] = new double[8];
	proj[0][0] = 1.0; proj[0][1] = 0.0; proj[0][2] = 0.0;
	proj[0][3] = 0.0; proj[0][4] = 1.0; proj[0][5] = 0.0;
	proj[0][6] = 0.0; proj[0][7] = 0.0;


	diff = new unsigned char*;
	diff[0] = NULL;

	maxDiff = 1.0;
}
inLine::~inLine()
{
	clear_data();
	return;
}
int inLine::change_lineNumber(int new_lineNumber)
{
	if(new_lineNumber < 1)
		return 0;
	clear_data();
	lineNumber = new_lineNumber;
	course = (course < lineNumber)? course : lineNumber - 1;
	width = 0;
	height = 0;
	
	imRGB = new unsigned char*[lineNumber];
	im = new unsigned char*[lineNumber];
	proj = new double*[lineNumber];
	diff = new unsigned char*[lineNumber];
	for(int i = 0; i < lineNumber; i++){
		imRGB[i] = NULL;
		im[i] = NULL;
		diff[i] = NULL;
		proj[i] = new double[8];
		
		proj[i][0] = 1.0; proj[i][1] = 0.0; proj[i][2] = 0.0;
		proj[i][3] = 0.0; proj[i][4] = 1.0; proj[i][5] = 0.0;
		proj[i][6] = 0.0; proj[i][7] = 0.0;
	}

	return 1;
}
int inLine::change_course(int new_course)
{
	if(new_course < 0 || new_course > lineNumber - 1)
		return 0;
	course = new_course;
	return 1;
}
int inLine::shift_queue()
{
	if(lineNumber < 2)
		return 1;
	if(!width || !height || !im || !imRGB || !diff || !proj)
		return 0;

	unsigned char* im_temp = im[0];
	unsigned char* imRGB_temp = imRGB[0];
	unsigned char* diff_temp = diff[0];
	double* proj_temp = proj[0];
	for(int i = 0; i < lineNumber - 1; i++){
		im[i] = im[i + 1];
		imRGB[i] = imRGB[i + 1];
		diff[i] = diff[i + 1];
		proj[i] = proj[i + 1];
	}
	if(!im_temp)	im_temp = new unsigned char[width * height];
	if(!imRGB_temp) imRGB_temp = new unsigned char[3 * width * height];
	if(!diff_temp)	diff_temp = new unsigned char[width * height];
	if(!proj_temp)	proj_temp = new double[8];

	im[lineNumber - 1] = im_temp;
	imRGB[lineNumber - 1] = imRGB_temp;
	diff[lineNumber - 1] = diff_temp;
	proj[lineNumber - 1] = proj_temp;

	if(lineNumber > 1) {
		if(im[lineNumber - 2])	
			memcpy(im[lineNumber - 1], im[lineNumber - 2], width * height * sizeof(unsigned char));
		if(imRGB[lineNumber - 2])
			memcpy(imRGB[lineNumber - 1], imRGB[lineNumber - 2], 3 * width * height * sizeof(unsigned char));
		memset(diff[lineNumber - 1], 0, width * height * sizeof(unsigned char));
		proj[lineNumber - 1][0] = 1.0; proj[lineNumber - 1][1] = 0.0; 
		proj[lineNumber - 1][2] = 0.0; proj[lineNumber - 1][3] = 0.0; 
		proj[lineNumber - 1][4] = 1.0; proj[lineNumber - 1][5] = 0.0;
		proj[lineNumber - 1][6] = 0.0; proj[lineNumber - 1][7] = 0.0;
	}
	
	return 1;
}
int inLine::clear_data()
{
	for(int i = 0; i < lineNumber; i++){
		if(im && im[i])			{delete[] im[i]; im[i] = NULL;};
		if(imRGB && imRGB[i])	{delete[] imRGB[i]; imRGB[i] = NULL;};
		if(diff && diff[i])		{delete[] diff[i]; diff[i] = NULL;}
		if(proj && proj[i])		{delete[] proj[i]; proj[i] = NULL;}
	}
	if(im)		{delete[] im; im = NULL;}
	if(imRGB)	{delete[] imRGB; imRGB = NULL;}
	if(diff)	{delete[] diff; diff = NULL;}
	if(proj)	{delete[] proj; proj = NULL;}

	width = 0;
	height = 0;
	
	return 1;
}
int inLine::gauss_filter(unsigned char* im, int im_width, int im_height)
{
	if(!im || !im_width || !im_height)
		return 0;
	unsigned char* dxx[5];
	int* gxx = new int[im_width];
	int* Gxx = new int[im_width - 4];
	int* s = new int[(im_width - 4) * (im_height - 4)];

	dxx[1] = im; dxx[2] = im + im_width;
	dxx[3] = im + 2 * im_width; dxx[4] = im + 3 * im_width;

	for(int j = 2; j < im_height - 2; j++){
		dxx[0] = dxx[1]; dxx[1] = dxx[2]; 
		dxx[2] = dxx[3]; dxx[3] = dxx[4];
		dxx[4] += im_width;

		gauss_sweep_one(dxx, im_width, gxx);
		gauss_sweep_two(gxx, im_width, Gxx);
		gauss_sweep_three(Gxx, im_width, s + (j - 2) * (im_width - 4));
	}
	gauss_sweep_four(s, im_width, im_height, im);

	delete [] gxx;
	delete [] Gxx;
	delete [] s;

	return 1;
}
int inLine::gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx)
{
	for(int i = 0; i < im_width; i++)
		gxx[i] = dxx[0][i] + (dxx[1][i] << 2) + (dxx[2][i] << 2) + (dxx[2][i] << 1) +
				(dxx[3][i] << 2) + dxx[4][i];
	return 1;	
}
int inLine::gauss_sweep_two(int* gxx, int im_width, int* Gxx)
{
	for(int i = 0; i < im_width - 4; i++)
		Gxx[i] = gxx[i] + (gxx[i + 1] << 2) + (gxx[i + 2] << 2) + 
			(gxx[i + 2] << 1) + (gxx[i + 3] << 2) + gxx[i + 4];
	return 1;
}
int inLine::gauss_sweep_three(int* Gxx, int im_width, int* s)
{
	for(int j = 0; j < im_width - 4; j++)
		s[j] = Gxx[j] >> 8;

	return 1;
}
int	inLine::gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im)
{
	memset(im, 0, im_width * im_height * sizeof(unsigned char));
	for(int j = 2; j < im_height - 2; j++)
		for(int i = 2; i < im_width - 2; i++)
			im[i + j * im_width] = s[i - 2 + (j - 2) * (im_width - 4)];
	return 1;
}
int inLine::load(unsigned char* image, unsigned char* imageRGB, double* projection, int im_width, int im_height)
{
	if(!image || !projection || !im_width || !im_height)
		return 0;
	if(!width || !height) {
		width = im_width;
		height = im_height;
	}
	if(width != im_width || height != im_height)
		return 0;
	shift_queue();

	if(!im[lineNumber - 1]) im[lineNumber - 1] = new unsigned char[width * height];
	memcpy(im[lineNumber - 1], image, width * height * sizeof(unsigned char));
	if(!proj[lineNumber - 1]) proj[lineNumber - 1] = new double[8];
	memcpy(proj[lineNumber - 1], projection, 8 * sizeof(double));
	
	if(imageRGB){
		if(!imRGB[lineNumber - 1])
			imRGB[lineNumber - 1] = new unsigned char[3 * width * height];
		memcpy(imRGB[lineNumber - 1], imageRGB, 3 * width * height * sizeof(unsigned char));
	}else {
		if(imRGB[lineNumber - 1]) delete[] imRGB[lineNumber - 1];
		imRGB[lineNumber - 1] = NULL;
	}

	for(int i = 0; i < gauss_depth; i++)
		gauss_filter(im[lineNumber - 1], width, height);
	if(!diff[lineNumber - 1]){
		diff[lineNumber - 1] = new unsigned char[width * height];
		memset(diff[lineNumber - 1], 0, width * height * sizeof(unsigned char));
	}
	if(im[lineNumber - 1 - course])
		calculate_diff();

	return 1;
}
int inLine::calculate_diff()
{
	int shift = width / 30;
	memset(diff[lineNumber - 1], 0, width * height * sizeof(unsigned char));
	for(int i = 0; i < course; i++)
		if(!im[lineNumber - 1 - i])
			return 0;
	double pr[8];
	calculate_projection(pr);
	#pragma omp parallel
	{
		#pragma omp for
		for(int j = shift; j < height - shift; j++)
			for(int i = shift; i < width - shift; i++){
				double ip = i - width  * 0.5;
				double jp = j - height * 0.5;
				double norm = 1 / (pr[6] * ip + pr[7] * jp + 1);
				double x = (pr[0] * ip + pr[1] * jp + pr[2]) * norm;
				double y = (pr[3] * ip + pr[4] * jp + pr[5]) * norm;
				x += width  * 0.5;
				y += height * 0.5;
				int value = 0;
				if(x >= 0 && x <= width - 1 && y >= 0 && y <= height - 1) {
					int ix = int(x);
					int iy = int(y);
					double dx = x - ix;
					double dy = y - iy;
					double course_color = 
						(1 - dx)*(1 - dy)*im[lineNumber - 1 - course][ix + iy * width] + 
						dx*(1 - dy)*im[lineNumber - 1 - course][ix + 1 + iy * width] + 
						(1 - dx)* dy * im[lineNumber - 1 - course][ix + (iy + 1) * width] + 
						dx * dy * im[lineNumber - 1 - course][(ix + 1) + (iy + 1) * width]; 

					value = int(im[lineNumber - 1][i + j * width] - course_color);
					value = abs(value);
				}
				diff[lineNumber - 1][i + j * width] = (value > treshold)? value : 0;
			}
	}
	double average_diff = 0;
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++){
			average_diff += diff[lineNumber - 1][i + j * width];
		}
	average_diff /= width * height;
	if(average_diff > maxDiff)
		memset(diff[lineNumber - 1], 0, width * height * sizeof(unsigned char));
	return 1;
}
int inLine::calculate_projection(double* projection)
{
	double q[8];
	double proj_new[8];
	q[0] = 1.0; q[1] = 0.0; q[2] = 0.0;
	q[3] = 0.0; q[4] = 1.0; q[5] = 0.0;
	q[6] = 0.0; q[7] = 0.0;
	for(int i = 0; i < course; i++){
		double denom = proj[lineNumber - 1 - i][6] * q[2] + proj[lineNumber - 1 - i][7] * q[5] + 1;
		if(!denom)
			return 0;
		proj_new[0] = proj[lineNumber - 1 - i][0] * q[0] + proj[lineNumber - 1 - i][1] * q[3] + proj[lineNumber - 1 - i][2] * q[6];
		proj_new[1] = proj[lineNumber - 1 - i][0] * q[1] + proj[lineNumber - 1 - i][1] * q[4] + proj[lineNumber - 1 - i][2] * q[7];
		proj_new[2] = proj[lineNumber - 1 - i][0] * q[2] + proj[lineNumber - 1 - i][1] * q[5] + proj[lineNumber - 1 - i][2];
		proj_new[3] = proj[lineNumber - 1 - i][3] * q[0] + proj[lineNumber - 1 - i][4] * q[3] + proj[lineNumber - 1 - i][5] * q[6];
		proj_new[4] = proj[lineNumber - 1 - i][3] * q[1] + proj[lineNumber - 1 - i][4] * q[4] + proj[lineNumber - 1 - i][5] * q[7];
		proj_new[5] = proj[lineNumber - 1 - i][3] * q[2] + proj[lineNumber - 1 - i][4] * q[5] + proj[lineNumber - 1 - i][5];
		proj_new[6] = proj[lineNumber - 1 - i][6] * q[0] + proj[lineNumber - 1 - i][7] * q[3] + q[6];
		proj_new[7] = proj[lineNumber - 1 - i][6] * q[1] + proj[lineNumber - 1 - i][7] * q[4] + q[7];
	
		proj_new[0] /= denom;
		proj_new[1] /= denom;
		proj_new[2] /= denom;
		proj_new[3] /= denom;
		proj_new[4] /= denom;
		proj_new[5] /= denom;
		proj_new[6] /= denom;
		proj_new[7] /= denom;

		memcpy(q, proj_new, 8 * sizeof(double));
	}
	memcpy(projection, proj_new, 8 * sizeof(double));
	return 1;
}
unsigned char** inLine::get_diff()
{
	return diff;
}
double** inLine::get_proj()
{
	return proj;
}
unsigned char** inLine::get_im()
{
	return im;
}
int inLine::change_treshold(int new_treshold)
{
	if(new_treshold < 0)
		return 0;
	treshold = new_treshold;
	return 1;
}
unsigned char** inLine::get_imRGB()
{
	return imRGB;
}
