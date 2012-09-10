/*
#include "image.h"

#include <stdio.h>
#include <libjpeg/jpeglib.h>
#include <libtiff/tiffio.h>
#include <string>

bool Image_validate(const char *path)
{
	const char *p = strrchr(path, '.');
	if (!p) return false;
	if (!_stricmp(p, ".bmp")) return true;
	if (!_stricmp(p, ".jpg")) return true;
	if (!_stricmp(p, ".tif")) return true;
	return false;
}

bool Image_getSizeBMP(const char *path, int &width, int &height)
{
	FILE *f;
	BITMAPINFOHEADER bmi;

	fopen_s(&f, path, "rb");
	if (!f) return false;

	fseek(f, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(&bmi, 1, sizeof(BITMAPINFOHEADER), f);

	fclose(f);

	width	= bmi.biWidth;
	height	= bmi.biHeight;

	return true;
}

bool Image_getSizeJPEG(const char *path, int &width, int &height)
{
	FILE *f = fopen(path, "rb");
	if (!f) return false;

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
  
	jpeg_stdio_src(&cinfo, f);
	jpeg_read_header(&cinfo, TRUE);

	width	= cinfo.image_width;
	height	= cinfo.image_height;
	
	jpeg_destroy_decompress(&cinfo);
	fclose(f);

	return true;
}

bool Image_getSizeTIFF(const char *path, int &width, int &height)
{
	static BOOL bFirstRun = true;
	if (bFirstRun) {
		TIFFSetWarningHandler(NULL);
		TIFFSetErrorHandler(NULL);
		bFirstRun = false;
	}

	TIFF *tif = TIFFOpen(path, "r");
	if (!tif) return false;

	BOOL	success	= FALSE;

	uint32 w, h;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,		&w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH,		&h);
	width	= w;
	height	= h;
	
	TIFFClose(tif);
	return true;
}

bool Image_getSizePNG(const char *path, int &width, int &height)
{
	FILE *fp;
	fopen_s(&fp, path, "rb");
	if (!fp)	return false;

	char header[8];		// 8 is the maximum size that can be checked
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8)) return false; // not a PNG file

	// create and initialize the png_struct
	png_structp	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		fclose(fp);
		return false; // png_create_read_struct failed
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

	width	= info_ptr->width;
	height	= info_ptr->height;

	// clean up after the read, and free any memory allocated
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

	fclose(fp);
	return true;
}

bool Image_getSizePGM(const char *path, int &width, int &height)
{
	FILE *f = fopen(path, "rb");
	if (!f) return false;

	char id[2];
	fread(id, 1, 2, f);
	if ((id[0] != 'P') || (id[1] != '5')) { fclose(f); return false; }
	char s[100];
	int parts = 0;
	int pos = 0;
	int sym = 0;
	do {
		fread(s + pos, 1, 1, f);
		if (sym && isspace(s[pos])) parts++;
		sym = !isspace(s[pos]);
		pos++;
	} while (parts < 3);
	s[pos] = 0;

	sscanf(s, "%d %d", &width, &height);

	fclose(f);
	return true;
}

typedef struct SGIHeader {
	unsigned short magic;
	unsigned short type;
	unsigned short dim;
	unsigned short xsize;
	unsigned short ysize;
	unsigned short zsize;
	unsigned int min;
	unsigned int max;
	unsigned int dummy0;
	char name[80];
	unsigned int colormap;
	char dummy1[404];
} SGIHeader;

#define SGI_SWAP_WORD(a) a = (a << 8) | (a >> 8)
bool Image_getSizeSGI(const char *path, int &width, int &height)
{
	SGIHeader header;
	size_t res;
	unsigned int *table		= NULL;
	unsigned int *rowStart	= NULL;
	unsigned int *rowSize	= NULL;
	UCHAR *temp	= NULL;
	UCHAR *buf	= NULL;
	BOOL bSuccess = FALSE;
	FILE *f = NULL;

	f = fopen(path, "rb");
	if (!f) return false;
	res = fread(&header, 1, sizeof(header), f);
	fclose(f);

	if (res != sizeof(header)) return false;

	SGI_SWAP_WORD(header.magic);
	SGI_SWAP_WORD(header.xsize);
	SGI_SWAP_WORD(header.ysize);

	if (header.magic != 474) return false;

	width	= header.xsize;
	height	= header.ysize;

	return true;
}

bool Image_getSize(const char *path, int &width, int &height)
{
	const char *p = strrchr(path, '.');
	if (!p) return false;
	if (!_stricmp(p, ".bmp")) return Image_getSizeBMP(path, width, height);
	if (!_stricmp(p, ".jpg")) return Image_getSizeJPEG(path, width, height);
	if (!_stricmp(p, ".tif")) return Image_getSizeTIFF(path, width, height);
	if (!_stricmp(p, ".png")) return Image_getSizePNG(path, width, height);
//	if (!_stricmp(p, ".pgm")) return Image_getSizePGM(path, width, height);
//	if (!_stricmp(p, ".sgi")) return Image_getSizeSGI(path, width, height);
	return false;
}


// http://www.wischik.com/lu/programmer/get-image-size.html

// get the size of a GIF/JPEG/PNG image

bool GetImageSize(const char *fn, int *x,int *y)
{
	FILE *f=fopen(fn,"rb"); if (f==0) return false;
	fseek(f,0,SEEK_END); long len=ftell(f); fseek(f,0,SEEK_SET); 
	if (len<24) {fclose(f); return false;}

	// Strategy:
	// reading GIF dimensions requires the first 10 bytes of the file
	// reading PNG dimensions requires the first 24 bytes of the file
	// reading JPEG dimensions requires scanning through jpeg chunks
	// In all formats, the file is at least 24 bytes big, so we'll read that always
	unsigned char buf[24]; fread(buf,1,24,f);

	// For JPEGs, we need to read the first 12 bytes of each chunk.
	// We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
	if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0
		&& buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F')
	{
		long pos=2;
		while (buf[2]==0xFF)
		{
			if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3
				|| buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB) break;
			pos += 2+(buf[4]<<8)+buf[5];
			if (pos+12>len) break;
			fseek(f,pos,SEEK_SET); fread(buf+2,1,12,f);
		}
	}

	fclose(f);

	// JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
	if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF)
	{
		*y = (buf[7]<<8) + buf[8];
		*x = (buf[9]<<8) + buf[10];
		return true;
	}

	// GIF: first three bytes say "GIF", next three give version number. Then dimensions
	if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
	{
		*x = buf[6] + (buf[7]<<8);
		*y = buf[8] + (buf[9]<<8);
		return true;
	}

	// PNG: the first frame is by definition an IHDR frame, which gives dimensions
	if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G'
		&& buf[4]==0x0D	&& buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A
		&& buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')
	{
		*x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
		*y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
		return true;
	}

	return false;
}
*/