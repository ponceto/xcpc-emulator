/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2006  John Elliott <seasip.webmaster@gmail.com>            *
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

#if (defined (DOS16FLOPPY) || defined (DOS32FLOPPY))
/* Declarations for the DOS INT25 driver */

typedef struct
{
	DSK_DRIVER   int25_super;
	DSK_GEOMETRY int25_geom;
	int          int25_readonly;
	int          int25_unit;
	unsigned short int25_dosver;
	unsigned char  int25_oem;
	unsigned char  int25_lock;
} INT25_DSK_DRIVER;


dsk_err_t int25_open(DSK_DRIVER *self, const char *filename);
dsk_err_t int25_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t int25_close(DSK_DRIVER *self);
dsk_err_t int25_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t int25_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t int25_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
/*
dsk_err_t int25_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
*/
dsk_err_t int25_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
/*
dsk_err_t int25_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);
*/

#endif
 
