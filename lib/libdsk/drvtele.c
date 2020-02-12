/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2,2007  John Elliott <seasip.webmaster@gmail.com>     *
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

/* This driver provides limited read-only support for Teledisk files. It is
 * based entirely on the file format documentation at
 * <http://www.fpns.net/willy/wteledsk.htm>. No code from wteledsk has been
 * used in this file, since it is under GPL and LibDsk is under LGPL.
 *
 * (GPL code from wteledsk was incorporated into comptlzh.c / comptlzh.h;
 *  in that case, it was relicensed as LGPL with permission).
 *
 * Current bugs / limitations:
 * - No write support. The Teledisk format isn't really adapted to 
 *   in-place rewrites of the sort that LibDsk likes to do, but in 
 *   theory it would be possible to support writing to sectors that are
 *   stored uncompressed (type 0). This would require updating sector CRCs.
 *
 * - No support for images split into multiple files (.TD0, .TD1, .TD2...)
 *   
 * - No support for advanced compression. Rather than having it built-in to 
 *   this driver, we use a compression module to convert advanced-compression 
 *   TD0 files to normal, using code from wteledsk.
 *
 *   There's also an "old advanced" compression, which uses 12-bit LZ 
 *   compression in 6k blocks, so another 'comp' driver would be needed for 
 *   that.
 *
 */


#include "drvi.h"
#include "drvtele.h"

extern unsigned short teledisk_crc(unsigned char *buf, unsigned short len);

DRV_CLASS dc_tele = 
{
	sizeof(TELE_DSK_DRIVER),
	"tele",
	"TeleDisk file driver",
	tele_open,
	NULL,			/* tele_creat */
	tele_close,
	tele_read,
	NULL,			/* tele_write */
	NULL,			/* tele_format */
	tele_getgeom,
	tele_secid,
	tele_xseek,
	tele_status,
	tele_xread,
	NULL,			/* tele_xwrite */
	NULL,			/* tele_tread */
	NULL,			/* tele_xtread */
	tele_option_enum,
	tele_option_set,
	tele_option_get,
	NULL,			/* tele_trackids */
	NULL			/* tele_rtread */
};

/* #define MONITOR(x) printf x     */

#define MONITOR(x) 


#define DC_CHECK(s) \
	TELE_DSK_DRIVER *self; \
	if (s->dr_class != &dc_tele) return DSK_ERR_BADPTR; \
	self = (TELE_DSK_DRIVER *)s;

/* Check that the data rate and recording mode match those used when the
 * disk was imaged. */
static dsk_err_t check_rate(TELE_DSK_DRIVER *self, const DSK_GEOMETRY *geom)
{
	int fm;

	switch(self->tele_head.datarate & 0x7F)
	{
/* Allow SD / DD interchangeably with 250/300. */
		case 0: 
		case 1: if (geom->dg_datarate != RATE_SD &&
			    geom->dg_datarate != RATE_DD) return DSK_ERR_NOADDR;
			break;
		case 2: if (geom->dg_datarate != RATE_HD) return DSK_ERR_NOADDR;
			break;
	}
	fm = ((geom->dg_fm & RECMODE_MASK) == RECMODE_FM) ? 0x80 : 0;
	if (fm != (self->tele_head.datarate & 0x80))
	{
		return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}

static dsk_err_t tele_fread(TELE_DSK_DRIVER *self, tele_byte *buf, int count)
{
	if (!buf) 
	{
		if (fseek(self->tele_fp, count, SEEK_CUR))
		{
			return DSK_ERR_SYSERR;
		}
	}
	else
	{
		if (fread(buf, 1, count, self->tele_fp) < (size_t)count)
		{
			return DSK_ERR_SYSERR;
		}
	}
	return DSK_ERR_OK;
}

static dsk_err_t tele_readsechead(TELE_DSK_DRIVER *self)
{
	dsk_err_t err;
	tele_byte buf[6];

	memset(&self->tele_sechead, 0, sizeof(self->tele_sechead));
	err = tele_fread(self, buf, 6);
	if (err) return err;
	self->tele_sechead.cylinder_id = buf[0];
	self->tele_sechead.head_id     = buf[1];
	self->tele_sechead.sector_id   = buf[2];
	self->tele_sechead.sector_len  = (128 << buf[3]);
	self->tele_sechead.syndrome    = buf[4];
	self->tele_sechead.header_crc  = buf[5];

	/* Syndromes 0x10 and 0x20 omit sector data */
	if ((self->tele_sechead.syndrome & 0x30) == 0)
	{
		err = tele_fread(self, buf, 3);
		if (err) return err;
		self->tele_sechead.compressed_len = (((tele_word)buf[1]) << 8) |
						buf[0];
		self->tele_sechead.encoding   = buf[2];
	}
	return DSK_ERR_OK;
}

/* Read the sector described by tele_sechead */
static dsk_err_t tele_readsec(TELE_DSK_DRIVER *self, unsigned char *buf)
{
	int n, w, wleft, plen, ptype;
	dsk_err_t err;
	size_t len = self->tele_sechead.sector_len;
	tele_byte pattern[257];

	if (self->tele_sechead.syndrome & 0x30)	/* Nothing there */
	{
		if (buf) memset(buf, 0xF6, len);
		return DSK_ERR_OK;
	}
	switch(self->tele_sechead.encoding)
	{
		case 0:	/* Uncompressed */
			MONITOR(("tele_readsec @ 0x%08lx: Type 0 len %d\n",
				 ftell(self->tele_fp), len));
			return tele_fread(self, buf, len);
		case 1:	/* One pattern */
			MONITOR(("tele_readsec @ 0x%08lx: Type 1 len 4\n",
				 ftell(self->tele_fp)));
			err = tele_fread(self, pattern, 4);
			if (err) return err;

			if (buf) for (n = 0; n < (int)len; n+= 2)
			{
				buf[n]   = pattern[2];
				buf[n+1] = pattern[3];
			}
			return DSK_ERR_OK;
		case 2:	/* More than one pattern */
			w = 0;
			MONITOR(("tele_readsec @ 0x%08lx: Type 2 \n",
				 ftell(self->tele_fp)));
			while ((size_t)w < len)
			{
				wleft = len - w;
				err = tele_fread(self, pattern, 2);
				if (err) return err;
				ptype = pattern[0];
				plen = pattern[1];
			MONITOR(("tele_readsec @ 0x%08lx: Type 2 ptype=%d plen=%d\n",
				 ftell(self->tele_fp), ptype, plen));
				if (ptype == 0)
				{
					err = tele_fread(self, pattern, plen);
					if (err) return err;	
					if (plen > wleft) plen = wleft;
					if (buf) memcpy(buf + w, pattern, plen);
					w += plen;
					continue;
				}
				err = tele_fread(self, pattern, (1 << ptype));
				for (n = 0; n < plen; n++)
				{
					if (buf)
					{
/* Ensure the amount of data written does not exceed len */
						if ((1 << ptype) > wleft)
							memcpy(buf + w,
							  pattern,wleft);	
						else	memcpy(buf + w,
							  pattern, 1 << ptype);
					}
					w += (1 << ptype);	
					wleft -= (1 << ptype);
				}
			}	
			return DSK_ERR_OK;
		default:	
#ifndef WIN16
			fprintf(stderr, "Teledisk: Unsupported sector compression method %d!\n", self->tele_sechead.encoding);
#endif
			break;
	}
	return DSK_ERR_NOTIMPL;
}

/* We cache previously-encountered track addresses in self->tele_track_addr
 * to avoid doing a linear search every time a track is needed. */
static dsk_err_t tele_seektrack(TELE_DSK_DRIVER *self, dsk_pcyl_t cylinder,
		dsk_phead_t head)
{
	long startpos;
	int c, h, s;
	dsk_err_t err;

	h = head;
	c = cylinder;
	startpos = 0;
	MONITOR(("tele_seektrack: cylinder=%d head=%d\n", cylinder, head));

	do
	{
		MONITOR(("tele_seektrack: Trying c%d h%d\n", c, h));
		if (c < 100 && h < 2 && self->tele_track_addr[c][h] != 0)
		{
			MONITOR(("tele_seektrack: starting at c%d h%d\n", c, h));
			startpos = self->tele_track_addr[c][h];
		}
		if (h == 0)
		{
			h = 1;
			--c;
		}
		else
		{
			--h;
		}
	}
	while (startpos == 0 && c >= 0);

	if (startpos == 0)
	{
#ifndef WIN16
		fprintf(stderr, "drv_tele internal error: Cannot find cylinder 0 head 0\n");
#endif
		return DSK_ERR_CTRLR;
	}

	if (fseek(self->tele_fp, startpos, SEEK_SET))
		return DSK_ERR_SYSERR;

	do
	{
		tele_byte buf[4];
		startpos = ftell(self->tele_fp);
		err = tele_fread(self, buf, 4);
		if (err) 
		{
			return DSK_ERR_NOADDR;
		}
		self->tele_trackhead.sectors  = buf[0];
		self->tele_trackhead.cylinder = buf[1];
		self->tele_trackhead.head     = buf[2];
		self->tele_trackhead.crc      = buf[3];
		c = self->tele_trackhead.cylinder;
		h = self->tele_trackhead.head;
		MONITOR(("tele_seektrack: c%d h%d at 0x%08lx\n", c, h, startpos));
		if (c < 100 && h < 2 && self->tele_track_addr[c][h] == 0)
		{
			self->tele_track_addr[c][h] = startpos;
		}
		if ((dsk_pcyl_t)c == cylinder && (dsk_phead_t)h == head)
		{
			return DSK_ERR_OK;
		}
		for (s = 0; s < self->tele_trackhead.sectors; s++)
		{
			err = tele_readsechead(self);	
			if (err) return err;
			err = tele_readsec(self, NULL);
			if (err) return err;
		}

	}
	while(1);
/* Should never happen, but need to shut Pacific C up */
	return DSK_ERR_UNKNOWN;
}

dsk_err_t tele_seeksec(TELE_DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		dsk_pcyl_t cylinder, dsk_phead_t head, dsk_pcyl_t cyl_expected,
		dsk_phead_t head_expected, dsk_psect_t sector, 
		size_t *request_len, size_t *sseclen)
{
	dsk_err_t err;
	int s;
	long pos;

	MONITOR(("tele_seeksec: Searching for c=%d h=%d s=%d on c=%d h=%d\n",
				cyl_expected, head_expected, sector,
				cylinder, head));
	err = tele_seektrack(self, cylinder, head);

	for (s = 0; s < self->tele_trackhead.sectors; s++)
	{
		pos = ftell(self->tele_fp);
		err = tele_readsechead(self);	
		if (err) return err;
		MONITOR(("tele_seeksec: c%d h%d s%d at 0x%08lx (%d/%d)\n",
				self->tele_sechead.cylinder_id, 
				self->tele_sechead.head_id, 
				self->tele_sechead.sector_id, pos,
				s, self->tele_trackhead.sectors));
		if (self->tele_sechead.sector_id   == sector   &&
		    self->tele_sechead.cylinder_id == cyl_expected &&
		    self->tele_sechead.head_id     == head_expected)
		{
			*sseclen = self->tele_sechead.sector_len;
/* Sector shorter than expected. Report a data error, and set
 * request_len to the actual size. */
			if (*sseclen < *request_len)
			{
				*request_len = *sseclen;
				err = DSK_ERR_DATAERR;
			}
/* Sector longer than expected. Report a data error but don't change 
 * request_len */
		        else if ((*sseclen) > (*request_len))
		        {
				err = DSK_ERR_DATAERR;
			}
			return err;
		}
		/* Skip over that sector and on to the next */
		err = tele_readsec(self, NULL);
	}
	return DSK_ERR_NOADDR;
}


dsk_err_t tele_open(DSK_DRIVER *s, const char *filename)
{
	unsigned char header[12];
	DC_CHECK(s);

	self->tele_fp = fopen(filename, "rb");
	if (!self->tele_fp) return DSK_ERR_NOTME;
	/* This does use fread not tele_fread, because it's not compressed
	 * by the Advanced compression */
	if (fread(header, 1, sizeof(header), self->tele_fp) < (int)sizeof(header) ||
	    (memcmp(header, "TD", 2) && memcmp(header, "td", 2)))
	{
		fclose(self->tele_fp);
		return DSK_ERR_NOTME;
	}
	memcpy(self->tele_head.magic, header, 2);
	self->tele_head.magic[2] = 0;
	self->tele_head.volume_seq  = header[2];
	self->tele_head.volume_id   = header[3];
	self->tele_head.ver         = header[4];
	self->tele_head.datarate    = header[5];
	self->tele_head.drivetype   = header[6];
	self->tele_head.doubletrack = header[7];
	self->tele_head.dosmode     = header[8];
	self->tele_head.sides       = header[9];
	self->tele_head.crc       = ((tele_word)header[11]) << 8 | header[10];

	/* Check header CRC. */
	if (teledisk_crc(header, 10) != self->tele_head.crc)
	{
		fclose(self->tele_fp);
		return DSK_ERR_NOTME;
	}

	/* Advanced compression not supported -- see separate comptele */
	if (!strcmp((char *)header, "td"))
	{
#ifndef WIN16
		fprintf(stderr, "LibDsk TD0 driver: Advanced compression not supported\n");
#endif
		fclose(self->tele_fp);
		return DSK_ERR_NOTIMPL;
	}
	/* Read a comment if there is one */
	if (self->tele_head.doubletrack & 0x80)
	{
		int comment_len, n;
		if (tele_fread(self, header, 10))
		{
			fclose(self->tele_fp);
			return DSK_ERR_SYSERR;
		}
		comment_len = ((int)(header[3])) << 8 | header[2];
		self->tele_comment = dsk_malloc(comment_len + sizeof(TELE_COMMENT));
		if (!self->tele_comment)
		{
			fclose(self->tele_fp);
			return DSK_ERR_NOMEM;
		}
		self->tele_comment->year = header[4] + 1900;
		self->tele_comment->mon = header[5];
		self->tele_comment->day = header[6];
		self->tele_comment->hour = header[7];
		self->tele_comment->min = header[8];
		self->tele_comment->sec = header[9];
		if (tele_fread(self, (tele_byte *)self->tele_comment->text, comment_len))
		{
			fclose(self->tele_fp);
			return DSK_ERR_SYSERR;
		}
		self->tele_comment->text[comment_len] = 0;
		for (n = 0; n < comment_len; n++)
		{
			if (self->tele_comment->text[n] == 0 &&
			    self->tele_comment->text[n+1] == 0)
			{
				self->tele_comment->text[n] = '\r';
				self->tele_comment->text[n+1] = '\n';
			}
		}	
		dsk_set_comment(s, self->tele_comment->text);
	}
	self->tele_track_addr[0][0] = ftell(self->tele_fp);

	return DSK_ERR_OK;
}

dsk_err_t tele_creat(DSK_DRIVER *self, const char *filename)
{
	return DSK_ERR_RDONLY;
}
dsk_err_t tele_close(DSK_DRIVER *s)
{
	DC_CHECK(s);

	if (fclose(self->tele_fp)) return DSK_ERR_SYSERR;
	if (self->tele_comment)
	{
		dsk_free(self->tele_comment);
		self->tele_comment = NULL;
	}	
	return DSK_ERR_OK;
}



dsk_err_t tele_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return DSK_ERR_RDONLY;
}
dsk_err_t tele_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	return DSK_ERR_RDONLY;
}
dsk_err_t tele_secid(DSK_DRIVER *s, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	unsigned int n, m;
	dsk_err_t err;

	DC_CHECK(s);

	/* Single-sided? */
	if (head > 0 && self->tele_head.sides < 2)
	{
		return DSK_ERR_NOADDR;
	}
	err = check_rate(self, geom); if (err) return err;
	err = tele_seektrack(self, cylinder, head);
	if (err) return err;

	m = self->tele_sector % self->tele_trackhead.sectors;
	++self->tele_sector;
	for (n = 0; n < m; n++)
	{
		err = tele_readsechead(self);	
		if (err) return err;
		err = tele_readsec(self, NULL);
		if (err) return err;
	}
	err = tele_readsechead(self);
	if (result)
	{
		result->fmt_cylinder = self->tele_sechead.cylinder_id;	
		result->fmt_head = self->tele_sechead.head_id;
		result->fmt_sector = self->tele_sechead.sector_id;
		result->fmt_secsize = self->tele_sechead.sector_len;
	}
	return DSK_ERR_OK;
}
dsk_err_t tele_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	return DSK_ERR_NOTIMPL;
}


dsk_err_t tele_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return tele_xread(self, geom, buf, cylinder, head, cylinder, 
		dg_x_head(geom, head),
		dg_x_sector(geom, head, sector), geom->dg_secsize, NULL);
}


dsk_err_t tele_xread(DSK_DRIVER *s, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, 
			      dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, 
			      int *deleted)
{
	dsk_err_t err = DSK_ERR_OK;
	unsigned char *secbuf;
	size_t len = sector_size;
	size_t sseclen;
	int want_deleted = 0;
	int try_again = 0;

	DC_CHECK(s);

	/* Single-sided? */
	if (head > 0 && self->tele_head.sides < 2)
	{
		return DSK_ERR_NOADDR;
	}

/* Check that the requested data rate is compatible. */
	err = check_rate(self, geom); if (err) return err;
	if (deleted && *deleted) want_deleted = 4;
	do
	{
		err = tele_seeksec(self, geom, cylinder, head, 
			cyl_expected, head_expected, 
			sector, &len, &sseclen);
/* Are we retrying because we are looking for deleted data and found 
 * nondeleted or vice versa?
 *
 * If so, and we have run out of sectors in this track, AND we are on head 0,
 * AND the disc has 2 heads, AND we are in multitrack mode, then look on head 1
 * as well. Amazing.
 * */
                if (try_again == 1 && err == DSK_ERR_NOADDR)
                {
                        err = DSK_ERR_NODATA;
                        if ((!geom->dg_nomulti) && head == 0 &&
                                  self->tele_head.sides > 1)
                        {
                                head++;
                                sector = geom->dg_secbase;
                                continue;
                        }
                }
		try_again = 0;
		if (err == DSK_ERR_NOADDR) self->tele_sector = 0;
		if (err != DSK_ERR_DATAERR && err != DSK_ERR_OK)
			return err;
		/* We have the sector. But does it contain deleted data? */
		if (deleted) *deleted = 0;
		if ((self->tele_sechead.syndrome & 4) != want_deleted)
		{
			if (geom->dg_noskip)
			{
				if (deleted) *deleted = 1;
			}	
			else
			{
				try_again = 1;
				++sector;
				continue;
			}
		}
		/* Read the whole sector into the buffer (if it's there) */
		secbuf = dsk_malloc(sseclen);
		if (!secbuf) return DSK_ERR_NOMEM;
		err = tele_readsec(self, secbuf);
		if (sseclen > sector_size) sseclen = sector_size;
		memcpy(buf, secbuf, sseclen);
		dsk_free(secbuf);
		err = DSK_ERR_OK;
		if (self->tele_sechead.syndrome & 2)  err = DSK_ERR_DATAERR;
		if (self->tele_sechead.syndrome & 32) err = DSK_ERR_NODATA;

	}
	while(try_again);

	return err;	
}


dsk_err_t tele_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted)
{
	return DSK_ERR_RDONLY;
}

dsk_err_t tele_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result)
{
	return DSK_ERR_NOTIMPL;
}

dsk_err_t tele_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result)
{
	return DSK_ERR_NOTIMPL;
}

dsk_err_t tele_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	return DSK_ERR_NOTIMPL;
}

dsk_err_t tele_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	return DSK_ERR_NOTIMPL;
}

dsk_err_t tele_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	return DSK_ERR_NOTIMPL;
}

/* Don't bother to implement this one. Fall back on the standard version. */
dsk_err_t tele_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *dg)
{
	return DSK_ERR_NOTIMPL;
}
