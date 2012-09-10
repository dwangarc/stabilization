#include "image.h"

bitmap *bitmap_loadBMP_scaled(boost::filesystem::path & aPath, int planes, int scale)
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

	if (!planes) planes = pbmi->bmiHeader.biBitCount / 8;
	bmp->width	= pbmi->bmiHeader.biWidth  / scale;
	bmp->height	= pbmi->bmiHeader.biHeight / scale;
	bmp->step	= ((bmp->width * planes + 3) / 4) * 4;
	bmp->nbytes	= planes;

	bmp->pbmi	= (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + (planes == 1 ? 256*4 : 0));
	memcpy(bmp->pbmi, pbmi, sizeof(BITMAPINFOHEADER));
	if (planes == 1 && pbmi->bmiHeader.biBitCount == 8)
		memcpy(bmp->pbmi->bmiColors, pbmi->bmiColors, 256*4);

	bmp->data	= (UCHAR*)malloc(bmp->step * bmp->height);
	memset(bmp->data, 0, bmp->step * bmp->height * sizeof(UCHAR));
	UCHAR *src	= NULL;
	UCHAR *dst	= NULL;

	int src_step= ((pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount/8 + 3)/4) * 4;
	int blen	= src_step * scale;
	UCHAR *buf	= (UCHAR*)malloc(blen);
	memset(buf, 0, blen * sizeof(UCHAR));

	int i, j, k, n, cR, cG, cB, gray;
	int norm = scale * scale;

	if (planes == 1 && pbmi->bmiHeader.biBitCount == 24) {
		for (j = 0; j < bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;		cG = 0;		cB = 0;
				for (k = 0; k < scale; k++) {
					src = buf + k * src_step + i * scale * 3;
					for (n = 0; n < scale; n++) {
						cB += *src++;
						cG += *src++;
						cR += *src++;
					}
				}
				//dst[0] = (int)( (0.212671 * cR + 0.715160 * cG + 0.072169 * cB) / norm );
				gray = 218 * cR + 732 * cG + 74 * cB;
				gray = gray >> 10;
				*dst++ = gray / norm;
			}
		}
	} else if (planes == 3 && pbmi->bmiHeader.biBitCount == 8) {
		for (j = 0; j < bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;
				for (k = 0; k < scale; k++) {
					src = buf + k * src_step + i * scale;
					for (n = 0; n < scale; n++) {
						cR += *src++;
					}
				}
				RGBQUAD *sp		= pbmi->bmiColors + cR / norm;
				RGBTRIPLE *t	= (RGBTRIPLE*)dst;
				t->rgbtRed		= sp->rgbRed;
				t->rgbtGreen	= sp->rgbGreen;
				t->rgbtBlue		= sp->rgbBlue;
				dst += 3;
			}
		}
	} else if (planes == 3 && pbmi->bmiHeader.biBitCount == 24) {
		for (j = 0; j < bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;		cG = 0;		cB = 0;
				for (k = 0; k < scale; k++) {
					src = buf + k * src_step + i * scale * 3;
					for (n = 0; n < scale; n++) {
						cB += *src++;
						cG += *src++;
						cR += *src++;
					}
				}
				*dst++ = cB / norm;
				*dst++ = cG / norm;
				*dst++ = cR / norm;
			}
		}
	} else if (planes == 1 && pbmi->bmiHeader.biBitCount == 8) {
		for (j = 0; j < bmp->height; j++) {
			fread(buf, blen, 1, f);
			dst = bmp->data + j * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;
				for (k = 0; k < scale; k++) {
					src = buf + k * src_step + i * scale;
					for (n = 0; n < scale; n++) {
						cR += src[2];
						src++;
					}
				}
				RGBQUAD *sp		= pbmi->bmiColors + cR / norm;
				RGBTRIPLE *t	= (RGBTRIPLE*)dst;
				t->rgbtRed		= sp->rgbRed;
				t->rgbtGreen	= sp->rgbGreen;
				t->rgbtBlue		= sp->rgbBlue;
				dst++;
			}
		}
	}

	fclose(f);

	if (buf)	free(buf);
	if (pbmi)	free(pbmi);

	if (planes == 1 && bmp->pbmi->bmiHeader.biBitCount == 24) {
		for (k=0; k < 256; k++) {
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

bitmap *bitmap_loadJPEG_scaled(boost::filesystem::path aPath, int planes, int scale)
{
	FILE *f;
	fopen_s(&f, aPath.file_string().data(), "rb");
	if (!f) return NULL;

	bitmap *bmp = new bitmap;

	struct jpeg_decompress_struct cinfo;
	JSAMPARRAY buffer;		// Output row buffer
	int row_stride;			// physical row width in output buffer

	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, f);
	jpeg_read_header(&cinfo, TRUE);
	switch (planes) {
		case 0:	cinfo.out_color_space = (cinfo.jpeg_color_space == JCS_GRAYSCALE ? JCS_GRAYSCALE : JCS_RGB); break;
		case 1:	cinfo.out_color_space = JCS_GRAYSCALE; break;
		case 3:	cinfo.out_color_space = JCS_RGB; break;
	}
	planes = (cinfo.out_color_space == JCS_RGB ? 3 : 1);

	jpeg_start_decompress(&cinfo);

	bmp->width		= cinfo.output_width  / scale;
	bmp->height		= cinfo.output_height / scale;
	bmp->step		= ((bmp->width * planes + 3) / 4) * 4;
	bmp->nbytes		= planes;
	bmp->data		= (UCHAR*)malloc(bmp->step * bmp->height);
	memset(bmp->data, 0, bmp->step * bmp->height * sizeof(UCHAR));

	row_stride		= cinfo.output_width * cinfo.output_components;
	buffer			= (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	UCHAR *tmp_buff	= (UCHAR*)malloc(row_stride * scale);
	int i, j, k, n, cR, cG, cB;
	int norm = scale * scale;
	UCHAR *src	= NULL;
	UCHAR *dst	= NULL;

	//	while (cinfo.output_scanline < cinfo.output_height) {
	//		jpeg_read_scanlines(&cinfo, buffer, 1);
	//		memcpy(bmp->data + (bmp->height-cinfo.output_scanline) * bmp->step, buffer[0], bmp->width * planes);
	//	}
	if (planes == 3) {
		for (j = 0; j < bmp->height; j++) {
			for (k = 0; k < scale; k++) {
				jpeg_read_scanlines(&cinfo, buffer, 1);
				memcpy(tmp_buff + k * row_stride, *buffer, row_stride);
			}
			dst = bmp->data + (bmp->height - 1 - j) * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;		cG = 0;		cB = 0;
				for (k = 0; k < scale; k++) {
					src = tmp_buff + k * row_stride + i * scale * 3;
					for (n = 0; n < scale; n++) {
						cB += *src++;
						cG += *src++;
						cR += *src++;
					}
				}
				*dst++ = cR / norm;
				*dst++ = cG / norm;
				*dst++ = cB / norm;
			}
		}
	} else if (planes == 1) {
		for (j = 0; j < bmp->height; j++) {
			for (k = 0; k < scale; k++) {
				jpeg_read_scanlines(&cinfo, buffer, 1);
				memcpy(tmp_buff + k * row_stride, *buffer, row_stride);
			}
			dst = bmp->data + (bmp->height - 1 - j) * bmp->step;
			for (i = 0; i < bmp->width; i++) {
				cR = 0;
				for (k = 0; k < scale; k++) {
					src = tmp_buff + k * row_stride + i * scale;
					for (n = 0; n < scale; n++) {
						cR += *src++;
					}
				}
				*dst++ = cR / norm;
			}
		}
	}

	for (k = j; k < (int)cinfo.output_height; k++)
		jpeg_read_scanlines(&cinfo, buffer, 1);	// reading remaining lines: (src_height - bmp->height * scale)

	if (tmp_buff)  free(tmp_buff);

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(f);

	bmp->pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + (planes == 1 ? 256*4 : 0));
	memset(bmp->pbmi, 0, sizeof(BITMAPINFOHEADER));
	bmp->pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmp->pbmi->bmiHeader.biWidth		= bmp->width;
	bmp->pbmi->bmiHeader.biHeight		= bmp->height;
	bmp->pbmi->bmiHeader.biBitCount		= (planes == 1 ? 8 : 24);
	bmp->pbmi->bmiHeader.biCompression	= 0;
	bmp->pbmi->bmiHeader.biClrUsed		= (planes == 1 ? 256 : 0);
	bmp->pbmi->bmiHeader.biClrImportant	= (planes == 1 ? 256 : 0);
	bmp->pbmi->bmiHeader.biPlanes		= 1;

	if (planes == 1) {
		for (k = 0; k < 256; k++) {
			bmp->pbmi->bmiColors[k].rgbRed		= k;
			bmp->pbmi->bmiColors[k].rgbGreen	= k;
			bmp->pbmi->bmiColors[k].rgbBlue		= k;
			bmp->pbmi->bmiColors[k].rgbReserved = 0;
		}
	}
	return bmp;
}

// auxiliary procedure for bitmap_loadTIFF_scaled() function
static void cpTileToImg_scaled(uint8* out, uint8* in, uint32 rows, uint32 cols, int outskew, int inskew, int samples, int scale)
{
	uint32	cR, cG, cB;
	int		i, k, n, m, ncols, nrows;
	int		norm = scale * scale;
	UCHAR	*src = in, *dst = out, *tmp = NULL;

	if (samples == 3) { // 3 -> 3
		ncols = cols / 3 / scale;
		nrows = rows / scale;
		for (i = 0; i < nrows; i++) {
			for (k = 0; k < ncols; k++) {
				cR = 0; cG = 0; cB = 0;
				for (n = 0; n < scale; n++) {
					//src = in + i * scale * inskew + n * inskew + k * scale * 3; // without tmp
					tmp = src;
					for (m = 0; m < scale; m++) {
						cB += *tmp++;
						cG += *tmp++;
						cR += *tmp++;
					}
					src += inskew;
				}
				src += (3 - inskew) * scale;
				*dst++ = cR / norm;
				*dst++ = cG / norm;
				*dst++ = cB / norm;
			}
			in += scale * inskew;
			src = in;
			out += outskew;
			dst = out;			
		}
	} else { // 1 -> 3
		ncols = cols / scale;
		nrows = rows / scale;
		for (i = 0; i < nrows; i++) {
			for (k = 0; k < ncols; k++) {
				cR = 0;
				for (n = 0; n < scale; n++) {
					//src = in + i * scale * inskew + n * inskew + k * scale; // without tmp
					tmp = src;
					for (m = 0; m < scale; m++) {
						cR += *tmp++;
					}
					src += inskew;
				}
				src += (1 - inskew) * scale;
				cR = cR / norm;
				*dst++ = cR;
				*dst++ = cR;
				*dst++ = cR;
			}
			in += scale * inskew;
			src = in;
			out += outskew;
			dst = out;
		}
	}
}

bitmap *bitmap_loadTIFF_scaled(boost::filesystem::path & aPath, int planes, int scale)
{
	static BOOL bFirstRun = true;
	if (bFirstRun) {
		TIFFSetWarningHandler(NULL);
		TIFFSetErrorHandler(NULL);
		bFirstRun	= false;
	}

	TIFF	*tif	= TIFFOpen(aPath.file_string().data(), "r");
	if (!tif)	return NULL;

	UCHAR*	dst		= NULL;
	UCHAR*	src		= NULL;
	UCHAR*	tmp		= NULL;
	bitmap*	bmp		= NULL;
	BOOL	success	= FALSE;

	uint32	imageWidth, imageHeight;
	uint32	 tileWidth,  tileHeight;
	uint32	rowsPerStrip;
	uint16	samples, config, colorType;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,		&imageWidth);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH,		&imageHeight);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL,	&samples);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG,		&config);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC,		&colorType);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP,		&rowsPerStrip);
	TIFFGetField(tif, TIFFTAG_TILEWIDTH,		&tileWidth);
	TIFFGetField(tif, TIFFTAG_TILELENGTH,		&tileHeight);

	if (config != PLANARCONFIG_CONTIG)	goto end; // PLANARCONFIG_SEPARATE doesn't supported
	if (colorType > PHOTOMETRIC_RGB)	goto end; // 0 - min value is white, 1 - min value is black, 2 - RGB color model
	if (samples != 1 && samples != 3)	goto end;
	if (planes == 0)	planes = samples;

	uint32	imagew		= TIFFScanlineSize(tif);
	tdata_t		buf = NULL;

	if (TIFFIsTiled(tif)) {
		if    (scale > 12) scale = 16;
		else if (scale > 5) scale = 8;
		else if (scale > 3) scale = 4;
		else if (scale > 1) scale = 2;
		else scale = 1;

		if (tileWidth > 10000 || tileHeight > 10000)	goto end;

		bmp		= bitmap_create(imageWidth / scale, imageHeight / scale, planes);
		if (!bmp)		goto end;

		buf	= _TIFFmalloc(TIFFTileSize(tif));
		if (!buf)	goto end;

		uint32 imagew = TIFFScanlineSize(tif);
		uint32 tilew  = TIFFTileRowSize(tif);

		int oskew = - bmp->step;
		dst = bmp->data + bmp->step * (bmp->height - 1);
		uint32  row;

		if (planes == 3 && samples == 3) {
			for (row = 0; row < imageHeight; row += tileHeight) {
				uint32 nrow = (row + tileHeight > imageHeight) ? imageHeight - row : tileHeight;
				uint32 col, colb = 0;
				for (col = 0; col < imageWidth; col += tileWidth) {
					if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 ) goto end;
					if (colb + tilew > imagew) {
						cpTileToImg_scaled(dst + colb / scale, (uint8*)buf, nrow, imagew - colb, oskew, tilew, samples, scale);
					} else {
						cpTileToImg_scaled(dst + colb / scale, (uint8*)buf, nrow, tilew, oskew, tilew, samples, scale);
					}
					colb += tilew;
				}
				dst -= bmp->step * nrow / scale;
			}
		} else if (planes == 3 && samples == 1) {
			for (row = 0; row < imageHeight; row += tileHeight) {
				uint32 nrow = (row + tileHeight > imageHeight) ? imageHeight - row : tileHeight;
				uint32 col, colb = 0;
				for (col = 0; col < imageWidth; col += tileWidth) {
					if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 ) goto end;
					if (colb + tilew > imagew) {
						cpTileToImg_scaled(dst + 3 * colb / scale, (uint8*)buf, nrow, imagew - colb, oskew, tilew, samples, scale);
					} else {
						cpTileToImg_scaled(dst + 3 * colb / scale, (uint8*)buf, nrow, tilew, oskew, tilew, samples, scale);
					}
					colb += tilew;
				}
				dst -= bmp->step * nrow / scale;
			}
		}

	} else { // strip or scanline
		if (rowsPerStrip > 1000)	goto end;
		bmp		= bitmap_create(imageWidth / scale, imageHeight / scale, planes);
		if (!bmp)		goto end;

		buf		= _TIFFmalloc(TIFFStripSize(tif));

		uint32	row, nrow, k, strip = 0;
		int		i, j, num = 0, indent;
		int		norm = scale * scale;
		uint32	cR, cG, cB;
		dst = bmp->data + (bmp->height - 1) * bmp->step;
		imageHeight = imageHeight / scale * scale;

		if (planes == samples) {
			for (row = 0; row < imageHeight; row += rowsPerStrip) {
				nrow = (row + rowsPerStrip > imageHeight) ? imageHeight - row : rowsPerStrip;
				TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
				for (k = 0; k < nrow; k++) {
					src = (UCHAR*)buf + k * imagew;
					indent = num / scale * bmp->step;
					tmp = dst - indent;
					for (i = 0; i < bmp->width; i++) {
						cR = 0;		cG = 0;		cB = 0;
						for (j = 0; j < scale; j++) {
							cR += *src++;
							cG += *src++;
							cB += *src++;
						}
						*tmp++ += cB / norm;
						*tmp++ += cG / norm;
						*tmp++ += cR / norm;
					}
					num ++;
				}
				strip++;
			}
		} else {
			for (row = 0; row < imageHeight; row += rowsPerStrip) {
				nrow = (row + rowsPerStrip > imageHeight) ? imageHeight - row : rowsPerStrip;
				TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
				for (k = 0; k < nrow; k++) {
					src = (UCHAR*)buf + k * imagew;
					indent = num / scale * bmp->step;
					tmp = dst - indent;
					for (i = 0; i < bmp->width; i++) {
						cR = 0;
						for (j = 0; j < scale; j++) {
							cR += *src++;
						}
						*tmp++ += cR / norm;
						*tmp++ += cR / norm;
						*tmp++ += cR / norm;
					}
					num ++;
				}
				strip++;
			}
		}
	}

	success = TRUE;
	_TIFFfree(buf);

end:
	if (tif)	TIFFClose(tif);
	if (!success) {
		if (bmp) bitmap_release(bmp);
		bmp = NULL;
	}
	return bmp;
}

bitmap *bitmap_loadPNG(boost::filesystem::path & aPath, int planes); // declaration

bitmap *bitmap_load_scaled(boost::filesystem::path & aPath, int planes, int scale)
{
	if (aPath.extension().compare(".bmp") == 0) return bitmap_loadBMP_scaled(aPath, planes, scale);
	if (aPath.extension().compare(".BMP") == 0) return bitmap_loadBMP_scaled(aPath, planes, scale);
	if (aPath.extension().compare(".jpg") == 0) return bitmap_loadJPEG_scaled(aPath, planes, scale);
	if (aPath.extension().compare(".JPG") == 0) return bitmap_loadJPEG_scaled(aPath, planes, scale);
	if (aPath.extension().compare(".tif") == 0) return bitmap_loadTIFF_scaled(aPath, planes, scale);
	if (aPath.extension().compare(".TIF") == 0) return bitmap_loadTIFF_scaled(aPath, planes, scale);
	//if (aPath.extension().compare(".png") == 0) return bitmap_loadPNG(aPath, planes);
	//if (aPath.extension().compare(".png") == 0) return bitmap_loadPNG(aPath, planes);

	//if (!_stricmp(p, ".bmp")) return bitmap_loadBMP_scaled( path, planes, scale);
	//if (!_stricmp(p, ".jpg")) return bitmap_loadJPEG_scaled(path, planes, scale);
	//if (!_stricmp(p, ".tif")) return bitmap_loadTIFF_scaled(path, planes, scale);
	//if (!_stricmp(p, ".png")) return bitmap_loadPNG(path);
	return NULL;
}