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

/* Wrapper functions for FORMAT calls */

#include "drvi.h"
#include "compi.h"

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pformat(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
        DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n;

        if (!self || !geom || !format || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;

        if (!dc->dc_format) return DSK_ERR_NOTIMPL;
	for (n = 0; n < self->dr_retry_count; n++)
	{
	        e = (dc->dc_format)(self,geom,cylinder,head,format,filler);      
		if (!DSK_TRANSIENT_ERROR(e)) break;
	}
	if (e == DSK_ERR_OK) self->dr_dirty = 1;
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lformat(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, const DSK_FORMAT *format, 
                                unsigned char filler)
{
        dsk_err_t e;
        dsk_pcyl_t c;
        dsk_phead_t h;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;
 
        e = dg_lt2pt(geom, track, &c, &h);
        if (e != DSK_ERR_OK) return e;
        e = dsk_pformat(self, geom, c, h, format, filler);                          
	if (e == DSK_ERR_OK) self->dr_dirty = 1;
	return e;
}


static DSK_FORMAT * dsk_formauto(const DSK_GEOMETRY *dg, 
				dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned int ns;
	DSK_FORMAT *fmt = calloc(dg->dg_sectors, sizeof(DSK_FORMAT));

	if (!fmt) return NULL;
	for (ns = 0; ns < dg->dg_sectors; ns++)
	{
		fmt[ns].fmt_cylinder = cylinder;
		fmt[ns].fmt_head     = dg_x_head(dg, head);
		fmt[ns].fmt_sector   = dg_x_sector(dg,head,dg->dg_secbase + ns);
		fmt[ns].fmt_secsize  = dg->dg_secsize;
	}
	return fmt;
}

/* Auto-format: generates the sector headers from "geom" */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_apform(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                unsigned char filler)
{
	DSK_FORMAT *fmt;
	dsk_err_t err;

	if (!geom) return DSK_ERR_BADPTR;

	fmt = dsk_formauto(geom, cylinder, head);
	if (!fmt)  return DSK_ERR_NOMEM;
	err = dsk_pformat(self,geom,cylinder,head,fmt,filler);
	free(fmt);
	return err;
}



LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_alform(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, unsigned char filler)
{
	dsk_pcyl_t cylinder; 
	dsk_phead_t head;
	DSK_FORMAT *fmt;
	dsk_err_t err;

	err = dg_lt2pt(geom, track, &cylinder, &head); 
	if (err) return err;

	fmt = dsk_formauto(geom, cylinder, head);
	if (!fmt)  return DSK_ERR_NOMEM;
	err = dsk_lformat(self,geom,track,fmt,filler);
	free(fmt);
	return err;
}

