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

/* This driver is the most basic of the drivers, and simply implements 
 * access to a flat file with the tracks laid out in the SIDES_ALT order */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvposix.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_posix = 
{
	sizeof(POSIX_DSK_DRIVER),
	"raw",
	"Raw file driver",
	posix_open,	/* open */
	posix_creat,	/* create new */
	posix_close,	/* close */
	posix_read,	/* read sector, working from physical address */
	posix_write,	/* write sector, working from physical address */
	posix_format,	/* format track, physical */
	NULL,		/* get geometry */
	NULL,		/* sector ID */
	posix_xseek,	/* seek to track */
	posix_status,	/* drive status */
};

dsk_err_t posix_open(DSK_DRIVER *self, const char *filename)
{
	POSIX_DSK_DRIVER *pxself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	pxself->px_fp = fopen(filename, "r+b");
	if (!pxself->px_fp) 
	{
		pxself->px_readonly = 1;
		pxself->px_fp = fopen(filename, "rb");
	}
	if (!pxself->px_fp) return DSK_ERR_NOTME;
/* v0.9.5: Record exact size, so we can tell if we're writing off the end
 * of the file. Under Windows, writing off the end of the file fills the 
 * gaps with random data, which can cause mess to appear in the directory;
 * and under UNIX, the entire directory is filled with zeroes. */
        if (fseek(pxself->px_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
        pxself->px_filesize = ftell(pxself->px_fp);

	return DSK_ERR_OK;
}


dsk_err_t posix_creat(DSK_DRIVER *self, const char *filename)
{
	POSIX_DSK_DRIVER *pxself;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	pxself->px_fp = fopen(filename, "w+b");
	pxself->px_readonly = 0;
	if (!pxself->px_fp) return DSK_ERR_SYSERR;
	pxself->px_filesize = 0;
	return DSK_ERR_OK;
}


dsk_err_t posix_close(DSK_DRIVER *self)
{
	POSIX_DSK_DRIVER *pxself;

	if (self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (pxself->px_fp) 
	{
		if (fclose(pxself->px_fp) == EOF) return DSK_ERR_SYSERR;
		pxself->px_fp = NULL;
	}
	return DSK_ERR_OK;	
}


dsk_err_t posix_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	POSIX_DSK_DRIVER *pxself;
	long offset;

	if (!buf || !self || !geom || self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (!pxself->px_fp) return DSK_ERR_NOTRDY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	offset *= geom->dg_sectors;
	offset += (sector - geom->dg_secbase);
	offset *=  geom->dg_secsize;

	if (fseek(pxself->px_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	if (fread(buf, 1, geom->dg_secsize, pxself->px_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}


static dsk_err_t seekto(POSIX_DSK_DRIVER *self, unsigned long offset)
{
	/* 0.9.5: Fill any "holes" in the file with 0xE5. Otherwise, UNIX would
	 * fill them with zeroes and Windows would fill them with whatever
	 * happened to be lying around */
	if (self->px_filesize < offset)
	{
		if (fseek(self->px_fp, self->px_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (self->px_filesize < offset)
		{
			if (fputc(0xE5, self->px_fp) == EOF) return DSK_ERR_SYSERR;
			++self->px_filesize;
		}
	}
	if (fseek(self->px_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}

dsk_err_t posix_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	POSIX_DSK_DRIVER *pxself;
	unsigned long offset;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (!pxself->px_fp) return DSK_ERR_NOTRDY;
	if (pxself->px_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	offset *= geom->dg_sectors;
	offset += (sector - geom->dg_secbase);
	offset *=  geom->dg_secsize;

	err = seekto(pxself, offset);
	if (err) return err;

	if (fwrite(buf, 1, geom->dg_secsize, pxself->px_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	if (pxself->px_filesize < offset + geom->dg_secsize)
		pxself->px_filesize = offset + geom->dg_secsize;
	return DSK_ERR_OK;
}


dsk_err_t posix_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw POSIX
 * images don't hold track headers.
 */
	POSIX_DSK_DRIVER *pxself;
	unsigned long offset;
	unsigned long trklen;
	dsk_err_t err;

   (void)format;
	if (!self || !geom || self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (!pxself->px_fp) return DSK_ERR_NOTRDY;
	if (pxself->px_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	trklen = geom->dg_sectors * geom->dg_secsize;
	offset *= trklen;

	err = seekto(pxself, offset);
	if (err) return err;
	if (pxself->px_filesize < offset + trklen)
		pxself->px_filesize = offset + trklen;

	while (trklen--) 
		if (fputc(filler, pxself->px_fp) == EOF) return DSK_ERR_SYSERR;	

	return DSK_ERR_OK;
}

	

dsk_err_t posix_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	POSIX_DSK_DRIVER *pxself;
	long offset;

	if (!self || !geom || self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (!pxself->px_fp) return DSK_ERR_NOTRDY;

	if (cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
		return DSK_ERR_SEEKFAIL;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	offset *= geom->dg_sectors * geom->dg_secsize;
	
	if (fseek(pxself->px_fp, offset, SEEK_SET)) return DSK_ERR_SEEKFAIL;

	return DSK_ERR_OK;
}

dsk_err_t posix_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	POSIX_DSK_DRIVER *pxself;

	if (!self || !geom || self->dr_class != &dc_posix) return DSK_ERR_BADPTR;
	pxself = (POSIX_DSK_DRIVER *)self;

	if (!pxself->px_fp) *result &= ~DSK_ST3_READY;
	if (pxself->px_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

