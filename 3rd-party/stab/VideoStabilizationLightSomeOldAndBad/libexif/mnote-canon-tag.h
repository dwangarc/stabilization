/* mnote-canon-tag.h
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __MNOTE_CANON_TAG_H__
#define __MNOTE_CANON_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int MnoteCanonTag;
#define MNOTE_CANON_TAG_UNKNOWN_0		0x0
#define MNOTE_CANON_TAG_SETTINGS_1		0x1
#define MNOTE_CANON_TAG_FOCAL_LENGTH	0x2
#define MNOTE_CANON_TAG_UNKNOWN_3		0x3
#define MNOTE_CANON_TAG_SETTINGS_2		0x4
#define MNOTE_CANON_TAG_PANORAMA		0x5
#define MNOTE_CANON_TAG_IMAGE_TYPE		0x6
#define MNOTE_CANON_TAG_FIRMWARE		0x7
#define MNOTE_CANON_TAG_IMAGE_NUMBER	0x8
#define MNOTE_CANON_TAG_OWNER			0x9
#define MNOTE_CANON_TAG_UNKNOWN_10		0xa
#define MNOTE_CANON_TAG_SERIAL_NUMBER	0xc
#define MNOTE_CANON_TAG_UNKNOWN_13		0xd
#define MNOTE_CANON_TAG_CUSTOM_FUNCS	0xf

const char *mnote_canon_tag_get_name        (MnoteCanonTag);
const char *mnote_canon_tag_get_name_sub    (MnoteCanonTag, unsigned int, ExifDataOption);
const char *mnote_canon_tag_get_title       (MnoteCanonTag);
const char *mnote_canon_tag_get_title_sub   (MnoteCanonTag, unsigned int, ExifDataOption);
const char *mnote_canon_tag_get_description (MnoteCanonTag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MNOTE_CANON_TAG_H__ */
