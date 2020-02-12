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
/* strerror() for all possible errors */

#include "drvi.h"
#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

LDPUBLIC32 char * LDPUBLIC16 dsk_strerror(dsk_err_t err)
{
	switch(err)
	{
		case DSK_ERR_OK:	return "No error.";
		case DSK_ERR_BADPTR:	return "Bad pointer passed to libdsk.";
		case DSK_ERR_DIVZERO:	return "Division by zero.";
		case DSK_ERR_BADPARM:	return "Bad parameter";
		case DSK_ERR_NODRVR:	return "Requested driver not found";
		case DSK_ERR_NOTME:	return "Disc rejected by driver.";
		case DSK_ERR_SYSERR:	return strerror(errno);
		case DSK_ERR_NOMEM:	return "Out of memory.";
		case DSK_ERR_NOTIMPL:	return "Driver does not support this function.";
		case DSK_ERR_MISMATCH:	return "Sector on disc does not match buffer.";
		case DSK_ERR_NOTRDY:	return "Drive not ready.";
		case DSK_ERR_RDONLY:	return "Disc is read-only.";
		case DSK_ERR_SEEKFAIL:	return "Seek fail.";
		case DSK_ERR_DATAERR:	return "Data error.";
		case DSK_ERR_NODATA:	return "No data.";
		case DSK_ERR_NOADDR:	return "Missing address mark.";
		case DSK_ERR_BADFMT:	return "Bad format.";
                case DSK_ERR_CHANGED:	return "Disc changed.";
		case DSK_ERR_ECHECK:	return "Equipment check.";
		case DSK_ERR_OVERRUN:	return "Overrun.";
		case DSK_ERR_ACCESS:	return "Access denied.";
                case DSK_ERR_CTRLR:	return "Controller failed.";
                case DSK_ERR_COMPRESS:	return "Compressed file is corrupt.";
		case DSK_ERR_RPC:	return "Invalid RPC packet.";
		case DSK_ERR_BADOPT:	return "Requested driver-specific feature not available.";
		case DSK_ERR_BADVAL:	return "Bad value for driver-specific feature.";
		case DSK_ERR_ABORT:	return "Operation was cancelled by user.";
		case DSK_ERR_TIMEOUT:	return "Timed out waiting for response.";
		case DSK_ERR_UNKRPC:	return "RPC server did not recognise function.";
		case DSK_ERR_BADMEDIA:	return "Disc is not suitable for this operation";
		case DSK_ERR_UNKNOWN:	return "Controller returned unknown error.";
 	}
	return "Unknown error.";
}


#ifdef TRACE_MALLOCS
#include <stdio.h>
#include <assert.h>
void *dsk_malloc(size_t size)
{
	void *ptr;

	assert(size);	
	printf("malloc(%d) ", size);
	fflush(stdout);
	ptr = malloc(size);
	printf("returns %x\n", (int)ptr);
	return ptr;
}


void  dsk_free(void *ptr)
{
	printf("free(%x)\n", (int)ptr);
	free(ptr);
}

#endif

