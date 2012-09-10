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
	int	width;		// ширина изображения
	int height;		// высота изображения
	int	step;		// длина строки в байтах, выровнена на границу в 4 байта
	int nbytes;		// число байтов на 1 пиксель, поддерживаются значения 1, 3 или 4 (с альфа-каналом)
	UCHAR *data;	// буфер изображения
	BITMAPINFO *pbmi;	// defines the dimensions and color information of bitmap
};

// my_image - структура для хранения буфера изображения
// в отличие от bitmap данные хранятся сверху вниз, но предусмотрен и обратный случай
struct my_image {
	int	width;		// ширина изображения
	int height;		// высота изображения
	int step;		// длина строки в байтах, обычно step = width * nbytes
					// но м.б. выровнена на границу в 4 байта (как в bitmap)
	int nbytes;		// число байтов на 1 пиксель, поддерживаются значения 1, 3 или 4 (с альфа-каналом)
	UCHAR *data;	// буфер изображения
};


// [Bitmap.cpp]

// создание bitmap и выделение памяти под буфер изображения
bitmap*		bitmap_create(int width, int height, int nbytes);

// удаление bitmap и высвобождение памяти
void		bitmap_release(bitmap *bmp);

// выравнивание цветности и яркости для bitmap с nbytes = 3
void		bitmap_equalizeGIMP(bitmap *bmp);

// меняет R и B каналы в bitmap с nbytes = 3
void		bitmap_rgb2bgr(bitmap *bmp);


// [Bitmap_io.cpp]

// загрузка изображения в структуру bitmap
bitmap*		bitmap_load(boost::filesystem::path & aPath, int planes = 0);

// сохранение изображения в формате BMP
bool 		bitmap_save(bitmap *bmp, boost::filesystem::path & aPath);

// сохранение изображения в формате JPG
bool		bitmap_saveJPEG(bitmap *bmp, boost::filesystem::path & aPath);

bitmap*		bitmap_copy(bitmap const* pic);


// [bitmap_io_scaled.cpp]

// TODO: переписать, добавить загрузку PNG
// загрузка уменьшенного изображения
bitmap*		bitmap_load_scaled(boost::filesystem::path & aPath, int planes, int scale);


// [image_io.cpp]

// Чтобы не дублировать код функций для загрузки bitmap, а воспользоваться my_image *load_image()
// и затем преобразовать my_image в bitmap с помощью convert_image_to_bitmap(), предусмотрены флаги:
//  for_bmp: если true, то step выравнивается как в bitmap, иначе step = width * nbytes;
//  inverse: поскольку данные в bitmap хранятся снизу вверх, для загрузки изображения с последующим 
// преобразованием в bitmap необходимо выставлять inverse = true;

// загрузка изображения в структуру my_image с возможным преобразованием цветов
// nbytes указывает, как загрузить изображение: 1 - grayscale, 3 - RGB, 4 - RGB с альфа-каналом
// если nbytes = 0, то изображение загружается с диска без преобразования цветов, как есть
my_image*	load_image(boost::filesystem::path & aPath, int nbytes = 0, bool for_bmp = 0, bool inverse = 0);

// создание my_image и выделение памяти под буфер изображения
my_image*	image_create(int width, int height, int nbytes, bool for_bmp = 0);

// удаление my_image и высвобождение памяти
void		image_release(my_image *image);

// преобразование my_image в bitmap, my_image удаляется;
// необходимо, чтобы step был выровнен как в bitmap, порядок строк д.б. снизу вверх
bitmap*		convert_image_to_bitmap(my_image *image);

// выравнивание цветности и яркости для my_image с nbytes = 3
void		image_equalizeGIMP(my_image *image);

// запись my_image на диск в формате JPEG, PNG или TIFF; необходимо, чтобы step = width * nbytes
bool		write_image(char *file_name, my_image *image,
						double *geotiffInfo = NULL, DWORD jpegQuality = 80, DWORD tiffCompression = COMPRESSION_NONE);
// то же самое
bool		write_image(char *file_name, int width, int height, int nbytes, UCHAR *image,
						double *geotiffInfo = NULL, DWORD jpegQuality = 80, DWORD tiffCompression = COMPRESSION_NONE);

// добавляет теги, необходимые для GeoTIFF
void		addGeoTiffTags();

// запись заголовка tiff
bool		write_tiff_header(TIFF *tif, int width, int height, int nbytes,
							  double *geo_info, DWORD m_dwExportQuality = 80, DWORD m_dwExportCompression = COMPRESSION_NONE);
