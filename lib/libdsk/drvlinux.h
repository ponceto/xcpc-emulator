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

#ifdef LINUXFLOPPY

/* Declarations for the Linux-specific floppy driver */

typedef struct
{
        DSK_DRIVER   lx_super;
	DSK_GEOMETRY lx_geom;
        int	     lx_fd;
	int          lx_readonly;
	int	     lx_forcehead;
	int          lx_doublestep;
	dsk_pcyl_t   lx_cylinder;
	unsigned char lx_status[4];
} LINUX_DSK_DRIVER;

dsk_err_t linux_open(DSK_DRIVER *self, const char *filename);
dsk_err_t linux_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t linux_close(DSK_DRIVER *self);
dsk_err_t linux_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t linux_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t linux_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t linux_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t linux_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t linux_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t linux_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t linux_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              dsk_phead_t head, unsigned char *result);
dsk_err_t linux_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t linux_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected);

/* List driver-specific options */
dsk_err_t linux_option_enum(DSK_DRIVER *self, int idx, char **optname);

/* Set a driver-specific option */
dsk_err_t linux_option_set(DSK_DRIVER *self, const char *optname, int value);
/* Get a driver-specific option */
dsk_err_t linux_option_get(DSK_DRIVER *self, const char *optname, int *value);


#endif

