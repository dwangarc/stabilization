#pragma once

#include <string.h>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <libjpeg/jpeglib.h>
#include <libpng/png.h>
#include <libtiff/tiffio.h>
#include "bitmapheader.h"


// Image in Windows bitmap format (rows in inverse order)
struct bitmap {
	int	width;		// ������ �����������
	int height;		// ������ �����������
	int	step;		// ����� ������ � ������, ��������� �� ������� � 4 �����
	int nbytes;		// ����� ������ �� 1 �������, �������������� �������� 1, 3 ��� 4 (� �����-�������)
	UCHAR *data;	// ����� �����������
	BITMAPINFO *pbmi;	// defines the dimensions and color information of bitmap
};

// my_image - ��������� ��� �������� ������ �����������
// � ������� �� bitmap ������ �������� ������ ����, �� ������������ � �������� ������
struct my_image {
	int	width;		// ������ �����������
	int height;		// ������ �����������
	int step;		// ����� ������ � ������, ������ step = width * nbytes
					// �� �.�. ��������� �� ������� � 4 ����� (��� � bitmap)
	int nbytes;		// ����� ������ �� 1 �������, �������������� �������� 1, 3 ��� 4 (� �����-�������)
	UCHAR *data;	// ����� �����������
};


// [Bitmap.cpp]

// �������� bitmap � ��������� ������ ��� ����� �����������
bitmap*		bitmap_create(int width, int height, int nbytes);

// �������� bitmap � ������������� ������
void		bitmap_release(bitmap *bmp);

// ������������ ��������� � ������� ��� bitmap � nbytes = 3
void		bitmap_equalizeGIMP(bitmap *bmp);

// ������ R � B ������ � bitmap � nbytes = 3
void		bitmap_rgb2bgr(bitmap *bmp);


// [Bitmap_io.cpp]

// �������� ����������� � ��������� bitmap
bitmap*		bitmap_load(boost::filesystem::path & aPath, int planes = 0);

// ���������� ����������� � ������� BMP
bool 		bitmap_save(bitmap *bmp, boost::filesystem::path & aPath);

// ���������� ����������� � ������� JPG
bool		bitmap_saveJPEG(bitmap *bmp, boost::filesystem::path & aPath);

bitmap*		bitmap_copy(bitmap const* pic);


// [bitmap_io_scaled.cpp]

// TODO: ����������, �������� �������� PNG
// �������� ������������ �����������
bitmap*		bitmap_load_scaled(boost::filesystem::path & aPath, int planes, int scale);


// [image_io.cpp]

// ����� �� ����������� ��� ������� ��� �������� bitmap, � ��������������� my_image *load_image()
// � ����� ������������� my_image � bitmap � ������� convert_image_to_bitmap(), ������������� �����:
//  for_bmp: ���� true, �� step ������������� ��� � bitmap, ����� step = width * nbytes;
//  inverse: ��������� ������ � bitmap �������� ����� �����, ��� �������� ����������� � ����������� 
// ��������������� � bitmap ���������� ���������� inverse = true;

// �������� ����������� � ��������� my_image � ��������� ��������������� ������
// nbytes ���������, ��� ��������� �����������: 1 - grayscale, 3 - RGB, 4 - RGB � �����-�������
// ���� nbytes = 0, �� ����������� ����������� � ����� ��� �������������� ������, ��� ����
my_image*	load_image(boost::filesystem::path & aPath, int nbytes = 0, bool for_bmp = 0, bool inverse = 0);

// �������� my_image � ��������� ������ ��� ����� �����������
my_image*	image_create(int width, int height, int nbytes, bool for_bmp = 0);

// �������� my_image � ������������� ������
void		image_release(my_image *image);

// �������������� my_image � bitmap, my_image ���������;
// ����������, ����� step ��� �������� ��� � bitmap, ������� ����� �.�. ����� �����
bitmap*		convert_image_to_bitmap(my_image *image);

// ������������ ��������� � ������� ��� my_image � nbytes = 3
void		image_equalizeGIMP(my_image *image);

// ������ my_image �� ���� � ������� JPEG, PNG ��� TIFF; ����������, ����� step = width * nbytes
bool		write_image(char *file_name, my_image *image,
						double *geotiffInfo = NULL, DWORD jpegQuality = 80, DWORD tiffCompression = COMPRESSION_NONE);
// �� �� �����
bool		write_image(char *file_name, int width, int height, int nbytes, UCHAR *image,
						double *geotiffInfo = NULL, DWORD jpegQuality = 80, DWORD tiffCompression = COMPRESSION_NONE);

// ��������� ����, ����������� ��� GeoTIFF
void		addGeoTiffTags();

// ������ ��������� tiff
bool		write_tiff_header(TIFF *tif, int width, int height, int nbytes,
							  double *geo_info, DWORD m_dwExportQuality = 80, DWORD m_dwExportCompression = COMPRESSION_NONE);
