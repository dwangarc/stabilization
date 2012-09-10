#include "image.h"

bitmap *bitmap_loadBMP(boost::filesystem::path & aPath, int planes)
{
	FILE *f;
	fopen_s(&f, aPath.file_string().data(), "rb");
	if (!f) return NULL;

	bitmap *bmp = new bitmap;

	BITMAPINFO *pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256*4);
	fseek(f, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(&pbmi->bmiHeader, 1, sizeof(BITMAPINFOHEADER), f);
	if (pbmi->bmiHeader.biBitCount == 8)
		fread(pbmi->bmiColors, 256 * 4, 1, f);

	bmp->width	= pbmi->bmiHeader.biWidth;
	bmp->height	= pbmi->bmiHeader.biHeight;
	if (!planes) planes = pbmi->bmiHeader.biBitCount / 8;
	bmp->step	= ((bmp->width * planes + 3) / 4) * 4;
	bmp->nbytes	= planes;

	bmp->pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + (planes == 1 ? 256*4 : 0));
	memcpy(bmp->pbmi, pbmi, sizeof(BITMAPINFOHEADER));
	if (planes == 1 && pbmi->bmiHeader.biBitCount == 8)
		memcpy(bmp->pbmi->bmiColors, pbmi->bmiColors, 256*4);

	int blen = ((bmp->width*pbmi->bmiHeader.biBitCount/8 + 3)/4)*4;
	UCHAR *buf = NULL;
	UCHAR *dst;

	bmp->data = (UCHAR*)malloc(bmp->step * bmp->height);
	memset(bmp->data, 0, bmp->step * bmp->height * sizeof(UCHAR));

	if (planes == 1 && pbmi->bmiHeader.biBitCount == 24) {
		int gray;
		buf = (UCHAR*)malloc(blen);
		for (int j=0; j<bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j*bmp->step;
			for (int k=0; k<bmp->width; k++) {
				RGBTRIPLE *sp = (RGBTRIPLE*)(buf + k * 3);
				//dst[k] = (int)(0.212671*sp->rgbtRed + 0.715160*sp->rgbtGreen + 0.072169*sp->rgbtBlue);
				gray = 218 * sp->rgbtRed + 732 * sp->rgbtGreen + 74 * sp->rgbtBlue;
				dst[k] = gray >> 10;
			}
		}
	} else if (planes == 3 && pbmi->bmiHeader.biBitCount == 8) {
		buf = (UCHAR*)malloc(blen);
		for (int j=0; j<bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j*bmp->step;
			for (int k=0; k<bmp->width; k++) {
				RGBQUAD *sp		= pbmi->bmiColors + buf[k];
				RGBTRIPLE *t	= (RGBTRIPLE*)(dst + k * 3);
				t->rgbtRed		= sp->rgbRed;
				t->rgbtGreen	= sp->rgbGreen;
				t->rgbtBlue		= sp->rgbBlue;
			}
		}
	} else
		fread(bmp->data, bmp->height, bmp->step, f);

	fclose(f);

	if (buf)	free(buf);
	if (pbmi)	free(pbmi);

	if (planes == 1 && bmp->pbmi->bmiHeader.biBitCount == 24) {
		for (int k=0; k<256; k++) {
			bmp->pbmi->bmiColors[k].rgbRed		= k;
			bmp->pbmi->bmiColors[k].rgbGreen	= k;
			bmp->pbmi->bmiColors[k].rgbBlue		= k;
			bmp->pbmi->bmiColors[k].rgbReserved	= 0;
		}
		bmp->pbmi->bmiHeader.biBitCount = 8;
		bmp->pbmi->bmiHeader.biClrUsed = 256;
		bmp->pbmi->bmiHeader.biClrImportant = 256;
	} else if (planes == 3 && bmp->pbmi->bmiHeader.biBitCount == 8) {
		bmp->pbmi->bmiHeader.biBitCount = 24;
		bmp->pbmi->bmiHeader.biClrUsed = 0;
		bmp->pbmi->bmiHeader.biClrImportant = 0;
	}

	return bmp;
}

bitmap *bitmap_load(boost::filesystem::path & aPath, int planes)
{
	// bmp
	if ( aPath.extension().compare(".bmp") == 0 || aPath.extension().compare(".BMP") == 0 )
		return bitmap_loadBMP(aPath, planes);

	// other formats
	my_image *image = load_image(aPath, planes, true, true);
	if (!image) return NULL;
	
	bitmap *bmp = convert_image_to_bitmap(image);
	if (!bmp) return NULL;

	image_release(image);
	if (bmp->nbytes == 3) bitmap_rgb2bgr(bmp);

	return bmp;
}

bool bitmap_save(bitmap *bmp, boost::filesystem::path & aPath)
{
	if (bmp->nbytes != 1 && bmp->nbytes != 3 && bmp->nbytes != 4)
		return 0;

	FILE *f = fopen(aPath.file_string().data(), "wb");
	if (!f)	return 0;

	//int planes = bmp->pbmi->bmiHeader.biBitCount / 8;

	BITMAPFILEHEADER bmfh;
	memset(&bmfh, 0, sizeof(bmfh));
	bmfh.bfType = ('M' << 8) + 'B';
	bmfh.bfSize = sizeof(bmfh) + sizeof(BITMAPINFOHEADER) + (bmp->nbytes == 1 ? 256*4 : 0) + bmp->height * bmp->step;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(BITMAPINFOHEADER) + (bmp->nbytes == 1 ? 256*4 : 0);

	fwrite(&bmfh, 1, sizeof(bmfh), f);
	fwrite(bmp->pbmi, 1, sizeof(BITMAPINFOHEADER) + (bmp->nbytes == 1 ? 256*4 : 0), f);
	fwrite(bmp->data, bmp->height, bmp->step, f);
	fclose(f);

	return 1;
}

bool bitmap_saveJPEG(bitmap *bmp, boost::filesystem::path & aPath)
{
	if (bmp->nbytes != 1 && bmp->nbytes != 3)
		return 0;

	FILE *f = fopen(aPath.file_string().data(), "wb");
	if (!f) return false;

	//BOOL bColor = (bmp->pbmi->bmiHeader.biBitCount == 8 ? FALSE : TRUE);
	BOOL bColor = (bmp->nbytes == 3 ? TRUE : FALSE);

	struct jpeg_compress_struct cinfo;
	JSAMPROW row_ptr[1];

	UCHAR *line = NULL;
	if (bColor) {
		line = (UCHAR*)malloc(bmp->width * (bColor ? 3 : 1));
		row_ptr[0] = line;
	}

	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
  	jpeg_stdio_dest(&cinfo, f);
	cinfo.image_width		= bmp->width;
	cinfo.image_height		= bmp->height;
	cinfo.input_components	= (bColor ? 3 : 1);
	cinfo.in_color_space	= (bColor ? JCS_RGB : JCS_GRAYSCALE);
    
	jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

	int x, y;
	if (bColor) {
		UCHAR *src = bmp->data + (bmp->height - 1) * bmp->step;
		for (y = 0; y < bmp->height; y++) {
			for (x = 0; x < 3*bmp->width; x+=3) {
				line[x]   = src[x+2];
				line[x+1] = src[x+1];
				line[x+2] = src[x];
			}
			jpeg_write_scanlines (&cinfo, row_ptr, 1);
			src -= bmp->step;
		}
	} else {
		UCHAR *src = bmp->data + (bmp->height - 1) * bmp->step;
		for (y = 0; y < bmp->height; y++) {
			row_ptr[0] = src;
			jpeg_write_scanlines (&cinfo, row_ptr, 1);
			src -= bmp->step;
		}
	}

    jpeg_finish_compress(&cinfo);
    fclose(f);

	if (line) free(line);
  
    jpeg_destroy_compress(&cinfo);

	return true;
}