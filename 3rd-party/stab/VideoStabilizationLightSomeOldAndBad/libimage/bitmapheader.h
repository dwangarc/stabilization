#pragma once

#ifndef _WINGDI_

// from windef.h
typedef unsigned char	UCHAR;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef int				BOOL;
typedef long			LONG;


typedef struct tagBITMAPINFOHEADER { // bmih
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	char	rgbBlue;
	char	rgbGreen;
	char	rgbRed;
	char	rgbReserved;
} RGBQUAD;

#pragma pack(push,1)

typedef struct tagRGBTRIPLE {
	char	rgbtBlue;
	char	rgbtGreen;
	char	rgbtRed;
} RGBTRIPLE;

#pragma pack(pop)

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD		bmiColors[1];
} BITMAPINFO;

#pragma pack(push,2)

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER;

#pragma pack(pop)

#endif // _WINGDI_