#include "image.h"

// JPG  8/24 bits
my_image *loadJPEG(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse)
{
	if ( nbytes != 0 && nbytes != 1 && nbytes != 3 ) return NULL;

	FILE *fp;
	fopen_s(&fp, aPath.file_string().data(), "rb");
	if (!fp) return NULL;

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);

	// определяемся с цветами
	int samples = (cinfo.jpeg_color_space == JCS_GRAYSCALE ? 1 : 3);
	if (nbytes == 0) nbytes = samples;
	cinfo.out_color_space = (nbytes == 1 ? JCS_GRAYSCALE : JCS_RGB);

	// инициализируем декомпрессию
	jpeg_start_decompress(&cinfo);

	// длина строки в выходном буфере
	int row_stride	= cinfo.output_width * cinfo.output_components;

	// выделяем память под буфер
	JSAMPARRAY	buffer	= (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	// создаём новое изображение
	my_image *image = image_create(cinfo.output_width, cinfo.output_height, nbytes, for_bmp);

	if (!image) {
		jpeg_destroy_decompress(&cinfo);
		fclose(fp);
		return NULL;
	}

	// длина строки в новом изображении
	int rowlen	= image->width * image->nbytes;
	int step	= image->step;
	UCHAR *dst	= image->data;

	if (inverse) {
		dst = image->data + (image->height - 1) * step;
		step = -step;
	}

	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(dst, *buffer, rowlen);
		dst += step;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	return image;
}

// Write a jpeg file; size of image: (width * nbytes) * height
bool write_jpeg(char *file_name, int width, int height, int nbytes, UCHAR *image, DWORD m_dwExportQuality)
{
	if ( nbytes != 1 && nbytes != 3 ) return false;

	FILE		*jpeg	= NULL;
	struct jpeg_compress_struct	cinfo;
	struct jpeg_error_mgr		jerr;
	JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s]
	int k;

	//запись jpg - файла
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg = fopen(file_name, "wb");
	if (!jpeg)	return false;

	jpeg_stdio_dest(&cinfo, jpeg);
	cinfo.image_width		= width;
	cinfo.image_height		= height;

	switch (nbytes) {
		case 1:
			cinfo.input_components	= 1;
			cinfo.in_color_space	= JCS_GRAYSCALE;
			break;
		case 3:
			cinfo.input_components	= 3;
			cinfo.in_color_space	= JCS_RGB;
			break;
	}

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, m_dwExportQuality, FALSE);
	jpeg_start_compress(&cinfo, TRUE);

	int rowlen = width * nbytes;

	for (k = 0; k < height; k++) {
		row_pointer[0] = image + k * rowlen;
		if (jpeg_write_scanlines(&cinfo, row_pointer, 1) != 1) {
			jpeg_abort_compress(&cinfo);
			if (jpeg)	fclose(jpeg);
			jpeg_destroy_compress(&cinfo);
			return false;
		}
	}
	jpeg_finish_compress(&cinfo);
	if (jpeg)	fclose(jpeg);
	jpeg_destroy_compress(&cinfo);

	return true;
}