#include "BackwardHM.h"
#include <stdlib.h>
#include <memory.h>

#define __MAX(a, b) (a > b)? a : b

BackwardHM::BackwardHM()
{
	width = 0;
	height = 0;

	mapShift = 11;
	HistoryMap = NULL;
}
BackwardHM::~BackwardHM()
{
	if(HistoryMap){
		delete[] HistoryMap;
		HistoryMap = NULL;
	}
}
unsigned char* BackwardHM::get_history_map()
{
	return HistoryMap;
}
int BackwardHM::load(unsigned char** image, int im_width, int im_height, int image_num, double** proj)
{
	if(!image || !im_width || !im_height || !proj || image_num < 1)
		return 0;
	for(int i = 0; i < image_num; i++){
		if(!image[i]) return 0;
		if(!proj[i]) return 0;
	}
	if(!width || !height){
		width = im_width;
		height = im_height;
	}
	if(!HistoryMap){
		HistoryMap = new unsigned char[width * height];
		memset(HistoryMap, 0, width * height * sizeof(unsigned char));
	}
	
	unsigned char* imgc = new unsigned char[width * height];
	memset(imgc, 0, width * height * sizeof(unsigned char));
	double cur_pr[8];
	cur_pr[0] = 1; cur_pr[1] = 0; cur_pr[2] = 0; cur_pr[3] = 0;
	cur_pr[4] = 1; cur_pr[5] = 0; cur_pr[6] = 0; cur_pr[7] = 0;

	for(int num = image_num - 1; num >= 0; num--){
		convert_image(imgc, cur_pr, width, height);
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++){
				if(image[num][i + j * width] > 0)
					imgc[i + j * width] = 255;
			}
		memcpy(cur_pr, proj[num], 8 * sizeof(double));  
	}

	memcpy(HistoryMap, imgc, width * height * sizeof(unsigned char));
	delete[] imgc;
	return 1;
}
int	BackwardHM::convert_image(unsigned char* img, double* proj, int width, int height)
{
	unsigned char* temp = new unsigned char[width * height];
	memset(temp, 0, width * height * sizeof(unsigned char)); 
	#pragma omp parallel
	{
		#pragma omp for
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++){
				double jp = j - height * 0.5;
				double ip = i - width  * 0.5;
				double norm = (proj[6] * ip + proj[7] * jp + 1);
				double x = (proj[0] * ip + proj[1] * jp + proj[2]) * norm;
				double y = (proj[3] * ip + proj[4] * jp + proj[5]) * norm;
				x += width  * 0.5;
				y += height * 0.5;
				if(x < 0 || x > width - 1 || y < 0 || y > height - 1)
					continue;
				temp[int(x) + int(y) * width] = __MAX(img[i + j *width] - 255 / mapShift, 0);
			}
	}
	memcpy(img, temp, width * height * sizeof(unsigned char));
	delete[] temp;
	return 1;
}

