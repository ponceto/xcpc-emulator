/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2002  John Elliott <seasip.webmaster@gmail.com>            *
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

/* RPC functions: client side.
 *
 * The only functions I've implemented are those needed by drvwin32.c */

#include "drvi.h"
#include "rpcfuncs.h"
#include "remote.h"

#define SMALLBUF 200
#define LARGEBUF 9000

dsk_err_t dsk_r_open(DSK_PDRIVER self, RPCFUNC func, unsigned int *nDriver, const char *filename, const char *type, const char *comp)
{
	unsigned char ibuf[PATH_MAX + 100], *iptr = ibuf;
	unsigned char obuf[20]            , *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	int32 nd;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_OPEN); if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, filename);	   if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, type);	   if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, comp);	   if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen); if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i32  (&optr, &olen, &nd);		   if (err) return err;
	*nDriver = (unsigned int)nd;
	return err2;
}


dsk_err_t dsk_r_creat(DSK_PDRIVER self,RPCFUNC func, unsigned int *nDriver, const char *filename, const char *type, const char *comp)
{
	unsigned char ibuf[PATH_MAX + 100], *iptr = ibuf;
	unsigned char obuf[20]            , *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	int32 nd;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_CREAT); if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, filename);	    if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, type);	    if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, comp);	    if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	    if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i32  (&optr, &olen, &nd);	    if (err) return err;
	*nDriver = (unsigned int)nd;
	return err2;
}



dsk_err_t dsk_r_close(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver)
{
	unsigned char ibuf[20], *iptr = ibuf;
	unsigned char obuf[20], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_CLOSE);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);  if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen); if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	return err2;
}



dsk_err_t dsk_r_read (DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, const DSK_GEOMETRY *geom, void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned char *buf2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PREAD);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);  if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector);       if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_bytes(&optr, &olen, &buf2);	   if (err) return err;
	memcpy(buf, buf2, geom->dg_secsize);
	return err2;
}


dsk_err_t dsk_r_secid(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, const DSK_GEOMETRY *geom, dsk_pcyl_t cylinder, 
					  dsk_phead_t head, DSK_FORMAT *result)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PSECID);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);  if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_format(&optr, &olen, result);	   if (err) return err;
	return err2;
}




dsk_err_t dsk_r_write(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, const DSK_GEOMETRY *geom, const void *buf, dsk_pcyl_t cylinder,
					  dsk_phead_t head, dsk_psect_t sector)
{
	unsigned char ibuf[LARGEBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PWRITE);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);		   if (err) return err;
	err = dsk_pack_bytes (&iptr, &ilen, buf, geom->dg_secsize); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector);      if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	return err2;
}


dsk_err_t dsk_r_format(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, DSK_GEOMETRY *geom, dsk_pcyl_t cylinder, 
					  dsk_phead_t head, const DSK_FORMAT *format, unsigned char filler)
{
	unsigned char ibuf[300], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned int n;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PFORMAT);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);		   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	for (n = 0; n < geom->dg_sectors; n++)
	{
		err = dsk_pack_format(&iptr, &ilen, &format[n]); if (err) return err;
	}
	err = dsk_pack_i16   (&iptr, &ilen, filler);	   if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_geom (&optr, &olen, geom);		   if (err) return err;
	return err2;
}


dsk_err_t dsk_r_drive_status(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, const DSK_GEOMETRY *geom,
					  dsk_phead_t head, unsigned char *result)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	int16 i;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_DRIVE_STATUS);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);		   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i16  (&optr, &olen, &i); 	   if (err) return err;
	*result = (unsigned char)i;
	return DSK_ERR_OK;
}


dsk_err_t dsk_r_getgeom (DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, 
		DSK_GEOMETRY *geom)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_GETGEOM);
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_geom (&optr, &olen, geom);	   if (err) return err;
	return err2;
}



dsk_err_t dsk_r_pseek(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, 
	const DSK_GEOMETRY *geom, dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PSEEK); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	return err2;
}


dsk_err_t dsk_r_xread (DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, 
		const DSK_GEOMETRY *geom, void *buf, dsk_pcyl_t cylinder,
		dsk_phead_t head, dsk_pcyl_t cyl_expected, 
		dsk_phead_t head_expected, dsk_psect_t sector, 
		size_t sector_size, int *deleted)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned char *buf2;
	int32 del = deleted ? *deleted : 0;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_XREAD);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);  if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cyl_expected); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head_expected);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector);       if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector_size);  if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, del);          if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_bytes(&optr, &olen, &buf2);	   if (err) return err;
	err = dsk_unpack_i32  (&optr, &olen, &del);        if (err) return err;
	memcpy(buf, buf2, sector_size);
	if (deleted) *deleted = del;
	return err2;

}

dsk_err_t dsk_r_xwrite(DSK_PDRIVER self, RPCFUNC func, unsigned int nDriver, 
		const DSK_GEOMETRY *geom, const void *buf, dsk_pcyl_t cylinder,
		dsk_phead_t head, dsk_pcyl_t cyl_expected, 
		dsk_phead_t head_expected, dsk_psect_t sector, 
		size_t sector_size, int deleted)
{
	unsigned char ibuf[LARGEBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_XWRITE);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);  if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_bytes (&iptr, &ilen, buf, sector_size); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cyl_expected); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head_expected);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector);       if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, sector_size);  if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, deleted);      if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	return err2;
}


dsk_err_t dsk_r_tread(DSK_DRIVER *self, RPCFUNC func, unsigned int nDriver,
		const DSK_GEOMETRY *geom, void *buf, dsk_pcyl_t cylinder, 
		dsk_phead_t head)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned char *buf2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PTREAD);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_bytes(&optr, &olen, &buf2);	   if (err) return err;
	memcpy(buf, buf2, geom->dg_secsize * geom->dg_sectors);
	return err2;

}



dsk_err_t dsk_r_xtread(DSK_DRIVER *self, RPCFUNC func, unsigned int nDriver,
		const DSK_GEOMETRY *geom, void *buf, dsk_pcyl_t cylinder, 
		dsk_phead_t head, dsk_pcyl_t cyl_expected, 
		dsk_phead_t head_expected)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned char *buf2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_XTREAD);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cyl_expected); if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head_expected);if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_bytes(&optr, &olen, &buf2);	   if (err) return err;
	memcpy(buf, buf2, geom->dg_secsize * geom->dg_sectors);
	return err2;
}


dsk_err_t dsk_r_option_enum(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		int idx, char **optname)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	/* Make the output buffer static; we will be returning a pointer 
	 * into it. */
	static unsigned char obuf[SMALLBUF];
	unsigned char *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	char *desc;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_OPTION_ENUM); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, idx);	   if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_string(&optr, &olen, &desc); 
	if (err) return err;
	if (optname) *optname = desc;
	return err2;
}




dsk_err_t dsk_r_option_set(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		const char *optname, int value)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_OPTION_SET); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, optname);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, value);	   if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	return err2;
}


dsk_err_t dsk_r_option_get(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		const char *optname, int *value)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[SMALLBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	int32 result;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_OPTION_GET); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, optname);	   if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i32  (&optr, &olen, &result);	   if (err) return err;
	if (value) *value = result;
	return err2;
}

dsk_err_t dsk_r_get_comment(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		char **comment)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	/* Make the output buffer static; we will be returning a pointer 
	 * into it. */
	static unsigned char obuf[2*SMALLBUF];
	unsigned char *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	char *desc;
	if (comment) *comment = NULL;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_GETCOMMENT); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_string(&optr, &olen, &desc); 
	if (err) return err;
	if (comment) *comment = desc;
	return err2;
}


dsk_err_t dsk_r_set_comment(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		const char *comment)
{
	unsigned char ibuf[2*SMALLBUF], *iptr = ibuf;
	unsigned char obuf[2*SMALLBUF];
	unsigned char *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_SETCOMMENT); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_string(&iptr, &ilen, comment);	   if (err) return err;
	
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err) return err;
	return err2;
}







dsk_err_t dsk_r_trackids(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		const DSK_GEOMETRY *geom, dsk_pcyl_t cylinder, 
		dsk_phead_t head, dsk_psect_t *count, DSK_FORMAT **result)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	int32 rcount, n;
	DSK_FORMAT *buf;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_TRACKIDS);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i32  (&optr, &olen, &rcount);	   if (err) return err;
	*count = rcount;
	*result = NULL;
	if (rcount)
	{
		buf = dsk_malloc(rcount * sizeof(DSK_FORMAT));
		for (n = 0; n < rcount; n++)
		{
			err = dsk_unpack_format(&optr, &olen, &buf[n]);
    			if (err) return err;
		}
		*result = buf;
	}
	return err2;
}







dsk_err_t dsk_r_rtread(DSK_DRIVER *self, RPCFUNC func, unsigned nDriver,
		const DSK_GEOMETRY *geom, void *buf, dsk_pcyl_t cylinder, 
		dsk_phead_t head, int reserved, size_t *bufsize)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[LARGEBUF], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	unsigned char *buf2;
	int32 buflen;

	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_RTREAD);if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = dsk_pack_geom  (&iptr, &ilen, geom);	   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, cylinder);     if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, head);         if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, reserved);     if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);	   if (err) return err;
	err = dsk_unpack_err  (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i32  (&optr, &olen, &buflen);	   if (err) return err;
	err = dsk_unpack_bytes(&optr, &olen, &buf2);	   if (err) return err;
	memcpy(buf, buf2, buflen); 
	*bufsize = buflen;
	return err2;
}





dsk_err_t dsk_r_properties(DSK_PDRIVER self, RPCFUNC func,
		unsigned int nDriver)
{
	unsigned char ibuf[SMALLBUF], *iptr = ibuf;
	unsigned char obuf[400], *optr = obuf;
	dsk_err_t err;
	int ilen = sizeof ibuf;
	int olen = sizeof obuf;
	dsk_err_t err2;
	int16 fncount, fnid;
	int n;
	char *desc;

	if (!self || !self->dr_remote) return DSK_ERR_BADPTR;
	err = dsk_pack_i16   (&iptr, &ilen, RPC_DSK_PROPERTIES); 
							   if (err) return err;
	err = dsk_pack_i32   (&iptr, &ilen, nDriver);	   if (err) return err;
	err = (*func)(self, ibuf, iptr - ibuf, obuf, &olen);
							   if (err) return err;
	err = dsk_unpack_err (&optr, &olen, &err2);	   if (err) return err;
	if (err2 == DSK_ERR_UNKRPC) return err2;
	err = dsk_unpack_i16 (&optr, &olen, &fncount);	   if (err) return err;
	if (self->dr_remote->rd_functions != NULL) 
	{
		dsk_free(self->dr_remote->rd_functions);
		self->dr_remote->rd_functions = NULL;	
	}
	if (fncount)
	{
		self->dr_remote->rd_functions 
			= dsk_malloc((fncount+1) * sizeof(unsigned));
		if (!self->dr_remote->rd_functions)
		{
			err2 = DSK_ERR_NOMEM;
		}
		else 
		{
			for (n = 0; n < fncount; n++)
			{
				err = dsk_unpack_i16 (&optr, &olen, &fnid);
				if (err) return err;
				self->dr_remote->rd_functions[n] = fnid;
			}
			self->dr_remote->rd_functions[n] = 0;
		}
	}
	err = dsk_unpack_string(&optr, &olen, &desc); 
	if (err) return err;
	if (self->dr_remote->rd_name) dsk_free(self->dr_remote->rd_name);
	self->dr_remote->rd_name = NULL;
	if (desc)
	{
		self->dr_remote->rd_name = dsk_malloc(1 + strlen(desc));
		if (!self->dr_remote->rd_name) err2 = DSK_ERR_NOMEM;
		else strcpy(self->dr_remote->rd_name, desc);
	}
	return err2;
}




