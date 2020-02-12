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


LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_set_comment(DSK_PDRIVER self, const char *comment)
{
	if (!self || !comment) return DSK_ERR_BADPTR;

	if (self->dr_comment) dsk_free(self->dr_comment);
	self->dr_comment = dsk_malloc(1 + strlen(comment));
	if (!self->dr_comment) return DSK_ERR_NOMEM;
	strcpy(self->dr_comment, comment);
	self->dr_dirty = 1;	/* Comment will want writing back */
	return DSK_ERR_OK;

}


LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_get_comment(DSK_PDRIVER self, char **comment)
{
	if (!self || !comment) return DSK_ERR_BADPTR;
	*comment = self->dr_comment;
	return DSK_ERR_OK;
}

