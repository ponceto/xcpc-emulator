/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2005  John Elliott <seasip.webmaster@gmail.com>       *
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

/* Access functions for CPCEMU discs */

#include "drvi.h"
#include "drvcpcem.h"


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

/* The CPCEMU drivers for normal and extended modes are in fact the same,
 * except for the "open" and "create" functions; these have been separated
 * simply so EDSKs can be created. */

DRV_CLASS dc_cpcemu = 
{
	sizeof(CPCEMU_DSK_DRIVER),
	"dsk",
	"CPCEMU .DSK driver",
	cpcemu_open,	/* open */
	cpcemu_creat,   /* create new */
	cpcemu_close,   /* close */
	cpcemu_read,	/* read sector, working from physical address */
	cpcemu_write,   /* write sector, working from physical address */
	cpcemu_format,  /* format track, physical */
	NULL,	  	/* get geometry */
	cpcemu_secid,   /* logical sector ID */
	cpcemu_xseek,   /* seek to track */
	cpcemu_status,  /* get drive status */
	cpcemu_xread,   /* read sector */
	cpcemu_xwrite,  /* write sector */ 
	NULL,		/* Read a track (8272 READ TRACK command) */
	NULL,		/* Read a track: Version where the sector ID doesn't necessarily match */
	cpcemu_option_enum,	/* List driver-specific options */
	cpcemu_option_set,	/* Set a driver-specific option */
	cpcemu_option_get,	/* Get a driver-specific option */
	NULL/*cpcemu_trackids*/,/* Read headers for an entire track at once */
	NULL			/* Read raw track, including sector headers */
};

DRV_CLASS dc_cpcext = 
{
	sizeof(CPCEMU_DSK_DRIVER),
	"edsk",
	"Extended .DSK driver",
	cpcext_open,	/* open */
	cpcext_creat,   /* create new */
	cpcemu_close,   /* close */
	cpcemu_read,	/* read sector, working from physical address */
	cpcemu_write,   /* write sector, working from physical address */
	cpcemu_format,  /* format track, physical */
	NULL,		/* get geometry */
	cpcemu_secid,   /* logical sector ID */
	cpcemu_xseek,   /* seek to track */
	cpcemu_status,  /* get drive status */
	cpcemu_xread,   /* read sector */
	cpcemu_xwrite,  /* write sector */ 
	NULL,			/* Read a track (8272 READ TRACK command) */
	NULL,			/* Read a track: Version where the sector ID doesn't necessarily match */
	cpcemu_option_enum,	/* List driver-specific options */
	cpcemu_option_set,	/* Set a driver-specific option */
	cpcemu_option_get,	/* Get a driver-specific option */
	NULL/*cpcemu_trackids*/,/* Read headers for an entire track at once */
	NULL			/* Read raw track, including sector headers */
};			  



static dsk_err_t cpc_open(DSK_DRIVER *self, const char *filename, int ext);
static dsk_err_t cpc_creat(DSK_DRIVER *self, const char *filename, int ext);


dsk_err_t cpcemu_open(DSK_DRIVER *self, const char *filename)
{
	return cpc_open(self, filename, 0);
}

dsk_err_t cpcext_open(DSK_DRIVER *self, const char *filename)
{
	return cpc_open(self, filename, 1);
}

dsk_err_t cpcemu_creat(DSK_DRIVER *self, const char *filename)
{
	return cpc_creat(self, filename, 0);
}

dsk_err_t cpcext_creat(DSK_DRIVER *self, const char *filename)
{
	return cpc_creat(self, filename, 1);
}


#define DC_CHECK(self) if (self->dr_class != &dc_cpcemu && self->dr_class != &dc_cpcext) return DSK_ERR_BADPTR;


/* Open DSK image, checking for the magic number */
static dsk_err_t cpc_open(DSK_DRIVER *self, const char *filename, int extended)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	int n;
	
	/* Sanity check: Is this meant for our driver? */
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	cpc_self->cpc_fp = fopen(filename, "r+b");
	if (!cpc_self->cpc_fp) 
	{
		cpc_self->cpc_readonly = 1;
		cpc_self->cpc_fp = fopen(filename, "rb");
	}
	if (!cpc_self->cpc_fp) return DSK_ERR_NOTME;
	/* Check for CPCEMU signature */
	if (fread(cpc_self->cpc_dskhead, 1, 256, cpc_self->cpc_fp) < 256) 
	{
/* 1.1.6 Don't leak file handles */
		fclose(cpc_self->cpc_fp);
		return DSK_ERR_NOTME;
	}

	if (extended)
	{
		if (memcmp("EXTENDED", cpc_self->cpc_dskhead, 8)) 
		{
/* 1.1.6 Don't leak file handles */
			fclose(cpc_self->cpc_fp);
			return DSK_ERR_NOTME; 
		}
	}
	else 
	{
		if (memcmp("MV - CPC", cpc_self->cpc_dskhead, 8))
		{
/* 1.1.6 Don't leak file handles */
			fclose(cpc_self->cpc_fp);
			return DSK_ERR_NOTME; 
		}
	}
	/* OK, got signature. */
	cpc_self->cpc_trkhead[0] = 0;
	for (n = 0; n < 4; n++)
	{
		cpc_self->cpc_statusw[n] = -1;
		cpc_self->cpc_status[n]  = 0;
	}
	return DSK_ERR_OK;
}

/* Create DSK image */
static dsk_err_t cpc_creat(DSK_DRIVER *self, const char *filename, int extended)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	int n;
	
	/* Sanity check: Is this meant for our driver? */
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	cpc_self->cpc_fp = fopen(filename, "w+b");
	cpc_self->cpc_readonly = 0;
	if (!cpc_self->cpc_fp) return DSK_ERR_SYSERR;
	memset(cpc_self->cpc_dskhead, 0, 256);
		
	if (extended) strcpy((char *)cpc_self->cpc_dskhead,
		"EXTENDED CPC DSK File\r\nDisk-Info\r\n(LIBDSK)");
	else strcpy((char *)cpc_self->cpc_dskhead,
		"MV - CPCEMU Disk-File\r\nDisk-Info\r\n(LIBDSK)");
	if (fwrite(cpc_self->cpc_dskhead, 1 , 256, cpc_self->cpc_fp) < 256) 
		return DSK_ERR_SYSERR;
	cpc_self->cpc_trkhead[0] = 0;
	for (n = 0; n < 4; n++)
	{
		cpc_self->cpc_statusw[n] = -1;
		cpc_self->cpc_status[n]  = 0;
	}
	return DSK_ERR_OK;
}


dsk_err_t cpcemu_close(DSK_DRIVER *self)
{
	CPCEMU_DSK_DRIVER *cpc_self;

	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	if (cpc_self->cpc_fp) 
	{
		if (fclose(cpc_self->cpc_fp) == EOF) return DSK_ERR_SYSERR;
		cpc_self->cpc_fp = NULL;
	}
	return DSK_ERR_OK;  
}






/* Find the offset in a DSK for a particular cylinder/head. 
 *
 * CPCEMU DSK files work in "tracks". For a single-sided disk, track number
 * is the same as cylinder number. For a double-sided disk, track number is
 * (2 * cylinder + head). This is independent of disc format.
 */
static long lookup_track(CPCEMU_DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned char *b;
	dsk_ltrack_t track;
	long trk_offset;
	unsigned int nt;

	if (!self->cpc_fp) return -1;

	/* [LIBDSK v0.6.0] Compare with our header, not the passed
	 * geometry */
	/* Seek off the edge of the drive? Note that we allow one 
	 * extra cylinder & one extra head, so that we can move to 
	 * a blank track to format it. */
	if (cylinder >  self->cpc_dskhead[0x30]) return -1;
	if (head     >  self->cpc_dskhead[0x31]) return -1;

	/* Convert cylinder & head to CPCEMU "track" */

	track = cylinder;
	if (self->cpc_dskhead[0x31] > 1) track *= 2;
	track += head;

	/* Look up the cylinder and head using the header. This behaves 
	 * differently in normal and extended DSK files */
	
	if (!memcmp(self->cpc_dskhead, "EXTENDED", 8))
	{
		trk_offset = 256;   /* DSK header = 256 bytes */
		b = self->cpc_dskhead + 0x34;
		for (nt = 0; nt < track; nt++)
		{
			trk_offset += 256 * b[nt]; /* [v0.9.0] */
		}
	}
	else	/* Normal; all tracks have the same length */
	{
		trk_offset = (self->cpc_dskhead[0x33] * 256);
		trk_offset += self->cpc_dskhead[0x32];

		trk_offset *= track;		/* No. of tracks */
		trk_offset += 256;	  /* DSK header */	
	}
	return trk_offset;
}





/* Seek to a cylinder. Checks if that particular cylinder exists. 
 * We test for the existence of a cylinder by looking for Track <n>, Head 0.
 * Fortunately the DSK format does not allow for discs with different numbers
 * of tracks on each side (though this is obviously possible with a real disc)
 * so if head 0 exists then the whole cylinder does. 


static dsk_err_t seek_cylinder(CPCEMU_DSK_DRIVER *self, DSK_GEOMETRY *geom, int cylinder)
{
	long nr;
	if (!self->cpc_fp) return DSK_ERR_NOTRDY;

	// Check if the DSK image goes out to the correct cylinder 
	nr = lookup_track(self, geom, cylinder, 0);
	
	if (nr < 0) return DSK_ERR_SEEKFAIL;
	return DSK_ERR_OK;
}
*/

/* Load the "Track-Info" header for the given cylinder and head */
static dsk_err_t load_track_header(CPCEMU_DSK_DRIVER *self, 
	const DSK_GEOMETRY *geom, int cylinder, int head)
{
	long track;
	int  sector_size;
	unsigned char rate, recording;

	track = lookup_track(self, geom, cylinder, head);
	if (track < 0) return DSK_ERR_SEEKFAIL;	   /* Bad track */
	fseek(self->cpc_fp, track, SEEK_SET);
	if (fread(self->cpc_trkhead, 1, 256, self->cpc_fp) < 256)
		return DSK_ERR_NOADDR;			  /* Missing address mark */
	if (memcmp(self->cpc_trkhead, "Track-Info", 10))
	{
		return DSK_ERR_NOADDR;
	}
	/* Check if the track density and recording mode match the density
	 * and recording mode in the geometry. */
	sector_size = 128 << self->cpc_trkhead[0x14];

	rate	  = self->cpc_trkhead[0x12];
	recording = self->cpc_trkhead[0x13];

	/* Guess the data rate used. We assume Double Density, and then
	 * look at the number of sectors in the track to see if the
	 * format looks like a High Density one. */
	if (rate == 0)
	{
		if (sector_size == 1024 && self->cpc_trkhead[0x15] >= 7) 
		{
			rate = 2; /* ADFS F */
		}
		else if (sector_size == 512 && self->cpc_trkhead[0x15] >= 15)
		{
			rate = 2; /* IBM PC 1.2M or 1.4M */
		}
		else rate = 1;
	}
	/* Similarly for recording mode. Note that I check for exactly
	 * 10 sectors, because the MD3 copy protection scheme uses 9 
	 * 256-byte sectors and they're recorded using MFM. */
	if (recording == 0)
	{
		if (sector_size == 256 && self->cpc_trkhead[0x15] == 10)
		{
			recording = 1;  /* BBC Micro DFS */
		}
		else recording = 2;
	}
	switch(rate)
	{
		/* 1: Single / Double Density */
		case 1: if (geom->dg_datarate != RATE_SD && 
			    geom->dg_datarate != RATE_DD) return DSK_ERR_NOADDR;
			break;
		/* 2: High density */
		case 2: if (geom->dg_datarate != RATE_HD) return DSK_ERR_NOADDR;
			break;
		/* 3: Extra High Density */
		case 3: if (geom->dg_datarate != RATE_ED) return DSK_ERR_NOADDR;
			break;
		/* Unknown density */
		default:
			return DSK_ERR_NOADDR;
	}
	/* Check data rate */
	switch(recording)
	{
		/* Recording mode: 1 for FM */
		case 1: if ((geom->dg_fm & RECMODE_MASK) != RECMODE_FM) 
				return DSK_ERR_NOADDR;
			break;
		/* Recording mode: 2 for MFM */
		case 2: if ((geom->dg_fm & RECMODE_MASK) != RECMODE_MFM) 
				return DSK_ERR_NOADDR;
			break;
		default:	/* GCR??? */
			return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}


/* Read a sector ID from a given track */
dsk_err_t cpcemu_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			dsk_pcyl_t cyl, dsk_phead_t head, DSK_FORMAT *result)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	dsk_err_t e;
	int offs;

	if (!self || !geom || !result)
		return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;
	
	if (!cpc_self->cpc_fp) return DSK_ERR_NOTRDY;

	/* lookup_track() allows us to seek to a nonexistent track.  */
	/* But we don't want this when reading; so ensure the track  */
	/* does actually exist. */
	if (cyl  >= cpc_self->cpc_dskhead[0x30]) return DSK_ERR_NOADDR;
	if (head >= cpc_self->cpc_dskhead[0x31]) return DSK_ERR_NOADDR;


	e = load_track_header(cpc_self, geom, cyl, head);
	if (e) return e;

	/* 1.1.11: If the track has no sectors, return DSK_ERR_NOADDR */
	if (cpc_self->cpc_trkhead[0x15] == 0)
		return DSK_ERR_NOADDR;
	
	/* Offset of the chosen sector header */
	++cpc_self->cpc_sector;
	offs = 0x18 + 8 * (cpc_self->cpc_sector % cpc_self->cpc_trkhead[0x15]);

	result->fmt_cylinder = cpc_self->cpc_trkhead[offs];
	result->fmt_head	 = cpc_self->cpc_trkhead[offs+1];
	result->fmt_sector   = cpc_self->cpc_trkhead[offs+2];
	result->fmt_secsize  = 128 << cpc_self->cpc_trkhead[offs+3];
	memset(cpc_self->cpc_status, 0, sizeof(cpc_self->cpc_status));
	return DSK_ERR_OK;  
}


/* Find the offset of a sector in the current track 
 * Enter with cpc_trkhead loaded and the file pointer 
 * just after it (ie, you have just called load_track_header() ) 
 *
 * Returns secid  = address of 8-byte sector info area in track header
 *	   seclen = actual length of sector data; may be a multiple of 
 *		   the sector size for weak sectors
 */ 

static long sector_offset(CPCEMU_DSK_DRIVER *self, dsk_psect_t sector, 
		size_t *seclen, unsigned char **secid)
{
	int maxsec = self->cpc_trkhead[0x15];
	long offset = 0;
	int n;

	/* Pointer to sector details */
	*secid = self->cpc_trkhead + 0x18;

	/* Length of sector */  
	*seclen = (0x80 << self->cpc_trkhead[0x14]);

	/* Extended DSKs have individual sector sizes */
	if (!memcmp(self->cpc_dskhead, "EXTENDED", 8))
	{
/* v1.1.11: Start by looking at the current sector to see if it's the one
 * requested. */ 
		if (self->cpc_sector >= 0 && self->cpc_sector < maxsec)
		{
/* Calculate the offset of the current sector */
			for (n = 0; n < self->cpc_sector; n++)
			{
				*seclen = (*secid)[6] + 256 * (*secid)[7]; /* [v0.9.0] */
				offset   += (*seclen);
				(*secid) += 8;
			}
			if ((*secid)[2] == sector) return offset;
		}
/* The current sector is not the requested one -- search the track for 
 * a sector with the correct ID */
		offset = 0;
		*secid = self->cpc_trkhead + 0x18;
		for (n = 0; n < maxsec; n++)
		{
			*seclen = (*secid)[6] + 256 * (*secid)[7]; /* [v0.9.0] */
			if ((*secid)[2] == sector) return offset;
			offset   += (*seclen);
			(*secid) += 8;
		}
	}
	else	/* Non-extended, all sector sizes are the same */
	{
		if (self->cpc_sector >= 0 && self->cpc_sector < maxsec)
		{
/* v1.1.11: As above, check the current sector first */
			offset += (*seclen) * self->cpc_sector;
			(*secid) += 8 * self->cpc_sector;
			if ((*secid)[2] == sector) return offset;
		}
/* And if that fails search from the beginning */
		offset = 0;
		*secid = self->cpc_trkhead + 0x18;
		for (n = 0; n < maxsec; n++)
		{
			if ((*secid)[2] == sector) return offset;
			offset   += (*seclen);
			(*secid) += 8;
		}
	}
	return -1;  /* Sector not found */
}


static unsigned char *sector_head(CPCEMU_DSK_DRIVER *self, int sector)
{
	int ms = self->cpc_trkhead[0x15];
	int sec;

	for (sec = 0; sec < ms; sec++)
	{
		if (self->cpc_trkhead[0x1A + 8 * sec] == sector)
			return self->cpc_trkhead + 0x18 + 8 * sec;
	}
	return NULL;	
}


/* Seek within the DSK file to a given head & sector in the current cylinder. 
 *
 * On entry, *request_len is the expected sector size.
 * If the sector is shorter than this, *request_len will be reduced.
 *
 * weak_copies will be set to 1 in normal use; 2 or more if multiple copies
 * of the sector have been saved.
 *
 * sseclen will be set to the actual size of a sector in the file, so that
 * a random copy can be extracted.
 */ 
static dsk_err_t seekto_sector(CPCEMU_DSK_DRIVER *self, 
	const DSK_GEOMETRY *geom, int cylinder, int head, int cyl_expected,
	int head_expected, int sector, size_t *request_len, int *weak_copies,
	size_t *sseclen)
{
	int offs;
	size_t seclen;	/* Length of sector data in file */
	dsk_err_t err;
	unsigned char *secid;
	long trkbase;

	*weak_copies = 1;
	err = load_track_header(self, geom, cylinder, head);
	if (err) return err;
	trkbase = ftell(self->cpc_fp);
	offs = (int)sector_offset(self, sector, &seclen, &secid);
	
	if (offs < 0) return DSK_ERR_NOADDR;	/* Sector not found */

	if (cyl_expected != secid[0] || head_expected != secid[1])
	{
		/* We are not in the right place */
		return DSK_ERR_NOADDR;
	}
	*sseclen = 128 << (secid[3] & 7);
/* Sector shorter than expected. Report a data error, and set
 * request_len to the actual size. */
	if ((*sseclen) < (*request_len))
	{
		err = DSK_ERR_DATAERR;
		*request_len = *sseclen;
	}
/* Sector longer than expected. Report a data error but don't change 
 * request_len */
	else if ((*sseclen) > (*request_len))
	{
		err = DSK_ERR_DATAERR;
	}
/* If there is room for two or more copies, we have a weak-recording 
 * situation. */
	if ((*sseclen) * 2 <= seclen)
	{
		*weak_copies = seclen / (*sseclen);
	}

	fseek(self->cpc_fp, trkbase + offs, SEEK_SET);

	return err;
}


/* Read a sector */
dsk_err_t cpcemu_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		      void *buf, dsk_pcyl_t cylinder,
		      dsk_phead_t head, dsk_psect_t sector)
{
	return cpcemu_xread(self, geom, buf, cylinder, head, cylinder,
				dg_x_head(geom, head), 
				dg_x_sector(geom, head, sector), 
				geom->dg_secsize, 0);
}

dsk_err_t cpcemu_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf, 
		       dsk_pcyl_t cylinder,   dsk_phead_t head, 
		       dsk_pcyl_t cyl_expect, dsk_phead_t head_expect,
		       dsk_psect_t sector, size_t sector_size, int *deleted)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	dsk_err_t err;
	int weak_copies;
	size_t sseclen;
	size_t len = sector_size;   /* 1.1.2: Was geom->dg_secsize; but
				     * that fails when individual sectors
				     * are bigger than the size in geom. */
	int rdeleted = 0;
	int try_again = 0;
	unsigned char *sh;

	if (!buf || !geom || !self) return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	if (deleted && *deleted) rdeleted = 0x40;

	do
	{
		err  = seekto_sector(cpc_self, geom, cylinder,head, 
				cyl_expect, head_expect, sector, &len,
				&weak_copies, &sseclen);
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
				  cpc_self->cpc_dskhead[0x31] > 0)
			{
				head++;
				sector = geom->dg_secbase;  
				continue;   
			}
		}
		try_again = 0;
		/* v1.1.11: Sector not found, we go back to start of track */
		if (err == DSK_ERR_NOADDR)
			cpc_self->cpc_sector = -1;
		if (err != DSK_ERR_DATAERR && err != DSK_ERR_OK)
			return err;
		/* We have the sector. But does it contain deleted data? */
		sh = sector_head(cpc_self, sector);
		if (!sh) return DSK_ERR_NODATA;

		if (deleted) *deleted = 0;
		if (rdeleted != (sh[5] & 0x40)) /* Mismatch! */
		{
			if (geom->dg_noskip) 
			{
				if (deleted) *deleted = 1;
			}
			else
			{
/* Try the next sector. */
				try_again = 1;
				++sector;
				continue;
			}
		}
/* This next line should never be true, because len starts as sector_size and 
 * seekto_sector() only ever reduces it. */
		if (len > sector_size) len = sector_size;

/* If there are multiple copies of the sector present, pick one at random */
		if (weak_copies > 1)
		{
			long offset = (rand() % weak_copies) * sseclen;
			fseek(cpc_self->cpc_fp, offset, SEEK_CUR);
		}

		if (fread(buf, 1, len, cpc_self->cpc_fp) < len) 
			err = DSK_ERR_DATAERR;
/* Sector header ST2: If bit 5 set, data error 
 * Maybe need to emulate some other bits in a similar way */
		if (sh[5] & 0x20) err = DSK_ERR_DATAERR;
		memset(cpc_self->cpc_status, 0, sizeof(cpc_self->cpc_status));
		/* Set ST1 and ST2 from results of sector read */
		cpc_self->cpc_status[1] = sh[4];
		cpc_self->cpc_status[2] = sh[5];
	}
	while (try_again);
	
	return err;
}	   


/* Write a sector */
dsk_err_t cpcemu_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			const void *buf, dsk_pcyl_t cylinder,
			dsk_phead_t head, dsk_psect_t sector)
{
	return cpcemu_xwrite(self, geom, buf, cylinder, head, cylinder,
				dg_x_head(geom, head),
				dg_x_sector(geom, head, sector), 
				geom->dg_secsize, 0);
}

dsk_err_t cpcemu_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			  const void *buf, 
			  dsk_pcyl_t cylinder,   dsk_phead_t head, 
			  dsk_pcyl_t cyl_expect, dsk_phead_t head_expect,
			  dsk_psect_t sector, size_t sector_size,
			  int deleted)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	dsk_err_t err;
	size_t len = sector_size; /* geom->dg_secsize; */
	int n, weak_copies;
	size_t sseclen;

	if (!buf || !geom || !self) return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	if (cpc_self->cpc_readonly) return DSK_ERR_RDONLY;

	err  = seekto_sector(cpc_self, geom, cylinder,head,  
				cyl_expect, head_expect, sector, &len,
				&weak_copies, &sseclen);
	if (err == DSK_ERR_DATAERR || err == DSK_ERR_OK)
	{
		unsigned char osh4, osh5;
		unsigned char *sh = sector_head(cpc_self, sector);
		err = DSK_ERR_OK;   /* Ignore data error (disc sector bigger than expected) */
/* Limit length by (firstly) the amount of space in the file, and (secondly)
 * the size specified in the sector header */
		if (len > sector_size) len = sector_size;
		if (len > sseclen)     len = sseclen;
/* If the file had multiple copies of a sector, overwrite them all */
		for (n = 0; n < weak_copies; n++)
		{
			if (fwrite(buf, 1, len, cpc_self->cpc_fp) < len)
				err = DSK_ERR_DATAERR;
		}

/* If writing deleted data, update the sector header accordingly */

		osh4 = sh[4];
		osh5 = sh[5];
/* If ST1 and ST2 have been set explicitly, store their new values */
		if (cpc_self->cpc_statusw[1] >= 0) sh[4] = cpc_self->cpc_statusw[1];
		if (cpc_self->cpc_statusw[2] >= 0) sh[5] = cpc_self->cpc_statusw[2];
				if (deleted) sh[5] |= 0x40;
		else		 sh[5] &= ~0x40;

		if (sh[5] != osh5 || sh[4] != osh4)
		{
			long track = lookup_track(cpc_self, geom, cylinder, head);
			if (track < 0) return DSK_ERR_SEEKFAIL;	   /* Bad track */
			fseek(cpc_self->cpc_fp, track, SEEK_SET);
			if (fwrite(cpc_self->cpc_trkhead, 1, 256, cpc_self->cpc_fp) < 256)
				return DSK_ERR_DATAERR;  /* Write failed */
		}
	}
	for (n = 0; n < 4; n++)
	{
		cpc_self->cpc_statusw[n] = -1;
		cpc_self->cpc_status[n]  = 0;
	}
	return err;
}


/* Format a track on a DSK. Can grow the DSK file. */
dsk_err_t cpcemu_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
			dsk_pcyl_t cylinder, dsk_phead_t head,
			const DSK_FORMAT *format, unsigned char filler)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	int ext;
	long trkoff;	/* Make these longs for 16-bit systems */
	unsigned long img_trklen;
	unsigned char oldhead[256];
	unsigned char *blanksec;
	unsigned n, trkno, trklen, seclen;
	
	if (!format || !geom || !self) return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	if (!cpc_self->cpc_fp)	  return DSK_ERR_NOTRDY;
	if (cpc_self->cpc_readonly) return DSK_ERR_RDONLY;

	ext = 0;
	memcpy(oldhead, cpc_self->cpc_dskhead, 256);

/* 1. Only if the DSK has either (1 track & 1 head) or (2 heads) can we
 *   format the second head
 */
	if (head)
	{
		if (cpc_self->cpc_dskhead[0x31] == 1 && 
			cpc_self->cpc_dskhead[0x30] > 1) return DSK_ERR_RDONLY;

		if (cpc_self->cpc_dskhead[0x31] == 1) 
			cpc_self->cpc_dskhead[0x31] = 2;
	}
/* 2. Find out the CPCEMU number of the new cylinder/head */

	if (cpc_self->cpc_dskhead[0x31] < 1) cpc_self->cpc_dskhead[0x31] = 1;
	trkno = cylinder;
	trkno *= cpc_self->cpc_dskhead[0x31];
	trkno += head;

/* 3. Find out how long the proposed new track is
 *
 * nb: All sizes *include* track header
 */
	trklen = 0;
	for (n = 0; n < geom->dg_sectors; n++)
	{
		trklen += format[n].fmt_secsize;
	}
	trklen += 256;  /* For header */
/* 4. Work out if this length is suitable
 */
	if (!memcmp(cpc_self->cpc_dskhead, "EXTENDED", 8))
	{
		unsigned char *b;
		/* For an extended DSK, work as follows: 
		 * If the track is reformatting an existing one, 
		 * the length must be <= what's there. 
		 * If the track is new, it must be contiguous with the 
		 * others */

		ext = 1;
		img_trklen = (cpc_self->cpc_dskhead[0x34 + trkno] * 256);
		if (img_trklen)
		{
			if (trklen > img_trklen) return DSK_ERR_RDONLY;
		}
		else if (trkno > 0) 
		{
			if (!cpc_self->cpc_dskhead[0x34 + trkno - 1]) 
			{
				memcpy(cpc_self->cpc_dskhead, oldhead, 256);
				return DSK_ERR_RDONLY;
			}
		}
		/* Work out where the track should be. */
		b = cpc_self->cpc_dskhead + 0x34;
		trkoff = 256; 
		for (n = 0; n < trkno; n++)
		{
			trkoff += 256 * b[n];
		}
		/* Store the length of the new track (rounding up)*/
		b[n] = (unsigned char)((trklen + 255) >> 8);
	}
	else
	{
		img_trklen = cpc_self->cpc_dskhead[0x32] + 256 *
			 cpc_self->cpc_dskhead[0x33];
		/* If no tracks formatted, or just the one track, length can
		 * be what we like */
		if ( (cpc_self->cpc_dskhead[0x30] == 0) ||
			 (cpc_self->cpc_dskhead[0x30] == 1 && 
			  cpc_self->cpc_dskhead[0x31] == 1) )
		{
			if (trklen > img_trklen)
			{
				cpc_self->cpc_dskhead[0x32] = (unsigned char)(trklen & 0xFF);
				cpc_self->cpc_dskhead[0x33] = (unsigned char)(trklen >> 8);
				img_trklen = trklen;	
			}
		}
		if (trklen > img_trklen)
		{
			memcpy(cpc_self->cpc_dskhead, oldhead, 256);
			return DSK_ERR_RDONLY;
		}
		trkoff = 256 + (img_trklen * trkno);
	}
/* Seek to the track. */
	fseek(cpc_self->cpc_fp, trkoff, SEEK_SET);
	/* Now generate and write a Track-Info buffer */
	memset(cpc_self->cpc_trkhead, 0, sizeof(cpc_self->cpc_trkhead));

	strcpy((char *)cpc_self->cpc_trkhead, "Track-Info\r\n");

	cpc_self->cpc_trkhead[0x10] = (unsigned char)cylinder;
	cpc_self->cpc_trkhead[0x11] = (unsigned char)head;
	switch (geom->dg_datarate)
	{
		case RATE_SD: cpc_self->cpc_trkhead[0x12] = 1; break;
		case RATE_DD: cpc_self->cpc_trkhead[0x12] = 1; break;
		case RATE_HD: cpc_self->cpc_trkhead[0x12] = 2; break;
		case RATE_ED: cpc_self->cpc_trkhead[0x12] = 3; break;
	}
	switch (geom->dg_fm & RECMODE_MASK)
	{
		case RECMODE_FM:  cpc_self->cpc_trkhead[0x13] = 1; break;
		case RECMODE_MFM: cpc_self->cpc_trkhead[0x13] = 2; break;
	}
	cpc_self->cpc_trkhead[0x14] = dsk_get_psh(format[0].fmt_secsize);
	cpc_self->cpc_trkhead[0x15] = (unsigned char)geom->dg_sectors;
	cpc_self->cpc_trkhead[0x16] = geom->dg_fmtgap;
	cpc_self->cpc_trkhead[0x17] = filler;
	for (n = 0; n < geom->dg_sectors; n++)
	{
/* The DSK format is limited to 29 sectors / track. More than that would result
 * in a buffer overflow. */
		if ((0x1F + 8 *n) >= sizeof(cpc_self->cpc_trkhead))
		{   
#ifndef HAVE_WINDOWS_H
			fprintf(stderr, "Overflow: DSK format cannot handle %d sectors / track", n);
#endif
			return DSK_ERR_OVERRUN;
		}
		cpc_self->cpc_trkhead[0x18 + 8*n] = (unsigned char)format[n].fmt_cylinder;
		cpc_self->cpc_trkhead[0x19 + 8*n] = (unsigned char)format[n].fmt_head;
		cpc_self->cpc_trkhead[0x1A + 8*n] = (unsigned char)format[n].fmt_sector;
		cpc_self->cpc_trkhead[0x1B + 8*n] = dsk_get_psh(format[n].fmt_secsize);
		if (ext)
		{
			seclen = format[n].fmt_secsize;
			cpc_self->cpc_trkhead[0x1E + 8 * n] = (unsigned char)(seclen & 0xFF);
			cpc_self->cpc_trkhead[0x1F + 8 * n] = (unsigned char)(seclen >> 8);
		}
	}
	if (fwrite(cpc_self->cpc_trkhead, 1, 256, cpc_self->cpc_fp) < 256)
	{
		memcpy(cpc_self->cpc_dskhead, oldhead, 256);
		return DSK_ERR_RDONLY;
	}
	/* Track header written. Write sectors */
	for (n = 0; n < geom->dg_sectors; n++)
	{
		seclen = format[n].fmt_secsize;
		blanksec = dsk_malloc(seclen);
		if (!blanksec)
		{
			memcpy(cpc_self->cpc_dskhead, oldhead, 256);
			return DSK_ERR_NOMEM;
		}
		memset(blanksec, filler, seclen);
		if (fwrite(blanksec, 1, seclen, cpc_self->cpc_fp) < seclen)
		{
			memcpy(cpc_self->cpc_dskhead, oldhead, 256);
			return DSK_ERR_SYSERR;
		}
		dsk_free(blanksec);
	}
	if (cylinder >= cpc_self->cpc_dskhead[0x30])
	{
		cpc_self->cpc_dskhead[0x30] = (unsigned char)(cylinder + 1);
	}
	/* Track formatted OK. Now write back the modified DSK header */
	fseek(cpc_self->cpc_fp, 0, SEEK_SET);
	if (fwrite(cpc_self->cpc_dskhead, 1, 256, cpc_self->cpc_fp) < 256)
	{
		memcpy(cpc_self->cpc_dskhead, oldhead, 256);
		return DSK_ERR_RDONLY;
	}
	/* If the disc image has grown because of this, record this in the
	 * disc geometry struct */

	if (geom->dg_heads	 < cpc_self->cpc_dskhead[0x31])
		geom->dg_heads	 = cpc_self->cpc_dskhead[0x31];
	if (geom->dg_cylinders < cpc_self->cpc_dskhead[0x30])
		geom->dg_cylinders = cpc_self->cpc_dskhead[0x30];
		
	return DSK_ERR_OK;
}



/* Seek to a cylinder. */
dsk_err_t cpcemu_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
					dsk_pcyl_t cyl, dsk_phead_t head)
{
	CPCEMU_DSK_DRIVER *cpc_self;

	if (!self || !geom) return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;
	
	if (!cpc_self->cpc_fp) return DSK_ERR_NOTRDY;

	/* See if the cylinder & head are in range */
	if (cyl  > cpc_self->cpc_dskhead[0x30] ||
	    head > cpc_self->cpc_dskhead[0x31]) return DSK_ERR_SEEKFAIL;
	return DSK_ERR_OK;
}

/* 1.1.11: This function shouldn't be necessary, because DSK's emulation of
 * a real diskette ought to be good enough that the generic one works.
 * I've left it in, but not compiling. */
#if 0
dsk_err_t cpcemu_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			  dsk_pcyl_t cyl, dsk_phead_t head,
			  dsk_psect_t *count, DSK_FORMAT **result)
{
	CPCEMU_DSK_DRIVER *cpc_self;
	dsk_err_t e;
	unsigned char *secid;
	int seclen;
	int maxsec;
	int n;

	DSK_FORMAT headers[256];

	if (!self || !geom || !result)
		return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;
	
	if (!cpc_self->cpc_fp) return DSK_ERR_NOTRDY;

	e = load_track_header(cpc_self, geom, cyl, head);
	if (e) return e;

	/* lookup_track() allows us to seek to a nonexistent track.  */
	/* But we don't want this when reading; so ensure the track  */
	/* does actually exist.									  */
	if (cyl  >= cpc_self->cpc_dskhead[0x30]) return DSK_ERR_NOADDR;
	if (head >= cpc_self->cpc_dskhead[0x31]) return DSK_ERR_NOADDR;

	//
	maxsec = cpc_self->cpc_trkhead[0x15];

	/* Pointer to sector details */
	secid = cpc_self->cpc_trkhead + 0x18;

	/* Length of sector */  
	seclen = (0x80 << cpc_self->cpc_trkhead[0x14]);

	/* Extended DSKs have individual sector sizes */
	if (!memcmp(cpc_self->cpc_dskhead, "EXTENDED", 8))
	{
		for (n = 0; n < maxsec; n++)
		{
			headers[n].fmt_cylinder = cyl;
			headers[n].fmt_head = head;
			headers[n].fmt_secsize = secid[6] + 256 * secid[7];
			headers[n].fmt_sector = secid[2];
			secid += 8;
		}
	}
	else	/* Non-extended, all sector sizes are the same */
	{
		for (n = 0; n < maxsec; n++)
		{
			headers[n].fmt_cylinder = cyl;
			headers[n].fmt_head = head;
			headers[n].fmt_secsize = seclen;
			headers[n].fmt_sector = secid[2];
			secid += 8;
		}
	}

	*count = maxsec;
	*result = dsk_malloc( maxsec * sizeof(DSK_FORMAT) );
	if (!(*result)) return DSK_ERR_NOMEM;
	memcpy(*result, headers, maxsec * sizeof(DSK_FORMAT)) ;
	return DSK_ERR_OK;
}
#endif

dsk_err_t cpcemu_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
					  dsk_phead_t head, unsigned char *result)
{
	CPCEMU_DSK_DRIVER *cpc_self;

	if (!self || !geom) return DSK_ERR_BADPTR;
	DC_CHECK(self)
	cpc_self = (CPCEMU_DSK_DRIVER *)self;

	if (!cpc_self->cpc_fp) *result &= ~DSK_ST3_READY;
	if (cpc_self->cpc_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}

dsk_err_t cpcemu_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	if (!self) return DSK_ERR_BADPTR;
	DC_CHECK(self);

	switch(idx)
	{
		case 0: if (optname) *optname = "ST0"; return DSK_ERR_OK;
		case 1: if (optname) *optname = "ST1"; return DSK_ERR_OK;
		case 2: if (optname) *optname = "ST2"; return DSK_ERR_OK;
		case 3: if (optname) *optname = "ST3"; return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;
}

dsk_err_t cpcemu_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	CPCEMU_DSK_DRIVER *cpcself;

	if (!self || !optname) return DSK_ERR_BADPTR;
	DC_CHECK(self);
	cpcself = (CPCEMU_DSK_DRIVER *)self;

	if (!strcmp(optname, "ST0"))
	{
		cpcself->cpc_statusw[0] = value;
	}
	else if (!strcmp(optname, "ST1"))
	{
		cpcself->cpc_statusw[1] = value;
	}
	else if (!strcmp(optname, "ST2"))
	{
		cpcself->cpc_statusw[2] = value;
	}
	else if (!strcmp(optname, "ST3"))
	{
		cpcself->cpc_statusw[3] = value;
	}
	else return DSK_ERR_BADOPT;
	return DSK_ERR_OK;
}
dsk_err_t cpcemu_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	CPCEMU_DSK_DRIVER *cpcself;

	if (!self || !optname) return DSK_ERR_BADPTR;
	DC_CHECK(self);
	cpcself = (CPCEMU_DSK_DRIVER *)self;

	if (!strcmp(optname, "ST0"))
	{
		if (value) *value = cpcself->cpc_status[0];
	}
	else if (!strcmp(optname, "ST1"))
	{
		if (value) *value = cpcself->cpc_status[1];
	}
	else if (!strcmp(optname, "ST2"))
	{
		if (value) *value = cpcself->cpc_status[2];
	}
	else if (!strcmp(optname, "ST3"))
	{
		if (value) *value = cpcself->cpc_status[3];
	}
	else return DSK_ERR_BADOPT;
	return DSK_ERR_OK;
}

