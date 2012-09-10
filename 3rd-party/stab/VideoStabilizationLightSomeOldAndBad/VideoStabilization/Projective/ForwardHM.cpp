#include "ForwardHM.h"
#include <stdlib.h>
#include <memory.h>

#define __MAX(a, b) (a > b)? a : b
ForwardHM::ForwardHM()
{
	HistoryMap = NULL;
	width = 0;
	height = 0;
	mapShift = 11;
}
ForwardHM::~ForwardHM()
{
	if(HistoryMap) {delete[] HistoryMap; HistoryMap = NULL;}
	return;
}
int ForwardHM::load(unsigned char* image, int im_width, int im_height, double* im_proj)
{
	if(!image || !im_width || !im_height || !im_proj)
		return 0;
	if(!width || !height){
		width = im_width;
		height = im_height;
	}
	if(width != im_width || height != im_height)
		return 0;
	if(!HistoryMap) {
		width = im_width;
		height = im_height;
		HistoryMap = new unsigned char[width * height];
		memset(HistoryMap, 0, width * height * sizeof(unsigned char));
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++){
				if(image[i + j * width]) HistoryMap[i + j * width] = 255;
			}
		return 1;
	}
	unsigned char* converted_HM = new unsigned char[width * height];
	memset(converted_HM, 0, width * height * sizeof(unsigned char));
	#pragma omp parallel
	{
		#pragma omp for
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++){
				double ip = i - width  * 0.5;
				double jp = j - height * 0.5;
				double norm = 1 / (im_proj[6] * ip + im_proj[7] * jp + 1);
				double x = (im_proj[0] * ip + im_proj[1] * jp + im_proj[2]) * norm;
				double y = (im_proj[3] * ip + im_proj[4] * jp + im_proj[5]) * norm;

				x += width  * 0.5;
				y += height * 0.5;
				if (x < 0 || x > (width - 1) || y < 0 || y > (height - 1)) {
					converted_HM[i + j * width] = 0;
					continue;
				}
				converted_HM[i + j * width] = 
					__MAX(HistoryMap[int(x) + int(y) * width] - 255 / mapShift, 0);
			}
	}
	memcpy(HistoryMap, converted_HM, width * height * sizeof(unsigned char));
	delete[] converted_HM;
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++) {
			if(image[i + j * width])
				HistoryMap[i + j * width] = 255;
		}
	return 1;
}
unsigned char* ForwardHM::get_history_map()
{
	return HistoryMap;
}
