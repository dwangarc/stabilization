#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include "exif-byte-order.h"
//#include "exif-data-type.h"
//#include "exif-ifd.h"
//#include "exif-log.h"
//#include "exif-tag.h"

typedef struct _ExifData			ExifData;
typedef struct _ExifDataPrivate		ExifDataPrivate;
typedef struct _ExifContent			ExifContent;
typedef struct _ExifContentPrivate	ExifContentPrivate;
typedef struct _ExifMem				ExifMem;
typedef struct _ExifEntry			ExifEntry;
typedef struct _ExifEntryPrivate	ExifEntryPrivate;
typedef struct _ExifMnoteData		ExifMnoteData;
typedef struct _ExifLog				ExifLog;

typedef int							ExifByteOrder;
typedef unsigned short				ExifIfd;
typedef int							ExifTag;
typedef unsigned short				ExifFormat;

typedef enum {
	EXIF_DATA_TYPE_UNCOMPRESSED_CHUNKY = 0,
	EXIF_DATA_TYPE_UNCOMPRESSED_PLANAR,
	EXIF_DATA_TYPE_UNCOMPRESSED_YCC,
	EXIF_DATA_TYPE_COMPRESSED,
	EXIF_DATA_TYPE_COUNT
} ExifDataType;

typedef enum {
	EXIF_SUPPORT_LEVEL_UNKNOWN = 0,
	EXIF_SUPPORT_LEVEL_NOT_RECORDED,
	EXIF_SUPPORT_LEVEL_MANDATORY,
	EXIF_SUPPORT_LEVEL_OPTIONAL
} ExifSupportLevel;

#define EXIF_BYTE_ORDER_MOTOROLA	0
#define EXIF_BYTE_ORDER_INTEL		1

#define EXIF_IFD_0					0
#define EXIF_IFD_1					1
#define EXIF_IFD_EXIF				2
#define EXIF_IFD_GPS				3
#define EXIF_IFD_INTEROPERABILITY	4
#define EXIF_IFD_COUNT				5

#define EXIF_TAG_INTEROPERABILITY_INDEX			0x0001
#define EXIF_TAG_INTEROPERABILITY_VERSION		0x0002
#define EXIF_TAG_NEW_SUBFILE_TYPE				0x00fe
#define EXIF_TAG_IMAGE_WIDTH 					0x0100
#define EXIF_TAG_IMAGE_LENGTH 					0x0101
#define EXIF_TAG_BITS_PER_SAMPLE 				0x0102
#define EXIF_TAG_COMPRESSION 					0x0103
#define EXIF_TAG_PHOTOMETRIC_INTERPRETATION 	0x0106
#define EXIF_TAG_FILL_ORDER 					0x010a
#define EXIF_TAG_DOCUMENT_NAME 					0x010d
#define EXIF_TAG_IMAGE_DESCRIPTION 				0x010e
#define EXIF_TAG_MAKE 							0x010f
#define EXIF_TAG_MODEL 							0x0110
#define EXIF_TAG_STRIP_OFFSETS 					0x0111
#define EXIF_TAG_ORIENTATION 					0x0112
#define EXIF_TAG_SAMPLES_PER_PIXEL 				0x0115
#define EXIF_TAG_ROWS_PER_STRIP 				0x0116
#define EXIF_TAG_STRIP_BYTE_COUNTS				0x0117
#define EXIF_TAG_X_RESOLUTION 					0x011a
#define EXIF_TAG_Y_RESOLUTION 					0x011b
#define EXIF_TAG_PLANAR_CONFIGURATION 			0x011c
#define EXIF_TAG_RESOLUTION_UNIT 				0x0128
#define EXIF_TAG_TRANSFER_FUNCTION 				0x012d
#define EXIF_TAG_SOFTWARE 						0x0131
#define EXIF_TAG_DATE_TIME						0x0132
#define EXIF_TAG_ARTIST							0x013b
#define EXIF_TAG_WHITE_POINT					0x013e
#define EXIF_TAG_PRIMARY_CHROMATICITIES			0x013f
#define EXIF_TAG_TRANSFER_RANGE					0x0156
#define EXIF_TAG_SUB_IFDS						0x014a
#define EXIF_TAG_JPEG_PROC						0x0200
#define EXIF_TAG_JPEG_INTERCHANGE_FORMAT		0x0201
#define EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH	0x0202
#define EXIF_TAG_YCBCR_COEFFICIENTS				0x0211
#define EXIF_TAG_YCBCR_SUB_SAMPLING				0x0212
#define EXIF_TAG_YCBCR_POSITIONING				0x0213
#define EXIF_TAG_REFERENCE_BLACK_WHITE			0x0214
#define EXIF_TAG_XML_PACKET						0x02bc
#define EXIF_TAG_RELATED_IMAGE_FILE_FORMAT		0x1000
#define EXIF_TAG_RELATED_IMAGE_WIDTH			0x1001
#define EXIF_TAG_RELATED_IMAGE_LENGTH			0x1002
#define EXIF_TAG_CFA_REPEAT_PATTERN_DIM			0x828d
#define EXIF_TAG_CFA_PATTERN					0x828e
#define EXIF_TAG_BATTERY_LEVEL					0x828f
#define EXIF_TAG_COPYRIGHT						0x8298
#define EXIF_TAG_EXPOSURE_TIME					0x829a
#define EXIF_TAG_FNUMBER						0x829d
#define EXIF_TAG_IPTC_NAA						0x83bb
#define EXIF_TAG_IMAGE_RESOURCES				0x8649
#define EXIF_TAG_EXIF_IFD_POINTER				0x8769
#define EXIF_TAG_INTER_COLOR_PROFILE			0x8773
#define EXIF_TAG_EXPOSURE_PROGRAM				0x8822
#define EXIF_TAG_SPECTRAL_SENSITIVITY			0x8824
#define EXIF_TAG_GPS_INFO_IFD_POINTER			0x8825
#define EXIF_TAG_ISO_SPEED_RATINGS				0x8827
#define EXIF_TAG_OECF							0x8828
#define EXIF_TAG_EXIF_VERSION					0x9000
#define EXIF_TAG_DATE_TIME_ORIGINAL				0x9003
#define EXIF_TAG_DATE_TIME_DIGITIZED			0x9004
#define EXIF_TAG_COMPONENTS_CONFIGURATION		0x9101
#define EXIF_TAG_COMPRESSED_BITS_PER_PIXEL		0x9102
#define EXIF_TAG_SHUTTER_SPEED_VALUE			0x9201
#define EXIF_TAG_APERTURE_VALUE					0x9202
#define EXIF_TAG_BRIGHTNESS_VALUE				0x9203
#define EXIF_TAG_EXPOSURE_BIAS_VALUE			0x9204
#define EXIF_TAG_MAX_APERTURE_VALUE				0x9205
#define EXIF_TAG_SUBJECT_DISTANCE				0x9206
#define EXIF_TAG_METERING_MODE					0x9207
#define EXIF_TAG_LIGHT_SOURCE					0x9208
#define EXIF_TAG_FLASH							0x9209
#define EXIF_TAG_FOCAL_LENGTH					0x920a
#define EXIF_TAG_SUBJECT_AREA					0x9214
#define EXIF_TAG_TIFF_EP_STANDARD_ID			0x9216
#define EXIF_TAG_MAKER_NOTE						0x927c
#define EXIF_TAG_USER_COMMENT					0x9286
#define EXIF_TAG_SUB_SEC_TIME					0x9290
#define EXIF_TAG_SUB_SEC_TIME_ORIGINAL			0x9291
#define EXIF_TAG_SUB_SEC_TIME_DIGITIZED			0x9292
#define EXIF_TAG_XP_TITLE						0x9c9b
#define EXIF_TAG_XP_COMMENT						0x9c9c
#define EXIF_TAG_XP_AUTHOR						0x9c9d
#define EXIF_TAG_XP_KEYWORDS					0x9c9e
#define EXIF_TAG_XP_SUBJECT						0x9c9f
#define EXIF_TAG_FLASH_PIX_VERSION				0xa000
#define EXIF_TAG_COLOR_SPACE					0xa001
#define EXIF_TAG_PIXEL_X_DIMENSION				0xa002
#define EXIF_TAG_PIXEL_Y_DIMENSION				0xa003
#define EXIF_TAG_RELATED_SOUND_FILE				0xa004
#define EXIF_TAG_INTEROPERABILITY_IFD_POINTER	0xa005
#define EXIF_TAG_FLASH_ENERGY					0xa20b
#define EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE		0xa20c
#define EXIF_TAG_FOCAL_PLANE_X_RESOLUTION		0xa20e
#define EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION		0xa20f
#define EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT	0xa210
#define EXIF_TAG_SUBJECT_LOCATION				0xa214
#define EXIF_TAG_EXPOSURE_INDEX					0xa215
#define EXIF_TAG_SENSING_METHOD					0xa217
#define EXIF_TAG_FILE_SOURCE					0xa300
#define EXIF_TAG_SCENE_TYPE						0xa301
#define EXIF_TAG_NEW_CFA_PATTERN				0xa302
#define EXIF_TAG_CUSTOM_RENDERED				0xa401
#define EXIF_TAG_EXPOSURE_MODE					0xa402
#define EXIF_TAG_WHITE_BALANCE					0xa403
#define EXIF_TAG_DIGITAL_ZOOM_RATIO				0xa404
#define EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM		0xa405
#define EXIF_TAG_SCENE_CAPTURE_TYPE				0xa406
#define EXIF_TAG_GAIN_CONTROL					0xa407
#define EXIF_TAG_CONTRAST						0xa408
#define EXIF_TAG_SATURATION						0xa409
#define EXIF_TAG_SHARPNESS						0xa40a
#define EXIF_TAG_DEVICE_SETTING_DESCRIPTION		0xa40b
#define EXIF_TAG_SUBJECT_DISTANCE_RANGE			0xa40c
#define EXIF_TAG_IMAGE_UNIQUE_ID				0xa420
#define EXIF_TAG_GAMMA							0xa500
#define EXIF_TAG_UNKNOWN_C4A5					0xc4a5

/* GPS tags overlap with above ones. */
#define EXIF_TAG_GPS_VERSION_ID        0x0000
#define EXIF_TAG_GPS_LATITUDE_REF      0x0001 /* INTEROPERABILITY_INDEX   */
#define EXIF_TAG_GPS_LATITUDE          0x0002 /* INTEROPERABILITY_VERSION */
#define EXIF_TAG_GPS_LONGITUDE_REF     0x0003
#define EXIF_TAG_GPS_LONGITUDE         0x0004
#define EXIF_TAG_GPS_ALTITUDE_REF      0x0005
#define EXIF_TAG_GPS_ALTITUDE          0x0006
#define EXIF_TAG_GPS_TIME_STAMP        0x0007
#define EXIF_TAG_GPS_SATELLITES        0x0008
#define EXIF_TAG_GPS_STATUS            0x0009
#define EXIF_TAG_GPS_MEASURE_MODE      0x000a
#define EXIF_TAG_GPS_DOP               0x000b
#define EXIF_TAG_GPS_SPEED_REF         0x000c
#define EXIF_TAG_GPS_SPEED             0x000d
#define EXIF_TAG_GPS_TRACK_REF         0x000e
#define EXIF_TAG_GPS_TRACK             0x000f
#define EXIF_TAG_GPS_IMG_DIRECTION_REF 0x0010
#define EXIF_TAG_GPS_IMG_DIRECTION     0x0011
#define EXIF_TAG_GPS_MAP_DATUM         0x0012
#define EXIF_TAG_GPS_DEST_LATITUDE_REF 0x0013
#define EXIF_TAG_GPS_DEST_LATITUDE     0x0014
#define EXIF_TAG_GPS_DEST_LONGITUDE_REF 0x0015
#define EXIF_TAG_GPS_DEST_LONGITUDE     0x0016
#define EXIF_TAG_GPS_DEST_BEARING_REF   0x0017
#define EXIF_TAG_GPS_DEST_BEARING       0x0018
#define EXIF_TAG_GPS_DEST_DISTANCE_REF  0x0019
#define EXIF_TAG_GPS_DEST_DISTANCE      0x001a
#define EXIF_TAG_GPS_PROCESSING_METHOD  0x001b
#define EXIF_TAG_GPS_AREA_INFORMATION   0x001c
#define EXIF_TAG_GPS_DATE_STAMP         0x001d
#define EXIF_TAG_GPS_DIFFERENTIAL       0x001e

//#include "exif-content.h"
//#include "exif-mnote-data.h"
//#include "exif-mem.h"

struct _ExifData
{
	ExifContent *ifd[EXIF_IFD_COUNT];

	unsigned char *data;
	unsigned int size;

	ExifDataPrivate *priv;
};

struct _ExifEntry {
	ExifTag tag;
	ExifFormat format;
	unsigned long components;

	unsigned char *data;
	unsigned int size;

	/* Content containing this entry */
	ExifContent *parent;

	ExifEntryPrivate *priv;
};

struct _ExifContent
{
	ExifEntry **entries;
	unsigned int count;

	/* Data containing this content */
	ExifData *parent;

	ExifContentPrivate *priv;
};

const char *exif_ifd_get_name (ExifIfd ifd);
const char *exif_byte_order_get_name (ExifByteOrder order);

ExifTag          exif_tag_from_name                (const char *);
const char      *exif_tag_get_name_in_ifd          (ExifTag, ExifIfd);
const char      *exif_tag_get_title_in_ifd         (ExifTag, ExifIfd);
const char      *exif_tag_get_description_in_ifd   (ExifTag, ExifIfd);
ExifSupportLevel exif_tag_get_support_level_in_ifd (ExifTag, ExifIfd,
                                                    ExifDataType);

/* Don't use these functions. They are here for compatibility only. */
const char     *exif_tag_get_name        (ExifTag tag);
const char     *exif_tag_get_title       (ExifTag tag);
const char     *exif_tag_get_description (ExifTag tag);

ExifData *exif_data_new           (void);
ExifData *exif_data_new_mem       (ExifMem *);

/*! \brief load exif data from file
 *  \param[in] path filename including path
 *  
 *  Foo bar blah bleh baz.
 */
ExifData *exif_data_new_from_file (const char *path);
ExifData *exif_data_new_from_data (const unsigned char *data,
				   unsigned int size);

void      exif_data_load_data (ExifData *data, const unsigned char *d, 
			       unsigned int size);
void      exif_data_save_data (ExifData *data, unsigned char **d,
			       unsigned int *size);

void      exif_data_ref   (ExifData *data);
void      exif_data_unref (ExifData *data);
void      exif_data_free  (ExifData *data);

ExifByteOrder exif_data_get_byte_order  (ExifData *data);
void          exif_data_set_byte_order  (ExifData *data, ExifByteOrder order);

ExifMnoteData *exif_data_get_mnote_data (ExifData *);
void           exif_data_fix (ExifData *);

typedef void (* ExifDataForeachContentFunc) (ExifContent *, void *user_data);
void          exif_data_foreach_content (ExifData *data,
					 ExifDataForeachContentFunc func,
					 void *user_data);

typedef int ExifDataOption;
#define EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS	1
#define EXIF_DATA_OPTION_FOLLOW_SPECIFICATION	2
#define EXIF_DATA_OPTION_DONT_CHANGE_MAKER_NOTE 4

const char *exif_data_option_get_name        (ExifDataOption);
const char *exif_data_option_get_description (ExifDataOption);
void        exif_data_set_option             (ExifData *, ExifDataOption);
void        exif_data_unset_option           (ExifData *, ExifDataOption);

void         exif_data_set_data_type (ExifData *, ExifDataType);
ExifDataType exif_data_get_data_type (ExifData *);

/* For debugging purposes and error reporting */
void exif_data_dump (ExifData *data);
void exif_data_log  (ExifData *data, ExifLog *log);

/* For your convenience */
#define exif_data_get_entry(d,t)					\
	(exif_content_get_entry(d->ifd[EXIF_IFD_0],t) ?			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_0],t) :			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_1],t) ?			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_1],t) :			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) ?		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) :		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) ?		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) :		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) ?	\
	 exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) : NULL)

/* Lifecycle */
ExifContent *exif_content_new     (void);
ExifContent *exif_content_new_mem (ExifMem *);
void         exif_content_ref     (ExifContent *content);
void         exif_content_unref   (ExifContent *content);
void         exif_content_free    (ExifContent *content);

void         exif_content_add_entry    (ExifContent *, ExifEntry *);
void         exif_content_remove_entry (ExifContent *, ExifEntry *);
ExifEntry   *exif_content_get_entry    (ExifContent *, ExifTag);
void         exif_content_fix          (ExifContent *);

typedef void (* ExifContentForeachEntryFunc) (ExifEntry *, void *user_data);
void         exif_content_foreach_entry (ExifContent *content,
					 ExifContentForeachEntryFunc func,
					 void *user_data);

/* For your convenience */
ExifIfd exif_content_get_ifd (ExifContent *);
#define exif_content_get_value(c,t,v,m)					\
	(exif_content_get_entry (c,t) ?					\
	 exif_entry_get_value (exif_content_get_entry (c,t),v,m) : NULL)

void exif_content_dump  (ExifContent *content, unsigned int indent);
void exif_content_log   (ExifContent *content, ExifLog *log);

#ifdef __cplusplus
}
#endif /* __cplusplus */
