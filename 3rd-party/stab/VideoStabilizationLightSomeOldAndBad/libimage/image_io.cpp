#include "image.h"


my_image *loadBMP (boost::filesystem::path & aPath, int nbytes, bool inverse);
my_image *loadJPEG(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse);
my_image *loadPNG (boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse);
my_image *loadTIFF(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse);


my_image *load_image(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse)
{
	if (aPath.extension().compare(".bmp") == 0) return loadBMP(  aPath, nbytes, inverse);
	if (aPath.extension().compare(".BMP") == 0) return loadBMP(  aPath, nbytes, inverse);
	if (aPath.extension().compare(".jpg") == 0) return loadJPEG( aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".JPG") == 0) return loadJPEG( aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".thb") == 0) return loadJPEG( aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".THB") == 0) return loadJPEG( aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".png") == 0) return loadPNG(  aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".PNG") == 0) return loadPNG(  aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".tif") == 0) return loadTIFF( aPath, nbytes, for_bmp, inverse);
	if (aPath.extension().compare(".TIF") == 0) return loadTIFF( aPath, nbytes, for_bmp, inverse);

	return NULL;
}

my_image *image_create(int width, int height, int nbytes, bool for_bmp)
{
	if ( nbytes != 1 && nbytes != 3 && nbytes != 4 ) return NULL;

	my_image *image = new my_image;
	image->width	= width;
	image->height	= height;
	image->nbytes	= nbytes;

	if (for_bmp)
		image->step		= ((width * nbytes + 3) / 4) * 4;
	else
		image->step		= width * nbytes;

	image->data		= (UCHAR*)malloc(image->step * height * sizeof(UCHAR));

	if ( !image->data ) {
		delete image;
		return NULL;
	}
	memset(image->data, 0, image->step * height * sizeof(UCHAR));

	return image;
}

void image_release(my_image *image)
{
	if (image) {
		if (image->data) {
			free(image->data);
			image->data = NULL;
		}
		delete image;
	}
}

// supported images with nbytes = 1,3,4;  step must be divisible by 4
bitmap* convert_image_to_bitmap(my_image *image)
{
	if ( (image->nbytes != 1) && (image->nbytes != 3) && (image->nbytes != 4) ) return NULL;

	int step = ( (image->width * image->nbytes + 3) / 4 ) * 4;
	if (step != image->step) return NULL;

	bitmap *bmp = new bitmap;
	if (!bmp) return NULL;
	memset(bmp, 0, sizeof(bitmap));

	bmp->width	= image->width;
	bmp->height	= image->height;
	bmp->nbytes	= image->nbytes;
	bmp->step	= image->step;
	bmp->data	= image->data;

	bmp->pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + (bmp->nbytes == 1 ? 256*4 : 0));
	if (!bmp->pbmi) delete bmp;

	BITMAPINFO *pbmi = bmp->pbmi;
	memset(pbmi, 0, sizeof(BITMAPINFOHEADER));
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= bmp->width;
	pbmi->bmiHeader.biHeight		= bmp->height;
	pbmi->bmiHeader.biBitCount		= bmp->nbytes * 8;
	pbmi->bmiHeader.biCompression	= 0;
	pbmi->bmiHeader.biClrUsed		= (bmp->nbytes == 1 ? 256 : 0);
	pbmi->bmiHeader.biClrImportant	= (bmp->nbytes == 1 ? 256 : 0);
	pbmi->bmiHeader.biPlanes		= 1;

	if (bmp->nbytes == 1) {
		for (int k = 0; k < 256; ++k) {
			pbmi->bmiColors[k].rgbRed		= k;
			pbmi->bmiColors[k].rgbGreen		= k;
			pbmi->bmiColors[k].rgbBlue		= k;
			pbmi->bmiColors[k].rgbReserved	= 0;
		}
	}

	image->data = NULL;
	return bmp;
}

static void computeHist(my_image *image, int *hist, int level)
{
	if ( image->nbytes != 3 ) return;

	int i, j;
	int width	= image->width;
	int height	= image->height;
	int step	= image->step;

	UCHAR *ptr;

	memset(hist, 0, sizeof(int) * 256);
	for (i = 0; i < height; ++i) {
		ptr = image->data + i * step + level;
		for (j = 0; j < width; ++j) {
			hist[*ptr]++;
			ptr += 3;
		}
	}
}

void image_equalizeGIMP(my_image *image)
{
	if ( image->nbytes != 3 ) return;

	int hist[256];
	int i, j, k, l, c;
	int range, count, low, high;

	int width	= image->width;
	int height	= image->height;
	int step	= image->step;
	int npixels	= width * height;

	double percent, nextPercent;
	double clamp = 0.006;

	for (l = 0; l < 3; ++l) {
		computeHist(image, hist, l);

		// find low level
		count = 0;
		low = 0;
		for (k = 0; k < 255; ++k) {
			count += hist[k];
			percent = (double)count / npixels;
			nextPercent = (double)(count + hist[k + 1]) / npixels;
			if (abs(percent - clamp) < abs(nextPercent - clamp)) {
				low = k + 1;
				break;
			}
		}

		// find high level
		count = 0;
		high = 255;
		for (k = 255; k > 0; --k) {
			count += hist[k];
			percent = (double)count / npixels;
			nextPercent = (double)(count + hist[k - 1]) / npixels;
			if (abs(percent - clamp) < abs(nextPercent - clamp)) {
				high = k - 1;
				break;
			}
		}

		range = high - low;
		if (range == 0)
			continue;

		// rescale colors
		UCHAR *ptr;
		for (i = 0; i < height; ++i) {
			ptr = image->data + i * step + l;
			for (j = 0; j < width; ++j) {
				c = 255 * (*ptr - low) / range;
				if (c < 0)
					c = 0;
				else
					if (c > 255)
						c = 255;
				*ptr = c;
				ptr += 3;
			}
		}
	}
}


bool write_jpeg(char *file_name, int width, int height, int nbytes, UCHAR *image, DWORD m_dwExportQuality);
bool write_tiff(char *file_name, int width, int height, int nbytes, UCHAR *image, double *geo_info, DWORD m_dwExportQuality, DWORD m_dwExportCompression);
bool write_png(char *file_name, int width, int height, int nbytes, UCHAR *image);


bool write_image(char *file_name, my_image *image, double *geotiffInfo, DWORD jpegQuality, DWORD tiffCompression)
{
	return write_image(file_name, image->width, image->height, image->nbytes, image->data,
		geotiffInfo, jpegQuality, tiffCompression);
}

bool write_image(char *file_name, int width, int height, int nbytes, UCHAR *image,
				 double *geotiffInfo, DWORD jpegQuality, DWORD tiffCompression)
{
	const char *p = strrchr(file_name, '.');
	if (!p) return false;

	if (!_stricmp(p, ".jpg"))
		return write_jpeg(file_name, width, height, nbytes, image, jpegQuality);

	if (!_stricmp(p, ".tif"))
		return write_tiff(file_name, width, height, nbytes, image, geotiffInfo, jpegQuality, tiffCompression);

	if (!_stricmp(p, ".png"))
		return write_png(file_name, width, height, nbytes, image);

	return false;
}