/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2008  John Elliott <seasip.webmaster@gmail.com>       *
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

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvsimh.h"

/* The SIMH disc image is assumed to be in a single fixed format, like the
 * MYZ80 disc image.
 *
 * Geometry:
 * 	254 tracks (presumed to be 127 cylinders, 2 heads)
 * 	 32 sectors / track
 *      137 bytes/sector: 3 bytes header, 128 bytes data, 4 bytes trailer
 *
 * CP/M filesystem parameters:
 * 	SPT = 0x20
 * 	BSH = 0x04
 * 	BLM = 0x0F
 * 	EXM = 0x00
 * 	DSM = 0x01EF
 * 	DRM = 0x00FF
 * 	AL0 = 0xF0
 * 	AL1 = 0x00
 *	CKS = 0x40
 *	OFF = 0x06
 */
/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_simh = 
{
	sizeof(SIMH_DSK_DRIVER),
	"simh",
	"SIMH disc image driver",
	simh_open,	/* open */
	simh_creat,	/* create new */
	simh_close,	/* close */
	simh_read,	/* read sector, working from physical address */
	simh_write,	/* write sector, working from physical address */
	simh_format,	/* format track, physical */
	simh_getgeom,	/* Get geometry */
	NULL,		/* sector ID */
	simh_xseek,	/* Seek to track */
	simh_status,	/* Get drive status */
};



dsk_err_t simh_open(DSK_DRIVER *self, const char *filename)
{
	SIMH_DSK_DRIVER *simh_self;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	simh_self->simh_fp = fopen(filename, "r+b");
	if (!simh_self->simh_fp) 
	{
		simh_self->simh_readonly = 1;
		simh_self->simh_fp = fopen(filename, "rb");
	}
	if (!simh_self->simh_fp) return DSK_ERR_NOTME;

	/* This code is left over from MYZ80 and may not be necessary: 
	 * allow shorter images than the standard, and grow them when
	 * necessary. */
	if (fseek(simh_self->simh_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	simh_self->simh_filesize = ftell(simh_self->simh_fp);

	/* Set up CP/M filesystem parameters */
	dsk_isetoption(self, "FS:CP/M:BSH", 4, 1);
	dsk_isetoption(self, "FS:CP/M:BLM", 15, 1);
	dsk_isetoption(self, "FS:CP/M:EXM", 0, 1); 
	dsk_isetoption(self, "FS:CP/M:DSM", 0x1ef, 1);
	dsk_isetoption(self, "FS:CP/M:DRM", 0x0ff, 1);
	dsk_isetoption(self, "FS:CP/M:AL0", 0xF0, 1);
	dsk_isetoption(self, "FS:CP/M:AL1", 0, 1);
	dsk_isetoption(self, "FS:CP/M:CKS", 0x40, 1);
	dsk_isetoption(self, "FS:CP/M:OFF", 6, 1);

	return DSK_ERR_OK;
}


dsk_err_t simh_creat(DSK_DRIVER *self, const char *filename)
{
	SIMH_DSK_DRIVER *simh_self;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	simh_self->simh_fp = fopen(filename, "w+b");
	simh_self->simh_readonly = 0;
	if (!simh_self->simh_fp) return DSK_ERR_SYSERR;
	dsk_isetoption(self, "FS:CP/M:BSH", 4, 1);
	dsk_isetoption(self, "FS:CP/M:BLM", 15, 1);
	dsk_isetoption(self, "FS:CP/M:EXM", 0, 1); 
	dsk_isetoption(self, "FS:CP/M:DSM", 0x1ef, 1);
	dsk_isetoption(self, "FS:CP/M:DRM", 0x0ff, 1);
	dsk_isetoption(self, "FS:CP/M:AL0", 0xF0, 1);
	dsk_isetoption(self, "FS:CP/M:AL1", 0, 1);
	dsk_isetoption(self, "FS:CP/M:CKS", 0x40, 1);
	dsk_isetoption(self, "FS:CP/M:OFF", 6, 1);

	return DSK_ERR_OK;
}


dsk_err_t simh_close(DSK_DRIVER *self)
{
	SIMH_DSK_DRIVER *simh_self;

	if (self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	if (simh_self->simh_fp) 
	{
		if (fclose(simh_self->simh_fp) == EOF) return DSK_ERR_SYSERR;
		simh_self->simh_fp = NULL;
	}
	return DSK_ERR_OK;	
}


dsk_err_t simh_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	SIMH_DSK_DRIVER *simh_self;
	long offset;
	unsigned aread;

	if (!buf || !self || !geom || self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	if (!simh_self->simh_fp) return DSK_ERR_NOTRDY;

	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (4384L * (2*cylinder+head)) + (137L * sector) + 3;

	if (fseek(simh_self->simh_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	/* Fill missing data with 0xE5 */
	aread = fread(buf, 1, geom->dg_secsize, simh_self->simh_fp);
	while (aread < geom->dg_secsize)
	{
		((unsigned char *)buf)[aread++] = 0xE5;	
	}
	return DSK_ERR_OK;
}

static unsigned char trailer[4] = { 0xE5, 0xE5, 0xE5, 0xE5 };

dsk_err_t simh_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	SIMH_DSK_DRIVER *simh_self;
	unsigned long offset;

	if (!buf || !self || !geom || self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	if (!simh_self->simh_fp) return DSK_ERR_NOTRDY;
	if (simh_self->simh_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (4384L * (2*cylinder+head)) + (137L * sector) + 3;

	if (simh_self->simh_filesize < offset)
	{
		if (fseek(simh_self->simh_fp, simh_self->simh_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (simh_self->simh_filesize < (offset + geom->dg_secsize))
		{
			if (fputc(0xE5, simh_self->simh_fp) == EOF) return DSK_ERR_SYSERR;
			++simh_self->simh_filesize;
		}
	}	
	if (fseek(simh_self->simh_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	if (fwrite(buf, 1, geom->dg_secsize, simh_self->simh_fp) < geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	/* After the sector, write 4 bytes trailer */
	if (fwrite(trailer, 1, sizeof(trailer), simh_self->simh_fp) < 
			(int)sizeof(trailer))
	{
		return DSK_ERR_NOADDR;
	}

	if (fseek(simh_self->simh_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	simh_self->simh_filesize = ftell(simh_self->simh_fp);
	return DSK_ERR_OK;
}


dsk_err_t simh_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw SIMH
 * images don't hold track headers.
 */
	SIMH_DSK_DRIVER *simh_self;
	unsigned long offset;
	long trklen;

	(void)format;
	if (!self || !geom || self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	simh_self = (SIMH_DSK_DRIVER *)self;

	if (!simh_self->simh_fp) return DSK_ERR_NOTRDY;
	if (simh_self->simh_readonly) return DSK_ERR_RDONLY;

	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	/* Convert from physical to logical sector, using the fixed geometry. */
	offset = (4384L * (2*cylinder+head)) + 3;
	trklen = 4348L;

	if (simh_self->simh_filesize < offset)
	{
		if (fseek(simh_self->simh_fp, simh_self->simh_filesize, SEEK_SET)) return DSK_ERR_SYSERR;
		while (simh_self->simh_filesize < offset + trklen)
		{
			if (fputc(0xE5, simh_self->simh_fp) == EOF) return DSK_ERR_SYSERR;
			++simh_self->simh_filesize;
		}
	}	
	if (fseek(simh_self->simh_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;

	while (trklen--) 
		if (fputc(filler, simh_self->simh_fp) == EOF) return DSK_ERR_SYSERR;	
	if (fseek(simh_self->simh_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	simh_self->simh_filesize = ftell(simh_self->simh_fp);

	return DSK_ERR_OK;
}

	
dsk_err_t simh_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	if (!geom || !self || self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
	geom->dg_sidedness = SIDES_ALT;
	geom->dg_cylinders = 127;
	geom->dg_heads     = 2;
	geom->dg_sectors   = 32;
	geom->dg_secbase   = 0;
	geom->dg_secsize   = 128;
	geom->dg_datarate  = RATE_DD;	/* From here on, values are dummy 
					 * values. I don't know what the
					 * correct entries are for an 8" 
					 * floppy, which is what I presume
					 * this image relates to. */
	geom->dg_rwgap     = 0x2A;
	geom->dg_fmtgap    = 0x52;
	geom->dg_fm        = RECMODE_MFM;
	geom->dg_nomulti   = 0;
	return DSK_ERR_OK;
}

dsk_err_t simh_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			dsk_pcyl_t cylinder, dsk_phead_t head)
{
	if (cylinder >= 128) return DSK_ERR_SEEKFAIL;
	return DSK_ERR_OK;
}


dsk_err_t simh_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
        SIMH_DSK_DRIVER *simh_self;

        if (!self || !geom || self->dr_class != &dc_simh) return DSK_ERR_BADPTR;
        simh_self = (SIMH_DSK_DRIVER *)self;

        if (!simh_self->simh_fp) *result &= ~DSK_ST3_READY;
        if (simh_self->simh_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

