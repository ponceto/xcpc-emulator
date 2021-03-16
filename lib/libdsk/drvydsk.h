/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001, 2008  John Elliott <seasip.webmaster@gmail.com>      *
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

/* Declarations for the YAZE YDSK driver */

typedef struct
{
        DSK_DRIVER ydsk_super;
        FILE *ydsk_fp;
	int   ydsk_readonly;
	int   ydsk_header_dirty;
	unsigned long  ydsk_filesize;	/* True length of the .DSK file */
	unsigned char  ydsk_header[128];
} YDSK_DSK_DRIVER;

dsk_err_t ydsk_open(DSK_DRIVER *self, const char *filename);
dsk_err_t ydsk_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t ydsk_close(DSK_DRIVER *self);
dsk_err_t ydsk_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t ydsk_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t ydsk_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t ydsk_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
dsk_err_t ydsk_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t ydsk_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);

dsk_err_t ydsk_option_enum(DSK_DRIVER *self, int idx, char **optname);
dsk_err_t ydsk_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t ydsk_option_get(DSK_DRIVER *self, const char *optname, int *value);
