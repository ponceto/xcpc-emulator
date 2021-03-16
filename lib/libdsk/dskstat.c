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

/* Get the status (ST3) of a drive.  */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_drive_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
                                dsk_phead_t head, unsigned char *status)
{
	DRV_CLASS *dc;
	dsk_err_t err;
	unsigned char ro = 0;

	if (!self || !geom || !status || !self->dr_class) return DSK_ERR_BADPTR;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
		ro = DSK_ST3_RO;

	/* Generate a default status. If the driver doesn't provide this 
	 * function, the default status will be used. */

	*status = DSK_ST3_READY | ro;
	if (geom->dg_heads > 1) *status |= DSK_ST3_DSDRIVE;	
	if (head)               *status |= DSK_ST3_HEAD1;
	
	dc = self->dr_class;
        if (!dc->dc_status) return DSK_ERR_OK;
	err = (dc->dc_status)(self,geom,head,status);	
	
	*status |= ro;
	return err;
}




