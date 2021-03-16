/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2  John Elliott <seasip.webmaster@gmail.com>          *
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

/* This driver shows how a disc image file with an awkward compressed format
 * can be handled - by loading it into memory in drv_open() and writing it
 * back (if changed) in drv_close() */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvcfi.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_cfi = 
{
	sizeof(CFI_DSK_DRIVER),
	"cfi",
	"CFI file driver",
	cfi_open,	/* open */
	cfi_creat,	/* create new */
	cfi_close,	/* close */
	cfi_read,	/* read sector, working from physical address */
	cfi_write,	/* write sector, working from physical address */
	cfi_format,	/* format track, physical */
	NULL,		/* get geometry */
	NULL,		/* sector ID */
	cfi_xseek,	/* seek to track */
	cfi_status,	/* drive status */
};

static dsk_err_t cfi_ensure_size(CFI_DSK_DRIVER *self, dsk_ltrack_t trk)
{
	/* Ensure the requested track is in range, by starting at 
	 * max (cfi_ntracks, 1) and doubling repeatedly */
	dsk_ltrack_t maxtrks = self->cfi_ntracks;

	if (!maxtrks) maxtrks = 1;
	while (trk >= maxtrks) maxtrks *= 2;

	/* Need to allocate more. */
	if (maxtrks != self->cfi_ntracks)
	{
		/* Create new array of tracks */
		CFI_TRACK *t = dsk_malloc(maxtrks * sizeof(CFI_TRACK));
		if (!t) return DSK_ERR_NOMEM;
		memset(t, 0, maxtrks * sizeof(CFI_TRACK));
		/* Copy over existing array */
		memcpy(t, self->cfi_tracks, self->cfi_ntracks * sizeof(CFI_TRACK));
		dsk_free(self->cfi_tracks);
		self->cfi_tracks = t;
		self->cfi_ntracks = maxtrks;
	}
	return DSK_ERR_OK;
}


/* Read a little-endian word from the file. Return DSK_ERR_SEEKFAIL if EOF. */
static dsk_err_t cfi_rdword(FILE *fp, unsigned short *s)
{
	int c = fgetc(fp);
	if (c == EOF) return DSK_ERR_SEEKFAIL;
	*s = (c & 0xFF);
	c = fgetc(fp);
	if (c == EOF) return DSK_ERR_SEEKFAIL;
	*s |= ((c << 8) & 0xFF00);
	return DSK_ERR_OK;
}

void cfi_free_track(CFI_TRACK *ltrk)
{
	if (ltrk && ltrk->cfit_data) 
	{
		dsk_free(ltrk->cfit_data);
		ltrk->cfit_data = NULL;
	}
}


/* Given a compressed buffer, either find its uncompressed length
 * or decompress it */
static dsk_err_t cfi_size_track(CFI_TRACK *ltrk, unsigned char *buf, 
				unsigned short blkc, int pass)
{
	unsigned char *bufp, *wrp;
	unsigned short blklen;
	
	wrp = NULL;	
	bufp = buf;
	if (pass)	/* Pass 2: Allocate the data buffer whose */
	{		/* size we found on pass 1. */
		ltrk->cfit_data = dsk_malloc(ltrk->cfit_length);
		if (!ltrk->cfit_data) return DSK_ERR_NOMEM;
		wrp = ltrk->cfit_data;
	}
	else ltrk->cfit_length = 0;	/* Pass 1: Reset total length */
	while (blkc)
	{
		blklen =   (*(bufp++)) & 0xFF;
		blklen |= ((*(bufp++)) << 8) & 0xFF00;

		if (blklen & 0x8000)	/* Compressed block */
		{
	/* There must be at least 3 bytes for this block! */
			if (blkc < 3) return DSK_ERR_NOTME;
			blklen &= 0x7FFF;
			blkc -= 3;
			if (pass)
			{
				memset(wrp, *bufp, blklen);
				wrp += blklen;
			}
			else ltrk->cfit_length += blklen;
			bufp++;
		}
		else			/* Uncompressed block */
		{
	/* There must be room for this block in the buffer! */
			if (blkc < (2+blklen) || (blklen == 0)) 
			{
				return DSK_ERR_NOTME;
			}
			blkc -= (blklen + 2);
			if (pass)
			{
				memcpy(wrp, bufp, blklen);
				wrp += blklen;
			}
			else ltrk->cfit_length += blklen;
			bufp += blklen;
		}
	}
	return DSK_ERR_OK;
}

static dsk_err_t cfi_load_track(CFI_DSK_DRIVER *self, dsk_ltrack_t trk, FILE *fp)
{
	dsk_err_t err;
	unsigned char *buf;
	unsigned short clen;
	CFI_TRACK *ltrk;
	
	/* Ensure that cfi_tracks is big enough to hold this track */
	err = cfi_ensure_size(self, trk); 
	if (err) return err;

	/* Load the track (compressed) length. If EOF, then
	 * return DSK_ERR_OVERRUN (EOF, but OK really) */
	err = cfi_rdword(fp, &clen);
	if (err == DSK_ERR_SEEKFAIL) return DSK_ERR_OVERRUN;
	/* Compressed length must be at least 3; a block can't be any smaller! */
	if (clen < 3) return DSK_ERR_NOTME;
	buf = dsk_malloc(clen);
	if (buf == NULL) return DSK_ERR_NOMEM;
	/* Try to load the track buffer. If that fails, bail out */
	if (fread(buf, 1, clen, fp) < clen) 
	{
		dsk_free(buf);
		return DSK_ERR_NOTME;	
	}
	ltrk = &self->cfi_tracks[trk];
	cfi_free_track(ltrk);
	/* Pass 1: Determine size of uncompressed data */
	err = cfi_size_track(ltrk, buf, clen, 0);
	/* Pass 2: Decompress */
	if (!err) err = cfi_size_track(ltrk, buf, clen, 1);
	dsk_free(buf);
	if (err) 
	{
		cfi_free_track(ltrk);
		return err;
	}
	return DSK_ERR_OK;
}

static int number_same(unsigned char *c, int tlen)
{
	unsigned char m = (*c); 
	int matched = 1;

	while (tlen > 0)
	{
		++c;
		--tlen;
		if (!tlen) break;
		if (*c != m) return matched;
		++matched;	
		if (matched == 0x7FFF) return matched;
	}
	return matched;	
}


static dsk_err_t cfi_save_track(CFI_DSK_DRIVER *self, dsk_ltrack_t trk, FILE *fp)
{
	CFI_TRACK *t = &self->cfi_tracks[trk];
	unsigned tlen, blklen;
	unsigned char *buf, *rdptr, *rdbase, *wrptr;
	int matched;
	
	if (!t->cfit_data) return DSK_ERR_OK;	/* Vacuous track */

	/* Compress the track using RLE. */
	buf = dsk_malloc(t->cfit_length + 4);
	if (!buf) return DSK_ERR_NOMEM;

	tlen = t->cfit_length;

	rdptr  = t->cfit_data;	/* -> current byte */
	rdbase = t->cfit_data;	/* -> start of current uncompressed block */
	wrptr  = buf + 2;
	blklen = 0;
	
	while (tlen)
	{
/* Check for a compressible run */
		matched = number_same(rdptr, tlen);
		if (matched > 5)	/* Start of compressed run */
		{
			if (blklen) 
			{
/* If we're navigating an uncompressed block, append it */
				wrptr[0] =  blklen & 0xFF;
				wrptr[1] = (blklen >> 8) & 0xFF;
				memcpy(wrptr + 2, rdbase, blklen);
				wrptr  += (blklen + 2);
				rdbase += blklen;
				blklen = 0;
			}
/* Append compressed block */
			wrptr[0] = matched & 0xFF;
			wrptr[1] = ((matched >> 8) & 0x7F) | 0x80;
			wrptr[2] = *rdptr;
			rdptr += matched;
			rdbase = rdptr;
			wrptr += 3;
			tlen -= matched;
		}
		else
		{
/* Not RLEable. */
			rdptr++;
			blklen++;
			tlen--;
		}
	}
/* Write the last uncompressed block if there is one */
	if (blklen)
	{
		wrptr[0] =  blklen & 0xFF;
		wrptr[1] = (blklen >> 8) & 0xFF;
		memcpy(wrptr + 2, rdbase, blklen);
		wrptr  += (blklen + 2);
	}
	tlen = (wrptr - buf) - 2;
	buf[0] = (tlen & 0xFF);
	buf[1] = (tlen >> 8) & 0xFF;
	
	if (fwrite(buf, 1, tlen + 2, fp) < (tlen + 2)) 
	{
		dsk_free(buf);
		return DSK_ERR_SYSERR;
	}
	dsk_free(buf);
	return DSK_ERR_OK;
}


dsk_err_t cfi_open(DSK_DRIVER *self, const char *filename)
{
	FILE *fp;
	CFI_DSK_DRIVER *cfiself;
	dsk_err_t err;	
	dsk_ltrack_t nt;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	fp = fopen(filename, "r+b");
	if (!fp) 
	{
		cfiself->cfi_readonly = 1;
		fp = fopen(filename, "rb");
	}
	if (!fp) return DSK_ERR_NOTME;

	cfiself->cfi_dirty = 0;
	/* Keep a copy of the filename; when writing back, we will need it */
	cfiself->cfi_filename = dsk_malloc(1 + strlen(filename));
	if (!cfiself->cfi_filename) return DSK_ERR_NOMEM;
	strcpy(cfiself->cfi_filename, filename);	

	/* Now to load the tracks. Allow 200; if there are more than
	 * that, we dynamically grow the array. */
	cfiself->cfi_ntracks = 200;
	cfiself->cfi_tracks = dsk_malloc(200 * sizeof(CFI_TRACK));
	if (!cfiself->cfi_tracks) 
	{
		dsk_free(cfiself->cfi_filename);
		return DSK_ERR_NOMEM;
	}
	memset(cfiself->cfi_tracks, 0, 200 * sizeof(CFI_TRACK));
	nt = 0;
	dsk_report("Loading CFI file into memory");
	while (!feof(fp))
	{
		err = cfi_load_track(cfiself, nt++, fp);
		/* DSK_ERR_OVERRUN: End of file */
		if (err == DSK_ERR_OVERRUN) break;
		if (err) 
		{
			dsk_free(cfiself->cfi_filename);
			dsk_free(cfiself->cfi_tracks);
			dsk_report_end();
			fclose(fp);
			return err;
		}
	} 
	dsk_report_end();
	fclose(fp);
	return DSK_ERR_OK;
}


dsk_err_t cfi_creat(DSK_DRIVER *self, const char *filename)
{
	CFI_DSK_DRIVER *cfiself;
	FILE *fp;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	/* See if the file can be created. But don't hold it open. */
	fp = fopen(filename, "wb");
	cfiself->cfi_readonly = 0;
	if (!fp) return DSK_ERR_SYSERR;
	fclose(fp);
	cfiself->cfi_dirty = 1;

	/* Keep a copy of the filename, for writing back */
	cfiself->cfi_filename = dsk_malloc(1 + strlen(filename));
	if (!cfiself->cfi_filename) return DSK_ERR_NOMEM;
	strcpy(cfiself->cfi_filename, filename);	
	
	cfiself->cfi_ntracks = 0;
	cfiself->cfi_tracks = NULL;

	return DSK_ERR_OK;
}


dsk_err_t cfi_close(DSK_DRIVER *self)
{
	CFI_DSK_DRIVER *cfiself;
	dsk_err_t err = DSK_ERR_OK;
	dsk_ltrack_t trk;

	if (self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (cfiself->cfi_filename && (cfiself->cfi_dirty))
	{
/* When writing back to a CFI, create the file from scratch. */
		FILE *fp = fopen(cfiself->cfi_filename, "wb");
		if (!fp) err = DSK_ERR_SYSERR;
		else
		{
			dsk_report("Compressing CFI file");
			for (trk = 0; trk < cfiself->cfi_ntracks; trk++)
			{
				err = cfi_save_track(cfiself, trk, fp);
				if (err) break;
			}
			fclose(fp);
			dsk_report_end();
		}
	}
/* Free track buffers if we have them */
	if (cfiself->cfi_tracks)
	{
		unsigned int n;
		for (n = 0; n < cfiself->cfi_ntracks; n++)
		{
			cfi_free_track(&cfiself->cfi_tracks[n]);
		}
		dsk_free(cfiself->cfi_tracks);
		cfiself->cfi_tracks = NULL;
		cfiself->cfi_ntracks = 0;
	}
	if (cfiself->cfi_filename) 
	{
		dsk_free(cfiself->cfi_filename);
		cfiself->cfi_filename = NULL;
	}
	return err;
}

static dsk_err_t cfi_find_sector(CFI_DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head, 
				dsk_psect_t sector, void **buf)
{
	dsk_ltrack_t track;
	CFI_TRACK *ltrk;
	unsigned long offset;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	track = (cylinder * geom->dg_heads) + head;	/* Drive track */

	/* Track out of range */
	if (track >= self->cfi_ntracks) return DSK_ERR_NOADDR;
	ltrk = &(self->cfi_tracks[track]);

	/* Track not loaded (ie, not formatted) */
	if (!ltrk->cfit_data) 
		return DSK_ERR_NOADDR;

	offset = (sector - geom->dg_secbase) * geom->dg_secsize;

	/* Sector out of range */
	if (offset + geom->dg_secsize > ltrk->cfit_length) 
		return DSK_ERR_NOADDR;

	*buf = (ltrk->cfit_data + offset);
	return DSK_ERR_OK;
}


dsk_err_t cfi_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	CFI_DSK_DRIVER *cfiself;
	void *secbuf;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (!cfiself->cfi_filename) return DSK_ERR_NOTRDY;

	err = cfi_find_sector(cfiself, geom, cylinder, head, sector, &secbuf);
	if (err) return err;
	memcpy(buf, secbuf, geom->dg_secsize);
	return DSK_ERR_OK;
}



dsk_err_t cfi_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	CFI_DSK_DRIVER *cfiself;
	dsk_err_t err;
	void *secbuf;

	if (!buf || !self || !geom || self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (!cfiself->cfi_filename) return DSK_ERR_NOTRDY;
	if (cfiself->cfi_readonly) return DSK_ERR_RDONLY;

	err = cfi_find_sector(cfiself, geom, cylinder, head, sector, &secbuf);
	if (err) return err;
	memcpy(secbuf, buf, geom->dg_secsize);
	cfiself->cfi_dirty = 1;
	return DSK_ERR_OK;
}



dsk_err_t cfi_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since CFI
 * images don't hold track headers.
 */
	CFI_DSK_DRIVER *cfiself;
	dsk_ltrack_t track;
	CFI_TRACK *ltrk;
	long trklen;
	dsk_err_t err;

	(void)format;
	if (!self || !geom || self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (!cfiself->cfi_filename) return DSK_ERR_NOTRDY;
	if (cfiself->cfi_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical track. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	track = (cylinder * geom->dg_heads) + head;	/* Drive track */

	err = cfi_ensure_size(cfiself, track);
	if (err) return err;

	/* Track out of range */
	if (track >= cfiself->cfi_ntracks) return DSK_ERR_NOADDR;
	ltrk = &(cfiself->cfi_tracks[track]);

	/* If existing data, zap them */
	cfi_free_track(ltrk);

	trklen = geom->dg_sectors * geom->dg_secsize;
	ltrk->cfit_data = dsk_malloc((unsigned)trklen);
	if (!ltrk->cfit_data) return DSK_ERR_NOMEM;
	ltrk->cfit_length = (unsigned)trklen;

	memset(ltrk->cfit_data, filler, (unsigned)trklen);	
	cfiself->cfi_dirty = 1;
	return DSK_ERR_OK;
}

	

dsk_err_t cfi_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	CFI_DSK_DRIVER *cfiself;
	unsigned long offset;

	if (!self || !geom || self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (!cfiself->cfi_filename) return DSK_ERR_NOTRDY;

	if (cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
		return DSK_ERR_SEEKFAIL;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */

	if (offset > cfiself->cfi_ntracks) return DSK_ERR_SEEKFAIL;

	return DSK_ERR_OK;
}

dsk_err_t cfi_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	CFI_DSK_DRIVER *cfiself;

	if (!self || !geom || self->dr_class != &dc_cfi) return DSK_ERR_BADPTR;
	cfiself = (CFI_DSK_DRIVER *)self;

	if (!cfiself->cfi_filename) *result &= ~DSK_ST3_READY;
	if (cfiself->cfi_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

