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

/* This driver is for a MYZ80 hard drive image, which has a fixed geometry:
 * 128 sectors, 1 head, 64 cylinders, 1k/sector, with 256 bytes of 0xE5 
 * before the first sector. */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvmyz80.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_myz80 = 
{
	sizeof(MYZ80_DSK_DRIVER),
	"myz80",
	"MYZ80 hard drive driver",
	myz80_open,	/* open */
	myz80_creat,	/* create new */
	myz80_close,	/* close */
	myz80_read,	/* read sector, working from physical address */
	myz80_write,	/* write sector, working from physical address */
	myz80_format,	/* format track, physical */
	myz80_getgeom,	/* Get geometry */
	NULL,		/* sector ID */
	myz80_xseek,	/* Seek to track */
	myz80_status,	/* Get drive status */
};

dsk_err_t myz80_open(DSK_DRIVER *self, const char *filename)
{
	MYZ80_DSK_DRIVER *mzself;
	unsigned char header[256];
	int n;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	mzself->mz_fp = fopen(filename, "r+b");
	if (!mzself->mz_fp) 
	{
		mzself->mz_readonly = 1;
		mzself->mz_fp = fopen(filename, "rb");
	}
	if (!mzself->mz_fp) return DSK_ERR_NOTME;
	if (fread(header, 1, 256, mzself->mz_fp) < 256) 
	{
		fclose(mzself->mz_fp);
		return DSK_ERR_NOTME;
	}
	for (n = 0; n < 256; n++) if (header[n] != 0xE5) 
	{
		fclose(mzself->mz_fp);
		return DSK_ERR_NOTME;
	}
/* v0.9.5: Record exact size, so we can tell if we're writing off the end
 * of the file. Under Windows, writing off the end of the file fills the 
 * gaps with random data, which can cause mess to appear in the directory;
 * and under UNIX, the entire directory is filled with zeroes. */
	if (fseek(mzself->mz_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	mzself->mz_filesize = ftell(mzself->mz_fp);

	/* Set up CP/M filesystem parameters */
	dsk_isetoption(self, "FS:CP/M:BSH", 5, 1);
	dsk_isetoption(self, "FS:CP/M:BLM", 31, 1);
	dsk_isetoption(self, "FS:CP/M:EXM", 1, 1); 
	dsk_isetoption(self, "FS:CP/M:DSM", 0x7ff, 1);
	dsk_isetoption(self, "FS:CP/M:DRM", 0x3ff, 1);
	dsk_isetoption(self, "FS:CP/M:AL0", 0xFF, 1);
	dsk_isetoption(self, "FS:CP/M:AL1", 0, 1);
	dsk_isetoption(self, "FS:CP/M:CKS", (int)0x8000, 1);
	dsk_isetoption(self, "FS:CP/M:OFF", 0, 1);

	return DSK_ERR_OK;
}


dsk_err_t myz80_creat(DSK_DRIVER *self, const char *filename)
{
	MYZ80_DSK_DRIVER *mzself;
	int n;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	mzself->mz_fp = fopen(filename, "w+b");
	mzself->mz_readonly = 0;
	if (!mzself->mz_fp) return DSK_ERR_SYSERR;
	for (n = 0; n < 256; n++) 
	{
		if (fputc(0xE5, mzself->mz_fp) == EOF)
		{
			fclose(mzself->mz_fp);
			return DSK_ERR_SYSERR;
		}

	}
	dsk_isetoption(self, "FS:CP/M:BSH", 5, 1);
	dsk_isetoption(self, "FS:CP/M:BLM", 31, 1);
	dsk_isetoption(self, "FS:CP/M:EXM", 1, 1); 
	dsk_isetoption(self, "FS:CP/M:DSM", 0x7ff, 1);
	dsk_isetoption(self, "FS:CP/M:DRM", 0x3ff, 1);
	dsk_isetoption(self, "FS:CP/M:AL0", 0xFF, 1);
	dsk_isetoption(self, "FS:CP/M:AL1", 0, 1);
	dsk_isetoption(self, "FS:CP/M:CKS", (int)0x8000, 1);
	dsk_isetoption(self, "FS:CP/M:OFF", 0, 1);
	return DSK_ERR_OK;
}


dsk_err_t myz80_close(DSK_DRIVER *self)
{
	MYZ80_DSK_DRIVER *mzself;

	if (self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	if (mzself->mz_fp) 
	{
		if (fclose(mzself->mz_fp) == EOF) return DSK_ERR_SYSERR;
		mzself->mz_fp = NULL;
	}
	return DSK_ERR_OK;	
}


dsk_err_t myz80_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	MYZ80_DSK_DRIVER *mzself;
	long offset;
	unsigned aread;

	if (!buf || !self || !geom || self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	if (!mzself->mz_fp) return DSK_ERR_NOTRDY;

	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (131072L * cylinder) + (1024L * sector) + 256;

	if (fseek(mzself->mz_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	/* v0.4.3: MYZ80 disc files can be shorter than the full 8Mb. If so,
	 *        the missing sectors are all assumed to be full of 0xE5s.
	 *        Unlike in "raw" files, it is not an error to try to read 
	 *        a missing sector. */
	aread = fread(buf, 1, geom->dg_secsize, mzself->mz_fp);
	while (aread < geom->dg_secsize)
	{
		((unsigned char *)buf)[aread++] = 0xE5;	
	}
	return DSK_ERR_OK;
}



dsk_err_t myz80_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	MYZ80_DSK_DRIVER *mzself;
	unsigned long offset;

	if (!buf || !self || !geom || self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	if (!mzself->mz_fp) return DSK_ERR_NOTRDY;
	if (mzself->mz_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (131072L * cylinder) + (1024L * sector) + 256;

	/* v0.9.5: If the file is extended, it must be with zeroes. Otherwise
	 * the intervening blocks get filled with zeroes (Unix) / 
	 * rubbish (Windows). Not a problem for data blocks, but dangerous
	 * for the directory. */
	if (mzself->mz_filesize < offset)
	{
		if (fseek(mzself->mz_fp, mzself->mz_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (mzself->mz_filesize < (offset + geom->dg_secsize))
		{
			if (fputc(0xE5, mzself->mz_fp) == EOF) return DSK_ERR_SYSERR;
			++mzself->mz_filesize;
		}
	}	
	if (fseek(mzself->mz_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	if (fwrite(buf, 1, geom->dg_secsize, mzself->mz_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	if (fseek(mzself->mz_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	mzself->mz_filesize = ftell(mzself->mz_fp);
	return DSK_ERR_OK;
}


dsk_err_t myz80_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw MYZ80
 * images don't hold track headers.
 */
	MYZ80_DSK_DRIVER *mzself;
	unsigned long offset;
	long trklen;

	(void)format;
	if (!self || !geom || self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	mzself = (MYZ80_DSK_DRIVER *)self;

	if (!mzself->mz_fp) return DSK_ERR_NOTRDY;
	if (mzself->mz_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (131072L * cylinder) + 256;
	trklen = 131072L;

	/* v0.9.5: If the file is extended, it must be with zeroes. Otherwise
	 * the intervening blocks get filled with zeroes (Unix) / 
	 * rubbish (Windows). Not a problem for data blocks, but dangerous
	 * for the directory. */
	if (mzself->mz_filesize < offset)
	{
		if (fseek(mzself->mz_fp, mzself->mz_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (mzself->mz_filesize < offset + trklen)
		{
			if (fputc(0xE5, mzself->mz_fp) == EOF) return DSK_ERR_SYSERR;
			++mzself->mz_filesize;
		}
	}	
	if (fseek(mzself->mz_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	while (trklen--) 
		if (fputc(filler, mzself->mz_fp) == EOF) return DSK_ERR_SYSERR;	
	if (fseek(mzself->mz_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	mzself->mz_filesize = ftell(mzself->mz_fp);

	return DSK_ERR_OK;
}

	
dsk_err_t myz80_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	if (!geom || !self || self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
	geom->dg_sidedness = SIDES_ALT;
	geom->dg_cylinders = 64;
	geom->dg_heads     = 1;
	geom->dg_sectors   = 128;
	geom->dg_secbase   = 0;
	geom->dg_secsize   = 1024;
	geom->dg_datarate  = RATE_ED;	/* From here on, values are dummy 
					 * values. It's highly unlikely 
					 * anyone will be able to format a
					 * floppy to this format! */
	geom->dg_rwgap     = 0x2A;
	geom->dg_fmtgap    = 0x52;
	geom->dg_fm        = RECMODE_MFM;
	geom->dg_nomulti   = 0;
	return DSK_ERR_OK;
}

dsk_err_t myz80_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			dsk_pcyl_t cylinder, dsk_phead_t head)
{
	if (cylinder >= 64) return DSK_ERR_SEEKFAIL;
	return DSK_ERR_OK;
}


dsk_err_t myz80_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
        MYZ80_DSK_DRIVER *mzself;

        if (!self || !geom || self->dr_class != &dc_myz80) return DSK_ERR_BADPTR;
        mzself = (MYZ80_DSK_DRIVER *)self;

        if (!mzself->mz_fp) *result &= ~DSK_ST3_READY;
        if (mzself->mz_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

