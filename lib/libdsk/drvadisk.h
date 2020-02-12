/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-3  John Elliott <seasip.webmaster@gmail.com>          *
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

/* Declarations for the APRIDISK driver. We handle this like the CFI 
 * driver, by decompressing into memory. 
 *
 * I'm doing compression in the driver rather than as a separate compression
 * module because the compression system is tied to the file format, the
 * same as for CFI. */


typedef struct
{
	unsigned long	adisks_magic;
	unsigned short	adisks_cylinder;
	unsigned char	adisks_head;
	unsigned char	adisks_sector;
	unsigned char	*adisks_data;
	unsigned long	adisks_datalen;
} ADISK_SECTOR;

typedef struct
{
    DSK_DRIVER adisk_super;
	char  adisk_header[128];
	char *adisk_filename;
	char *adisk_creator;
	int   adisk_readonly;
	ADISK_SECTOR *adisk_sectors;
	dsk_lsect_t adisk_nsectors;
	dsk_lsect_t adisk_maxsectors;
	int   adisk_dirty;
	dsk_psect_t adisk_sec;
} ADISK_DSK_DRIVER;

dsk_err_t adisk_open(DSK_DRIVER *self, const char *filename);
dsk_err_t adisk_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t adisk_close(DSK_DRIVER *self);
dsk_err_t adisk_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t adisk_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t adisk_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t adisk_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t adisk_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_phead_t head, unsigned char *result);
dsk_err_t adisk_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cyl, dsk_phead_t head, DSK_FORMAT *result);


