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

#include "drvi.h"

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_ptread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned int sec;
	dsk_err_t err;
	unsigned char *b;
	DRV_CLASS *dc;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	if (dc->dc_tread) 
	{
		err = (dc->dc_tread)(self,geom,buf,cylinder,head);	

		/* If set to store bytes complemented, flip them as they come
		 * out. (If we are using the dsk_pread() fallback, the 
		 * complement will take place in dsk_pread() ) */
		if (geom->dg_fm & RECMODE_COMPLEMENT)
		{
			for (sec = 0; sec < geom->dg_sectors * geom->dg_secsize;
				sec++)
			{
				((char *)buf)[sec] = ~((char *)buf)[sec];
			}
		}

		if (err != DSK_ERR_NOTIMPL) return err;
	}

	b = (unsigned char *)buf;

	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		err = dsk_pread(self, geom, b + sec * geom->dg_secsize,
			cylinder, head, sec + geom->dg_secbase);
		if (err) return err;
	}
	return DSK_ERR_OK;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_ltread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf, 
			dsk_ltrack_t track)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_err_t e;
 
        e = dg_lt2pt(geom, track, &c, &h);
        if (e != DSK_ERR_OK) return e;
        return dsk_ptread(self, geom, buf, c, h);
}

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
				dsk_pcyl_t cyl_expected, 
				dsk_phead_t head_expected)
{
	unsigned sec;
	dsk_err_t err;
	unsigned char *b;
	DRV_CLASS *dc;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	if (dc->dc_xtread) 
	{
		err = (dc->dc_xtread)(self,geom,buf,cylinder,head,
				cyl_expected, head_expected);	
		if (err != DSK_ERR_NOTIMPL) return err;
	}

	b = (unsigned char *)buf;

	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		err = dsk_xread(self, geom, b + sec * geom->dg_secsize,
			cylinder, head, 
			cyl_expected, head_expected, sec + geom->dg_secbase,
			geom->dg_secsize, 0);
		if (err) return err;
	}
	return DSK_ERR_OK;
}



