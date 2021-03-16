/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2,2015  John Elliott <seasip.webmaster@gmail.com>     *
 *                                                                         *
 *    This library is free software; you can redistribute it and/or        *
 *    modify it under the terms of the GNU Library General Public          *
 *    License as published by the Free Software Foundation; either         *
 *    version 2 of the License, or (at your option) any later version.     *
 *                                                                         *
 *    This library is distributed in the hope that it will be useful,      *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *    Library General Public License for more details.                     *
 *                                                                         *
 *    You should have received a copy of the GNU Library General Public    *
 *    License along with this library; if not, write to the Free           *
 *    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,      *
 *    MA 02111-1307, USA                                                   *
 *                                                                         *
 ***************************************************************************/

/* Declarations for the JV3 driver */

/* JV3 spec at <http://www.tim-mann.org/trs80/dskspec.html> */

#define JV3_HEADER_COUNT  2901
#define JV3_HEADER_LEN  ((JV3_HEADER_COUNT * 3) + 1)

#define JV3_DENSITY     0x80  /* 1=dden, 0=sden */
#define JV3_DAM         0x60  /* data address mark code; see below */
#define JV3_SIDE        0x10  /* 0=side 0, 1=side 1 */
#define JV3_ERROR       0x08  /* 0=ok, 1=CRC error */
#define JV3_NONIBM      0x04  /* 0=normal, 1=short */
#define JV3_SIZE        0x03  /* in used sectors: 0=256,1=128,2=1024,3=512
                                 in free sectors: 0=512,1=1024,2=128,3=256 */

#define JV3_FREE        0xFF  /* in track and sector fields of free sectors */
#define JV3_FREEF       0xFC  /* in flags field, or'd with size code */

typedef struct
{
	DSK_DRIVER	jv3_super;
	unsigned char   jv3_header[JV3_HEADER_LEN];
	FILE		*jv3_fp;
	/* Fake sector for READ ID command */
	unsigned int	jv3_sector;
	long		jv3_len;	/* File size */
} JV3_DSK_DRIVER;


dsk_err_t jv3_open(DSK_DRIVER *self, const char *filename);
dsk_err_t jv3_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t jv3_close(DSK_DRIVER *self);
dsk_err_t jv3_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);

dsk_err_t jv3_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t jv3_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);

dsk_err_t jv3_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t jv3_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t jv3_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t jv3_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t jv3_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result);

dsk_err_t jv3_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result);
/* No options
dsk_err_t jv3_option_enum(DSK_DRIVER *self, int idx, char **optname);

dsk_err_t jv3_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t jv3_option_get(DSK_DRIVER *self, const char *optname, int *value);
*/
dsk_err_t jv3_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *dg);

