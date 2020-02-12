/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2, 2016  John Elliott <seasip.webmaster@gmail.com>* 
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

/* This driver works with the ImageDisk "IMD" format. This compresses 
 * individual sectors using RLE, so we have to load the whole image into 
 * memory and work on it as an array of sectors.
 */

#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvimd.h"
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#define ST_NODATA    0	/* ID but no data */
#define ST_NORMAL    1	/* Normal data, uncompressed */
#define ST_CNORMAL   2	/* Normal data, compressed */
#define ST_DELETED   3	/* Deleted data, uncompressed */
#define ST_CDELETED  4	/* Deleted data, compressed */
#define ST_DATAERR   5	/* Normal data, uncompressed, data error on read */
#define ST_CDATAERR  6	/* Normal data, compressed, data error on read */
#define ST_DELERR    7	/* Deleted data, uncompressed, data error on read */
#define ST_CDELERR   8	/* Deleted data, compressed, data error on read */

/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_imd = 
{
	sizeof(IMD_DSK_DRIVER),
	"imd",
	"IMD file driver",
	imd_open,	/* open */
	imd_creat,	/* create new */
	imd_close,	/* close */
	imd_read,	/* read sector, working from physical address */
	imd_write,	/* write sector, working from physical address */
	imd_format,	/* format track, physical */
	imd_getgeom,	/* get geometry */
	imd_secid,	/* sector ID */
	imd_xseek,	/* seek to track */
	imd_status,	/* drive status */
	imd_xread,	/* Extended sector read */
	imd_xwrite,	/* Extended sector write */
};


static IMD_TRACK *imd_alloc_track(int sectors)
{
	int n;

	IMD_TRACK *t = dsk_malloc(sizeof(IMD_TRACK) + 
				sectors * sizeof(IMD_SECTOR *));

	if (!t) return NULL;
	for (n = 0; n < sectors; n++)
	{
		t->imdt_sec[n] = NULL;
	}
	return t;
}

static void imd_free_track(IMD_TRACK *t)
{
	int n;

	if (!t) return;

	for (n = 0; n < t->imdt_sectors; n++)
	{
		if (t->imdt_sec[n]) dsk_free(t->imdt_sec[n]);
	}
	dsk_free(t);
}

/* For reading strings from the file: Read up to the next occurrence of 
 * either of the specified characters c1 / c2, and return how many bytes 
 * that is (including the terminator).
 *
 * If only interested in one terminating character, pass c2 = c1
 */
static dsk_err_t imd_readto(FILE *fp, char c1, char c2, int *count, int *termch)
{
	int ch;
	long pos = ftell(fp);
	int cnt = 0;

	*termch = EOF;	
	if (pos < 0)
	{
		return DSK_ERR_SYSERR;
	}
	while (1)
	{
		++cnt;
		ch = fgetc(fp);
		if (ch == EOF || ch == c1 || ch == c2) 
		{
			*termch = ch;
			break;
		}	
	}
	if (fseek(fp, pos, SEEK_SET))
	{
		return DSK_ERR_SYSERR;
	}
	*count = cnt;
	return DSK_ERR_OK;
}

/* Ensure there are always at least 'count' + 1 tracks in the 
 * self->imd_tracks array */
static dsk_err_t imd_ensure_trackcount(IMD_DSK_DRIVER *self, dsk_ltrack_t trk)
{
	IMD_TRACK **ptr;
	unsigned n, newc;

	if (trk < self->imd_ntracks) return DSK_ERR_OK;

	/* Need to malloc some more */
	if (self->imd_ntracks == 0)
	{
		newc = 20;
	}
	else
	{
		newc = 2 * self->imd_ntracks;
	}
	ptr = dsk_malloc(newc * sizeof(IMD_TRACK *));
	if (!ptr) return DSK_ERR_NOMEM;

	/* Copy existing array (if any) */
	for (n = 0; n < self->imd_ntracks; n++)
		ptr[n] = self->imd_tracks[n];

	/* Blank additional pointers */
	for (n = self->imd_ntracks; n < newc; n++)
		ptr[n] = NULL;
	
	dsk_free(self->imd_tracks);
	self->imd_tracks = ptr;
	self->imd_ntracks = newc;
	return DSK_ERR_OK;
}



static dsk_err_t imd_load_track(IMD_DSK_DRIVER *self, dsk_ltrack_t count, 
	FILE *fp)
{
	/* Start by loading the track header: Fixed */
	IMD_TRACK tmp, *trkh;
	IMD_SECTOR *tmpsec;
	dsk_err_t err;
	int n, c;

/*	printf("Loading track %ld, offset %ld \n", count, ftell(fp)); */
	if (fread(&tmp.imdt_mode, 1, 4, fp) < 4)
	{
		return DSK_ERR_OVERRUN;	/* EOF */
	}
	c = fgetc(fp);
	if (c == EOF)
	{
		return DSK_ERR_OVERRUN;	/* EOF */
	}
	if (c == 0xFF)	tmp.imdt_seclen = 0xFFFF;
	else		tmp.imdt_seclen = (128 << c);	
/*	printf("Mode %d Cyl %d Head %d Secs %d Size %d\n",
		tmp.imdt_mode, tmp.imdt_cylinder, tmp.imdt_head,
		tmp.imdt_sectors, tmp.imdt_seclen); */

	err = imd_ensure_trackcount(self, count);
	if (err) return err;

	/* Allocate the full variable-size structure */
	trkh = imd_alloc_track(tmp.imdt_sectors);
	if (!trkh)
	{
		return DSK_ERR_NOMEM;
	}
	/* Copy fixed parts */
	memcpy(trkh, &tmp, sizeof(tmp));
	/* [1.4.2] But not the sector buffer (so we don't double-free) */
	trkh->imdt_sec[0] = NULL;

	/* Create a temporary array of sector headers */
	tmpsec = dsk_malloc(tmp.imdt_sectors * sizeof(IMD_SECTOR));
	if (!tmpsec)
	{
		imd_free_track(trkh);
		return DSK_ERR_NOMEM;
	}
	/* Populate temporary sector headers */
	for (n = 0; n < tmp.imdt_sectors; n++)
	{
		tmpsec[n].imds_cylinder = tmp.imdt_cylinder;
		tmpsec[n].imds_head     = tmp.imdt_head & 0x3F;
		tmpsec[n].imds_sector   = 0;
		tmpsec[n].imds_status   = ST_NODATA;
		tmpsec[n].imds_datalen  = 0;
		tmpsec[n].imds_seclen   = tmp.imdt_seclen;
	}
	/* Load sector IDs */	
	for (n = 0; n < tmp.imdt_sectors; n++)
	{
		c = fgetc(fp); 	
		if (c == EOF) 
		{
			dsk_free(tmpsec);
			imd_free_track(trkh);
			return DSK_ERR_SYSERR;
		}
	
		tmpsec[n].imds_sector = c;	
	}
	/* Load sector cylinder map (if present) */
	if (tmp.imdt_head & 0x80)
	{
		for (n = 0; n < tmp.imdt_sectors; n++)
		{
			c = fgetc(fp); 	
			if (c == EOF) 
			{
				dsk_free(tmpsec);
				imd_free_track(trkh);
				return DSK_ERR_SYSERR;
			}
		
			tmpsec[n].imds_cylinder = c;	
		}
	}
	/* Load sector head map (if present) */
	if (tmp.imdt_head & 0x40)
	{
		for (n = 0; n < tmp.imdt_sectors; n++)
		{
			c = fgetc(fp); 	
			if (c == EOF) 
			{
				dsk_free(tmpsec);
				imd_free_track(trkh);
				return DSK_ERR_SYSERR;
			}
		
			tmpsec[n].imds_head = c;	
		}
	}
	/* Load sector lengths (if present) */
	if (tmp.imdt_seclen == 0xFFFF)
	{
		int l, h;

		for (n = 0; n < tmp.imdt_sectors; n++)
		{
			l = fgetc(fp); 	
			h = fgetc(fp); 	
			if (l == EOF || h == EOF) 
			{
				dsk_free(tmpsec);
				imd_free_track(trkh);
				return DSK_ERR_SYSERR;
			}
			tmpsec[n].imds_seclen = (l & 0xFF) | ((h << 8) & 0xFF00);	
		}
	}


	for (n = 0; n < tmp.imdt_sectors; n++)
	{
		/* Get status for sector */
		c = fgetc(fp); 	
		if (c == EOF) 
		{
			dsk_free(tmpsec);
			imd_free_track(trkh);
			return DSK_ERR_SYSERR;
		}
		tmpsec[n].imds_status = c;
		switch(tmpsec[n].imds_status)
		{
			default: 
#ifndef WIN16
				fprintf(stderr, "Unsupported IMD status "
					"0x%02x\n", tmpsec[n].imds_status);  
#endif
				dsk_free(tmpsec);
				imd_free_track(trkh);
				return DSK_ERR_NOTME;
			/* ID, no data */
			case ST_NODATA: tmpsec[n].imds_datalen = 0; break;	
			/* Uncompressed */
			case ST_NORMAL: 		
			case ST_DELETED:
			case ST_DATAERR:
			case ST_DELERR: tmpsec[n].imds_datalen = 
						tmpsec[n].imds_seclen; break;
			/* Compressed */
			case ST_CNORMAL:  
			case ST_CDELETED:
			case ST_CDATAERR:
			case ST_CDELERR: tmpsec[n].imds_datalen = 1; break;
		}	
		trkh->imdt_sec[n] = dsk_malloc(sizeof(IMD_SECTOR) + 
						tmpsec[n].imds_datalen);
		if (!trkh->imdt_sec[n])
		{
			dsk_free(tmpsec);
			imd_free_track(trkh);
			return DSK_ERR_NOMEM;
		}
		memcpy(trkh->imdt_sec[n], &tmpsec[n], sizeof(IMD_SECTOR));

		if (tmpsec[n].imds_datalen)
		{
			if (fread(trkh->imdt_sec[n]->imds_data, 1, 
				tmpsec[n].imds_datalen, fp) < 
					tmpsec[n].imds_datalen)
			{
				dsk_free(tmpsec);
				imd_free_track(trkh);
				return DSK_ERR_SYSERR;
			}
		}
	}
	self->imd_tracks[count] = trkh;
	dsk_free(tmpsec);
//	for (n = 0; n < tmp.imdt_sectors; n++)
//	{
//		printf("c=%d h=%d s=%d len=%d status=%d datalen=%d\n",
//			tmpsec[n].imds_cylinder, tmpsec[n].imds_head,
//			tmpsec[n].imds_sector, tmp.imdt_secsize,
//			tmpsec[n].imds_status, tmpsec[n].imds_len);
//	}
	return DSK_ERR_OK;
}



dsk_err_t imd_open(DSK_DRIVER *self, const char *filename)
{
	FILE *fp;
	IMD_DSK_DRIVER *imdself;
	dsk_err_t err;	
	int ccmt, n;
	dsk_ltrack_t count = 0;
	char *comment;
	int termch;

	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	fp = fopen(filename, "r+b");
	if (!fp) 
	{
		imdself->imd_readonly = 1;
		fp = fopen(filename, "rb");
	}
	if (!fp) return DSK_ERR_NOTME;

	/* Try to check the magic number. Read the first line, which
 	 * may terminate with '\n' if a comment follows, or 0x1A 
	 * otherwise. */
	err = imd_readto(fp, '\n', 0x1A, &ccmt, &termch);
	if (err)
	{
		fclose(fp);
		return DSK_ERR_NOTME;
	}
	comment = dsk_malloc(1 + ccmt);
	if (!comment)
	{
		fclose(fp);
		return DSK_ERR_NOTME;
	}
	if ((int)fread(comment, 1, ccmt, fp) < ccmt)
	{
		fclose(fp);
		return DSK_ERR_SYSERR;
	}
	comment[ccmt] = 0;

/*	printf("Header='%s' pos=%ld\n", comment, ftell(fp)); */

	/* IMD signature is 4 bytes magic, then the rest of the line is 
	 * freeform (but probably includes a date stamp) */
	if (memcmp(comment, "IMD ", 4))
	{
		dsk_free(comment);
		fclose(fp);
		return DSK_ERR_NOTME;
	}
	dsk_free(comment);

	/* If the header wasn't terminated by 0x1A, then a comment
	 * follows. */
	if (termch != 0x1A)
	{
		err = imd_readto(fp, 0x1A, 0x1A, &ccmt, &termch);
		if (err)
		{
			fclose(fp);
			return DSK_ERR_NOTME;
		}
		comment = dsk_malloc(1 + ccmt);
		if (!comment)
		{
			fclose(fp);
			return DSK_ERR_NOMEM;
		}
		if ((int)fread(comment, 1, ccmt, fp) < ccmt)
		{
			fclose(fp);
			return DSK_ERR_SYSERR;
		}	
		comment[ccmt - 1] = 0;
		dsk_set_comment(self, comment);
		dsk_free(comment);
	}
	imdself->imd_dirty = 0;
	imdself->imd_sec = 0;
	/* Keep a copy of the filename; when writing back, we will need it */
	imdself->imd_filename = dsk_malloc(1 + strlen(filename));
	if (!imdself->imd_filename) return DSK_ERR_NOMEM;
	strcpy(imdself->imd_filename, filename);	

	/* And now we're onto the tracks */
	ccmt = 0;
	dsk_report("Loading IMD file into memory");

	err = DSK_ERR_OK;

	while (!feof(fp))
	{
		err = imd_load_track(imdself, count, fp);
		if (err == DSK_ERR_OVERRUN) 	/* EOF */
		{
			dsk_report_end();
			return DSK_ERR_OK;
		}
		else if (err) 
		{
			dsk_free(imdself->imd_filename);
			for (n = 0; n < (int)(imdself->imd_ntracks); n++)
			{
				imd_free_track(imdself->imd_tracks[n]);
			}
			dsk_report_end();
			return err;
		}
		++count;
	} 
	/* Should never get here! */
	dsk_report_end();
	return DSK_ERR_OK;
}


dsk_err_t imd_creat(DSK_DRIVER *self, const char *filename)
{
	IMD_DSK_DRIVER *imdself;
	FILE *fp;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	/* See if the file can be created. But don't hold it open. */
	fp = fopen(filename, "wb");
	imdself->imd_readonly = 0;
	if (!fp) return DSK_ERR_SYSERR;
	fclose(fp);
	imdself->imd_dirty = 1;

	/* Keep a copy of the filename, for writing back */
	imdself->imd_filename = dsk_malloc(1 + strlen(filename));
	if (!imdself->imd_filename) return DSK_ERR_NOMEM;
	strcpy(imdself->imd_filename, filename);	
	
	imdself->imd_ntracks = 0;
	imdself->imd_tracks = NULL;

	return DSK_ERR_OK;
}

static int compare_tracks(const void *a, const void *b)
{
	const IMD_TRACK *ta = *(const IMD_TRACK **)a;
	const IMD_TRACK *tb = *(const IMD_TRACK **)b;

	/* Sort by cylinder and head, with nulls at the end */
	if (ta == NULL && tb == NULL) return 0;
	if (ta == NULL) return 1;
	if (tb == NULL) return -1;

	if (ta->imdt_cylinder != tb->imdt_cylinder)
		return ta->imdt_cylinder - tb->imdt_cylinder;
	return (ta->imdt_head & 0x3F) - (tb->imdt_head & 0x3F);
}

static dsk_err_t imd_save_track(IMD_DSK_DRIVER *self, IMD_TRACK *trk, FILE *fp)
{
	int nsec;
	unsigned char secsize = dsk_get_psh(trk->imdt_seclen);
	IMD_SECTOR *sec;

	/* See if we need to write cylinder / head maps */
	trk->imdt_head &= 0x3F;

	for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
	{
		sec = trk->imdt_sec[nsec];
		if (sec->imds_cylinder != trk->imdt_cylinder)
			trk->imdt_head |= 0x80;
		if (sec->imds_head != (trk->imdt_head & 0x3F))
			trk->imdt_head |= 0x40;
		if (sec->imds_seclen != trk->imdt_seclen)
			secsize = 0xFF;
	}

	
	/* Write fixed part of header */
	if (fwrite(&trk->imdt_mode, 1, 4, fp) < 4
	||  fputc(secsize, fp) == EOF)
	{
		return DSK_ERR_SYSERR;
	}
		
	/* Write sector IDs */	
	for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
	{
		sec = trk->imdt_sec[nsec];
		if (fputc(sec->imds_sector, fp) == EOF)
			return DSK_ERR_SYSERR;
	}
	/* Write cylinder IDs, if necessary */
	if (trk->imdt_head & 0x80)
	{
		for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
		{
			sec = trk->imdt_sec[nsec];
			if (fputc(sec->imds_cylinder, fp) == EOF)
				return DSK_ERR_SYSERR;
		}
	}
	/* Write head IDs, if necessary */
	if (trk->imdt_head & 0x40)
	{
		for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
		{
			sec = trk->imdt_sec[nsec];
			if (fputc(sec->imds_head, fp) == EOF)
				return DSK_ERR_SYSERR;
		}
	}
	/* Write sector sizes, if necessary */
	if (secsize == 0xFF)
	{
		for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
		{
			sec = trk->imdt_sec[nsec];
			if (fputc(sec->imds_seclen & 0xFF, fp) == EOF)
				return DSK_ERR_SYSERR;
			if (fputc(sec->imds_seclen >> 8, fp) == EOF)
				return DSK_ERR_SYSERR;
		}
	}

	/* Write sector data */
	for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
	{
		sec = trk->imdt_sec[nsec];
		if (fputc(sec->imds_status, fp) == EOF)
			return DSK_ERR_SYSERR;
		if (sec->imds_status)
		{
			if (fwrite(sec->imds_data, 1, sec->imds_datalen, fp) < 
					sec->imds_datalen)
				return DSK_ERR_SYSERR;
		}
	}
	return DSK_ERR_OK;
}



dsk_err_t imd_close(DSK_DRIVER *self)
{
	IMD_DSK_DRIVER *imdself;
	dsk_err_t err = DSK_ERR_OK;
	dsk_ltrack_t trk;
	char buf[128];
	FILE *fp;

	if (self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (imdself->imd_filename && (imdself->imd_dirty))
	{
/* When writing back to a IMD, create the file from scratch. We might as well
 * sort the tracks, too.*/

		qsort(imdself->imd_tracks, imdself->imd_ntracks,
			sizeof(IMD_TRACK *), compare_tracks);

		fp = fopen(imdself->imd_filename, "wb");
		if (!fp) err = DSK_ERR_SYSERR;
		else
		{
			time_t t;
			struct tm *ptm;
			char *comment;

			time (&t);
			ptm = localtime(&t);

			if (dsk_get_comment(self, &comment) != DSK_ERR_OK ||
				comment == NULL)
			{
				comment = "\r\n";
			}
			dsk_report("Writing IMD file");
/* The spec seems to imply the header must read "IMD 1.18: datestamp" but 
 * TD02IMD writes a completely different header... */
			sprintf(buf, "IMD LibDsk %s: %02d/%02d/%04d "
					"%02d:%02d:%02d\r\n",
				LIBDSK_VERSION,
				ptm->tm_mday, ptm->tm_mon + 1, 
				ptm->tm_year + 1900,
				ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			
			if (fwrite(buf, 1, strlen(buf), fp) < strlen(buf))
			{
				err = DSK_ERR_SYSERR;
			}
			/* Write comment */
			else if (fwrite(comment, 1, strlen(comment), fp) < strlen(comment))
			{
				err = DSK_ERR_SYSERR;
			}
			else if (fputc(0x1A, fp) == EOF)
			{
				err = DSK_ERR_SYSERR;
			}
			else for (trk = 0; trk < imdself->imd_ntracks; trk++)
			{
				if (imdself->imd_tracks[trk])
				{	
					err = imd_save_track(imdself, 
						imdself->imd_tracks[trk], fp);
				}
				if (err) break;
			}
			fclose(fp);
			dsk_report_end();
		}
	}
/* Free track buffers if we have them */
	if (imdself->imd_tracks)
	{
		unsigned int n;
		for (n = 0; n < imdself->imd_ntracks; n++)
		{
			imd_free_track(imdself->imd_tracks[n]);
		}
		dsk_free(imdself->imd_tracks);
		imdself->imd_tracks = NULL;
		imdself->imd_ntracks = 0;
	}
	if (imdself->imd_filename) 
	{
		dsk_free(imdself->imd_filename);
		imdself->imd_filename = NULL;
	}
	return err;
}

static int encode_mode(const DSK_GEOMETRY *geom)
{
	int fm = ((geom->dg_fm & RECMODE_MASK) == RECMODE_FM);
	switch (geom->dg_datarate)
	{
		case RATE_SD:	return fm ? 2 : 5;
		case RATE_DD:	return fm ? 1 : 4;
		case RATE_HD:	return fm ? 0 : 3;
		case RATE_ED:	/* Not supported by IMD format, so 
				 * we'll just have to improvise */	
				return fm ? 6 : 9;
	}
	return -1;
}

/* In our internal array, locate the track with the specified cylinder 
 * and head (if it's there) */
static dsk_err_t imd_seektrack(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head, int *result)
{
	IMD_DSK_DRIVER *imdself;
	int track;
	int mode = encode_mode(geom);

	if (!self || !geom || self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (!imdself->imd_filename) return DSK_ERR_NOTRDY;

	for (track = 0; track < (int)(imdself->imd_ntracks); track++)
	{
		if (imdself->imd_tracks[track] == NULL) continue;

		if ( imdself->imd_tracks[track]->imdt_cylinder == cylinder &&
		    (imdself->imd_tracks[track]->imdt_head & 0x3F) == (int)head &&
		     imdself->imd_tracks[track]->imdt_mode == mode)
		{
			if (result) *result = track;	
			return DSK_ERR_OK;
		}
	}
	return DSK_ERR_SEEKFAIL;
}

static int isdeleted(int status)
{
	return (status == ST_DELETED || status == ST_CDELETED || 
		status == ST_DELERR  || status == ST_CDELERR);
}

static int iscompressed(int status)
{
	return (status == ST_CNORMAL  || status == ST_CDELETED || 
		status == ST_CDATAERR || status == ST_CDELERR);
}


static dsk_err_t imd_find_sector(IMD_DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head, 
				dsk_psect_t sector, dsk_pcyl_t cyl_expected,
				dsk_phead_t head_expected, int *deleted,
				IMD_TRACK **restrack, IMD_SECTOR **result)
{
	int ntrack, nsec;
	IMD_TRACK *trk;
	IMD_SECTOR *sec;
	int want_deleted = 0;
	int have_deleted = 0;
	dsk_err_t err;

	*restrack = NULL;
	*result = NULL;
	if (deleted && *deleted) want_deleted = 1;

	err = imd_seektrack(&self->imd_super, geom, cylinder, head, &ntrack);
	if (err) return err;

	trk = self->imd_tracks[ntrack];
	for (nsec = 0; nsec < trk->imdt_sectors; nsec++)
	{
		sec = trk->imdt_sec[nsec];

		if (sec->imds_cylinder == cyl_expected &&
		    sec->imds_head     == head_expected &&
		    sec->imds_sector   == sector)	
		{
			if (isdeleted(sec->imds_status)) have_deleted = 1;

			if (geom->dg_noskip == 0 && 
				want_deleted != have_deleted) continue;

			if (deleted) *deleted = have_deleted;
			*restrack = trk;
			*result = sec;

			/* Translate status to libdsk error code */
			switch (sec->imds_status)
			{
				case ST_NODATA: return DSK_ERR_NODATA;	
						/* ID but no data */
				case ST_DATAERR:
				case ST_CDATAERR:
				case ST_DELERR:
				case ST_CDELERR: return DSK_ERR_DATAERR;
				default:return DSK_ERR_OK;
			}
		}
	}
	self->imd_sec = 0;
	return DSK_ERR_NOADDR;
}


dsk_err_t imd_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
        return imd_xread(self, geom, buf, cylinder, head, cylinder,
                dg_x_head(geom, head),
                dg_x_sector(geom, head, sector), geom->dg_secsize, NULL);
}


/* Expand a (possibly compressed) sector */
static void expand_sector(unsigned char *bbuf, size_t sector_size, 
				IMD_SECTOR *sec)
{
	unsigned n;

	if (iscompressed(sec->imds_status))
	{
		for (n = 0; n < sector_size; n++)
		{
			bbuf[n] = sec->imds_data[0];
		}		
	}
	else
	{
		for (n = 0; n < sector_size; n++)
		{
			if (n < sec->imds_datalen)
				bbuf[n] = sec->imds_data[n];
			else 	bbuf[n] = 0xE5;
		}		
	}
}

dsk_err_t imd_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
		void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, 
		dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		dsk_psect_t sector, size_t sector_size, int *deleted)
{
	IMD_DSK_DRIVER *imdself;
	IMD_SECTOR *sec = NULL;
	IMD_TRACK *trk = NULL;
	unsigned char *bbuf = (unsigned char *)buf;
	dsk_err_t err;

	if (!buf || !self || !geom || self->dr_class != &dc_imd) 
		return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (!imdself->imd_filename) return DSK_ERR_NOTRDY;

	err = imd_find_sector(imdself, geom, cylinder, head, sector, 
		cyl_expected, head_expected, deleted, &trk, &sec);
	
	if (sec && (err == DSK_ERR_OK || err == DSK_ERR_DATAERR))
	{
		expand_sector(bbuf, sector_size, sec);
	}
	return err;
}



dsk_err_t imd_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return imd_xwrite(self, geom, buf, cylinder, head,
			cylinder, dg_x_head(geom, head),
			dg_x_sector(geom, head, sector), 
			geom->dg_secsize, 0);
}

dsk_err_t imd_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
		const void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, 
		dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		dsk_psect_t sector, size_t sector_size, int deleted)
{
	IMD_DSK_DRIVER *imdself;
	IMD_SECTOR *sec = NULL, *newsec = NULL;
	IMD_TRACK *trk = NULL;
	dsk_err_t err;
	unsigned n;
	int newstatus;
	size_t secsize;
	unsigned char *curbuf;

	if (!buf || !self || !geom || self->dr_class != &dc_imd) 
		return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (!imdself->imd_filename) return DSK_ERR_NOTRDY;
	if (imdself->imd_readonly)  return DSK_ERR_RDONLY;

	err = imd_find_sector(imdself, geom, cylinder, head, sector, 
		cyl_expected, head_expected, &deleted, &trk, &sec);

	if (err != DSK_ERR_OK && err != DSK_ERR_DATAERR) return err;	

	secsize = sec->imds_seclen;
	curbuf = dsk_malloc(secsize);
	if (!curbuf) return DSK_ERR_NOMEM;

	expand_sector(curbuf, secsize, sec);

	if (sector_size > secsize)
		memcpy(curbuf, buf, secsize);
	else	memcpy(curbuf, buf, sector_size);

	/* Curbuf is the buffer to write back. */
	newstatus = deleted ? 4 : 2;	

	/* See if we can compress. */
	for (n = 1; n < secsize; n++)
	{
		if (curbuf[n] != curbuf[0])	/* Can't compress */
		{
			--newstatus;
			break;
		}
	}
	/* Fancy that! Writing back exactly the same data! */
	if (newstatus == sec->imds_status && 
	    !memcmp(curbuf, sec->imds_data, sec->imds_datalen))
	{
		dsk_free(curbuf);
		return DSK_ERR_OK;
	}
	/* OK. We need to do an actual write. */
	if (newstatus == 1 || newstatus == 3)	
	{
		newsec = dsk_malloc(sizeof(IMD_SECTOR) + secsize);
	}
	else
	{
		newsec = dsk_malloc(sizeof(IMD_SECTOR) + 1);
	}
	if (!newsec)
	{
		dsk_free(curbuf);
		return DSK_ERR_NOMEM;
	}
	newsec->imds_cylinder = sec->imds_cylinder;
	newsec->imds_head     = sec->imds_head;
	newsec->imds_sector   = sec->imds_sector;
	newsec->imds_seclen   = secsize;
	newsec->imds_status   = newstatus;
	newsec->imds_datalen  = (newstatus == 1 || newstatus == 3) ? secsize : 1;	
	memcpy(newsec->imds_data, curbuf, newsec->imds_datalen);
	/* Replace the old sector record with the new one we have 
	 * constructed */
	for (n = 0; n < trk->imdt_sectors; n++)
	{
		if (trk->imdt_sec[n] == sec) 
			trk->imdt_sec[n] = newsec;
	}
	dsk_free(curbuf);
	/* Free the old sector */
	dsk_free(sec);
	
	/* And mark the file as dirty */
	imdself->imd_dirty = 1;
	return DSK_ERR_OK;
}



dsk_err_t imd_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	IMD_DSK_DRIVER *imdself;
	dsk_err_t err;
	dsk_psect_t nlsec;
	IMD_TRACK *trk;
	IMD_SECTOR *sec;
	int ntrack;

	if (!self || !geom || self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (!imdself->imd_filename) return DSK_ERR_NOTRDY;
	if (imdself->imd_readonly) return DSK_ERR_RDONLY;

	/* Construct a new track */
	trk = imd_alloc_track(geom->dg_sectors);
	if (!trk) return DSK_ERR_NOMEM;

	trk->imdt_mode     = encode_mode(geom);
	trk->imdt_cylinder = cylinder;
	trk->imdt_head     = head;
	trk->imdt_sectors  = geom->dg_sectors;
	
	for (nlsec = 0; nlsec < geom->dg_sectors; nlsec++)
	{
		sec = dsk_malloc(1 + sizeof(IMD_SECTOR));
		if (!sec)
		{
			imd_free_track(trk);
			return DSK_ERR_NOMEM;
		}
		
		sec->imds_seclen  = format[nlsec].fmt_secsize;
		if (nlsec == 0)
		{
			trk->imdt_seclen = sec->imds_seclen;
		}
		else if(trk->imdt_seclen != sec->imds_seclen)
		{
			trk->imdt_seclen = 0xFFFF;
		}
		sec->imds_cylinder = format[nlsec].fmt_cylinder;
		sec->imds_head     = format[nlsec].fmt_head;
		sec->imds_sector   = format[nlsec].fmt_sector;
		sec->imds_status   = ST_CNORMAL;
		sec->imds_datalen  = 1;
		sec->imds_data[0]  = filler;
		trk->imdt_sec[nlsec] = sec;
	}	
	/* Track populated. Now add it to the image (if it's not there
	 * already) */
	err = imd_seektrack(self, geom, cylinder, head, &ntrack);

	if (!err)
	{
		imd_free_track(imdself->imd_tracks[ntrack]);
		imdself->imd_tracks[ntrack] = trk;	
	}
	else	/* New track, add it */
	{
		for (ntrack = 0; ntrack < (int)imdself->imd_ntracks; ntrack++)
		{
			/* Look for a blank slot */
			if (imdself->imd_tracks[ntrack] == NULL)
				break;
		}
		/* If no blank slot found, use imd_ensure_trackcount to add
		 * some more blank slots */
		err = imd_ensure_trackcount(imdself, ntrack);
		if (err) 
		{
			imd_free_track(trk);
			return err;
		}
		imdself->imd_tracks[ntrack] = trk;
	}
	/* And mark the file as dirty */
	imdself->imd_dirty = 1;

	return DSK_ERR_OK;
}
	


dsk_err_t imd_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	return imd_seektrack(self, geom, cylinder, head, NULL);
}

dsk_err_t imd_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	IMD_DSK_DRIVER *imdself;

	if (!self || !geom || self->dr_class != &dc_imd) return DSK_ERR_BADPTR;
	imdself = (IMD_DSK_DRIVER *)self;

	if (!imdself->imd_filename) *result &= ~DSK_ST3_READY;
	if (imdself->imd_readonly) *result |= DSK_ST3_RO;
	return DSK_ERR_OK;
}


/* Read a sector ID from a given track */
dsk_err_t imd_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head, DSK_FORMAT *result)
{
        IMD_DSK_DRIVER *imdself;
	IMD_TRACK *trk;
	IMD_SECTOR *sec;
	dsk_psect_t count; 
	int ntrack;
	dsk_err_t err;

        if (!self || !geom || !result || self->dr_class != &dc_imd)
                return DSK_ERR_BADPTR;
        imdself = (IMD_DSK_DRIVER *)self;

	err = imd_seektrack(self, geom, cylinder, head, &ntrack);
	if (err) return err;
	trk = imdself->imd_tracks[ntrack];

	count = imdself->imd_sec;
	++imdself->imd_sec;

	sec = trk->imdt_sec[count % trk->imdt_sectors];

	result->fmt_cylinder = sec->imds_cylinder;
	result->fmt_head     = sec->imds_head;
	result->fmt_sector   = sec->imds_sector;
	result->fmt_secsize  = sec->imds_seclen;
	return DSK_ERR_OK;
}


/* An IMD file contains enough information to take a decent stab at 
 * determining the drive geometry. So if the default libdsk probe 
 * fails, have a go ourselves. */

dsk_err_t imd_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	dsk_err_t err;
        IMD_DSK_DRIVER *imdself;
	IMD_TRACK *trk;
	IMD_SECTOR *sec;
	DSK_GEOMETRY testgeom;
	int track, sector, es;
	int minsec0, maxsec0, minsec1, maxsec1;
	int fm = 0;

        if (!self || !geom || self->dr_class != &dc_imd)
                return DSK_ERR_BADPTR;
        imdself = (IMD_DSK_DRIVER *)self;
	err = dsk_defgetgeom(self, geom);
	if (err == DSK_ERR_OK)
		return err;

	/* Initialise our test structure to known values */
	dg_stdformat(&testgeom, FMT_180K, NULL, NULL);
	testgeom.dg_cylinders = 0;
	testgeom.dg_sectors = 0;
	testgeom.dg_heads = 0;

	minsec0 = minsec1 = 256;
	maxsec0 = maxsec1 = 0;
	es = 0;

	for (track = 0; track < (int)imdself->imd_ntracks; track++)
	{
		trk = imdself->imd_tracks[track];
		if (trk == NULL) continue;

		if (trk->imdt_sectors > testgeom.dg_sectors)
			testgeom.dg_sectors = trk->imdt_sectors;
		if (trk->imdt_cylinder >= testgeom.dg_cylinders)
			testgeom.dg_cylinders = trk->imdt_cylinder + 1;
		if ((trk->imdt_head & 0x3F) >= (int)(testgeom.dg_heads))
			testgeom.dg_heads = (trk->imdt_head & 0x3F) + 1;

		switch (trk->imdt_mode)
		{
			case 0: fm = 1; testgeom.dg_datarate = RATE_HD; break;
			case 1: fm = 1; testgeom.dg_datarate = RATE_DD; break;
			case 2: fm = 1; testgeom.dg_datarate = RATE_SD; break;
			case 3: fm = 0; testgeom.dg_datarate = RATE_HD; break;
			case 4: fm = 0; testgeom.dg_datarate = RATE_DD; break;
			case 5: fm = 0; testgeom.dg_datarate = RATE_SD; break;
			case 6: fm = 1; testgeom.dg_datarate = RATE_ED; break;
			case 9: fm = 0; testgeom.dg_datarate = RATE_ED; break;
		}	
		testgeom.dg_fm = (fm ? RECMODE_FM : RECMODE_MFM);
		for (sector = 0; sector < trk->imdt_sectors; sector++)
		{
			sec = trk->imdt_sec[sector];
			if (sec == NULL) continue;
		
			testgeom.dg_secsize = sec->imds_seclen;
		
			if ((trk->imdt_head & 0x3F) == 1)
			{
				if (sec->imds_head == 0) es = 1;
				if (sec->imds_sector < minsec1) 
					minsec1 = sec->imds_sector;
				if (sec->imds_sector > maxsec1) 
					maxsec1 = sec->imds_sector;
			}	
			if ((trk->imdt_head & 0x3F) == 0)
			{
				if (sec->imds_sector < minsec0) 
					minsec0 = sec->imds_sector;
				if (sec->imds_sector > maxsec0) 
					maxsec0 = sec->imds_sector;
			}	
		}
	}
	testgeom.dg_secbase = minsec0;
	testgeom.dg_sectors = (maxsec0 - minsec0) + 1;

	/* Check for an 'extended surface' format: 2 heads, the 
	 * sector ranges on each side are disjoint, and sectors on head 1
	 * are labelled as head 0 */
	if (testgeom.dg_heads == 2 && (maxsec0 + 1 == minsec1) && es)
	{
		testgeom.dg_sidedness = SIDES_EXTSURFACE;
	}

	if (testgeom.dg_cylinders == 0 ||
	    testgeom.dg_sectors   == 0) return DSK_ERR_BADFMT;

	memcpy(geom, &testgeom, sizeof(*geom));
	return DSK_ERR_OK;
}

