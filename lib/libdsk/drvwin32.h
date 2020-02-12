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

#ifdef WIN32FLOPPY
/* Declarations for the WIN32 driver */

typedef struct
{
        DSK_DRIVER w32_super;
	DSK_GEOMETRY w32_geom;
	int    w32_readonly;
	int    w32_unit;
	HANDLE w32_hdisk;
        HANDLE w32_hmail;
        char   w32_slotname[PATH_MAX];
} WIN32_DSK_DRIVER;


/* Note that all the functions are either prefixed win32c (for Win32 versions 
 * running on DOS) and win32 (for Win32 versions running on the NT kernel). 
 * Most of them have both variants, but some only make sense on one of the
 * two platforms.
 * */

dsk_err_t win32_open(DSK_DRIVER *self, const char *filename);
dsk_err_t win32c_open(DSK_DRIVER *self, const char *filename);
dsk_err_t win32_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t win32_close(DSK_DRIVER *self);
dsk_err_t win32c_close(DSK_DRIVER *self);
dsk_err_t win32_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32c_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32c_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t win32c_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t win32c_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t win32_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
dsk_err_t win32_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);
dsk_err_t win32c_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);

dsk_err_t win32m_open(DSK_DRIVER *self, const char *filename);
dsk_err_t win32m_close(DSK_DRIVER *self);
dsk_err_t win32m_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32m_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t win32m_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t win32m_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t win32m_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);

#endif
 
