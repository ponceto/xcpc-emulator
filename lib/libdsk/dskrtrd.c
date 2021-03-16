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


LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_rtread(DSK_PDRIVER self, 
	                const DSK_GEOMETRY *geom, void *buf, 
	                dsk_pcyl_t cylinder,  dsk_phead_t head, int reserved)
{
	DRV_CLASS *dc;
	size_t bufsiz;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (!dc->dc_rtread) return DSK_ERR_NOTIMPL;
	return (dc->dc_rtread)(self,geom,buf,cylinder,head,reserved, &bufsiz);	

}
