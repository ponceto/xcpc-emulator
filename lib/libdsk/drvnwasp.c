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

/* This driver is used to access Microbee files written by the NanoWasp 
 * emulator. This is similar to the POSIX driver, but sectors are stored in 
 * the file in SIDES_OUTOUT rather than SIDES_ALT order (though the MicroBee
 * actually writes them in SIDES_ALT order). Additionally, I'm reversing the 
 * sector skew so that sectors come out in the logical order, though this is
 * actually an abuse of libdsk (skewing should be done at the cpmtools level). 
 * However, cpmtools doesn't support the type of skewing done by the microbee
 * (sector 1 doesn't map to sector 1). */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvnwasp.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_nwasp = 
{
	sizeof(NWASP_DSK_DRIVER),
	"nanowasp",
	"NanoWasp image file driver",
	nwasp_open,	/* open */
	nwasp_creat,	/* create new */
	nwasp_close,	/* close */
	nwasp_read,	/* read sector, working from physical address */
	nwasp_write,	/* write sector, working from physical address */
	nwasp_format,	/* format track, physical */
	nwasp_getgeom,	/* get geometry */
	NULL,		/* sector ID */
	nwasp_xseek,	/* seek to track */
	nwasp_status,	/* drive status */
};

dsk_err_t nwasp_open(DSK_DRIVER *self, const char *filename)
{
	NWASP_DSK_DRIVER *nwself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	nwself->nw_fp = fopen(filename, "r+b");
	if (!nwself->nw_fp) 
	{
		nwself->nw_readonly = 1;
		nwself->nw_fp = fopen(filename, "rb");
	}
	if (!nwself->nw_fp) return DSK_ERR_NOTME;
/* v0.9.5: Record exact size, so we can tell if we're writing off the end
 * of the file. Under Windows, writing off the end of the file fills the 
 * gaps with random data, which can cause mess to appear in the directory;
 * and under UNIX, the entire directory is filled with zeroes. */
        if (fseek(nwself->nw_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
        nwself->nw_filesize = ftell(nwself->nw_fp);

	return DSK_ERR_OK;
}


dsk_err_t nwasp_creat(DSK_DRIVER *self, const char *filename)
{
	NWASP_DSK_DRIVER *nwself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	nwself->nw_fp = fopen(filename, "w+b");
	nwself->nw_readonly = 0;
	if (!nwself->nw_fp) return DSK_ERR_SYSERR;
	nwself->nw_filesize = 0;
	return DSK_ERR_OK;
}


dsk_err_t nwasp_close(DSK_DRIVER *self)
{
	NWASP_DSK_DRIVER *nwself;

	if (self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (nwself->nw_fp) 
	{
		if (fclose(nwself->nw_fp) == EOF) return DSK_ERR_SYSERR;
		nwself->nw_fp = NULL;
	}
	return DSK_ERR_OK;	
}

static const int skew[10] = { 1,4,7,0,3,6,9,2,5,8 };


dsk_err_t nwasp_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	NWASP_DSK_DRIVER *nwself;
	long offset;

	if (!buf || !self || !geom || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (!nwself->nw_fp) return DSK_ERR_NOTRDY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_OUTOUT" mapping */
	offset = 204800L * head + 5120L * cylinder + 512 * skew[sector-1];

	if (fseek(nwself->nw_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	if (fread(buf, 1, geom->dg_secsize, nwself->nw_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}


static dsk_err_t seekto(NWASP_DSK_DRIVER *self, unsigned long offset)
{
	/* 0.9.5: Fill any "holes" in the file with 0xE5. Otherwise, UNIX would
	 * fill them with zeroes and Windows would fill them with whatever
	 * happened to be lying around */
	if (self->nw_filesize < offset)
	{
		if (fseek(self->nw_fp, self->nw_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (self->nw_filesize < offset)
		{
			if (fputc(0xE5, self->nw_fp) == EOF) return DSK_ERR_SYSERR;
			++self->nw_filesize;
		}
	}
	if (fseek(self->nw_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}

dsk_err_t nwasp_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	NWASP_DSK_DRIVER *nwself;
	unsigned long offset;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (!nwself->nw_fp) return DSK_ERR_NOTRDY;
	if (nwself->nw_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_OUTOUT" mapping */
	offset = 204800L * head + 5120L * cylinder + 512 * skew[sector-1];

	err = seekto(nwself, offset);
	if (err) return err;

	if (fwrite(buf, 1, geom->dg_secsize, nwself->nw_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	if (nwself->nw_filesize < offset + geom->dg_secsize)
		nwself->nw_filesize = offset + geom->dg_secsize;
	return DSK_ERR_OK;
}


dsk_err_t nwasp_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw NWASP
 * images don't hold track headers.
 */
	NWASP_DSK_DRIVER *nwself;
	unsigned long offset;
	unsigned long trklen;
	dsk_err_t err;

   (void)format;
	if (!self || !geom || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (!nwself->nw_fp) return DSK_ERR_NOTRDY;
	if (nwself->nw_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_OUTOUT" mapping */
	offset = 204800L * head + 5120L * cylinder;/* + 512 * skew[sector-1];*/
	trklen = 5120L;

	err = seekto(nwself, offset);
	if (err) return err;
	if (nwself->nw_filesize < offset + trklen)
		nwself->nw_filesize = offset + trklen;

	while (trklen--) 
		if (fputc(filler, nwself->nw_fp) == EOF) return DSK_ERR_SYSERR;	

	return DSK_ERR_OK;
}

	

dsk_err_t nwasp_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	NWASP_DSK_DRIVER *nwself;
	long offset;

	if (!self || !geom || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (!nwself->nw_fp) return DSK_ERR_NOTRDY;

	if (cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
		return DSK_ERR_SEEKFAIL;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_OUTOUT" mapping */
	offset = 204800L * head + 5120L * cylinder;/* + 512 * skew[sector-1];*/
	
	if (fseek(nwself->nw_fp, offset, SEEK_SET)) return DSK_ERR_SEEKFAIL;

	return DSK_ERR_OK;
}

dsk_err_t nwasp_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	NWASP_DSK_DRIVER *nwself;

	if (!self || !geom || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
	nwself = (NWASP_DSK_DRIVER *)self;

	if (!nwself->nw_fp) *result &= ~DSK_ST3_READY;
	if (nwself->nw_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}


dsk_err_t nwasp_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
        if (!geom || !self || self->dr_class != &dc_nwasp) return DSK_ERR_BADPTR;
        geom->dg_sidedness = SIDES_ALT;
        geom->dg_cylinders = 40;
        geom->dg_heads     = 2;
        geom->dg_sectors   = 10;
        geom->dg_secbase   = 1;
        geom->dg_secsize   = 512;
        geom->dg_datarate  = RATE_DD;   /* These values (for accessing  
                                         * mbee floppies) are untested. */
        geom->dg_rwgap     = 0x0C;
        geom->dg_fmtgap    = 0x17;
        geom->dg_fm        = RECMODE_MFM;
        geom->dg_nomulti   = 0;
        return DSK_ERR_OK;
}

