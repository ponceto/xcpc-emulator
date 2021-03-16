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

/* This driver works with the compressed "apridisk" format. The compression
 * works by doing RLE on individual sectors. We use the same method as for
 * CFI - decompressing the whole file into memory, and writing back in 
 * drv_close(). 
 */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvadisk.h"

#define APRIDISK_DELETED      0xE31D0000UL
#define APRIDISK_MAGIC        0xE31D0001UL
#define APRIDISK_COMMENT      0xE31D0002UL
#define APRIDISK_CREATOR           0xE31D0003UL
#define APRIDISK_UNCOMPRESSED 0x9E90
#define APRIDISK_COMPRESSED   0x3E5A

/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_adisk = 
{
	sizeof(ADISK_DSK_DRIVER),
	"apridisk",
	"APRIDISK file driver",
	adisk_open,	/* open */
	adisk_creat,	/* create new */
	adisk_close,	/* close */
	adisk_read,	/* read sector, working from physical address */
	adisk_write,	/* write sector, working from physical address */
	adisk_format,	/* format track, physical */
	NULL,		/* get geometry */
	adisk_secid,	/* sector ID */
	adisk_xseek,	/* seek to track */
	adisk_status,	/* drive status */
};

static char adisk_wmagic[128] =
{
	'A','C','T',' ','A','p','r','i','c','o','t',' ','d','i','s','k',
	' ','i','m','a','g','e',26,4 
};

static dsk_err_t adisk_ensure_size(ADISK_DSK_DRIVER *self, dsk_lsect_t nsec)
{
	/* Ensure the requested track is in range, by starting at 
	 * max (adisk_maxsectors, 1) and doubling repeatedly */
	dsk_ltrack_t maxsecs = self->adisk_maxsectors;

	if (!maxsecs) maxsecs = 1;
	while (nsec >= maxsecs) maxsecs *= 2;

	/* Need to allocate more. */
	if (maxsecs != self->adisk_maxsectors)
	{
		/* Create new array of tracks */
		ADISK_SECTOR *t = dsk_malloc(maxsecs * sizeof(ADISK_SECTOR));
		if (!t) return DSK_ERR_NOMEM;
		memset(t, 0, maxsecs * sizeof(ADISK_SECTOR));
		/* Copy over existing array */
		memcpy(t, self->adisk_sectors, self->adisk_maxsectors * sizeof(ADISK_SECTOR));
		dsk_free(self->adisk_sectors);
		self->adisk_sectors  = t;
		self->adisk_maxsectors = maxsecs;
	}
	return DSK_ERR_OK;
}


/* Read a little-endian word from the file. Return DSK_ERR_SEEKFAIL if EOF. */
static dsk_err_t adisk_rdshort(FILE *fp, unsigned short *s)
{
	int c = fgetc(fp);
	if (c == EOF) return DSK_ERR_SEEKFAIL;
	*s = (c & 0xFF);
	c = fgetc(fp);
	if (c == EOF) return DSK_ERR_SEEKFAIL;
	*s |= ((c << 8) & 0xFF00);
	return DSK_ERR_OK;
}


static dsk_err_t adisk_rdlong(FILE *fp, unsigned long *l)
{
	unsigned short w0, w1;
	dsk_err_t e;

	e = adisk_rdshort(fp, &w0); if (e) return e;
	e = adisk_rdshort(fp, &w1); if (e) return e;
	*l = (((unsigned long)(w1)) << 16) | w0;
	return DSK_ERR_OK;
}



void adisk_free_sector(ADISK_SECTOR *ltrk)
{
	if (ltrk && ltrk->adisks_data) 
	{
		dsk_free(ltrk->adisks_data);
		ltrk->adisks_data = NULL;
	}
}


/* Given a compressed buffer, either find its uncompressed length
 * or decompress it */
static dsk_err_t adisk_size_sector(ADISK_SECTOR *psec, unsigned char *buf, 
				unsigned short blkc, int pass)
{
	unsigned char *bufp, *wrp;
	unsigned short blklen;
	
	wrp = NULL;	
	bufp = buf;
	if (pass)	/* Pass 2: Allocate the data buffer whose */
	{		/* size we found on pass 1. */
		psec->adisks_data = dsk_malloc(psec->adisks_datalen);
		if (!psec->adisks_data) return DSK_ERR_NOMEM;
		wrp = psec->adisks_data;
	}
	else psec->adisks_datalen = 0;	/* Pass 1: Reset total length */
	while (blkc)
	{
		blklen =   (*(bufp++)) & 0xFF;
		blklen |= ((*(bufp++)) << 8) & 0xFF00;

		if (pass)
		{
			memset(wrp, *bufp, blklen);
			wrp += blklen;
		}
		else psec->adisks_datalen += blklen;
		bufp++;
		blkc -= 3;
	}
	return DSK_ERR_OK;
}



static dsk_err_t adisk_load_sector(ADISK_DSK_DRIVER *self, dsk_lsect_t nsec, FILE *fp)
{
	dsk_err_t err;
	unsigned char *buf;
	ADISK_SECTOR *psec;
/* Sector header */
	unsigned long sh_magic;
	unsigned short sh_compressed;
	unsigned short sh_hdrsize;
	unsigned long  sh_datasize;
	unsigned long  sh_identity;
	
	/* Ensure that adisk_sectors is big enough to hold this track */
	err = adisk_ensure_size(self, nsec); 
	if (err) return err;

	/* Load the track header. */

	err = adisk_rdlong(fp, &sh_magic); if (err) return DSK_ERR_OVERRUN;
	if (sh_magic != APRIDISK_MAGIC 
	&&  sh_magic != APRIDISK_COMMENT 
	&&  sh_magic != APRIDISK_DELETED
	&&  sh_magic != APRIDISK_CREATOR)
	{
		return DSK_ERR_NOTME;
	}
	err = adisk_rdshort(fp, &sh_compressed); if (err) return DSK_ERR_OVERRUN;
	if (sh_compressed != APRIDISK_COMPRESSED 
	&&  sh_compressed != APRIDISK_UNCOMPRESSED) 
	{
		return DSK_ERR_NOTME;
	}
	err = adisk_rdshort(fp, &sh_hdrsize);  if (err) return DSK_ERR_OVERRUN;
	err = adisk_rdlong (fp, &sh_datasize); if (err) return DSK_ERR_OVERRUN;
	err = adisk_rdlong (fp, &sh_identity); if (err) return DSK_ERR_OVERRUN;

	/* Compressed length must be at least 3; a block can't be any smaller! */
	if (sh_datasize < 3 && sh_compressed == APRIDISK_COMPRESSED) 
	{
		return DSK_ERR_NOTME;
	}
	buf = dsk_malloc(1 + sh_datasize);
	buf[sh_datasize] = 0;
	if (buf == NULL) return DSK_ERR_NOMEM;
	/* Skip over any extra bytes of sector header (we ignore them) */
	while (sh_hdrsize > 0x10)
	{
		if (fgetc(fp) == EOF) return DSK_ERR_OVERRUN;
		--sh_hdrsize;
	}
	if (sh_magic != APRIDISK_MAGIC && sh_magic != APRIDISK_COMMENT && sh_magic != APRIDISK_CREATOR)
	{
		while (sh_datasize > 0)
		{
			if (fgetc(fp) == EOF) return DSK_ERR_OVERRUN;
			--sh_datasize;
		}
		return DSK_ERR_OK;
	}
	/* Try to load the track buffer. If that fails, bail out */
	if (fread(buf, 1, sh_datasize, fp) < sh_datasize) 
	{
		dsk_free(buf);
		return DSK_ERR_NOTME;	
	}
	/* Handle comments here */
	if (sh_magic == APRIDISK_COMMENT)
	{
		int n;
		/* Don't support compressed comments */
		if (sh_compressed == APRIDISK_COMPRESSED) return DSK_ERR_OK;

		/* APRIDISK comments for some reason are terminated by \r */	
		for (n = 0; buf[n]; n++) 
			if (buf[n] == '\r' && buf[n+1] != '\n') buf[n] = '\n';
		dsk_set_comment(&self->adisk_super, (char *)buf);
		return DSK_ERR_OK;
	}
	if (sh_magic == APRIDISK_CREATOR)
	{
		/* Don't support compressed creator blocks */
		if (sh_compressed == APRIDISK_COMPRESSED) return DSK_ERR_OK;
		self->adisk_creator = dsk_malloc(1 + strlen((char *)buf));
		if (self->adisk_creator)
			strcpy(self->adisk_creator, (char *)buf);
		return DSK_ERR_OK;
	}
	psec = &self->adisk_sectors[nsec];
	adisk_free_sector(psec);
	if (sh_compressed == APRIDISK_COMPRESSED)
	{
		/* Pass 1: Determine size of uncompressed data */
		err = adisk_size_sector(psec, buf, (unsigned short)sh_datasize, 0);
		/* Pass 2: Decompress */
		if (!err) err = adisk_size_sector(psec, buf, (unsigned short)sh_datasize, 1);
		dsk_free(buf);
	}
	else
	{
		psec->adisks_data = buf;
		psec->adisks_datalen = sh_datasize;
		err = DSK_ERR_OK;
	}
	if (err) 
	{
		adisk_free_sector(psec);
		return err;
	}
	psec->adisks_magic    = sh_magic;
	psec->adisks_cylinder = (unsigned short)(sh_identity >> 16);
	psec->adisks_head     = (unsigned char)(sh_identity & 0xFF);
	psec->adisks_sector   = (unsigned char)((sh_identity >> 8) & 0xFF);
	return DSK_ERR_OK;
}

/* Number of bytes in a run. For RLE. 
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
*/

static dsk_err_t adisk_save_sector(ADISK_DSK_DRIVER *self, ADISK_SECTOR *psec, FILE *fp)
{
	unsigned n;
	unsigned long slen;
	int compress = 1;
	unsigned char *buf;
	
	if (!psec->adisks_data) return DSK_ERR_OK;	/* Vacuous sector */

	/* Compress the sector using RLE. Only compress if all bytes are the
	 * same. */
	for (n = 1; n < psec->adisks_datalen; n++)	
	{
		if (psec->adisks_data[n] != psec->adisks_data[0]) 
		{
			compress = 0;
			break;
		}
	}
	
	if (compress) slen = 16 + 3;
	else	      slen = 16 + psec->adisks_datalen;

	buf = dsk_malloc(slen); if (!buf) return DSK_ERR_NOMEM;

	if (compress)
	{
		buf[ 4] = APRIDISK_COMPRESSED & 0xFF;
		buf[ 5] = APRIDISK_COMPRESSED >> 8;
		buf[16] = (unsigned char)( psec->adisks_datalen & 0xFF);
		buf[17] = (unsigned char)((psec->adisks_datalen >> 8) & 0xFF);
		buf[18] =  psec->adisks_data[0];
	}
	else
	{
		buf[ 4] = APRIDISK_UNCOMPRESSED & 0xFF;
		buf[ 5] = APRIDISK_UNCOMPRESSED >> 8;
		memcpy(buf + 16, psec->adisks_data, psec->adisks_datalen);
	}
	buf[ 0] =   APRIDISK_MAGIC        & 0xFF;
	buf[ 1] =  (APRIDISK_MAGIC >> 8 ) & 0xFF;
	buf[ 2] =  (APRIDISK_MAGIC >> 16) & 0xFF;
	buf[ 3] =  (APRIDISK_MAGIC >> 24) & 0xFF;
	buf[ 6] =  0x10;
	buf[ 7] =  0;
	buf[ 8] = (unsigned char)( (slen - 16) & 0xFF);
	buf[ 9] = (unsigned char)(((slen - 16) >>  8) & 0xFF);
	buf[10] = (unsigned char)(((slen - 16) >> 16) & 0xFF);
	buf[11] = (unsigned char)(((slen - 16) >> 24) & 0xFF);
	buf[12] =  psec->adisks_head;
	buf[13] =  psec->adisks_sector;
	buf[14] =  psec->adisks_cylinder & 0xFF;
	buf[15] = (psec->adisks_cylinder >> 8) & 0xFF;

	if (fwrite(buf, 1, slen, fp) < slen)
	{
		dsk_free(buf);
		return DSK_ERR_SYSERR;
	}
	dsk_free(buf);
	return DSK_ERR_OK;
}

static dsk_err_t adisk_save_comment(ADISK_DSK_DRIVER *self, FILE *fp)
{
	char *cmt = NULL;
	unsigned char *buf;
	unsigned long slen;
	int n;

	dsk_get_comment(&self->adisk_super, &cmt); 
	if (!cmt) return DSK_ERR_OK;	/* No comment */

	buf = dsk_malloc(slen = 17 + strlen(cmt));
	if (!buf) return DSK_ERR_OK;
	memset(buf, 0, slen);
	strcpy((char *)buf + 16, cmt);
	/* Convert lone newlines to lone CRs */
	for (n = 17; buf[n]; n++)	 
	{
		if (buf[n] == '\n' && buf[n-1] != '\r') buf[n] = '\r';
	}	
	buf[ 0] =   APRIDISK_COMMENT        & 0xFF;
	buf[ 1] =  (APRIDISK_COMMENT >> 8 ) & 0xFF;
	buf[ 2] =  (APRIDISK_COMMENT >> 16) & 0xFF;
	buf[ 3] =  (APRIDISK_COMMENT >> 24) & 0xFF;
	buf[ 4] = APRIDISK_UNCOMPRESSED & 0xFF;
	buf[ 5] = APRIDISK_UNCOMPRESSED >> 8;
	buf[ 6] =  0x10;
	buf[ 7] =  0;
	buf[ 8] = (unsigned char)( (slen - 16) & 0xFF);
	buf[ 9] = (unsigned char)(((slen - 16) >>  8) & 0xFF);
	buf[10] = (unsigned char)(((slen - 16) >> 16) & 0xFF);
	buf[11] = (unsigned char)(((slen - 16) >> 24) & 0xFF);

	if (fwrite(buf, 1, slen, fp) < slen)
	{
		dsk_free(buf);
		return DSK_ERR_SYSERR;
	}
	dsk_free(buf);
	return DSK_ERR_OK;
}



static dsk_err_t adisk_save_creator(ADISK_DSK_DRIVER *self, FILE *fp)
{
	char *cmt = NULL;
	unsigned char *buf;
	unsigned long slen;
	int n;

/*
	if (self->adisk_creator) cmt = self->adisk_creator; 
	else
*/
	cmt = "LIBDSK v" LIBDSK_VERSION;
	
	buf = dsk_malloc(slen = 17 + strlen(cmt));
	if (!buf) return DSK_ERR_OK;
	memset(buf, 0, slen);
	strcpy((char *)buf + 16, cmt);
	/* Convert lone newlines to lone CRs */
	for (n = 17; buf[n]; n++)	 
	{
		if (buf[n] == '\n' && buf[n-1] != '\r') buf[n] = '\r';
	}	
	buf[ 0] =   APRIDISK_CREATOR        & 0xFF;
	buf[ 1] =  (APRIDISK_CREATOR >> 8 ) & 0xFF;
	buf[ 2] =  (APRIDISK_CREATOR >> 16) & 0xFF;
	buf[ 3] =  (APRIDISK_CREATOR >> 24) & 0xFF;
	buf[ 4] = APRIDISK_UNCOMPRESSED & 0xFF;
	buf[ 5] = APRIDISK_UNCOMPRESSED >> 8;
	buf[ 6] =  0x10;
	buf[ 7] =  0;
	buf[ 8] = (unsigned char)( (slen - 16) & 0xFF);
	buf[ 9] = (unsigned char)(((slen - 16) >>  8) & 0xFF);
	buf[10] = (unsigned char)(((slen - 16) >> 16) & 0xFF);
	buf[11] = (unsigned char)(((slen - 16) >> 24) & 0xFF);

	if (fwrite(buf, 1, slen, fp) < slen)
	{
		dsk_free(buf);
		return DSK_ERR_SYSERR;
	}
	dsk_free(buf);
	return DSK_ERR_OK;
}



dsk_err_t adisk_open(DSK_DRIVER *self, const char *filename)
{
	FILE *fp;
	ADISK_DSK_DRIVER *adiskself;
	dsk_err_t err;	
	dsk_lsect_t count;
	unsigned long magic;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	fp = fopen(filename, "r+b");
	if (!fp) 
	{
		adiskself->adisk_readonly = 1;
		fp = fopen(filename, "rb");
	}
	if (!fp) return DSK_ERR_NOTME;

	/* Try to check the magic number at 0x80 */
	if (fread(adiskself->adisk_header, 1, sizeof(adiskself->adisk_header), 
				fp) < (int)sizeof(adiskself->adisk_header) 
	|| (memcmp(adiskself->adisk_header, adisk_wmagic, sizeof(adisk_wmagic)))
	|| (adisk_rdlong(fp, &magic) != DSK_ERR_OK) 
	|| (magic != APRIDISK_MAGIC && 
	    magic != APRIDISK_CREATOR &&
	    magic != APRIDISK_COMMENT &&
	    magic != APRIDISK_DELETED))
	{
		fclose(fp);
		return DSK_ERR_NOTME;
	}
	/* Seek to end of header */
	fseek(fp, 0x80, SEEK_SET);	

	adiskself->adisk_dirty = 0;
	adiskself->adisk_sec = 0;
	/* Keep a copy of the filename; when writing back, we will need it */
	adiskself->adisk_filename = dsk_malloc(1 + strlen(filename));
	if (!adiskself->adisk_filename) return DSK_ERR_NOMEM;
	strcpy(adiskself->adisk_filename, filename);	

	/* Now to load the sectors. Allow 1440 (a 720k floppy); 
	 * if there are more than that, we dynamically grow the array. */
	adiskself->adisk_maxsectors = 1440;
	adiskself->adisk_sectors = dsk_malloc(1440 * sizeof(ADISK_SECTOR));
	if (!adiskself->adisk_sectors) 
	{
		dsk_free(adiskself->adisk_filename);
		return DSK_ERR_NOMEM;
	}
	memset(adiskself->adisk_sectors, 0, 1440 * sizeof(ADISK_SECTOR));
	count = 0;
	dsk_report("Loading APRIDISK file into memory");
	while (!feof(fp))
	{
		err = adisk_load_sector(adiskself, count++, fp);
		/* DSK_ERR_OVERRUN: End of file */
		adiskself->adisk_nsectors = count;
		if (err == DSK_ERR_OVERRUN) 
		{
			dsk_report_end();
			return DSK_ERR_OK;
		}
		if (err) 
		{
			dsk_free(adiskself->adisk_filename);
			dsk_free(adiskself->adisk_sectors);
			dsk_report_end();
			return err;
		}
	} 
	dsk_report_end();
	return DSK_ERR_OK;
}


dsk_err_t adisk_creat(DSK_DRIVER *self, const char *filename)
{
	ADISK_DSK_DRIVER *adiskself;
	FILE *fp;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	/* See if the file can be created. But don't hold it open. */
	fp = fopen(filename, "wb");
	adiskself->adisk_readonly = 0;
	if (!fp) return DSK_ERR_SYSERR;
	fclose(fp);
	adiskself->adisk_dirty = 1;

	/* Keep a copy of the filename, for writing back */
	adiskself->adisk_filename = dsk_malloc(1 + strlen(filename));
	if (!adiskself->adisk_filename) return DSK_ERR_NOMEM;
	strcpy(adiskself->adisk_filename, filename);	
	
	adiskself->adisk_maxsectors = 0;
	adiskself->adisk_sectors = NULL;

	return DSK_ERR_OK;
}


dsk_err_t adisk_close(DSK_DRIVER *self)
{
	ADISK_DSK_DRIVER *adiskself;
	dsk_err_t err = DSK_ERR_OK;
	dsk_ltrack_t trk;

	if (self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (adiskself->adisk_filename && (adiskself->adisk_dirty))
	{
/* When writing back to a ADISK, create the file from scratch. */
/* XXX Sort the sectors... */
		FILE *fp = fopen(adiskself->adisk_filename, "wb");
		if (!fp) err = DSK_ERR_SYSERR;
		else
		{
			dsk_report("Compressing APRIDISK file");
			if (fwrite(adisk_wmagic, 1, 128, fp) < 128)
			{
				err = DSK_ERR_SYSERR;
			}
			else if ((err = adisk_save_creator(adiskself, fp))) { }
			else for (trk = 0; trk < adiskself->adisk_maxsectors; trk++)
			{
				err = adisk_save_sector(adiskself, 
					&adiskself->adisk_sectors[trk], fp);
				if (err) break;
			}
			if (!err) err = adisk_save_comment(adiskself, fp);
			fclose(fp);
			dsk_report_end();
		}
	}
/* Free track buffers if we have them */
	if (adiskself->adisk_sectors)
	{
		unsigned int n;
		for (n = 0; n < adiskself->adisk_maxsectors; n++)
		{
			adisk_free_sector(&adiskself->adisk_sectors[n]);
		}
		dsk_free(adiskself->adisk_sectors);
		adiskself->adisk_sectors = NULL;
		adiskself->adisk_maxsectors = 0;
	}
	if (adiskself->adisk_filename) 
	{
		dsk_free(adiskself->adisk_filename);
		adiskself->adisk_filename = NULL;
	}
	if (adiskself->adisk_creator)
	{
		dsk_free(adiskself->adisk_creator);
		adiskself->adisk_creator = NULL;
	}
	return err;
}

static dsk_err_t adisk_find_sector(ADISK_DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head, 
				dsk_psect_t sector, void **buf, unsigned long *ns)
{
	ADISK_SECTOR *psec;
	dsk_lsect_t nsec;

	for (nsec = 0; nsec < self->adisk_maxsectors; nsec++)
	{
		psec = &self->adisk_sectors[nsec];
		if (psec->adisks_cylinder == cylinder &&
		    psec->adisks_head     == head     &&
		    psec->adisks_sector   == sector) break;
	}
	/* Sector not found? */
	if (nsec >= self->adisk_maxsectors) return DSK_ERR_NOADDR;
	/* Sector has no data? */
	if (psec->adisks_data == NULL)    return DSK_ERR_NODATA;

	*ns  = nsec;
	*buf = psec->adisks_data;
	return DSK_ERR_OK;
}


dsk_err_t adisk_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	ADISK_DSK_DRIVER *adiskself;
	void *secbuf;
	dsk_err_t err;
	unsigned long nsec;

	if (!buf || !self || !geom || self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (!adiskself->adisk_filename) return DSK_ERR_NOTRDY;

	err = adisk_find_sector(adiskself, geom, cylinder, head, sector, &secbuf, &nsec);
	if (err) return err;
	memcpy(buf, secbuf, geom->dg_secsize);
	return DSK_ERR_OK;
}



dsk_err_t adisk_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	ADISK_DSK_DRIVER *adiskself;
	dsk_err_t err;
	void *secbuf;
	unsigned long nsec;

	if (!buf || !self || !geom || self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (!adiskself->adisk_filename) return DSK_ERR_NOTRDY;
	if (adiskself->adisk_readonly) return DSK_ERR_RDONLY;

	err = adisk_find_sector(adiskself, geom, cylinder, head, sector, &secbuf, &nsec);
	if (err) return err;
	memcpy(secbuf, buf, geom->dg_secsize);
	adiskself->adisk_dirty = 1;
	return DSK_ERR_OK;
}



dsk_err_t adisk_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	ADISK_DSK_DRIVER *adiskself;
	dsk_err_t err;
	dsk_psect_t nlsec;
	unsigned long nsec, secsize;
	void *secbuf;
	ADISK_SECTOR *psec;

	if (!self || !geom || self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (!adiskself->adisk_filename) return DSK_ERR_NOTRDY;
	if (adiskself->adisk_readonly) return DSK_ERR_RDONLY;

	/* See if there's a sector header for this sector already */
	for (nlsec = 0; nlsec < geom->dg_sectors; nlsec++)
	{
		secsize = format[nlsec].fmt_secsize;
		err = adisk_find_sector(adiskself, geom, cylinder, head, 
				format[nlsec].fmt_sector, &secbuf, &nsec);
		if (err)	/* new sector */
		{
			nsec = adiskself->adisk_nsectors;
			err = adisk_ensure_size(adiskself, nsec);
			if (err) return err;
			psec = &adiskself->adisk_sectors[nsec];
/* Allocate a new sector buffer  */
			psec->adisks_magic    = APRIDISK_MAGIC;
			psec->adisks_cylinder = cylinder;
			psec->adisks_head     = head;
			psec->adisks_sector   = format[nlsec].fmt_sector;
			psec->adisks_data = dsk_malloc(secsize);
			if (!psec->adisks_data) return DSK_ERR_NOMEM;
			else psec->adisks_datalen = secsize;
			++adiskself->adisk_nsectors;
		}
		else
		{
			psec = &adiskself->adisk_sectors[nsec];
			/* Readjust size */
			if (psec->adisks_datalen != secsize)
			{
				if (psec->adisks_data) dsk_free(psec->adisks_data);
				psec->adisks_data = dsk_malloc(secsize);
				if (!psec->adisks_data) return DSK_ERR_NOMEM;
				else psec->adisks_datalen = secsize;
			}
		}
		memset(psec->adisks_data, filler, secsize);
	}	
	return DSK_ERR_OK;
}
	

dsk_err_t adisk_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	ADISK_DSK_DRIVER *adiskself;
	unsigned long nsec;

	if (!self || !geom || self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (!adiskself->adisk_filename) return DSK_ERR_NOTRDY;

	if (cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
		return DSK_ERR_SEEKFAIL;

	/* Find a sector with the given track / head */
	for (nsec = 0; nsec < adiskself->adisk_nsectors; nsec++)
	{
		if (adiskself->adisk_sectors[nsec].adisks_cylinder == cylinder &&
		    adiskself->adisk_sectors[nsec].adisks_head     == head)
			return DSK_ERR_OK;
	}
	return DSK_ERR_SEEKFAIL;
}

dsk_err_t adisk_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	ADISK_DSK_DRIVER *adiskself;

	if (!self || !geom || self->dr_class != &dc_adisk) return DSK_ERR_BADPTR;
	adiskself = (ADISK_DSK_DRIVER *)self;

	if (!adiskself->adisk_filename) *result &= ~DSK_ST3_READY;
	if (adiskself->adisk_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}


/* Read a sector ID from a given track */
dsk_err_t adisk_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head, DSK_FORMAT *result)
{
        ADISK_DSK_DRIVER *adiskself;
	dsk_psect_t count; 
	int nsec, lastmatch, zeromatch;

        if (!self || !geom || !result || self->dr_class != &dc_adisk)
                return DSK_ERR_BADPTR;
        adiskself = (ADISK_DSK_DRIVER *)self;

	/* Number of matches to skip */
	count = adiskself->adisk_sec + 1;
	lastmatch = -1;
	zeromatch = -1;

	/* Find sectors which match the criteria. 
	 * Set zeromatch to be the one found "count" sectors in.
	 * Set lastmatch to be the last one found. 
	 * If zeromatch=lastmatch, reset the count so that the next
	 * call returns the first sector */
	for (nsec = 0; nsec < (int)adiskself->adisk_nsectors; nsec++)
	{
		if (adiskself->adisk_sectors[nsec].adisks_cylinder == cylinder &&
		    adiskself->adisk_sectors[nsec].adisks_head     == head &&
		    adiskself->adisk_sectors[nsec].adisks_datalen)
		{
			lastmatch = nsec;
			if (count)
			{
				--count;
				if (!count) zeromatch = nsec;
			}
		}
	}
	/* No suitable sectors found */
	if (lastmatch == -1) return DSK_ERR_NOADDR;
	if (zeromatch == -1) zeromatch = lastmatch;
	/* If lastmatch == zeromatch, then the next search will run out of 
	 * sectors. Reset the counter. */
	if (zeromatch == lastmatch) adiskself->adisk_sec = 0;
	else			    adiskself->adisk_sec++;

	result->fmt_cylinder = adiskself->adisk_sectors[zeromatch].adisks_cylinder;
	result->fmt_head     = adiskself->adisk_sectors[zeromatch].adisks_head;
	result->fmt_sector   = adiskself->adisk_sectors[zeromatch].adisks_sector;
	result->fmt_secsize  = adiskself->adisk_sectors[zeromatch].adisks_datalen;
	
	return DSK_ERR_OK;
}


