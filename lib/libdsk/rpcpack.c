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

/* RPC functions: Packing & unpacking */

#include "drvi.h"
#include "rpcfuncs.h"


/* RPC unpack functions */
dsk_err_t dsk_unpack_i16(unsigned char **input, int *inp_len, int16 *function)
	{
	if (*inp_len < 2) return DSK_ERR_RPC;
	
	(*inp_len) -= 2;
	*function  = ((int16)**input) << 8;
	++(*input);
	*function |= **input;
	++(*input);
	return DSK_ERR_OK;
	}


dsk_err_t dsk_unpack_i32(unsigned char **input, int *inp_len, int32 *function)
	{
	if (*inp_len < 4) return DSK_ERR_RPC;
	
	(*inp_len) -= 4;
	*function  = ((int32)**input) << 24;
	++(*input);
	*function |= ((int32)**input) << 16;
	++(*input);
	*function |= ((int32)**input) << 8;
	++(*input);
	*function |= **input;
	++(*input);
	return DSK_ERR_OK;
	}

dsk_err_t dsk_unpack_err(unsigned char **input, int *inp_len, dsk_err_t *value)
	{
	signed short val = DSK_ERR_OK;
	dsk_err_t err;

	err = dsk_unpack_i16(input, inp_len, (int16 *)&val);
	*value = (dsk_err_t)val;
	return err;
	}


/* Memory blocks are stored as 2-byte length, followed by data. */ 
dsk_err_t dsk_unpack_bytes(unsigned char **input, int *inp_len, unsigned char **buf)
	{
	dsk_err_t err;
	int16 len;

	err = dsk_unpack_i16(input, inp_len, &len); if (err) return err;
	if (inp_len[0] < (int)len) return DSK_ERR_RPC;
	*buf = *input;

	/* Null pointers are encoded as 0-byte blocks */
	if (len == 0)
	{
		*buf = NULL;
	}

	/* *buf = dsk_malloc(len); if (!buf) return DSK_ERR_NOMEM;
	memcpy(*buf, *input, len);
	*/
	(*input)   += len;
	(*inp_len) -= len;

	return DSK_ERR_OK;
	}

/* Strings are stored as memory blocks */
dsk_err_t dsk_unpack_string(unsigned char **input, int *inp_len, char **buf)
	{
	return dsk_unpack_bytes(input, inp_len, (unsigned char **)buf); 
	}

dsk_err_t dsk_unpack_geom  (unsigned char **input, int *inp_len, DSK_GEOMETRY *g)
	{
	dsk_err_t err;
	int16 i = 0;

	err = dsk_unpack_i16(input, inp_len, &i); g->dg_sidedness = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_cylinders = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_heads     = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_sectors   = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_secbase   = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_secsize   = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_datarate  = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_rwgap     = (unsigned char)i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_fmtgap    = (unsigned char)i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_fm        = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_nomulti   = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); g->dg_noskip    = i; if (err) return err;
	return DSK_ERR_OK;
	}


dsk_err_t dsk_unpack_format(unsigned char **input, int *inp_len, DSK_FORMAT *f)
	{
	dsk_err_t err;
	int16 i = 0;

	err = dsk_unpack_i16(input, inp_len, &i); f->fmt_cylinder = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); f->fmt_head     = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); f->fmt_sector   = i; if (err) return err;
	err = dsk_unpack_i16(input, inp_len, &i); f->fmt_secsize  = i; if (err) return err;
	return DSK_ERR_OK;
	}

/* ///////////////////////////////////////////////////////////////////////// */


/* RPC pack functions */
dsk_err_t dsk_pack_i16(unsigned char **output, int *out_len, int16 value)
	{
	if (*out_len < 2) return DSK_ERR_RPC;

	**output = (unsigned char)(value >> 8);
	++(*output);
	**output = (unsigned char)(value & 0xFF);
	++(*output);
	(*out_len) -= 2;
	return DSK_ERR_OK;
	}


dsk_err_t dsk_pack_i32(unsigned char **output, int *out_len, int32 value)
	{
	if (*out_len < 4) return DSK_ERR_RPC;

	**output = (unsigned char)(value >> 24);
	++(*output);
	**output = (unsigned char)((value >> 16) & 0xFF);
	++(*output);
	**output = (unsigned char)((value >> 8) & 0xFF);
	++(*output);
	**output = (unsigned char)(value & 0xFF);
	++(*output);
	(*out_len) -= 4;
	return DSK_ERR_OK;
	}

dsk_err_t dsk_pack_err(unsigned char **output, int *out_len, dsk_err_t value)
	{
	/* signed short serr = (signed short)value; */

	return dsk_pack_i16(output, out_len, (int16)value);
	}


dsk_err_t dsk_pack_bytes(unsigned char **output, int *out_len,  const unsigned char *buf, size_t len)
	{
	dsk_err_t err;

	if (*out_len < (int)(len+2)) return DSK_ERR_RPC;

	if (((int16)len) != len) return DSK_ERR_RPC;

	err = dsk_pack_i16(output, out_len, (int16)len); if (err) return err;
	memcpy(*output, buf, len);
	(*output)  += len;
	(*out_len) -= len;
	return DSK_ERR_OK;
	}

dsk_err_t dsk_pack_string(unsigned char **output, int *out_len, const char *str)
	{
	if (str == NULL) return dsk_pack_bytes(output, out_len, NULL, 0);
	else			 return dsk_pack_bytes(output, out_len, (const unsigned char *)str, 1 + strlen(str));
	}



dsk_err_t dsk_pack_geom  (unsigned char **output, int *out_len, const DSK_GEOMETRY *g)
	{
	dsk_err_t err;

	err = dsk_pack_i16(output, out_len, (int16)g->dg_sidedness); if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_cylinders); if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_heads);     if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_sectors);   if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_secbase);   if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_secsize);   if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_datarate);  if (err) return err;
	err = dsk_pack_i16(output, out_len, g->dg_rwgap);     if (err) return err;
	err = dsk_pack_i16(output, out_len, g->dg_fmtgap);    if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_fm);   if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_nomulti);   if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)g->dg_noskip);    if (err) return err;
	return DSK_ERR_OK;
	}



dsk_err_t dsk_pack_format(unsigned char **output, int *out_len, const DSK_FORMAT *f)
	{
	dsk_err_t err;

	err = dsk_pack_i16(output, out_len, (int16)f->fmt_cylinder);  if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)f->fmt_head);      if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)f->fmt_sector);    if (err) return err;
	err = dsk_pack_i16(output, out_len, (int16)f->fmt_secsize);   if (err) return err;
	return DSK_ERR_OK;
	}
