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

/* RPC functions: Server side */

#include "drvi.h"
#include "rpcfuncs.h"

/* Decode an RPC packet and execute it.
 * If nRefCount is nonzero, it is increased each time a dsk_open or dsk_creat 
 * succeeds, and decreased when a dsk_close succeeds. This allows a server 
 * program to terminate automatically when the reference count reaches 0. 
 *
 * The only RPC codes that are currently obeyed are those sent by drvwin32.c
 * through rpccli.c
 */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_rpc_server(unsigned char *input,  int inp_len,
	   		unsigned char *output, int *out_len, int *nRefCount)
	{
	dsk_err_t err, err2;
	int16 function;
	DSK_DRIVER *pDriver;
	DSK_GEOMETRY geom;
	DSK_FORMAT fmt, *pfmt;
	int32 nd, int1, int2, int3, int4, int5, int6, int7;
	int16 filler;
	char  *str1, *str2, *str3;
	dsk_psect_t rcount;
	DSK_FORMAT *trkids;
	unsigned int nDriver, n;
	unsigned char secbuf[8192], *pbuf;
	unsigned char status;
	int deleted, value;
	int16 props[sizeof(DRV_CLASS)];

	err = dsk_unpack_i16(&input, &inp_len, &function); if (err) return err;
	switch(function)
		{
		case RPC_DSK_OPEN:
				err = dsk_unpack_string(&input, &inp_len, &str1); if (err) return err;
				err = dsk_unpack_string(&input, &inp_len, &str2); if (err) return err;
				err = dsk_unpack_string(&input, &inp_len, &str3); if (err) return err;
				err2= dsk_open(&pDriver, str1, str2, str3);
				err = dsk_map_dtoi(pDriver, &nDriver);            if (err) return err;
				err = dsk_pack_err  (&output, out_len, err2);    if (err) return err;
				err = dsk_pack_i32  (&output, out_len, nDriver); if (err) return err;
				if (nRefCount && err2 == DSK_ERR_OK) ++(*nRefCount);
				return DSK_ERR_OK;
		case RPC_DSK_CREAT:
				err = dsk_unpack_string(&input, &inp_len, &str1); if (err) return err;
				err = dsk_unpack_string(&input, &inp_len, &str2); if (err) return err;
				err = dsk_unpack_string(&input, &inp_len, &str3); if (err) return err;
				err2= dsk_creat(&pDriver, str1, str2, str3);
				err = dsk_map_dtoi(pDriver, &nDriver);            if (err) return err;
				err = dsk_pack_err  (&output, out_len, err2);    if (err) return err;
				err = dsk_pack_i32  (&output, out_len, nDriver); if (err) return err;
				if (nRefCount && err2 == DSK_ERR_OK) ++(*nRefCount);
				return DSK_ERR_OK;
		case RPC_DSK_CLOSE:
				err = dsk_unpack_i32(&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_close(&pDriver);
				err = dsk_pack_err(&output,  out_len, err2);	  if (err) return err;
				if (nRefCount) --(*nRefCount);
				return DSK_ERR_OK;
		case RPC_DSK_PREAD:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_pread(pDriver, &geom, secbuf, (dsk_pcyl_t)int1, (dsk_phead_t)int2, (dsk_psect_t)int3);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_bytes(&output, out_len, secbuf, geom.dg_secsize); if (err) return err;
				return DSK_ERR_OK;
		case RPC_DSK_PWRITE:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_bytes(&input, &inp_len, &pbuf);  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_pwrite(pDriver, &geom, pbuf, (dsk_pcyl_t)int1, (dsk_phead_t)int2, (dsk_psect_t)int3);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_PFORMAT:
				pfmt = (DSK_FORMAT *)secbuf;
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				for (n = 0; n < geom.dg_sectors; n++)
					{
					err = dsk_unpack_format(&input, &inp_len, &pfmt[n]); if (err) return err;
					}
				err = dsk_unpack_i16 (&input, &inp_len, &filler);  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_pformat(pDriver, &geom, (dsk_pcyl_t)int1, (dsk_phead_t)int2, pfmt, (unsigned char)filler);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_geom(&output, out_len, &geom); if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_PSECID:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_psecid(pDriver, &geom, (dsk_pcyl_t)int1, (dsk_phead_t)int2, &fmt);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_format(&output, out_len, &fmt); if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_DRIVE_STATUS:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_drive_status(pDriver, &geom, (dsk_phead_t)int1, &status);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_i16(&output, out_len, status); if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_GETGEOM:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_map_itod(nDriver, &pDriver);		  if (err) return err;
				err2= dsk_getgeom(pDriver, &geom);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_geom(&output, out_len, &geom);	  if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_PSEEK:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);		  if (err) return err;
				err2= dsk_pseek(pDriver, &geom, (dsk_pcyl_t)int1, (dsk_phead_t)int2);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_XREAD:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int4);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int5);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int6);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int7);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				deleted = int7;
				err2= dsk_xread(pDriver, &geom, secbuf, 
						(dsk_pcyl_t)int1, 
						(dsk_phead_t)int2, 
						(dsk_pcyl_t)int3, 
						(dsk_phead_t)int4, 
						(dsk_psect_t)int5,
						(size_t)int6,
						&deleted);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_bytes(&output, out_len, secbuf, int6); if (err) return err;
				err = dsk_pack_i32(&output, out_len, deleted);
				return DSK_ERR_OK;
		case RPC_DSK_XWRITE:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_bytes(&input, &inp_len, &pbuf);  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int4);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int5);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int6);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int7);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_xwrite(pDriver, &geom, pbuf, 
						(dsk_pcyl_t)int1, 
						(dsk_phead_t)int2, 
						(dsk_pcyl_t)int3, 
						(dsk_phead_t)int4, 
						(dsk_psect_t)int5,
						(size_t)int6,
						(int)int7);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				return DSK_ERR_OK;
		case RPC_DSK_TRACKIDS:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2 = dsk_ptrackids(pDriver, &geom, int1,
						int2, &rcount, &trkids);
				if (err2 || !trkids) rcount = 0;
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_i32(&output, out_len, rcount);	  if (err) return err;
				for (n = 0; n < rcount; n++)
				{
					err = dsk_pack_format(&output, out_len, &trkids[n]);
					if (err) return err;
				}
				if (trkids) dsk_free(trkids);
				return DSK_ERR_OK;	
		case RPC_DSK_PTREAD:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_ptread(pDriver, &geom, secbuf, (dsk_pcyl_t)int1, (dsk_phead_t)int2);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_bytes(&output, out_len, secbuf, geom.dg_secsize * geom.dg_sectors); if (err) return err;
				return DSK_ERR_OK;
		case RPC_DSK_XTREAD:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int4);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_xtread(pDriver, &geom, secbuf, (dsk_pcyl_t)int1, (dsk_phead_t)int2, (dsk_pcyl_t)int3, (dsk_phead_t)int4);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_bytes(&output, out_len, secbuf, geom.dg_secsize * geom.dg_sectors); if (err) return err;
				return DSK_ERR_OK;

		case RPC_DSK_OPTION_ENUM:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_option_enum(pDriver, int1, &str1);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_string(&output, out_len, str1);	  if (err) return err;	
				return DSK_ERR_OK;
		case RPC_DSK_OPTION_SET:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_string(&input, &inp_len, &str1); if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_set_option(pDriver, str1, int1);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				return DSK_ERR_OK;
		case RPC_DSK_OPTION_GET:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_string(&input, &inp_len, &str1); if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_get_option(pDriver, str1, &value);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_i32 (&output, out_len, value);	  if (err) return err;
				return DSK_ERR_OK;
		case RPC_DSK_GETCOMMENT:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_get_comment(pDriver, &str1);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				err = dsk_pack_string(&output, out_len, str1);	  if (err) return err;	
				return DSK_ERR_OK;
		case RPC_DSK_SETCOMMENT:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err = dsk_unpack_string(&input, &inp_len, &str1); if (err) return err;
				err2= dsk_set_comment(pDriver, str1);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
				return DSK_ERR_OK;


		case RPC_DSK_RTREAD:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_unpack_geom(&input, &inp_len, &geom);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int1);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int2);	  if (err) return err;
				err = dsk_unpack_i32 (&input, &inp_len, &int3);	  if (err) return err;
				err = dsk_map_itod(nDriver, &pDriver);			  if (err) return err;
				err2= dsk_rtread(pDriver, &geom, secbuf, (dsk_pcyl_t)int1, (dsk_phead_t)int2, int3);
				err = dsk_pack_err(&output, out_len, err2);	  if (err) return err;
/* XXX This is a completely arbitrary buffer size */
				err = dsk_pack_i32(&output, out_len, geom.dg_secsize * geom.dg_sectors);  if (err) return err;
				err = dsk_pack_bytes(&output, out_len, secbuf, geom.dg_secsize * geom.dg_sectors); if (err) return err;
				return DSK_ERR_OK;


/* Return a list of implemented functions */
		case RPC_DSK_PROPERTIES:
				err = dsk_unpack_i32 (&input, &inp_len, &nd);	  if (err) return err;	nDriver = (unsigned int)nd;
				err = dsk_map_itod(nDriver, &pDriver);		  if (err) return err;
				int1 = 0;	/* Count of implemented functions */
#define PROPCHECK(func, rid) if (pDriver->dr_class->func) props[int1++] = rid;
				PROPCHECK(dc_open,    RPC_DSK_OPEN)
				PROPCHECK(dc_creat,   RPC_DSK_CREAT)
				PROPCHECK(dc_close,   RPC_DSK_CLOSE)
				PROPCHECK(dc_read,    RPC_DSK_PREAD)
				PROPCHECK(dc_write,   RPC_DSK_PWRITE)
				PROPCHECK(dc_format,  RPC_DSK_PFORMAT)
				PROPCHECK(dc_secid,   RPC_DSK_PSECID)
				PROPCHECK(dc_getgeom, RPC_DSK_GETGEOM)
				PROPCHECK(dc_xseek,   RPC_DSK_PSEEK)
				PROPCHECK(dc_status,  RPC_DSK_DRIVE_STATUS)
				PROPCHECK(dc_xread,   RPC_DSK_XREAD)
				PROPCHECK(dc_xwrite,  RPC_DSK_XWRITE)
				PROPCHECK(dc_tread,   RPC_DSK_PTREAD)
				PROPCHECK(dc_xtread,  RPC_DSK_XTREAD)
				PROPCHECK(dc_option_enum, RPC_DSK_OPTION_ENUM)
				PROPCHECK(dc_option_get, RPC_DSK_OPTION_GET)
				PROPCHECK(dc_option_set, RPC_DSK_OPTION_SET)
				PROPCHECK(dc_trackids, RPC_DSK_TRACKIDS)
				PROPCHECK(dc_rtread, RPC_DSK_RTREAD)
#undef PROPCHECK
				props[int1++] = RPC_DSK_PROPERTIES;
				err = dsk_pack_err(&output, out_len, DSK_ERR_OK);	  if (err) return err;
				err = dsk_pack_i16(&output, out_len, (unsigned short)int1); if (err) return err;
				for (int2 = 0; int2 < int1; int2++)
				{
					err = dsk_pack_i16(&output, out_len, props[int2]); if (err) return err;
				}
				err = dsk_pack_string(&output, out_len, pDriver->dr_class->dc_description); if (err) return err;
				return DSK_ERR_OK;

/* Unsupported function. Return DSK_ERR_UNKRPC */
		default: 
				err2 = dsk_pack_err(&output, out_len, 
						DSK_ERR_UNKRPC);
				return DSK_ERR_OK;
		}

	}



