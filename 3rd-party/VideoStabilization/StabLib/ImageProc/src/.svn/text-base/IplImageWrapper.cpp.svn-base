#include "stdafx.h"
#include "../inc/IplImageWrapper.h"

IplImageWrapper::IplImageWrapper(Picture pic)
{
	image = new IplImage();
	image->nSize = sizeof(IplImage);
	image->nChannels = pic.getColors();
	image->depth = 8;
	strcpy(image->colorModel, "RGB");
	strcpy(image->channelSeq, "BGR");
	image->align = 8;
	image->width = pic.getWidth();
	image->height = pic.getHeight();
	
	image->widthStep = image->width;
	image->widthStep *= image->nChannels;
	int shift = image->widthStep % image->align;
	image->widthStep += shift == 0 ? 0 : image->align - shift;
	
	image->imageSize = image->widthStep * image->height;
	image->imageDataOrigin = 0;

	image->imageData = new char[image->imageSize];
	unsigned char* data = (unsigned char*)image->imageData;
	for (int i = 0; i < image->height; ++i)
	{
		memcpy(data, pic.getPictureChar() + (pic.getHeight() - i - 1) * pic.getWidth() * pic.getColors(), pic.getWidth() * pic.getColors());
		data += image->widthStep;
	}

}

IplImage const* IplImageWrapper::getImage() const
{
	return image;
}

IplImageWrapper::~IplImageWrapper()
{
	delete[] image->imageData;
	delete image;
}
