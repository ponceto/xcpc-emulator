/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2005  John Elliott <seasip.webmaster@gmail.com>            *
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

/* PASSES is the number of times a sector header is seen before we deduce 
 * that all the sectors on the disc have been seen by the scan. Higher
 * values of PASSES may make for more accurate results but slower scans. */
#define PASSES 3	

LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ptrackids(DSK_PDRIVER self,
				const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				dsk_psect_t *count, DSK_FORMAT **results)
{
	dsk_err_t err;
	DRV_CLASS *dc;
	DSK_FORMAT fmt;
	DSK_GEOMETRY gtemp;

	if (!self || !geom || !self->dr_class || !count || !results)
	       	return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (dc->dc_trackids) 
	{
		err = (dc->dc_trackids)(self,geom,cylinder,head,count,results);
		if (err != DSK_ERR_NOTIMPL) return err;
	}
	if (dc->dc_secid)
	{
/* The driver doesn't provide a 'bulk' sector ID function, but we can 
 * try to get at it one sector at a time. */	

/* This is a big buffer and may need to be made static for DOS versions */
		DSK_FORMAT headers[256];
		unsigned char found[256];
		int lcount = 0;

		dsk_psect_t sector = 0;
		memcpy(&gtemp, geom, sizeof(gtemp));

/* Start by forcing a read error. We set sector size to 256
 * i) because with any luck, the floppy we're scanning won't have 256-byte 
 *   sectors.
 * ii) because the buffer we're using is only 256 bytes long. */
		gtemp.dg_secsize = 256;
		do
		{
			err = dsk_pread(self,&gtemp,found,cylinder,head,
					sector++);
		}
		while (err == DSK_ERR_OK);
/* After the read error, we should be right after the index hole. So read
 * off the sector IDs in order. But in case we miss one, go around a few
 * times. */
 
		memset(found, 0, sizeof(found));
		for (;;)
		{
			err = (dc->dc_secid)(self,geom,cylinder,head,&fmt);
			if (err) return err;
			if (!found[fmt.fmt_sector & 0xFF])
			{
				memcpy(&headers[lcount], &fmt, sizeof(fmt));
				++lcount;
			}
			found[fmt.fmt_sector & 0xFF]++;
			if (found[fmt.fmt_sector] > PASSES) break;
		}
		if (!lcount) return DSK_ERR_NOADDR;	/* Track blank */

		*count = lcount;
		*results = dsk_malloc( lcount * sizeof(DSK_FORMAT) );
		if (!(*results)) return DSK_ERR_NOMEM;
		memcpy(*results, headers, lcount * sizeof(DSK_FORMAT)) ;
		return DSK_ERR_OK;
	}
	return DSK_ERR_NOTIMPL;
}


LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ltrackids(DSK_PDRIVER self,
				const DSK_GEOMETRY *geom,
				dsk_ltrack_t track,
				dsk_psect_t *count, DSK_FORMAT **result)
{
        dsk_err_t e;
        dsk_pcyl_t cyl;
        dsk_psect_t sec;
 
        e = dg_lt2pt(geom, track, &cyl, &sec);
        if (e) return e;
        return dsk_ptrackids(self, geom, cyl, sec, count, result);   

}

