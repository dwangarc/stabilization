#include "image.h"

#define INT_MAX		2147483647
#define MAX_SIZE	65535

// supported only 1, 3 or 4 bytes / pixel
bitmap *bitmap_create(int width, int height, int nbytes)
{
	if (nbytes != 1 && nbytes != 3 && nbytes != 4)	return NULL;
	if (width <= 0 || height <= 0)					return NULL;
	if (width > MAX_SIZE || height > MAX_SIZE)		return NULL;

	int step = ( (width * nbytes + 3) & 0x7FFFFFFC);
	//int step = ( (width * nbytes + 3) / 4 ) * 4;

	// check for scanlength overflow
	if (width > (INT_MAX & 0x7FFFFFFC) / nbytes)	return NULL;
	// check for buffer size overflow
	if (height > INT_MAX / step)					return NULL;
	
	bitmap *bmp = new bitmap;
	if (!bmp) return NULL;
	memset(bmp, 0, sizeof(bitmap));

	bmp->width		= width;
	bmp->height		= height;
	bmp->step		= step;
	bmp->nbytes		= nbytes;

	bmp->data = (UCHAR*)malloc(step * height);
	bmp->pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + (nbytes == 1 ? 256*4 : 0));

	if (!bmp->data || !bmp->pbmi) {
		if (bmp->data) free(bmp->data);
		if (bmp->pbmi) free(bmp->pbmi);
		delete bmp;
		return NULL;
	}
	memset(bmp->data, 0, step * height * sizeof(UCHAR));

	BITMAPINFO *pbmi = bmp->pbmi;
	memset(pbmi, 0, sizeof(BITMAPINFOHEADER));
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= width;
	pbmi->bmiHeader.biHeight		= height;
	pbmi->bmiHeader.biBitCount		= nbytes * 8;
	pbmi->bmiHeader.biCompression	= 0;
	pbmi->bmiHeader.biClrUsed		= (nbytes == 1 ? 256 : 0);
	pbmi->bmiHeader.biClrImportant	= (nbytes == 1 ? 256 : 0);
	pbmi->bmiHeader.biPlanes		= 1;
	if (nbytes == 1) {
		for (int k = 0; k < 256; ++k) {
			pbmi->bmiColors[k].rgbRed		= k;
			pbmi->bmiColors[k].rgbGreen		= k;
			pbmi->bmiColors[k].rgbBlue		= k;
			pbmi->bmiColors[k].rgbReserved	= 0;
		}
	}

	return bmp;
}

void bitmap_release(bitmap *bmp)
{
	if (bmp->data) free(bmp->data);
	if (bmp->pbmi) free(bmp->pbmi);
	delete bmp;
}

static void computeHist(bitmap *bmp, int *hist, int level)
{
	int i, j;
	int width	= bmp->width;
	int height	= bmp->height;
	int step	= bmp->step;

	memset(hist, 0, sizeof(int) * 256);
	for (i = 0; i < height; ++i) {
		UCHAR *ptr = bmp->data + i * step + level;
		for (j = 0; j < width; ++j) {
			hist[*ptr]++;
			ptr += 3;
		}
	}
}

void bitmap_equalizeGIMP(bitmap *bmp)
{
	if (bmp->nbytes != 3) return;

	int hist[256];
	double clamp = 0.006;

	int i, j, k, l;
	int width	= bmp->width;
	int height	= bmp->height;
	int step	= bmp->step;
	int npixels	= width * height;

	for (l = 0; l < 3; ++l) {
		int low = 0, high = 255;

		computeHist(bmp, hist, l);

		// find high and low levels
		int count = 0;
		for (k = 0; k < 255; ++k) {
			count += hist[k];
			double percent = (double)count / npixels;
			double nextPercent = (double)(count + hist[k + 1]) / npixels;
			if (abs(percent - clamp) < abs(nextPercent - clamp)) {
				low = k + 1;
				break;
			}
		}

		count = 0;
		for (k = 255; k > 0; --k) {
			count += hist[k];
			double percent = (double)count / npixels;
			double nextPercent = (double)(count + hist[k - 1]) / npixels;
			if (abs(percent - clamp) < abs(nextPercent - clamp)) {
				high = k - 1;
				break;
			}
		}

		int range = high - low;
		if (range == 0)
			continue;

		// rescale colors
		for (i = 0; i < height; ++i) {
			UCHAR *ptr = bmp->data + i * step + l;
			for (j = 0; j < width; ++j) {
				int c = 255 * (*ptr - low) / range;
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

void bitmap_rgb2bgr(bitmap *bmp)
{
	if (bmp->nbytes != 3) return;

	int width	= bmp->width;
	int height	= bmp->height;
	int step	= bmp->step;
	UCHAR *data = bmp->data;
	int i, j, k, r, b;

	for (j = 0; j < height; ++j) {
		k = 0;
		for (i = 0; i < width; ++i) {
			r = data[k];
			b = data[k + 2];
			data[k]     = b;
			data[k + 2] = r;
			k += 3;
		}
		data += step;
	}
}

bitmap*	bitmap_copy(bitmap const* pic)
{
	bitmap* res = bitmap_create(pic->width, pic->height, pic->nbytes);
	if (res->step != pic->step) return 0;
	for (int i = 0; i < pic->step * pic->height; ++i)
	{
		res->data[i] = pic->data[i];
	}
	return res;
}
