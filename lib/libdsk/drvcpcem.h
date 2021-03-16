/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <seasip.webmaster@gmail.com>            *
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

/* Declarations for the CPCEMU driver */

typedef struct
{
        DSK_DRIVER cpc_super;
        FILE *cpc_fp;
	int   cpc_readonly;
	int   cpc_sector;	/* Last sector used for DD READ ID, or -1
       				 * if valid last sector not seen */
	unsigned char cpc_dskhead[256];	/* DSK header */
	unsigned char cpc_trkhead[256];	/* Track header */
	unsigned char cpc_status[4];	/* Dummy FDC status regs ST0-ST3: Read */
	int	      cpc_statusw[4];	/* Dummy FDC status regs ST0-ST3: Write */
} CPCEMU_DSK_DRIVER;

/* v0.9.0: Use subclassing to create separate drivers for normal and 
 * extended .DSK images. This way we can create extended images by 
 * using "-type edsk" or similar */
dsk_err_t cpcext_open(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcext_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_open(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_close(DSK_DRIVER *self);
dsk_err_t cpcemu_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t cpcemu_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t cpcemu_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t cpcemu_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t cpcemu_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t cpcemu_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t cpcemu_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t cpcemu_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result);
dsk_err_t cpcemu_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result);

dsk_err_t cpcemu_option_enum(DSK_DRIVER *self, int idx, char **optname);

dsk_err_t cpcemu_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t cpcemu_option_get(DSK_DRIVER *self, const char *optname, int *value);

