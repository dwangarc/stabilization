#include "image.h"
#include "geotiff_tags.h"

// auxiliary procedure for loadTIFF() function
static void cpTileToImg(uint8* out, uint8* in, uint32 rows, uint32 cols,
						int outskew, int inskew, int samples, int nbytes)
{
	if ( samples == nbytes ) { // 3 -> 3 or 1 -> 1
		while (rows-- > 0) {
			memcpy(out, in, cols);
			out += outskew + cols;
			in += inskew + cols;
		}
	} else if (samples == 1 && nbytes == 3) { // 1 -> 3
		while (rows-- > 0) {
			uint32 j = cols;
			while (j-- > 0) {
				*out++ = *in;
				*out++ = *in;
				*out++ = *in;
				in++;
			}
			out += outskew;
			in += inskew;
		}
	} else if (samples == 3 && nbytes == 1) { // 3 -> 1
		cols /= 3;
		uint32 cR, cG, cB, gray;
		while (rows-- > 0) {
			uint32 j = cols;
			while (j-- > 0) {
				cR = *in++;
				cG = *in++;
				cB = *in++;
				gray = 218 * cR + 732 * cG + 74 * cB;
				gray = gray >> 10;
				*out++ = gray;
			}
			out += outskew;
			in += inskew;
		}
	}
}

// TIFF  8/24 bits
my_image *loadTIFF(boost::filesystem::path & aPath, int nbytes, bool for_bmp, bool inverse)
{
	if ( nbytes != 0 && nbytes != 1 && nbytes != 3 ) return NULL;

	static bool bFirstRun = true;
	if (bFirstRun) {
		TIFFSetWarningHandler(NULL);
		TIFFSetErrorHandler(NULL);
		bFirstRun = false;
	}

	TIFF *tif = TIFFOpen(aPath.file_string().data(), "r");
	if (!tif) return NULL;

	//int i, ndir = 2;	// skip first ndir pages in multipage tiff
	//for (i = 0; i < ndir; i++)	TIFFReadDirectory(tif);

	UCHAR*	dst		= NULL;
	UCHAR*	src		= NULL;
	UCHAR*	tmp		= NULL;

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
	//TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE,	JPEGCOLORMODE_RGB);

	//uint16 pn = (uint16) -1, ptotal = (uint16) -1;
	//TIFFGetField(tif, TIFFTAG_PAGENUMBER, &pn, &ptotal);

	// PLANARCONFIG_SEPARATE not supported
	if (config != PLANARCONFIG_CONTIG) {
		TIFFClose(tif);
		return NULL;
	}
	// 0 - min value is white, 1 - min value is black, 2 - RGB color model
	if (colorType > PHOTOMETRIC_RGB) {
		TIFFClose(tif);
		return NULL;
	}

	if (nbytes == 0) nbytes = samples;

	if ( samples != 1 && samples != 3 ) {
		TIFFClose(tif);
		return NULL;
	}

	uint32	imagew	= TIFFScanlineSize(tif);
	tdata_t		buf = NULL;

	// create new image
	my_image *image = image_create(imageWidth, imageHeight, nbytes, for_bmp);

	if (!image) {
		TIFFClose(tif);
		return NULL;
	}

	int rowlen	= image->width * image->nbytes;
	int step	= image->step;


	if (TIFFIsTiled(tif)) {	// tiles

		if (tileWidth > 10000 || tileHeight > 10000) {
			TIFFClose(tif);
			image_release(image);
			return NULL;
		}

		buf	= _TIFFmalloc(TIFFTileSize(tif));
		if (!buf) {
			TIFFClose(tif);
			image_release(image);
			return NULL;
		}

		uint32 imagew = TIFFScanlineSize(tif);
		uint32 tilew  = TIFFTileRowSize(tif);

		int oskew = rowlen - tileWidth * nbytes;
		dst = image->data;

		if (inverse) { // vertical flip
			oskew = - (int)(rowlen + tileWidth * nbytes);
			dst = image->data + step * (image->height - 1);
			step = -step;
		}

		uint32  row;

		if ( samples == nbytes ) { // 3 -> 3 or 1 -> 1
			for (row = 0; row < imageHeight; row += tileHeight) {
				uint32 nrow = (row + tileHeight > imageHeight) ? imageHeight - row : tileHeight;
				uint32 col, colb = 0;
				for (col = 0; col < imageWidth; col += tileWidth) {
					if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 ) {
						// error
						TIFFClose(tif);
						_TIFFfree(buf);
						image_release(image);
						return NULL;
					}
					if (colb + tilew > imagew) {
						uint32 width = imagew - colb;
						uint32 iskew = tilew - width;
						cpTileToImg(dst + colb, (uint8*)buf, nrow, width, oskew + iskew, iskew, samples, nbytes);
					} else {
						cpTileToImg(dst + colb, (uint8*)buf, nrow, tilew, oskew, 0, samples, nbytes);
					}
					colb += tilew;
				}
				dst += step * nrow;
			}
		} else if (samples == 1 && nbytes == 3) {
			for (row = 0; row < imageHeight; row += tileHeight) {
				uint32 nrow = (row + tileHeight > imageHeight) ? imageHeight - row : tileHeight;
				uint32 col, colb = 0;
				for (col = 0; col < imageWidth; col += tileWidth) {
					if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 ) {
						// error
						TIFFClose(tif);
						_TIFFfree(buf);
						image_release(image);
						return NULL;
					}
					if (colb + tilew > imagew) {
						uint32 width = imagew - colb;
						uint32 iskew = tilew - width;
						cpTileToImg(dst + 3 * colb, (uint8*)buf, nrow, width, oskew + 3 * iskew, iskew, samples, nbytes);
					} else {
						cpTileToImg(dst + 3 * colb, (uint8*)buf, nrow, tilew, oskew, 0, samples, nbytes);
					}
					colb += tilew;
				}
				dst += step * nrow;
			}
		} else if (samples == 3 && nbytes == 1) {
			for (row = 0; row < imageHeight; row += tileHeight) {
				uint32 nrow = (row + tileHeight > imageHeight) ? imageHeight - row : tileHeight;
				uint32 col, colb = 0, colb_3 = 0; // Note: colb_3 = colb / 3
				for (col = 0; col < imageWidth; col += tileWidth) {
					if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 ) {
						// error
						TIFFClose(tif);
						_TIFFfree(buf);
						image_release(image);
						return NULL;
					}
					if (colb + tilew > imagew) {
						uint32 width = imagew - colb;
						uint32 iskew = tilew - width;
						cpTileToImg(dst + colb_3, (uint8*)buf, nrow, width, oskew + iskew / 3, iskew, samples, nbytes);
					} else {
						cpTileToImg(dst + colb_3, (uint8*)buf, nrow, tilew, oskew, 0, samples, nbytes);
					}
					colb += tilew;
					colb_3 += tileWidth;
				}
				dst += step * nrow;
			}
		}

	} else { // strip or scanline

		if (rowsPerStrip > 2000) {
			TIFFClose(tif);
			image_release(image);
			return NULL;
		}

		buf = _TIFFmalloc(TIFFStripSize(tif));
		if (!buf) {
			TIFFClose(tif);
			image_release(image);
			return NULL;
		}

		uint32	row, nrow, k, strip = 0;
		int	i;
		dst = image->data;
		if (inverse) { // vertical flip
			dst = image->data + (image->height - 1) * step;
			step = -step;
		}

		if ( (samples == 3 && nbytes == 3) || (samples == 1 && nbytes == 1) ) {
			for (row = 0; row < imageHeight; row += rowsPerStrip) {
				nrow = (row + rowsPerStrip > imageHeight) ? imageHeight - row : rowsPerStrip;

				if (TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1) < 0) {
					// error
					TIFFClose(tif);
					_TIFFfree(buf);
					image_release(image);
					return NULL;
				}

				memcpy(dst, buf, imagew * nrow * sizeof(UCHAR));
				dst += step * nrow;
				strip++;
			}
		} else if (samples == 1 && nbytes == 3) {
			for (row = 0; row < imageHeight; row += rowsPerStrip) {
				nrow = (row + rowsPerStrip > imageHeight) ? imageHeight - row : rowsPerStrip;

				if (TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1) < 0) {
					// error
					TIFFClose(tif);
					_TIFFfree(buf);
					image_release(image);
					return NULL;
				}

				for (k = 0; k < nrow; k++) {
					src = (UCHAR*)buf + k * imagew;
					tmp = dst;
					for (i = 0; i < image->width; i++) {
						*tmp++ = *src;
						*tmp++ = *src;
						*tmp++ = *src;
						src++;
					}
					dst += step;
				}
				strip++;
			}
		} else if (samples == 3 && nbytes == 1) {
			uint32 cR, cG, cB, gray;
			for (row = 0; row < imageHeight; row += rowsPerStrip) {
				nrow = (row + rowsPerStrip > imageHeight) ? imageHeight - row : rowsPerStrip;

				if (TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1) < 0) {
					// error
					TIFFClose(tif);
					_TIFFfree(buf);
					image_release(image);
					return NULL;
				}

				for (k = 0; k < nrow; k++) {
					src = (UCHAR*)buf + k * imagew;
					tmp = dst;
					for (i = 0; i < image->width; i++) {
						cR = *src++;
						cG = *src++;
						cB = *src++;
						gray = 218 * cR + 732 * cG + 74 * cB;
						gray = gray >> 10;
						*tmp++ = gray;
					}
					dst += step;
				}
				strip++;
			}
		}
	}

	_TIFFfree(buf);
	if (tif) TIFFClose(tif);

	return image;
}

// Write a tiff file; size of image: (width * nbytes) * height
bool write_tiff(char *file_name, int width, int height, int nbytes, UCHAR *image,
				double *geo_info, DWORD m_dwExportQuality, DWORD m_dwExportCompression)
{
	TIFF		*tif	= NULL;
	tif = TIFFOpen(file_name, "w");
	if (!tif)	return false;

	int NPAGES = 1;
	for (int page = 0; page < NPAGES; page++)
	{
		if (page == 0)
			write_tiff_header(tif, width, height, nbytes, geo_info, m_dwExportQuality, m_dwExportCompression);
		else
			write_tiff_header(tif, width, height, nbytes, 0, m_dwExportQuality, m_dwExportCompression);

		// We are writing single page of the multipage file
		if (NPAGES > 1)	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
		// Set the page number
		if (NPAGES > 1)	TIFFSetField(tif, TIFFTAG_PAGENUMBER, page, NPAGES);

		for (int k = 0; k < height; k++) {
			if ( TIFFWriteScanline(tif, image + k * width * nbytes, k, 0) != 1 ) {
				if (tif)	TIFFFlushData (tif);
				if (tif)	TIFFClose(tif);
				return false;
			}
		}
		// Write single page
		if (NPAGES > 1)	TIFFWriteDirectory(tif);
	}

	if (tif)	TIFFFlushData (tif);
	if (tif)	TIFFClose(tif);

	return true;
}


bool write_tiff_header(TIFF *tif, int width, int height, int nbytes,
					   double *geo_info, DWORD m_dwExportQuality, DWORD m_dwExportCompression)
{
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE,		0);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,		width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH,		height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,	8);
	TIFFSetField(tif, TIFFTAG_ORIENTATION,		ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_COMPRESSION,		m_dwExportCompression);
	if (m_dwExportCompression == COMPRESSION_JPEG)
		TIFFSetField(tif, TIFFTAG_JPEGQUALITY,	m_dwExportQuality);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,		PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL,	nbytes);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,		64);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG,		PLANARCONFIG_CONTIG);

	if (geo_info) {
		double tiff_tiepoints[6], tiff_scale[3];

		memset(tiff_tiepoints,	0, sizeof(tiff_tiepoints));
		memset(tiff_scale,		0, sizeof(tiff_scale));

		tiff_scale[0]		= geo_info[0];
		tiff_scale[1]		= geo_info[1];
		tiff_tiepoints[3]	= geo_info[2];
		tiff_tiepoints[4]	= geo_info[3];

		TIFFSetField(tif, TIFFTAG_GEOTIEPOINTS,		6,	tiff_tiepoints);
		TIFFSetField(tif, TIFFTAG_GEOPIXELSCALE,	3,	tiff_scale);
		TIFFSetField(tif, TIFFTAG_GEOKEYDIRECTORY,
			sizeof(GeoKeyDirectory) / sizeof(GeoKeyDirectory[0]),	GeoKeyDirectory);
	}

	return true;
}


void addGeoTiffTags()
{
	if (bFirstRunGeoTiff) {
		ParentExtender = TIFFSetTagExtender(GeoTagExtender);
		TIFFSetWarningHandler(NULL);
		TIFFSetErrorHandler(NULL);
		bFirstRunGeoTiff = false;
	}
}