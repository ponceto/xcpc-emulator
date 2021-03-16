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
#include "compi.h"

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n, m;
	unsigned char *inv_buf = NULL;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	if (self && self->dr_compress && self->dr_compress->cd_readonly)
		return DSK_ERR_RDONLY;

	if (!dc->dc_write) return DSK_ERR_NOTIMPL;

	/* If we are storing the complement, generate complemented sector */
	if (geom->dg_fm & RECMODE_COMPLEMENT)
	{
		inv_buf = dsk_malloc(geom->dg_secsize);
	
		if (!inv_buf) return DSK_ERR_NOMEM;
		for (m = 0; m < geom->dg_secsize; m++) 
			inv_buf[m] = ~((char *)buf)[m];
		buf = inv_buf;
	}

	for (n = 0; n < self->dr_retry_count; n++)
	{
		e = (dc->dc_write)(self,geom,buf,cylinder,head,sector); 
		if (e == DSK_ERR_OK) self->dr_dirty = 1;
		if (!DSK_TRANSIENT_ERROR(e)) 
		{
			if (inv_buf != NULL) dsk_free(inv_buf);
			return e;
		}
	}
	if (inv_buf != NULL) dsk_free(inv_buf);
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_lsect_t sector)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_psect_t s;
        dsk_err_t e;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;
 
	e = dg_ls2ps(geom, sector, &c, &h, &s);
	if (e != DSK_ERR_OK) return e;
	e = dsk_pwrite(self, geom, buf, c, h, s);
	if (e == DSK_ERR_OK) self->dr_dirty = 1;
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
            const void *buf, 
                        dsk_pcyl_t cylinder,   dsk_phead_t head,
                        dsk_pcyl_t cyl_expect, dsk_phead_t head_expect,
                        dsk_psect_t sector, size_t sector_len, int deleted)
{
        DRV_CLASS *dc;
	dsk_err_t err = DSK_ERR_UNKNOWN;
	unsigned n, m;
	unsigned char *inv_buf = NULL;

        if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;

        if (!dc->dc_xwrite) return DSK_ERR_NOTIMPL;
	/* If we are storing the complement, generate complemented sector */
	if (geom->dg_fm & RECMODE_COMPLEMENT)
	{
		inv_buf = dsk_malloc(sector_len);
	
		if (!inv_buf) return DSK_ERR_NOMEM;
		for (m = 0; m < sector_len; m++) 
			inv_buf[m] = ~((char *)buf)[m];
		buf = inv_buf;
	}
	for (n = 0; n < self->dr_retry_count; n++)
	{
		err = (dc->dc_xwrite)(self,geom,buf,cylinder,head, cyl_expect, 
                	head_expect, sector, sector_len, deleted);
       		if (err == DSK_ERR_OK) self->dr_dirty = 1;
		if (!DSK_TRANSIENT_ERROR(err)) 
		{
			if (inv_buf != NULL) dsk_free(inv_buf);
			return err;
		}
	}
	if (inv_buf != NULL) dsk_free(inv_buf);
	return err;
}
                                                                                        
