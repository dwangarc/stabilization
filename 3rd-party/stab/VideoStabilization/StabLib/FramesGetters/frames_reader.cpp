#include "stdafx.h"
#include "frames_reader.h"

#include "Include/different_libs.h"
#include "Libs/CxImage/ximage.h"


#include <fstream>
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>

FramesReader::FramesReader(/*std::string const& Opref, std::string const& Oextent, int numberSize*/)
:/*numSize(numberSize),*/ selectedFramesNames()
{
	//outNum = 0;
	//outPrefix = Opref;
	//outExtention = Oextent;
	//latest = 0;
}

//void FramesReader::saveFrame(Picture pic, char const* fileName)
//{
//	int width = pic.getWidth();
//	int height = pic.getHeight();
//	int colors = pic.getColors();
////	PictureConverter converter(width, height);
////	unsigned char* resPic = converter.convertGrey(pic, 3);
//
//	bitmap* bitMapH = bitmap_create(width, height, 3);
//	for (int i = 0; i < height; ++i)
//	{
//		for (int j = 0; j < width; ++j)
//		{
//			for (int c = 0; c < 3; ++c)
//			{
//				bitMapH->data[i * bitMapH->step + 3 * j + c] = pic.getPictureChar()[i * width * colors + j * colors + (colors != 1 ? c : 0)];
//			}
//		}
//	}
//	if (!fileName)
//	{
////		saveFrame(bitMapH);
//	}
//	else
//	{
//		b_path file_path = fileName;
//		bitmap_save(bitMapH, fileName/*file_path*/);
//	}
//
//	bitmap_release(bitMapH);
//}

FramesReader::~FramesReader()
{
	//if (latest)
	//{
	//	bitmap_release(latest);
	//}
}

//void FramesReader::saveFrame(bitmap* btm)
//{
//	std::string st = sztToStr(outNum++);
//	st = addLeadingZeros(st, numSize);
//	st = outPrefix + st + outExtention;
//	b_path file_path = st.c_str();
//	bitmap_save(btm, st.c_str()/*file_path*/);
//}

Picture FramesReader::nextPic()
{
	if (selectedFramesNames.size() == 0) return Picture();
	std::string fileName = *(selectedFramesNames.begin());
//	std::wstring wfileName = dif_lib::s2ws(fileName);
	selectedFramesNames.pop_front();
	CxImage* image = 0;
	
	FILE* file;
	file = fopen(fileName.c_str(), "r");
	if (file == NULL)
	{
		return Picture();
	}

	int ps = fileName.find_last_of('.');
	std::string dim = fileName.substr(ps + 1, fileName.length());
	if (dim == "bmp")
	{
		image = new CxImage(file, CXIMAGE_FORMAT_BMP);
	}
	if (dim == "jpeg" || dim == "jpg")
	{
		image = new CxImage(file, CXIMAGE_FORMAT_JPG);
	}
	if (dim == "png")
	{
		image = new CxImage(file, CXIMAGE_FORMAT_PNG);
	}
	fclose(file);
	
	Picture pic = Picture::createFromCxImage(image);
	delete image;
	return pic;
}


//bitmap const* FramesReader::nextSelectedFrame()
//{
//	if (selectedFramesNames.size() == 0) return 0;
//	std::string fileName = *(selectedFramesNames.begin());
//	selectedFramesNames.pop_front();
//	b_path file_path = fileName.c_str();
//	try
//	{
//		if (latest)
//		{
//			bitmap_release(latest);
//		}
//		int ps = fileName.find_last_of('.');
//		std::string dim = fileName.substr(ps + 1, fileName.length());
//		// bmp
//		if (dim == "bmp")
//		{
//			latest = bitmap_load_bmp(fileName.c_str(), 3);
//		}
//		else
//		{
//			// jpeg
//			if (dim == "jpeg" || dim == "jpg")
//			{
//				latest = bitmap_load_jpeg(fileName.c_str(), 3);
//			}
//			else
//			{
//				latest = bitmap_load(fileName.c_str(), 3);
//			}
//		}
//		return latest;
//	}
//	catch (...)
//	{
//		return 0;
//	}
//}

//Picture FramesReader::nextSelectedFramePic()
//{
//	bitmap const* bitMap = nextSelectedFrame();
//	if (!bitMap) return Picture();
//	Picture pic(bitMap->width, bitMap->height, bitMap->step / bitMap->width/*nbytes*/);
//	pic.copyPic(bitMap->data);
//	return pic;
//}


void FramesReader::addSelectedFrame(std::string const& frameName)
{
	selectedFramesNames.push_back(frameName);
}

void FramesReader::clearSelectedFrames()
{
	selectedFramesNames.clear();
}

