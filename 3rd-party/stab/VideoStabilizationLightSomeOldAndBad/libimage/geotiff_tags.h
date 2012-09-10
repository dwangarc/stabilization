#pragma once


#define TIFFTAG_GEOPIXELSCALE		33550
#define TIFFTAG_GEOTIEPOINTS		33922
#define TIFFTAG_GEOTRANSMATRIX		34264
#define TIFFTAG_GEOKEYDIRECTORY		34735
#define TIFFTAG_GEODOUBLEPARAMS		34736
#define TIFFTAG_GEOASCIIPARAMS		34737

#define GTModelTypeGeoKey			1024
#define GTRasterTypeGeoKey			1025
#define GeographicTypeGeoKey		2048
#define ProjCoordTransGeoKey		3075
#define CT_MillerCylindrical		20

static TIFFExtendProc ParentExtender;

static bool bFirstRunGeoTiff = true;

/*  Tiff info structure.
 *
 *     Entry format:
 *        { TAGNUMBER, ReadCount, WriteCount, DataType, FIELDNUM, OkToChange, PassDirCountOnSet, AsciiName }
 *
 *     For ReadCount, WriteCount, -1 = unknown.
 */
static const TIFFFieldInfo xtiffFieldInfo[] = {
	{ TIFFTAG_GEOPIXELSCALE,	-1,-1, TIFF_DOUBLE,	FIELD_CUSTOM, TRUE,	TRUE,	"GeoPixelScale" },
//	{ TIFFTAG_GEOTRANSMATRIX,	-1,-1, TIFF_DOUBLE,	FIELD_CUSTOM, TRUE,	TRUE,	"GeoTransformationMatrix" },
	{ TIFFTAG_GEOTIEPOINTS,		-1,-1, TIFF_DOUBLE,	FIELD_CUSTOM, TRUE,	TRUE,	"GeoTiePoints" },
	{ TIFFTAG_GEOKEYDIRECTORY,	-1,-1, TIFF_SHORT,	FIELD_CUSTOM, TRUE,	TRUE,	"GeoKeyDirectory" },
//	{ TIFFTAG_GEODOUBLEPARAMS,	-1,-1, TIFF_DOUBLE,	FIELD_CUSTOM, TRUE,	TRUE,	"GeoDoubleParams" },
//	{ TIFFTAG_GEOASCIIPARAMS,	-1,-1, TIFF_ASCII,	FIELD_CUSTOM, TRUE,	FALSE,	"GeoASCIIParams" },
};

static void	GeoTagExtender(TIFF *tif)
{
	/* set up our own defaults */
    TIFFMergeFieldInfo(tif, xtiffFieldInfo, sizeof(xtiffFieldInfo) / sizeof(xtiffFieldInfo[0]));

	/* Since an XTIFF client module may have overridden
	 * the default directory method, we call it now to
	 * allow it to set up the rest of its own methods.
	 */
	if (ParentExtender) 
		(*ParentExtender)(tif);
}

static short GeoKeyDirectory[] = {
	1,						// GeoTIFF Version
	1,						// GeoKey Major Revision #
	0,						// GeoKey Minor Revision #
	4,						// Number of GeoKeys

	GTModelTypeGeoKey,		// GeoKey ID
	0,						// TIFF Tag ID or 0
	1,						// GeoKey value count
	2,						// value or tag offset	(ModelTypeGeographic)

	GTRasterTypeGeoKey,		// GeoKey ID
	0,						// TIFF Tag ID or 0
	1,						// GeoKey value count
	1,						// value or tag offset	(RasterPixelIsArea)

	GeographicTypeGeoKey,	// GeoKey ID
	0,						// TIFF Tag ID or 0
	1,						// GeoKey value count
	4326,					// value or tag offset	(GCS_WGS_84)

	//ProjCoordTransGeoKey,	// GeoKey ID
	//0,						// TIFF Tag ID or 0
	//1,						// GeoKey value count
	//CT_MillerCylindrical	// value or tag offset
};