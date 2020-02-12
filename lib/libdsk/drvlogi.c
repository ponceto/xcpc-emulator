/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2007  John Elliott <seasip.webmaster@gmail.com>       *
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

/* This driver implements access to a flat file, like drvposix, but with the
 * sides laid out in the order specified by the disk geometry. What you end
 * up with is a logical filesystem image, hence the name. */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvlogi.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_logical = 
{
	sizeof(LOGICAL_DSK_DRIVER),
	"logical",
	"Raw file logical sector order",
	logical_open,	/* open */
	logical_creat,	/* create new */
	logical_close,	/* close */
	logical_read,	/* read sector, working from physical address */
	logical_write,	/* write sector, working from physical address */
	logical_format,	/* format track, physical */
	NULL,		/* get geometry */
	NULL,		/* sector ID */
	logical_xseek,	/* seek to track */
	logical_status,	/* drive status */
};

dsk_err_t logical_open(DSK_DRIVER *self, const char *filename)
{
	LOGICAL_DSK_DRIVER *lpxself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	lpxself->lpx_fp = fopen(filename, "r+b");
	if (!lpxself->lpx_fp) 
	{
		lpxself->lpx_readonly = 1;
		lpxself->lpx_fp = fopen(filename, "rb");
	}
	if (!lpxself->lpx_fp) return DSK_ERR_NOTME;
/* v0.9.5: Record exact size, so we can tell if we're writing off the end
 * of the file. Under Windows, writing off the end of the file fills the 
 * gaps with random data, which can cause mess to appear in the directory;
 * and under UNIX, the entire directory is filled with zeroes. */
        if (fseek(lpxself->lpx_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
        lpxself->lpx_filesize = ftell(lpxself->lpx_fp);

	return DSK_ERR_OK;
}


dsk_err_t logical_creat(DSK_DRIVER *self, const char *filename)
{
	LOGICAL_DSK_DRIVER *lpxself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	lpxself->lpx_fp = fopen(filename, "w+b");
	lpxself->lpx_readonly = 0;
	if (!lpxself->lpx_fp) return DSK_ERR_SYSERR;
	lpxself->lpx_filesize = 0;
	return DSK_ERR_OK;
}


dsk_err_t logical_close(DSK_DRIVER *self)
{
	LOGICAL_DSK_DRIVER *lpxself;

	if (self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (lpxself->lpx_fp) 
	{
		if (fclose(lpxself->lpx_fp) == EOF) return DSK_ERR_SYSERR;
		lpxself->lpx_fp = NULL;
	}
	return DSK_ERR_OK;	
}


dsk_err_t logical_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	LOGICAL_DSK_DRIVER *lpxself;
	dsk_lsect_t offset;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (!lpxself->lpx_fp) return DSK_ERR_NOTRDY;

	err = dg_ps2ls(geom, cylinder, head, sector, &offset);
	if (err) return err;
	offset *=  geom->dg_secsize;

	if (fseek(lpxself->lpx_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	if (fread(buf, 1, geom->dg_secsize, lpxself->lpx_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}


static dsk_err_t seekto(LOGICAL_DSK_DRIVER *self, unsigned long offset)
{
	/* 0.9.5: Fill any "holes" in the file with 0xE5. Otherwise, UNIX would
	 * fill them with zeroes and Windows would fill them with whatever
	 * happened to be lying around */
	if (self->lpx_filesize < offset)
	{
		if (fseek(self->lpx_fp, self->lpx_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (self->lpx_filesize < offset)
		{
			if (fputc(0xE5, self->lpx_fp) == EOF) return DSK_ERR_SYSERR;
			++self->lpx_filesize;
		}
	}
	if (fseek(self->lpx_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}

dsk_err_t logical_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	LOGICAL_DSK_DRIVER *lpxself;
	dsk_lsect_t offset;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (!lpxself->lpx_fp) return DSK_ERR_NOTRDY;
	if (lpxself->lpx_readonly) return DSK_ERR_RDONLY;

	err = dg_ps2ls(geom, cylinder, head, sector, &offset);
	if (err) return err;
	offset *=  geom->dg_secsize;

	err = seekto(lpxself, offset);
	if (err) return err;

	if (fwrite(buf, 1, geom->dg_secsize, lpxself->lpx_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	if (lpxself->lpx_filesize < offset + geom->dg_secsize)
		lpxself->lpx_filesize = offset + geom->dg_secsize;
	return DSK_ERR_OK;
}


dsk_err_t logical_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw LOGICAL
 * images don't hold track headers.
 */
	LOGICAL_DSK_DRIVER *lpxself;
	dsk_lsect_t offset;
	unsigned long trklen;
	dsk_err_t err;

   (void)format;
	if (!self || !geom || self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (!lpxself->lpx_fp) return DSK_ERR_NOTRDY;
	if (lpxself->lpx_readonly) return DSK_ERR_RDONLY;

	trklen = geom->dg_sectors * geom->dg_secsize;

	err = dg_ps2ls(geom, cylinder, head, geom->dg_secbase, &offset);
	if (err) return err;
	offset *= geom->dg_secsize;

	err = seekto(lpxself, offset);
	if (err) return err;
	if (lpxself->lpx_filesize < offset + trklen)
		lpxself->lpx_filesize = offset + trklen;

	while (trklen--) 
		if (fputc(filler, lpxself->lpx_fp) == EOF) return DSK_ERR_SYSERR;	

	return DSK_ERR_OK;
}

	

dsk_err_t logical_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	LOGICAL_DSK_DRIVER *lpxself;
	dsk_err_t err;
	dsk_lsect_t offset;

	if (!self || !geom || self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (!lpxself->lpx_fp) return DSK_ERR_NOTRDY;

	if (cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
		return DSK_ERR_SEEKFAIL;

	err = dg_ps2ls(geom, cylinder, head, geom->dg_secbase, &offset);
	if (err) return err;
	offset *= geom->dg_secsize;
	
	if (fseek(lpxself->lpx_fp, offset, SEEK_SET)) return DSK_ERR_SEEKFAIL;

	return DSK_ERR_OK;
}

dsk_err_t logical_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	LOGICAL_DSK_DRIVER *lpxself;

	if (!self || !geom || self->dr_class != &dc_logical) return DSK_ERR_BADPTR;
	lpxself = (LOGICAL_DSK_DRIVER *)self;

	if (!lpxself->lpx_fp) *result &= ~DSK_ST3_READY;
	if (lpxself->lpx_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

