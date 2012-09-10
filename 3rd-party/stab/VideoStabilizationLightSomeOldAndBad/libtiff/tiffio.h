/* $Id: tiffio.h,v 1.50 2006/03/21 16:37:51 dron Exp $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define	_TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"

#define TIFFLIB_VERSION_STR "LIBTIFF, Version 3.8.2\nCopyright (c) 1988-1996 Sam Leffler\nCopyright (c) 1991-1996 Silicon Graphics, Inc."
/*
 * This define can be used in code that requires
 * compilation-related definitions specific to a
 * version or versions of the library.  Runtime
 * version checking should be done based on the
 * string returned by TIFFGetVersion.
 */
#define TIFFLIB_VERSION 20060323

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef	struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * to pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double) and because we defined pseudo-tags that are
 *     outside the range of legal Aldus-assigned tags.
 * NB: tsize_t is int32 and not uint32 because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets being the most important, and to ensure
 *     that it is unsigned, rather than signed.
 */
typedef	uint32 ttag_t;		/* directory tag */
typedef	uint16 tdir_t;		/* directory index */
typedef	uint16 tsample_t;	/* sample number */
typedef	uint32 tstrip_t;	/* strip number */
typedef uint32 ttile_t;		/* tile number */
typedef	int32 tsize_t;		/* i/o size in bytes */
typedef	void* tdata_t;		/* image data ref */
typedef	uint32 toff_t;		/* file offset */

#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32))
#define __WIN32__
#endif

/*
 * On windows you should define USE_WIN32_FILEIO if you are using tif_win32.c
 * or AVOID_WIN32_FILEIO if you are using something else (like tif_unix.c).
 *
 * By default tif_unix.c is assumed.
 */

#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#  define BINMODE "b"
#  if !defined(__CYGWIN) && !defined(AVOID_WIN32_FILEIO) && !defined(USE_WIN32_FILEIO)
#    define AVOID_WIN32_FILEIO
#  endif
#  include <fcntl.h>
#  include <io.h>
#  ifdef SET_BINARY
#    undef SET_BINARY
#  endif /* SET_BINARY */
#  define SET_BINARY(f) do {if (!_isatty(f)) _setmode(f,_O_BINARY);} while (0)
#else /* Windows */
#  define BINMODE
#  define SET_BINARY(f) (void)0
#endif /* Windows */

#if defined(USE_WIN32_FILEIO)
# define VC_EXTRALEAN
# include <windows.h>
# ifdef __WIN32__
DECLARE_HANDLE(thandle_t);	/* Win32 file handle */
# else
typedef	HFILE thandle_t;	/* client data handle */
# endif /* __WIN32__ */
#else
typedef	void* thandle_t;	/* client data handle */
#endif /* USE_WIN32_FILEIO */

#ifndef NULL
# define NULL	(void *)0
#endif

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define	TIFFPRINT_NONE		0x0		/* no extra info */
#define	TIFFPRINT_STRIPS	0x1		/* strips/tiles info */
#define	TIFFPRINT_CURVES	0x2		/* color/gray response curves */
#define	TIFFPRINT_COLORMAP	0x4		/* colormap */
#define	TIFFPRINT_JPEGQTABLES	0x100		/* JPEG Q matrices */
#define	TIFFPRINT_JPEGACTABLES	0x200		/* JPEG AC tables */
#define	TIFFPRINT_JPEGDCTABLES	0x200		/* JPEG DC tables */

/* 
 * Colour conversion stuff
 */

/* reference white */
#define D65_X0 (95.0470F)
#define D65_Y0 (100.0F)
#define D65_Z0 (108.8827F)

#define D50_X0 (96.4250F)
#define D50_Y0 (100.0F)
#define D50_Z0 (82.4680F)

/* Structure for holding information about a display device. */

typedef	unsigned char TIFFRGBValue;		/* 8-bit samples */

typedef struct {
	float d_mat[3][3]; 		/* XYZ -> luminance matrix */
	float d_YCR;			/* Light o/p for reference white */
	float d_YCG;
	float d_YCB;
	uint32 d_Vrwr;			/* Pixel values for ref. white */
	uint32 d_Vrwg;
	uint32 d_Vrwb;
	float d_Y0R;			/* Residual light for black pixel */
	float d_Y0G;
	float d_Y0B;
	float d_gammaR;			/* Gamma values for the three guns */
	float d_gammaG;
	float d_gammaB;
} TIFFDisplay;

typedef struct {				/* YCbCr->RGB support */
	TIFFRGBValue* clamptab;			/* range clamping table */
	int*	Cr_r_tab;
	int*	Cb_b_tab;
	int32*	Cr_g_tab;
	int32*	Cb_g_tab;
        int32*  Y_tab;
} TIFFYCbCrToRGB;

typedef struct {				/* CIE Lab 1976->RGB support */
	int	range;				/* Size of conversion table */
#define CIELABTORGB_TABLE_RANGE 1500
	float	rstep, gstep, bstep;
	float	X0, Y0, Z0;			/* Reference white point */
	TIFFDisplay display;
	float	Yr2r[CIELABTORGB_TABLE_RANGE + 1];  /* Conversion of Yr to r */
	float	Yg2g[CIELABTORGB_TABLE_RANGE + 1];  /* Conversion of Yg to g */
	float	Yb2b[CIELABTORGB_TABLE_RANGE + 1];  /* Conversion of Yb to b */
} TIFFCIELabToRGB;

/*
 * RGBA-style image support.
 */
typedef struct _TIFFRGBAImage TIFFRGBAImage;
/*
 * The image reading and conversion routines invoke
 * ``put routines'' to copy/image/whatever tiles of
 * raw image data.  A default set of routines are 
 * provided to convert/copy raw image data to 8-bit
 * packed ABGR format rasters.  Applications can supply
 * alternate routines that unpack the data into a
 * different format or, for example, unpack the data
 * and draw the unpacked raster on the display.
 */
typedef void (*tileContigRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*);
typedef void (*tileSeparateRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*, unsigned char*, unsigned char*, unsigned char*);
/*
 * RGBA-reader state.
 */
struct _TIFFRGBAImage {
	TIFF*	tif;				/* image handle */
	int	stoponerr;			/* stop on read error */
	int	isContig;			/* data is packed/separate */
	int	alpha;				/* type of alpha data present */
	uint32	width;				/* image width */
	uint32	height;				/* image height */
	uint16	bitspersample;			/* image bits/sample */
	uint16	samplesperpixel;		/* image samples/pixel */
	uint16	orientation;			/* image orientation */
	uint16	req_orientation;		/* requested orientation */
	uint16	photometric;			/* image photometric interp */
	uint16*	redcmap;			/* colormap pallete */
	uint16*	greencmap;
	uint16*	bluecmap;
						/* get image data routine */
	int	(*get)(TIFFRGBAImage*, uint32*, uint32, uint32);
	union {
	    void (*any)(TIFFRGBAImage*);
	    tileContigRoutine	contig;
	    tileSeparateRoutine	separate;
	} put;					/* put decoded strip/tile */
	TIFFRGBValue* Map;			/* sample mapping array */
	uint32** BWmap;				/* black&white map */
	uint32** PALmap;			/* palette image map */
	TIFFYCbCrToRGB* ycbcr;			/* YCbCr conversion state */
        TIFFCIELabToRGB* cielab;		/* CIE L*a*b conversion state */

        int	row_offset;
        int     col_offset;
};

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define	TIFFGetR(abgr)	((abgr) & 0xff)
#define	TIFFGetG(abgr)	(((abgr) >> 8) & 0xff)
#define	TIFFGetB(abgr)	(((abgr) >> 16) & 0xff)
#define	TIFFGetA(abgr)	(((abgr) >> 24) & 0xff)

/*
 * A CODEC is a software package that implements decoding,
 * encoding, or decoding+encoding of a compression algorithm.
 * The library provides a collection of builtin codecs.
 * More codecs may be registered through calls to the library
 * and/or the builtin implementations may be overridden.
 */
typedef	int (*TIFFInitMethod)(TIFF*, int);
typedef struct {
	char*		name;
	uint16		scheme;
	TIFFInitMethod	init;
} TIFFCodec;

#include <stdio.h>
#include <stdarg.h>

/* share internal LogLuv conversion routines? */
#ifndef LOGLUV_PUBLIC
#define LOGLUV_PUBLIC		1
#endif

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
typedef	void (*TIFFErrorHandler)(const char*, const char*, va_list);
typedef	void (*TIFFErrorHandlerExt)(thandle_t, const char*, const char*, va_list);
typedef	tsize_t (*TIFFReadWriteProc)(thandle_t, tdata_t, tsize_t);
typedef	toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
typedef	int (*TIFFCloseProc)(thandle_t);
typedef	toff_t (*TIFFSizeProc)(thandle_t);
typedef	int (*TIFFMapFileProc)(thandle_t, tdata_t*, toff_t*);
typedef	void (*TIFFUnmapFileProc)(thandle_t, tdata_t, toff_t);
typedef	void (*TIFFExtendProc)(TIFF*); 

const char* TIFFGetVersion(void);

const TIFFCodec* TIFFFindCODEC(uint16);
TIFFCodec* TIFFRegisterCODEC(uint16, const char*, TIFFInitMethod);
void TIFFUnRegisterCODEC(TIFFCodec*);
int TIFFIsCODECConfigured(uint16);
TIFFCodec* TIFFGetConfiguredCODECs(void);

/*
 * Auxiliary functions.
 */

tdata_t _TIFFmalloc(tsize_t);
tdata_t _TIFFrealloc(tdata_t, tsize_t);
void _TIFFmemset(tdata_t, int, tsize_t);
void _TIFFmemcpy(tdata_t, const tdata_t, tsize_t);
int _TIFFmemcmp(const tdata_t, const tdata_t, tsize_t);
void _TIFFfree(tdata_t);

/*
** Stuff, related to tag handling and creating custom tags.
*/
int  TIFFGetTagListCount( TIFF * );
ttag_t TIFFGetTagListEntry( TIFF *, int tag_index );
    
#define	TIFF_ANY	TIFF_NOTYPE	/* for field descriptor searching */
#define	TIFF_VARIABLE	-1		/* marker for variable length tags */
#define	TIFF_SPP	-2		/* marker for SamplesPerPixel tags */
#define	TIFF_VARIABLE2	-3		/* marker for uint32 var-length tags */

#define FIELD_CUSTOM    65    

typedef	struct {
	ttag_t	field_tag;		/* field's tag */
	short	field_readcount;	/* read count/TIFF_VARIABLE/TIFF_SPP */
	short	field_writecount;	/* write count/TIFF_VARIABLE */
	TIFFDataType field_type;	/* type of associated data */
        unsigned short field_bit;	/* bit in fieldsset bit vector */
	unsigned char field_oktochange;	/* if true, can change while writing */
	unsigned char field_passcount;	/* if true, pass dir count on set */
	char	*field_name;		/* ASCII name */
} TIFFFieldInfo;

typedef struct _TIFFTagValue {
    const TIFFFieldInfo  *info;
    int             count;
    void           *value;
} TIFFTagValue;

void TIFFMergeFieldInfo(TIFF*, const TIFFFieldInfo[], int);
const TIFFFieldInfo* TIFFFindFieldInfo(TIFF*, ttag_t, TIFFDataType);
const TIFFFieldInfo* TIFFFindFieldInfoByName(TIFF* , const char *, TIFFDataType);
const TIFFFieldInfo* TIFFFieldWithTag(TIFF*, ttag_t);
const TIFFFieldInfo* TIFFFieldWithName(TIFF*, const char *);

typedef	int (*TIFFVSetMethod)(TIFF*, ttag_t, va_list);
typedef	int (*TIFFVGetMethod)(TIFF*, ttag_t, va_list);
typedef	void (*TIFFPrintMethod)(TIFF*, FILE*, long);
    
typedef struct {
    TIFFVSetMethod	vsetfield;	/* tag set routine */
    TIFFVGetMethod	vgetfield;	/* tag get routine */
    TIFFPrintMethod	printdir;	/* directory print routine */
} TIFFTagMethods;
        
TIFFTagMethods *TIFFAccessTagMethods( TIFF * );
void *TIFFGetClientInfo( TIFF *, const char * );
void TIFFSetClientInfo( TIFF *, void *, const char * );

void TIFFCleanup(TIFF*);
void TIFFClose(TIFF*);
int TIFFFlush(TIFF*);
int TIFFFlushData(TIFF*);
int TIFFGetField(TIFF*, ttag_t, ...);
int TIFFVGetField(TIFF*, ttag_t, va_list);
int TIFFGetFieldDefaulted(TIFF*, ttag_t, ...);
int TIFFVGetFieldDefaulted(TIFF*, ttag_t, va_list);
int TIFFReadDirectory(TIFF*);
int TIFFReadCustomDirectory(TIFF*, toff_t, const TIFFFieldInfo[], size_t);
int TIFFReadEXIFDirectory(TIFF*, toff_t);
tsize_t TIFFScanlineSize(TIFF*);
tsize_t TIFFRasterScanlineSize(TIFF*);
tsize_t TIFFStripSize(TIFF*);
tsize_t TIFFRawStripSize(TIFF*, tstrip_t);
tsize_t TIFFVStripSize(TIFF*, uint32);
tsize_t TIFFTileRowSize(TIFF*);
tsize_t TIFFTileSize(TIFF*);
tsize_t TIFFVTileSize(TIFF*, uint32);
uint32 TIFFDefaultStripSize(TIFF*, uint32);
void TIFFDefaultTileSize(TIFF*, uint32*, uint32*);
int TIFFFileno(TIFF*);
int TIFFSetFileno(TIFF*, int);
thandle_t TIFFClientdata(TIFF*);
thandle_t TIFFSetClientdata(TIFF*, thandle_t);
int TIFFGetMode(TIFF*);
int TIFFSetMode(TIFF*, int);
int TIFFIsTiled(TIFF*);
int TIFFIsByteSwapped(TIFF*);
int TIFFIsUpSampled(TIFF*);
int TIFFIsMSB2LSB(TIFF*);
int TIFFIsBigEndian(TIFF*);
TIFFReadWriteProc TIFFGetReadProc(TIFF*);
TIFFReadWriteProc TIFFGetWriteProc(TIFF*);
TIFFSeekProc TIFFGetSeekProc(TIFF*);
TIFFCloseProc TIFFGetCloseProc(TIFF*);
TIFFSizeProc TIFFGetSizeProc(TIFF*);
TIFFMapFileProc TIFFGetMapFileProc(TIFF*);
TIFFUnmapFileProc TIFFGetUnmapFileProc(TIFF*);
uint32 TIFFCurrentRow(TIFF*);
tdir_t TIFFCurrentDirectory(TIFF*);
tdir_t TIFFNumberOfDirectories(TIFF*);
uint32 TIFFCurrentDirOffset(TIFF*);
tstrip_t TIFFCurrentStrip(TIFF*);
ttile_t TIFFCurrentTile(TIFF*);
int TIFFReadBufferSetup(TIFF*, tdata_t, tsize_t);
int TIFFWriteBufferSetup(TIFF*, tdata_t, tsize_t);
int TIFFSetupStrips(TIFF *);
int TIFFWriteCheck(TIFF*, int, const char *);
void TIFFFreeDirectory(TIFF*);
int TIFFCreateDirectory(TIFF*);
int TIFFLastDirectory(TIFF*);
int TIFFSetDirectory(TIFF*, tdir_t);
int TIFFSetSubDirectory(TIFF*, uint32);
int TIFFUnlinkDirectory(TIFF*, tdir_t);
int TIFFSetField(TIFF*, ttag_t, ...);
int TIFFVSetField(TIFF*, ttag_t, va_list);
int TIFFWriteDirectory(TIFF *);
int TIFFCheckpointDirectory(TIFF *);
int TIFFRewriteDirectory(TIFF *);
int TIFFReassignTagToIgnore(enum TIFFIgnoreSense, int);

#if defined(c_plusplus) || defined(__cplusplus)
void TIFFPrintDirectory(TIFF*, FILE*, long = 0);
int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int = 0);
int TIFFReadRGBAImageOriented(TIFF*, uint32, uint32, uint32*,
				      int = ORIENTATION_BOTLEFT, int = 0);
#else
void TIFFPrintDirectory(TIFF*, FILE*, long);
int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t);
int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t);
int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int);
int TIFFReadRGBAImageOriented(TIFF*, uint32, uint32, uint32*, int, int);
#endif

int TIFFReadRGBAStrip(TIFF*, tstrip_t, uint32 * );
int TIFFReadRGBATile(TIFF*, uint32, uint32, uint32 * );
int TIFFRGBAImageOK(TIFF*, char [1024]);
int TIFFRGBAImageBegin(TIFFRGBAImage*, TIFF*, int, char [1024]);
int TIFFRGBAImageGet(TIFFRGBAImage*, uint32*, uint32, uint32);
void TIFFRGBAImageEnd(TIFFRGBAImage*);
TIFF* TIFFOpen(const char*, const char*);
# ifdef __WIN32__
TIFF* TIFFOpenW(const wchar_t*, const char*);
# endif /* __WIN32__ */
TIFF* TIFFFdOpen(int, const char*, const char*);
TIFF* TIFFClientOpen(const char*, const char*,
	    thandle_t,
	    TIFFReadWriteProc, TIFFReadWriteProc,
	    TIFFSeekProc, TIFFCloseProc,
	    TIFFSizeProc,
	    TIFFMapFileProc, TIFFUnmapFileProc);
const char* TIFFFileName(TIFF*);
const char* TIFFSetFileName(TIFF*, const char *);
void TIFFError(const char*, const char*, ...);
void TIFFErrorExt(thandle_t, const char*, const char*, ...);
void TIFFWarning(const char*, const char*, ...);
void TIFFWarningExt(thandle_t, const char*, const char*, ...);
TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler);
TIFFErrorHandlerExt TIFFSetErrorHandlerExt(TIFFErrorHandlerExt);
TIFFErrorHandler TIFFSetWarningHandler(TIFFErrorHandler);
TIFFErrorHandlerExt TIFFSetWarningHandlerExt(TIFFErrorHandlerExt);
TIFFExtendProc TIFFSetTagExtender(TIFFExtendProc);
ttile_t TIFFComputeTile(TIFF*, uint32, uint32, uint32, tsample_t);
int TIFFCheckTile(TIFF*, uint32, uint32, uint32, tsample_t);
ttile_t TIFFNumberOfTiles(TIFF*);
tsize_t TIFFReadTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
tsize_t TIFFWriteTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
tstrip_t TIFFComputeStrip(TIFF*, uint32, tsample_t);
tstrip_t TIFFNumberOfStrips(TIFF*);
tsize_t TIFFReadEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t TIFFReadRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t TIFFReadEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t TIFFReadRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t TIFFWriteEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t TIFFWriteRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
tsize_t TIFFWriteEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
tsize_t TIFFWriteRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
int TIFFDataWidth(TIFFDataType);    /* table of tag datatype widths */
void TIFFSetWriteOffset(TIFF*, toff_t);
void TIFFSwabShort(uint16*);
void TIFFSwabLong(uint32*);
void TIFFSwabDouble(double*);
void TIFFSwabArrayOfShort(uint16*, unsigned long);
void TIFFSwabArrayOfTriples(uint8*, unsigned long);
void TIFFSwabArrayOfLong(uint32*, unsigned long);
void TIFFSwabArrayOfDouble(double*, unsigned long);
void TIFFReverseBits(unsigned char *, unsigned long);
const unsigned char* TIFFGetBitRevTable(int);

#ifdef LOGLUV_PUBLIC
#define U_NEU		0.210526316
#define V_NEU		0.473684211
#define UVSCALE		410.
double LogL16toY(int);
double LogL10toY(int);
void XYZtoRGB24(float*, uint8*);
int uv_decode(double*, double*, int);
void LogLuv24toXYZ(uint32, float*);
void LogLuv32toXYZ(uint32, float*);
#if defined(c_plusplus) || defined(__cplusplus)
int LogL16fromY(double, int = SGILOGENCODE_NODITHER);
int LogL10fromY(double, int = SGILOGENCODE_NODITHER);
int uv_encode(double, double, int = SGILOGENCODE_NODITHER);
uint32 LogLuv24fromXYZ(float*, int = SGILOGENCODE_NODITHER);
uint32 LogLuv32fromXYZ(float*, int = SGILOGENCODE_NODITHER);
#else
int LogL16fromY(double, int);
int LogL10fromY(double, int);
int uv_encode(double, double, int);
uint32 LogLuv24fromXYZ(float*, int);
uint32 LogLuv32fromXYZ(float*, int);
#endif
#endif /* LOGLUV_PUBLIC */
    
int TIFFCIELabToRGBInit(TIFFCIELabToRGB*, TIFFDisplay *, float*);
void TIFFCIELabToXYZ(TIFFCIELabToRGB *, uint32, int32, int32, float *, float *, float *);
void TIFFXYZToRGB(TIFFCIELabToRGB *, float, float, float, uint32 *, uint32 *, uint32 *);

int TIFFYCbCrToRGBInit(TIFFYCbCrToRGB*, float*, float*);
void TIFFYCbCrtoRGB(TIFFYCbCrToRGB *, uint32, int32, int32, uint32 *, uint32 *, uint32 *);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* _TIFFIO_ */

/* vim: set ts=8 sts=8 sw=8 noet: */
