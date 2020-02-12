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


/* Read a random sector header from current track */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_psecid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	DRV_CLASS *dc;
	if (!self || !geom || !result || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (!dc->dc_secid) return DSK_ERR_NOTIMPL;
	return (dc->dc_secid)(self,geom,cylinder,head,result);	

}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lsecid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, DSK_FORMAT *result)
{
        dsk_err_t e;
        dsk_pcyl_t cyl;
        dsk_psect_t sec;
 
        e = dg_lt2pt(geom, track, &cyl, &sec);
        if (e) return e;
        return dsk_psecid(self, geom, cyl, sec, result);   
}

