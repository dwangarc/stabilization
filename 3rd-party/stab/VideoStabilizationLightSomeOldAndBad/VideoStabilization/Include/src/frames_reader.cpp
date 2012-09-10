#include "stdafx.h"
#include "../frames_reader.h"
#include "ImageProc/inc/pictureConverter.h"

#include <fstream>
#include <iostream>
#include <Windows.h>
#include <math.h>

FramesReader::FramesReader(std::string const& pref, std::string const& extent, size_t numberSize, DWORD slpTm, DWORD maxTime, size_t firstFrame)
:prefix(pref), extention(extent), numSize(numberSize), maxWait(maxTime), sleepTime(slpTm)
{
	outNum = firstFrame;
	outPrefix = "Snapshots/out";
	latest = 0;
	curFrame = firstFrame - 1;
	maxFrameNum = (size_t)pow(10.0, (double)numSize) - 1;
}

void FramesReader::saveFrame(unsigned char const* pic, int width, int height, int colors, char const* fileName)
{
//	PictureConverter converter(width, height);
//	unsigned char* resPic = converter.convertGrey(pic, 3);

	bitmap* bitMapH = bitmap_create(width, height, 3);
	for (int i = 0; i < width * height; ++i)
	{
		for (int c = 0; c < 3; ++c)
		{
			bitMapH->data[3 * i + c] = pic[i * colors + (colors != 1 ? c : 0)];
		}
	}
	if (!fileName)
	{
		saveFrame(bitMapH);
	}
	else
	{
		b_path file_path = fileName;
		bitmap_save(bitMapH, file_path);
	}

	bitmap_release(bitMapH);
//	delete[] resPic;
}


bitmap* FramesReader::nextFrame()
{
	++curFrame;
	if (curFrame > maxFrameNum) return bitmap_copy(latest);
	std::string fileName = getName(curFrame + 1);
	DWORD tm = GetTickCount();
	std::fstream file;
	do
	{
		Sleep(sleepTime);
		file.open(fileName.c_str(), std::ios::in);
	}while (!file.is_open() && GetTickCount() - tm < maxWait);
	if (!file.is_open()) return bitmap_copy(latest);
	file.close();
	fileName = getName(curFrame);
	if (FileSize64(fileName.c_str()) < MIN_FILE_SIZE) return bitmap_copy(latest);
	if (latest)
	{
		bitmap_release(latest);
	}
	b_path file_path = fileName.c_str();
	latest = bitmap_load(file_path, 3);
//		bitmap_load_pnm(fileName.c_str(), 3);
	return bitmap_copy(latest);
}

std::string FramesReader::getName(size_t num)
{
	std::string st = sztToStr(num);
	st = addLeadingZeros(st, numSize);
	return prefix + st + "." + extention;
}

FramesReader::~FramesReader()
{
	if (latest)
	{
		bitmap_release(latest);
	}
}

void FramesReader::saveFrame(bitmap* btm)
{
	std::string st = sztToStr(outNum++);
	st = addLeadingZeros(st, numSize);
	st = outPrefix + st + ".jpeg";
	b_path file_path = st.c_str();
	bitmap_save(btm, file_path);
}
