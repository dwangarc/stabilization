#include "image.h"

// PNG  8/24/32 bits
my_image *loadPNG(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse)
{
	if ( nbytes != 0 && nbytes != 1 && nbytes != 3 && nbytes != 4 ) return NULL;

	FILE *fp;
	fopen_s(&fp, aPath.file_string().data(), "rb");
	if (!fp) return NULL;

	// read header
	char header[8];
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8)) { // not a PNG file
		fclose(fp);
		return NULL;
	}

	// create and initialize the png_struct
	png_structp	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		return NULL;
	}

	// allocate the memory for image information
	png_infop	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		fclose(fp);
		return NULL;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		// free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		return NULL;
	}

	// set up the input control (use standard C streams)
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	// read information from the PNG file
	png_read_info(png_ptr, info_ptr);

	// bytes per pixel in stored image
	int samples;

	png_byte	color_type	= info_ptr->color_type;
	png_byte	bit_depth	= info_ptr->bit_depth;

	// interlacing temporary not supported
	if ( info_ptr->interlace_type != PNG_INTERLACE_NONE ) {
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		return NULL;
	}

	//strip 16 bit/color files down to 8 bits/color
	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	// expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	// expand paletted colors into true RGB triplets
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	switch (color_type) {
		case PNG_COLOR_TYPE_GRAY:
			samples = 1;
			break;
		case PNG_COLOR_TYPE_PALETTE:
			color_type = PNG_COLOR_TYPE_RGB;
			samples = 3;
			break;
		case PNG_COLOR_TYPE_RGB:
			samples = 3;
			break;
		case PNG_COLOR_TYPE_RGBA:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			samples = 4;
			break;
		default: // unknown color type
			png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
			fclose(fp);
			return NULL;
			break;
	}

	if (nbytes == 0) nbytes = samples;

	if (nbytes == 1) {
		// convert an RGB or RGBA image to grayscale or grayscale with alpha
		if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
			png_set_rgb_to_gray_fixed(png_ptr, 1, 21268, 71514);
		// strip alpha bytes
		if (color_type & PNG_COLOR_MASK_ALPHA)
			png_set_strip_alpha(png_ptr);
	}

	if (nbytes == 3) {
		// convert a grayscale image to RGB
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		// strip alpha bytes
		if (color_type & PNG_COLOR_MASK_ALPHA)
			png_set_strip_alpha(png_ptr);
	}

	if (nbytes == 4) {
		// convert a grayscale image to RGB
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		// add alpha bytes
		if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY)
			png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
	}

	// call to update the users info_ptr structure
	png_read_update_info(png_ptr, info_ptr);

	// allocate memory for one row
	png_byte *row_pointer = (png_byte*) malloc(info_ptr->rowbytes);

	// create new image
	my_image *image = image_create(info_ptr->width, info_ptr->height, nbytes, for_bmp);

	// can't allocate memory
	if ( !image || !row_pointer ) {
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		if (row_pointer) free(row_pointer);
		if (image) image_release(image);
		return NULL;
	}

	// length of row in new image
	int rowlen	= image->width * image->nbytes;
	int height	= image->height;
	int step	= image->step;
	UCHAR *dst	= image->data;
	if (inverse) {
		dst = image->data + (height - 1) * step;
		step = -step;
	}

	// TODO: add interlacing support
	// //turn on interlace handling
	//int number_of_passes = png_set_interlace_handling(png_ptr);
	// //for interlaced image (temporary, not so good !!! )
	//for (int pass = 0; pass < number_of_passes - 1; pass++)
	//	for (j = 0; j < height; j++)
	//		png_read_rows(png_ptr, &row_pointer, png_bytepp_NULL, 1);

	for (int j = 0; j < height; j++) {
		png_read_rows(png_ptr, &row_pointer, png_bytepp_NULL, 1);
		memcpy(dst, (UCHAR*)row_pointer, rowlen);
		dst += step;
	}

	if (row_pointer) free(row_pointer);

	// read rest of file, and get additional chunks in info_ptr
	png_read_end(png_ptr, info_ptr);

	// clean up after the read, and free any memory allocated
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

	fclose(fp);
	return image;
}


// Write a png file; size of image: (width * nbytes) * height
bool write_png(char *file_name, int width, int height, int nbytes, UCHAR *image)
{
	int	bit_depth	= 8;

	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;

	fp = fopen(file_name, "wb");
	if (fp == NULL) return false;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}

	// Allocate/initialize the image information data
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
		return false;
	}

	// Set error handling.  REQUIRED if you aren't supplying your own
	// error handling functions in the png_create_write_struct() call.
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);		// If we get here, we had a problem writing the file
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	// I/O initialization method: using standard C streams
	png_init_io(png_ptr, fp);

	// Set the image information here.  Width and height are up to 2^31,
	// bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	// the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
	// PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	// or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	// PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	// currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED

	if (nbytes == 4)
		png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width, (png_uint_32)height, bit_depth,
		PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	else if (nbytes == 3)
		png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width, (png_uint_32)height, bit_depth,
		PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	else if (nbytes == 1)
		png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width, (png_uint_32)height, bit_depth,
		PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);	// Write the file header information.

	png_bytep *row_pointers = NULL;
	row_pointers = (png_bytep*)malloc( height * sizeof(png_bytep) );

	if (height > PNG_UINT_32_MAX/png_sizeof(png_bytep))
		png_error (png_ptr, "Image is too tall to process in memory");

	for (int k = 0; k < height; k++)
		row_pointers[k] = (png_bytep)(image + k * width * nbytes);

	//for (int k = 0; k < height; k++)
	//	for (int i = 0; i < width; i++) {
	//		memset(row_pointers[k] + nbytes * i + 0, 55, 1);
	//		memset(row_pointers[k] + nbytes * i + 1, 55, 1);
	//	}

	// Write a few rows of image data
	// png_write_rows(png_ptr, row_pointers, height);

	// Write out the entire image data in one call
	png_write_image(png_ptr, row_pointers);

	// It is REQUIRED to call this to finish writing the rest of the file
	png_write_end(png_ptr, info_ptr);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	free(row_pointers); row_pointers = NULL;
	fclose(fp);

	return true;
}