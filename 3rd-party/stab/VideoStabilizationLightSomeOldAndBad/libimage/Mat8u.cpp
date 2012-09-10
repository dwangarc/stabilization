#include "mat8u.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <libjpeg/jpeglib.h>
#include <libtiff/tiffio.h>
#include <libpng/png.h>
#include <boost/filesystem.hpp>
#include "image.h"

#define ALIGN 8

//mat8u::mat8u(boost::filesystem::path & aPath)
//{
//	this->initialize(aPath);
//}
//
//mat8u::mat8u(const mat8u & aMap8u)
//{
//	width = aMap8u.width;
//	height = aMap8u.height;
//	step = aMap8u.step;
//
//	int dataSize = height * step;
//	data = (UCHAR*)malloc(dataSize);
//	memcpy(data, aMap8u.data, dataSize);
//}

mat8u::~mat8u()
{
	//free(data);
}

unsigned char * mat8u::getData()
{
	return data;
}

int mat8u::getWidth()
{
	return width;
}

int mat8u::getHeight()
{
	return height;
}

int mat8u::getStep()
{
	return step;
}

void mat8u::initialize(boost::filesystem::path & aPath)
{
	boost::shared_ptr<ImageInfo> imageInfo = ImageInfo::createByExtension(aPath.extension());
	this->allocateMemory(imageInfo->getSize(aPath));
	this->load(aPath);
}

void mat8u::allocateMemory(WidthAndHeight & aWidthAndHeight)
{
	if (aWidthAndHeight.width > INT_MAX - ALIGN)
	{
		throw new std::runtime_error("row size overflow");
	};
	if (aWidthAndHeight.height > INT_MAX / aWidthAndHeight.width)
	{
		throw new std::runtime_error("buffer size overflow");
	};

	width = aWidthAndHeight.width;
	height = aWidthAndHeight.height;
	step = aWidthAndHeight.width;
	//data = (UCHAR*)malloc(aWidthAndHeight.width * aWidthAndHeight.height);

	//if (data == NULL)
	//{
	//	throw new std::runtime_error("unable to allocate memory");
	//}
}

ScannerMat8u::ScannerMat8u(const ScannerMat8u & aMat8u): mat8u(aMat8u)
{
}

boost::shared_ptr<mat8u> ScannerMat8u::copy()
{
	return boost::shared_ptr<mat8u>(new ScannerMat8u(*this));
}

void ScannerMat8u::load(boost::filesystem::path & aPath)
{
	throw new std::runtime_error("illegal method call. this method should not be implemented");
}

BmpMat8u::BmpMat8u(const BmpMat8u &aMap8u): mat8u(aMap8u)
{
}

boost::shared_ptr<mat8u> BmpMat8u::copy()
{
	return boost::shared_ptr<mat8u>(new BmpMat8u(*this));
}

void BmpMat8u::load(boost::filesystem::path & aPath)
{
	BITMAPINFO	*bmi = NULL;
	UCHAR		*buf = NULL;

	FILE *f = fopen(aPath.file_string().data(), "rb");
	if (!f)
	{
		throw new std::runtime_error("unable to open file");
	}

	bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256*4);
	fseek(f, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(bmi, 1, sizeof(BITMAPINFOHEADER), f);

	int width		= bmi->bmiHeader.biWidth;
	int height		= bmi->bmiHeader.biHeight;
	int scanlength	= ((width*bmi->bmiHeader.biBitCount/8+3)/4)*4;
	int step		= this->step;

	if (this->width != width || this->height != height) goto end;

	if (bmi->bmiHeader.biBitCount == 8)
		fread(bmi->bmiColors, 1, 256*4, f);

	buf = (UCHAR*)malloc(scanlength*bmi->bmiHeader.biHeight);
	fread(buf, scanlength, height, f);
	fclose(f);

	UCHAR *src = buf + scanlength * (height - 1);
	UCHAR *dst = this->getData();
	if (bmi->bmiHeader.biBitCount == 8) {
		int gray;
		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				RGBQUAD *quad = bmi->bmiColors + src[i];
				//dst[i] = (int)(0.212671*quad->rgbRed + 0.715160*quad->rgbGreen + 0.072169*quad->rgbBlue);
				gray = 218 * quad->rgbRed + 732 * quad->rgbGreen + 74 * quad->rgbBlue;
				dst[i] = gray >> 10;
			}
			src -= scanlength;
			dst += step;
		}
	} else if (bmi->bmiHeader.biBitCount == 24) {
		int gray;
		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				RGBTRIPLE *triple = (RGBTRIPLE*)(src + 3*i);
				//dst[i] = (int)(0.212671*triple->rgbtRed + 0.715160*triple->rgbtGreen + 0.072169*triple->rgbtBlue);
				gray = 218 * triple->rgbtRed + 732 * triple->rgbtGreen + 74 * triple->rgbtBlue;
				dst[i] = gray >> 10;
			}
			src -= scanlength;
			dst += width;
		}
	}

end:
	if (buf)	free(buf);
	if (bmi)	free(bmi);
}

JpgMat8u::JpgMat8u(const JpgMat8u &aMap8u): mat8u(aMap8u)
{
}

boost::shared_ptr<mat8u> JpgMat8u::copy()
{
	return boost::shared_ptr<mat8u>(new JpgMat8u(*this));
}

void JpgMat8u::load(boost::filesystem::path & aPath)
{
	struct jpeg_decompress_struct cinfo;
	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;

	infile = fopen(aPath.file_string().data(), "rb");
	if (!infile)
	{
		throw new std::runtime_error("mat8u: unable to open file");
	};

	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
  
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	cinfo.out_color_space = JCS_GRAYSCALE;

	int fileWidth		= cinfo.image_width;
	int fileHeight		= cinfo.image_height;
	int fileStep		= this->step;

	if (this->width != fileWidth || this->height != fileHeight) goto fail;

	jpeg_start_decompress(&cinfo);

	row_stride		= cinfo.output_width * cinfo.output_components;
	buffer			= (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	if (!buffer)
	{
		throw new std::runtime_error("mat8u: unable to allocate buffer");
	};

	UCHAR *dst = this->getData();
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(dst, (UCHAR*)buffer[0], width);
		dst += step;
	}
	jpeg_finish_decompress(&cinfo);

fail:
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
}

TifMat8u::TifMat8u(const TifMat8u &aMap8u): mat8u(aMap8u)
{
}

boost::shared_ptr<mat8u> TifMat8u::copy()
{
	return boost::shared_ptr<mat8u>(new TifMat8u(*this));
}

void TifMat8u::load(boost::filesystem::path & aPath)
{
	if (this->getData()) {
		free (this->getData());
	}

	my_image *img = load_image(aPath, 1);

	if (!img)
	{
		throw new std::runtime_error("unable to open file");
	};

	this->width  = img->width;
	this->height = img->height;
	this->step   = img->width;
	this->data   = img->data;
	delete img;
}

void mat8u::setDataPointer(unsigned char *newdata)
{
	data = newdata;
}