/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2003  John Elliott <seasip.webmaster@gmail.com>            *
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



/* Set / get the retry count. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_set_retry(DSK_PDRIVER self, unsigned int count)
{
	if (!self) return DSK_ERR_BADPTR;
        if (count == 0) return DSK_ERR_BADVAL;
        self->dr_retry_count = count;
        return DSK_ERR_OK;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_get_retry(DSK_PDRIVER self, unsigned int *count)
{
        if (!count || !self) return DSK_ERR_BADPTR;
        *count = self->dr_retry_count;
        return DSK_ERR_OK;
}

