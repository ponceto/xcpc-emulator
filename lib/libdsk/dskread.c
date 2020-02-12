/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001, 2015  John Elliott <seasip.webmaster@gmail.com>  *
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

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n, m;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	/* LDTRACE(("dsk_pread (%d,%d,%d)\n", cylinder, head, sector)); */
        if (!dc->dc_read) return DSK_ERR_NOTIMPL;
	for (n = 0; n < self->dr_retry_count; n++)
	{
		e = (dc->dc_read)(self,geom,buf,cylinder,head,sector);
		/* If flagged to complement bytes, complement them */
		if (geom->dg_fm & RECMODE_COMPLEMENT)
		{
			for (m = 0; m < geom->dg_secsize; m++)
			{
				((char *)buf)[m] = ~((char *)buf)[m];
			}
		}	
/* 		LDTRACE(("  err=%d\n", e)); */
		if (!DSK_TRANSIENT_ERROR(e)) return e; 
	}
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_lsect_t sector)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_psect_t s;
        dsk_err_t e;
 
        e = dg_ls2ps(geom, sector, &c, &h, &s);
        if (e != DSK_ERR_OK) return e;
        return dsk_pread(self, geom, buf, c, h, s);
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf, 
			dsk_pcyl_t cylinder,   dsk_phead_t head, 
			dsk_pcyl_t cyl_expect, dsk_phead_t head_expect, 
			dsk_psect_t sector, size_t sector_len, int *deleted)
{
	DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n, m;
	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

/*	LDTRACE(("dsk_xread (%d,%d,%d) (%d,%d)\n", cylinder, head, sector,
				cyl_expect, head_expect)); */
        if (!dc->dc_xread) 
	{
		return DSK_ERR_NOTIMPL;
	}
	for (n = 0; n < self->dr_retry_count; n++)
	{
		e = (dc->dc_xread)(self,geom,buf,cylinder,head,
			cyl_expect, head_expect, sector, sector_len, deleted);	
			/* If flagged to complement bytes, complement them */
		if (geom->dg_fm & RECMODE_COMPLEMENT)
		{
			for (m = 0; m < sector_len; m++)
			{
				((char *)buf)[m] = ~((char *)buf)[m];
			}
		}
		/* LDTRACE(("  err=%d\n", e)); */
		if (!DSK_TRANSIENT_ERROR(e)) return e;
	}
	return e;
}

