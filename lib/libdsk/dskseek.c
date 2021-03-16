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


/* Seek to a track */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	DRV_CLASS *dc;
	if (!self || !geom || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (!dc->dc_xseek) return DSK_ERR_NOTIMPL;
	return (dc->dc_xseek)(self,geom,cylinder,head);	

}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_ltrack_t track)
{
        dsk_err_t e;
        dsk_pcyl_t cyl;
        dsk_phead_t head;
 
        e = dg_lt2pt(geom, track, &cyl, &head);
        if (e) return e;
        return dsk_pseek(self, geom, cyl, head);
}

