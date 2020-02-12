/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001, 2008  John Elliott <seasip.webmaster@gmail.com>      *
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

/* This driver is for a YAZE YDSK hard drive image, which begins with a
 * 128-byte header describing the geometry. Note that many of the fields 
 * are CP/M filesystem parameters; libdsk cannot set these */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvydsk.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_ydsk = 
{
	sizeof(YDSK_DSK_DRIVER),
	"ydsk",
	"YAZE YDSK driver",
	ydsk_open,	/* open */
	ydsk_creat,	/* create new */
	ydsk_close,	/* close */
	ydsk_read,	/* read sector, working from physical address */
	ydsk_write,	/* write sector, working from physical address */
	ydsk_format,	/* format track, physical */
	ydsk_getgeom,	/* Get geometry */
	NULL,		/* sector ID */
	ydsk_xseek,	/* Seek to track */
	ydsk_status,	/* Get drive status */
	NULL,		/* xread */
	NULL,		/* xwrite */
	NULL,		/* tread */
	NULL,		/* xtread */
	ydsk_option_enum,	/* List options */
	ydsk_option_set,	/* Set option */
	ydsk_option_get,	/* Get option */
};

static void update_geometry(YDSK_DSK_DRIVER *self, const DSK_GEOMETRY *geom)
{
	unsigned short spt = self->ydsk_header[32] + 256 * self->ydsk_header[33];
	unsigned short psh = self->ydsk_header[47];
	unsigned long secsize = (128L << psh);

	if (geom->dg_sectors != (spt >> psh) || secsize != geom->dg_secsize)
	{
		spt = geom->dg_sectors << psh;
		self->ydsk_header_dirty = 1;
		self->ydsk_super.dr_dirty = 1;
		self->ydsk_header[47] = dsk_get_psh(geom->dg_secsize);
		self->ydsk_header[32] = spt & 0xFF;
		self->ydsk_header[33] = (spt >> 8) & 0xFF;

		/* If we have to record a sector size not equal to 128 bytes,
		 * mark this as a YAZE 1.x image */
		if (self->ydsk_header[47] != 0)
		{
			self->ydsk_header[16] = 1;
		}
	}	
}

static dsk_err_t ydsk_seek(YDSK_DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			dsk_pcyl_t cylinder, dsk_phead_t head, 
			dsk_psect_t sector, int extend)
{
/* Get size of a track */
	unsigned short spt = self->ydsk_header[32] + 256 * self->ydsk_header[33];
	unsigned short psh = self->ydsk_header[47];
	unsigned long secsize = (128L << psh);
	unsigned long tracklen = secsize * (spt >> psh);
/* And convert from that to offset */
	unsigned long offset;
       

	if (geom->dg_heads == 1) offset = cylinder * tracklen;
	else			 offset = (cylinder*2 + head) * tracklen;

	offset += (sector * secsize) + 128;

/* If the file is smaller than required, grow it */
	if (extend && self->ydsk_filesize < offset)
	{
		if (fseek(self->ydsk_fp, self->ydsk_filesize, 
					SEEK_SET)) return DSK_ERR_SYSERR;
		while (self->ydsk_filesize < (offset + secsize))
		{
			if (fputc(0xE5, self->ydsk_fp) == EOF) 
				return DSK_ERR_SYSERR;
			++self->ydsk_filesize;
		}
	}
	if (fseek(self->ydsk_fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}


dsk_err_t ydsk_open(DSK_DRIVER *self, const char *filename)
{
	YDSK_DSK_DRIVER *ydsk_self;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	ydsk_self->ydsk_fp = fopen(filename, "r+b");
	if (!ydsk_self->ydsk_fp) 
	{
		ydsk_self->ydsk_readonly = 1;
		ydsk_self->ydsk_fp = fopen(filename, "rb");
	}
	if (!ydsk_self->ydsk_fp) return DSK_ERR_NOTME;
	if (fread(ydsk_self->ydsk_header, 1, 128, ydsk_self->ydsk_fp) < 128) 
	{
		fclose(ydsk_self->ydsk_fp);
		return DSK_ERR_NOTME;
	}
/* Check magic number */
	if (memcmp(ydsk_self->ydsk_header, "<CPM_Disk>", 10))
	{
		fclose(ydsk_self->ydsk_fp);
		return DSK_ERR_NOTME;
	}
/* Record exact size, so we can tell if we're writing off the end
 * of the file. Under Windows, writing off the end of the file fills the 
 * gaps with random data, which can cause mess to appear in the directory;
 * and under UNIX, the entire directory is filled with zeroes. */
	if (fseek(ydsk_self->ydsk_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	ydsk_self->ydsk_filesize = ftell(ydsk_self->ydsk_fp);

	return DSK_ERR_OK;
}


dsk_err_t ydsk_creat(DSK_DRIVER *self, const char *filename)
{
	YDSK_DSK_DRIVER *ydsk_self;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	ydsk_self->ydsk_fp = fopen(filename, "w+b");
	ydsk_self->ydsk_readonly = 0;
	if (!ydsk_self->ydsk_fp) return DSK_ERR_SYSERR;

	/* Zap the header */
	memset(ydsk_self->ydsk_header, 0, sizeof(ydsk_self->ydsk_header));
	memcpy(ydsk_self->ydsk_header, "<CPM_Disk>", 10);

	/* Give it 128 sectors/track */
	ydsk_self->ydsk_header[32] = 128;

	if (fwrite(ydsk_self->ydsk_header, 1, 128, ydsk_self->ydsk_fp) < 128)
	{
		fclose(ydsk_self->ydsk_fp);
		return DSK_ERR_SYSERR;
	}
	return DSK_ERR_OK;
}


dsk_err_t ydsk_close(DSK_DRIVER *self)
{
	YDSK_DSK_DRIVER *ydsk_self;

	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	if (ydsk_self->ydsk_fp) 
	{
		if (ydsk_self->ydsk_header_dirty)
		{
/* Write out the header */
			if (fseek(ydsk_self->ydsk_fp, 0, SEEK_SET)
			||  fwrite(ydsk_self->ydsk_header, 1, 128, 
						ydsk_self->ydsk_fp) < 128)
			{
				fclose(ydsk_self->ydsk_fp);
				return DSK_ERR_SYSERR;
			}
		}
		if (fclose(ydsk_self->ydsk_fp))
			return DSK_ERR_SYSERR;
	}
	return DSK_ERR_OK;
}





dsk_err_t ydsk_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	YDSK_DSK_DRIVER *ydsk_self;
	unsigned aread;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	if (!ydsk_self->ydsk_fp) return DSK_ERR_NOTRDY;

	err = ydsk_seek(ydsk_self, geom, cylinder, head, sector - geom->dg_secbase, 0);
	if (err) return err;
	/* Assume unwritten sectors hold 0xE5 */
	aread = fread(buf, 1, geom->dg_secsize, ydsk_self->ydsk_fp);
	while (aread < geom->dg_secsize)
	{
		((unsigned char *)buf)[aread++] = 0xE5;	
	}
	return DSK_ERR_OK;
}



dsk_err_t ydsk_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	YDSK_DSK_DRIVER *ydsk_self;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_ydsk) 
		return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	if (!ydsk_self->ydsk_fp) return DSK_ERR_NOTRDY;
	if (ydsk_self->ydsk_readonly) return DSK_ERR_RDONLY;

	err = ydsk_seek(ydsk_self, geom, cylinder, head, sector - geom->dg_secbase, 1);
	if (err) return err;

	if (fwrite(buf, 1, geom->dg_secsize, ydsk_self->ydsk_fp) < 
			geom->dg_secsize)
	{
		return DSK_ERR_NOADDR;
	}
	if (fseek(ydsk_self->ydsk_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	ydsk_self->ydsk_filesize = ftell(ydsk_self->ydsk_fp);
	return DSK_ERR_OK;
}


dsk_err_t ydsk_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
/*
 * Note that we completely ignore the "format" parameter, since raw YDSK
 * images don't hold track headers.
 */
	YDSK_DSK_DRIVER *ydsk_self;
	unsigned short spt, psh;
	unsigned long secsize, tracklen;
	dsk_err_t err;

	(void)format;
	if (!self || !geom || self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	if (!ydsk_self->ydsk_fp) return DSK_ERR_NOTRDY;
	if (ydsk_self->ydsk_readonly) return DSK_ERR_RDONLY;
	spt = ydsk_self->ydsk_header[32] + 256 * ydsk_self->ydsk_header[33];
	psh = ydsk_self->ydsk_header[47];
	secsize = (128L << psh);
	tracklen = secsize * (spt >> psh);

	/* Update the header if the format geometry doesn't match the 
	 * existing YDSK geometry */
	update_geometry(ydsk_self, geom);

	err = ydsk_seek(ydsk_self, geom, cylinder, head, 0, 1);
	if (err) return err;

	while (tracklen--) 
	{
		if (fputc(filler, ydsk_self->ydsk_fp) == EOF) return DSK_ERR_SYSERR;	
	}
	if (fseek(ydsk_self->ydsk_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	ydsk_self->ydsk_filesize = ftell(ydsk_self->ydsk_fp);

	return DSK_ERR_OK;
}


/* We have a CP/M disk parameter block. Convert that to drive geometry */
dsk_err_t ydsk_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	unsigned short spt, psh, off;
	unsigned long secsize, blocksize, drivesize, dsm, tracklen;
	YDSK_DSK_DRIVER *ydsk_self;

	if (!self || !geom || self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	spt = ydsk_self->ydsk_header[32] + 256 * ydsk_self->ydsk_header[33];
	psh = ydsk_self->ydsk_header[47];
	secsize = (128L << psh);
	tracklen = secsize * (spt >> psh);
	/* Work out how many cylinders we have. */
	/* If there is a full DPB, we can use that. */
	dsm = ydsk_self->ydsk_header[37] + 256 * ydsk_self->ydsk_header[38];
	off = ydsk_self->ydsk_header[45] + 256 * ydsk_self->ydsk_header[46];
	if (dsm && ydsk_self->ydsk_header[34])
	{
		blocksize = 128L << ydsk_self->ydsk_header[34];
/* Drive size is now size of data area + size of system area */
		drivesize = (blocksize * (1 + dsm)) + tracklen * off;
	}
	else	/* No DPB. Guess. */
	{
		if (fseek(ydsk_self->ydsk_fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
		drivesize = ftell(ydsk_self->ydsk_fp) - 128;
	}

	geom->dg_sidedness = SIDES_ALT;
	geom->dg_cylinders = (drivesize + (tracklen - 1)) / tracklen;
	geom->dg_heads     = 1;
	geom->dg_sectors   = (spt >> psh);
	geom->dg_secbase   = 0;
	geom->dg_secsize   = secsize;
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

dsk_err_t ydsk_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			dsk_pcyl_t cylinder, dsk_phead_t head)
{
	YDSK_DSK_DRIVER *ydsk_self;
	if (!self || !geom || self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	return ydsk_seek(ydsk_self, geom, cylinder, head, 0, 0);
}


dsk_err_t ydsk_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
        YDSK_DSK_DRIVER *ydsk_self;

        if (!self || !geom || self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
        ydsk_self = (YDSK_DSK_DRIVER *)self;

        if (!ydsk_self->ydsk_fp) *result &= ~DSK_ST3_READY;
        if (ydsk_self->ydsk_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

/* CP/M-specific filesystem parameters */
static char *option_names[] = 
{
	"FS:CP/M:BSH", "FS:CP/M:BLM", "FS:CP/M:EXM",
	"FS:CP/M:DSM", "FS:CP/M:DRM", "FS:CP/M:AL0", "FS:CP/M:AL1",
	"FS:CP/M:CKS", "FS:CP/M:OFF",
};

#define MAXOPTION (sizeof(option_names) / sizeof(option_names[0]))

dsk_err_t ydsk_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	if (!self) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;

	if (idx >= 0 && idx < (int)MAXOPTION)
	{
		if (optname) *optname = option_names[idx];
		return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;	

}

/* Set a driver-specific option */
dsk_err_t ydsk_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	YDSK_DSK_DRIVER *ydsk_self;
	unsigned idx;

	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	for (idx = 0; idx < MAXOPTION; idx++)
	{
		if (!strcmp(optname, option_names[idx]))
			break;
	}
	if (idx >= MAXOPTION) return DSK_ERR_BADOPT;

	ydsk_self->ydsk_header_dirty = 1;
	self->dr_dirty = 1;
	switch(idx)
	{
		case 0: ydsk_self->ydsk_header[34] = value & 0xFF;	// BSH
			break;
		case 1: ydsk_self->ydsk_header[35] = value & 0xFF;	// BLM
			break;
		case 2: ydsk_self->ydsk_header[36] = value & 0xFF;	// EXM
			break;
		case 3: ydsk_self->ydsk_header[37] = value & 0xFF;	// DSM
			ydsk_self->ydsk_header[38] = (value >> 8) & 0xFF;
			break;
		case 4: ydsk_self->ydsk_header[39] = value & 0xFF;	// DRM
			ydsk_self->ydsk_header[40] = (value >> 8) & 0xFF;
			break;
		case 5: ydsk_self->ydsk_header[41] = value & 0xFF;	// AL0 
			break;
		case 6: ydsk_self->ydsk_header[42] = value & 0xFF;	// AL1
			break;
		case 7: ydsk_self->ydsk_header[43] = value & 0xFF;	// CKS
			ydsk_self->ydsk_header[44] = (value >> 8) & 0xFF;
			break;
		case 8: ydsk_self->ydsk_header[45] = value & 0xFF;	// OFF
			ydsk_self->ydsk_header[46] = (value >> 8) & 0xFF;
			break;
	}
	return DSK_ERR_OK;
}


dsk_err_t ydsk_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	YDSK_DSK_DRIVER *ydsk_self;
	unsigned idx;
	int v = 0;

	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ydsk) return DSK_ERR_BADPTR;
	ydsk_self = (YDSK_DSK_DRIVER *)self;

	for (idx = 0; idx < MAXOPTION; idx++)
	{
		if (!strcmp(optname, option_names[idx]))
			break;
	}
	if (idx >= MAXOPTION) return DSK_ERR_BADOPT;

	switch(idx)
	{
		case 0: v = ydsk_self->ydsk_header[34];	// BSH
			break;
		case 1: v = ydsk_self->ydsk_header[35];	// BLM
			break;
		case 2: v = ydsk_self->ydsk_header[36];	// EXM
			break;
		case 3: v = ydsk_self->ydsk_header[37] + 256 *	// DSM
			    ydsk_self->ydsk_header[38];
			break;
		case 4: v = ydsk_self->ydsk_header[39] + 256 *	// DRM
			    ydsk_self->ydsk_header[40];
			break;
		case 5: v = ydsk_self->ydsk_header[41];	// AL0 
			break;
		case 6: v = ydsk_self->ydsk_header[42];	// AL1
			break;
		case 7: v = ydsk_self->ydsk_header[43] + 256 *	// CKS
		            ydsk_self->ydsk_header[44];
			break;
		case 8: v = ydsk_self->ydsk_header[45] + 256 *	// OFF
		            ydsk_self->ydsk_header[46];
			break;
	}
	if (value) *value = v;
	return DSK_ERR_OK;
}

