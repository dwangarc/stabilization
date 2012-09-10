#include "ImageInfo.h"
#include <exception>
#include <stdio.h>
#include <algorithm>
#include "image.h"

WidthAndHeight PngImageInfo::getSize(boost::filesystem::path & aPath)
{
	WidthAndHeight result;

	FILE *fp;
	fopen_s(&fp, aPath.file_string().data(), "rb");
	if (!fp)
	{
		throw std::runtime_error("cannot open file [" + aPath.file_string() + "]");
	}

	char header[8];		// 8 is the maximum size that can be checked
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8))
	{
		throw std::runtime_error("cannot open file [" + aPath.file_string() + "]");
	}

	// create and initialize the png_struct
	png_structp	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		fclose(fp);
		throw std::runtime_error("cannot create png structure for file [" + aPath.file_string() + "]");
	}

	// allocate the memory for image information
	png_infop	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		fclose(fp);
		throw std::exception("cannot create png information structure");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		// free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		throw std::exception("unexpected error while reading png file information");
	}

	// set up the input control (use standard C streams)
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	// read information from the PNG file
	png_read_info(png_ptr, info_ptr);

	result.width = info_ptr->width;
	result.height = info_ptr->height;

	// clean up after the read, and free any memory allocated
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

	fclose(fp);
	return result;
}

WidthAndHeight JpgImageInfo::getSize(boost::filesystem::path & aPath)
{
	WidthAndHeight result;

	FILE *f = fopen(aPath.file_string().data(), "rb");
	if (!f)
	{
		throw std::runtime_error("cannot open file [" + aPath.file_string() + "]");
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, f);
	ImageInfoException::throwIf(jpeg_read_header(&cinfo, TRUE) == 0, "unable to read jpeg header");

	result.width = cinfo.image_width;
	result.height = cinfo.image_height;

	jpeg_destroy_decompress(&cinfo);
	fclose(f);

	return result;
}

WidthAndHeight TifImageInfo::getSize(boost::filesystem::path & aPath)
{
	WidthAndHeight result;

	static BOOL bFirstRun = true;
	if (bFirstRun) {
		TIFFSetWarningHandler(NULL);
		TIFFSetErrorHandler(NULL);
		bFirstRun = false;
	}

	TIFF *tif = TIFFOpen(aPath.file_string().data(), "r");
	if (!tif)
	{
		throw std::runtime_error("cannot open file [" + aPath.file_string() + "]");
	}

	BOOL	success	= FALSE;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,		&result.width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH,		&result.height);

	TIFFClose(tif);

	return result;
}

WidthAndHeight BmpImageInfo::getSize(boost::filesystem::path & aPath)
{
	WidthAndHeight result;

	FILE *f;
	BITMAPINFOHEADER bmi;

	fopen_s(&f, aPath.file_string().data(), "rb");
	if (!f)
	{
		throw std::runtime_error("cannot open file [" + aPath.file_string() + "]");
	}

	fseek(f, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(&bmi, 1, sizeof(BITMAPINFOHEADER), f);

	fclose(f);

	result.width = bmi.biWidth;
	result.height = bmi.biHeight;

	return result;
}
