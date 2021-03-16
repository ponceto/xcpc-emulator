/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-3, 2015  John Elliott <seasip.webmaster@gmail.com>    *
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

/* Declarations for the IMD driver. We handle this like the CFI and APRIDISK
 * drivers, by unpacking the disc image into memory. 
 *
 */

typedef struct
{
	unsigned char	imds_cylinder;	/* Cylinder ID */
	unsigned char 	imds_head;	/* Head ID */
	unsigned char	imds_sector;	/* Sector ID */
	unsigned char 	imds_status;	/* Status */
	unsigned short 	imds_seclen;	/* Length of sector on disk */
	unsigned short  imds_datalen;	/* Length of (compressed) data */
	unsigned char	imds_data[1];	/* Data */
} IMD_SECTOR;


typedef struct
{
	unsigned char   imdt_mode;	/* Recording mode */
	unsigned char   imdt_cylinder;	/* Cylinder */
	unsigned char 	imdt_head;	/* Head and flags */
	unsigned char	imdt_sectors;	/* Sector count */
	unsigned short	imdt_seclen;	/* Default sector length */
	IMD_SECTOR	*imdt_sec[1];	/* Pointers to sector data */
} IMD_TRACK;


typedef struct
{
	DSK_DRIVER 	imd_super;
	IMD_TRACK 	**imd_tracks;
	char		*imd_filename;
	dsk_ltrack_t	imd_ntracks;
	int		imd_dirty;
	int		imd_readonly;
	dsk_psect_t	imd_sec;
} IMD_DSK_DRIVER;

dsk_err_t imd_open(DSK_DRIVER *self, const char *filename);
dsk_err_t imd_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t imd_close(DSK_DRIVER *self);
dsk_err_t imd_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t imd_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t imd_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t imd_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t imd_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_phead_t head, unsigned char *result);
dsk_err_t imd_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cyl, dsk_phead_t head, DSK_FORMAT *result);

dsk_err_t imd_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);

dsk_err_t imd_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
		void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, 
		dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		dsk_psect_t sector, size_t sector_size, int *deleted);

dsk_err_t imd_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
		const void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, 
		dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		dsk_psect_t sector, size_t sector_size, int deleted);

