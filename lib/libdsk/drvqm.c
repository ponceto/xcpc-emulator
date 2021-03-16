/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2,2005  John Elliott <seasip.webmaster@gmail.com>     *
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
/*
 *  This driver is for CopyQM images
 *  Written by Per Ola Ingvarsson.
 *  Thanks to Roger Plant for the expandqm program which has helped a lot.
 *  Currently it is read only
 */
/*
 *  Release 2011-04-17
 *  This is an rewritten enhanced QM driver with write-back capabilty.
 *  The image is loaded into memory in drv_open() and written
 *  back (if changed) in drv_close().
 *  Written by Ralf-Peter Nerlich <early8bitz@arcor.de>.
 *  From an idea by Matt Knoth <www.knothusa.net>.
 *  Please read the CHANGELOG.DRVQM for information.
 *  Warning! Code is highly experimental and not tested by any
 *  other person.
 *  !! Use at your own risk !!
 *  !! Don't use in production environment !!
 */
#include <stdio.h>
#include <string.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvqm.h"
#include "crctable.h"
#ifdef HAVE_TIME_H
#include <time.h>
#endif

/* #define DRV_QM_DEBUG */
#undef DRV_QM_DEBUG

#define MAKE_CHECK_SELF QM_DSK_DRIVER* qm_self;\
                        if ( self->dr_class != &dc_qm ) return DSK_ERR_BADPTR;\
                        qm_self = (QM_DSK_DRIVER*)self;

dsk_err_t drv_qm_open(DSK_DRIVER * self, const char *filename);
dsk_err_t drv_qm_create(DSK_DRIVER * self, const char *filename);
dsk_err_t drv_qm_close(DSK_DRIVER * self);
dsk_err_t drv_qm_read(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		      void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, dsk_psect_t sector);
dsk_err_t drv_qm_write(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		       const void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, dsk_psect_t sector);
dsk_err_t drv_qm_format(DSK_DRIVER * self, DSK_GEOMETRY * geom,
			dsk_pcyl_t cylinder, dsk_phead_t head, const DSK_FORMAT * format,
			unsigned char filler);
dsk_err_t drv_qm_xseek(DSK_DRIVER * self, const DSK_GEOMETRY * geom, dsk_pcyl_t cylinder,
		       dsk_phead_t head);
dsk_err_t drv_qm_status(DSK_DRIVER * self, const DSK_GEOMETRY * geom, dsk_phead_t head,
			unsigned char *result);
dsk_err_t drv_qm_getgeom(DSK_DRIVER * self, DSK_GEOMETRY * geom);

dsk_err_t drv_qm_secid(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		       dsk_pcyl_t cylinder, dsk_phead_t head, DSK_FORMAT * result);

/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass
 */
DRV_CLASS dc_qm = {
    sizeof(QM_DSK_DRIVER),
    "qm",
    "CopyQM file driver",
    drv_qm_open,					   /* open */
    drv_qm_create,					   /* create new */
    drv_qm_close,					   /* close */
    drv_qm_read,					   /* read sector */
    drv_qm_write,					   /* write sector */
    drv_qm_format,					   /* format track, physical */
    drv_qm_getgeom,					   /* get geometry */
    drv_qm_secid,					   /* Sector ID */
    drv_qm_xseek,					   /* seek to track */
    drv_qm_status,					   /* drive status */
};

/************************************************
 * misc. internal functions                     *
 ************************************************/
static int get_i16(unsigned char *buf, int pos)
{
    unsigned char low_byte;
    unsigned char high_byte;
    int outInt;

    low_byte = buf[pos];
    high_byte = buf[++pos];

    /* Signed, eh. Lets see. */
    outInt = 0;
    if(high_byte & 0x80)				   /* then negative */
	/* Set all to ones except for the lower 16 */
	/* Should work if sizeof(int) >= 16 */
	outInt = (-1) << 16;
    return (outInt |= (high_byte << 8) | low_byte);
}

static unsigned int get_u16(unsigned char *buf, int pos)
{
    return ((unsigned int) get_i16(buf, pos)) & 0xffff;
}

static void put_u16(unsigned char *buf, int pos, unsigned int val)
{
    buf[pos] = (unsigned char) val;
    buf[++pos] = (unsigned char) (val >> 8);
}

static unsigned long get_u32(unsigned char *buf, int pos)
{
    int i;
    unsigned long ret_val = 0;
    for(i = 3; i >= 0; --i)
    {
	ret_val <<= 8;
	ret_val |= ((unsigned long) buf[pos + i] & 0xff);
    }
    return ret_val;
}

static void drv_qm_update_crc(unsigned long *crc, unsigned char byte)
{
    /* Note that there is a bug in the CopyQM CRC calculation  */
    /* When indexing in this table, they shift the crc ^ data  */
    /* 2 bits up to address longwords, but they do that in an  */
    /* eight bit register, so that the top 2 bits are lost,    */
    /* thus the anding with 0x3f                               */
    *crc = crc32r_table[(byte ^ (unsigned char) *crc) & 0x3f] ^ (*crc >> 8);
}

static long drv_qm_calc_offset(const DSK_GEOMETRY * geom,
			       dsk_pcyl_t rq_cyl, dsk_phead_t rq_head, dsk_psect_t rq_sec)
{
    long offset;

    offset = (rq_cyl * geom->dg_heads) + rq_head;	   /* Drive track */
    offset *= geom->dg_sectors;
    offset += (rq_sec - geom->dg_secbase);
    return offset * geom->dg_secsize;
}

/************************************************
 * set the geometry at 1st access, if the image *
 * is new created by drv_qm_creat               *
 * used by drv_qm_read, *write, *format, *xseek *
 * *secid and *status                           *
 ************************************************/
static dsk_err_t drv_qm_set_geometry(QM_DSK_DRIVER * qm_self, const DSK_GEOMETRY * geom)
{
    size_t image_size;

    qm_self->qm_h_sector_size = geom->dg_secsize;
    qm_self->qm_h_nbr_sec_per_track = geom->dg_sectors;
    qm_self->qm_h_nbr_heads = geom->dg_heads;
    qm_self->qm_h_used_cyls = qm_self->qm_h_total_cyls = geom->dg_cylinders;
    qm_self->qm_h_secbase = (signed char) geom->dg_secbase - 1;
    qm_self->qm_h_nbr_sectors = geom->dg_cylinders * geom->dg_heads * geom->dg_sectors;
    switch (geom->dg_datarate)
    {
    case RATE_DD:
	qm_self->qm_h_density = QM_DENS_DD;
	break;
    case RATE_HD:
	qm_self->qm_h_density = QM_DENS_HD;
	break;
    case RATE_ED:
	qm_self->qm_h_density = QM_DENS_ED;
	break;
    default:
	qm_self->qm_h_density = QM_DENS_DD;
    }

    image_size = (size_t) qm_self->qm_h_nbr_sectors * (size_t) qm_self->qm_h_sector_size;

    /* Alloc memory for the image */
    qm_self->qm_image = dsk_malloc(image_size);
    if(!qm_self->qm_image)
	return DSK_ERR_NOMEM;
    qm_self->qm_super.dr_dirty = 1;			   /* new images are always dirty */

    return DSK_ERR_OK;
}

/************************************************
 * read the QM header                           *
 * used by drv_qm_open                          *
 ************************************************/
static dsk_err_t drv_qm_load_header(QM_DSK_DRIVER * qm_self, unsigned char *header)
{
    int i;

    /* Check the header checksum */
    unsigned char chksum = 0;
    for(i = QM_H_BASE; i < QM_HEADER_SIZE; i++)
	chksum += header[i];

    if(chksum)						   /* should be 0x00 */
    {
#ifdef DRV_QM_DEBUG
	fprintf(stderr, "qm: header checksum error\n");
#endif
	return DSK_ERR_NOTME;
    } else
    {
#ifdef DRV_QM_DEBUG
	fprintf(stderr, "qm: header checksum is ok!\n");
#endif
    }
    if(header[QM_H_BASE] != 'C' || header[QM_H_BASE + 1] != 'Q')
    {
#ifdef DRV_QM_DEBUG
	fprintf(stderr, "qm: First two chars are %c%c\n", header[QM_H_BASE], header[QM_H_BASE + 1]);
#endif
	return DSK_ERR_NOTME;
    }
    /* I'm guessing sector size is at 3. Yes! Expandqm thinks 7. False! */
    qm_self->qm_h_sector_size = get_u16(header, QM_H_SECSIZE);
    /* Number of sectors 0x0B-0x0C, strange number for non-blind, often 116 */
    qm_self->qm_h_nbr_sectors = get_u16(header, QM_H_SECTOTL);
    /* Number of sectors per track */
    qm_self->qm_h_nbr_sec_per_track = get_u16(header, QM_H_SECPTRK);
    /* Number of heads */
    qm_self->qm_h_nbr_heads = get_u16(header, QM_H_HEADS);
    /* Blind or not */
    qm_self->qm_h_blind = header[QM_H_BLIND];
    /* Density - 0 is DD, 1 means HD */
    qm_self->qm_h_density = header[QM_H_DENS];
    /* Number of used tracks */
    qm_self->qm_h_used_cyls = header[QM_H_USED_CYL];
    /* Number of total tracks */
    qm_self->qm_h_total_cyls = header[QM_H_TOTL_CYL];
    /* CRC 0x5c - 0x5f */
    qm_self->qm_h_crc = get_u32(header, QM_H_DATA_CRC);
    /* Length of comment */
    qm_self->qm_h_comment_len = get_u16(header, QM_H_CMT_SIZE);
    /* 0x71 is first sector number - 1 */
    qm_self->qm_h_secbase = (signed char) (header[QM_H_SECBASE]);
    /* 0x74 is interleave, I think. Normally 1, but 0 for old copyqm */
    qm_self->qm_h_interleave = header[QM_H_INTLV];
    /* 0x75 is skew. Normally 0. Negative number for alternating sides */
    qm_self->qm_h_skew = header[QM_H_SKEW];
    /* 0x76 is the source drive type */
    qm_self->qm_h_drive = header[QM_H_DRIVE];

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "qm: sector size is %d\n", qm_self->qm_h_sector_size);
    fprintf(stderr, "qm: nbr sectors %d\n", qm_self->qm_h_nbr_sectors);
    fprintf(stderr, "qm: nbr sectors/track %d\n", qm_self->qm_h_nbr_sec_per_track);
    fprintf(stderr, "qm: nbr heads %d\n", qm_self->qm_h_nbr_heads);
    fprintf(stderr, "qm: secbase %d (%d in image)\n", qm_self->qm_h_secbase + 1,
	    qm_self->qm_h_secbase);
    fprintf(stderr, "qm: density %d\n", qm_self->qm_h_density);
    fprintf(stderr, "qm: used cylinders %d\n", qm_self->qm_h_used_cyls);
    fprintf(stderr, "qm: CRC 0x%08lx\n", qm_self->qm_h_crc);
    fprintf(stderr, "qm: interleave %d\n", qm_self->qm_h_interleave);
    fprintf(stderr, "qm: skew %d\n", qm_self->qm_h_skew);
    fprintf(stderr, "qm: drive type %d\n", qm_self->qm_h_drive);
    fprintf(stderr, "qm: description \"%s\"\n", header + QM_H_DESCR);
#endif

    /* Fix the interleave value for old versions of CopyQM */
    if(qm_self->qm_h_interleave == 0)
	qm_self->qm_h_interleave = 1;

    return DSK_ERR_OK;
}

/************************************************
 * read run length coded data                   *
 * used by drv_qm_open                          *
 ************************************************/
static dsk_err_t drv_qm_load_image(QM_DSK_DRIVER * qm_self, FILE * fp)
{
    unsigned char *image = NULL;
    size_t curwritepos = 0;
    size_t image_size;
    unsigned char lengthBuf[2];
    int length, c;
    dsk_err_t errcond = DSK_ERR_OK;

    /* FIXME: Use the used tracks instead of the total tracks to detect */
    /*        that there is the correct amount of data in the image     */
    image_size = (size_t) qm_self->qm_h_nbr_sec_per_track *
	(size_t) qm_self->qm_h_nbr_heads *
	(size_t) qm_self->qm_h_total_cyls * (size_t) qm_self->qm_h_sector_size;

    /* Set the position after the header and comment */
    if(fseek(fp, QM_HEADER_SIZE + qm_self->qm_h_comment_len, SEEK_SET))
	return DSK_ERR_NOTME;

    /* Alloc memory for the image */
    qm_self->qm_image = dsk_malloc(image_size);
    if(!qm_self->qm_image)
	return DSK_ERR_NOMEM;
    image = qm_self->qm_image;

    /* Start reading */
    /* Note that it seems like each track starts a new block */
    while(curwritepos < image_size)
    {
	/* Read the length */
	if(1 != fread(lengthBuf, 2, 1, fp))
	{
	    if(feof(fp))				   /* EOF or other error? */
	    {
		/* End of file - fill with f6 - do not update CRC for these */
		memset(image + curwritepos, 0xf6, image_size - curwritepos);
		curwritepos = image_size;		   /* max reached, stop while loop */
	    } else
		return DSK_ERR_NOTME;			   /* Other disk error */
	} else
	{
	    length = get_i16(lengthBuf, 0);
	    if(length < 0)
	    {
		/* Negative number - next byte is repeated (-length) times */
		c = fgetc(fp);
		if(feof(fp))
		    return DSK_ERR_DATAERR;		   /* Orig. DSK_ERR_NOTME */
		/* Copy the byte into memory and update the offset */
		memset(image + curwritepos, c, -length);
		curwritepos -= length;
		/* Update CRC */
		while(length++)				   /* incremet up to 0 */
		    drv_qm_update_crc(&qm_self->qm_calc_crc, (unsigned char) c);
	    } else
	    {
		if(length != 0)
		{
		    /* Positive number - length different characters */
		    if(1 != fread(image + curwritepos, length, 1, fp))
			return DSK_ERR_DATAERR;		   /* Orig. DSK_ERR_NOTME */
		    /* Update CRC (and write pos) */
		    while(length--)			   /* decrement down to 0 */
			drv_qm_update_crc(&qm_self->qm_calc_crc, image[curwritepos++]);
		}
	    }
	}
    }
#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_load_image - crc from header = 0x%08lx, "
	    "calc = 0x%08lx\n", qm_self->qm_h_crc, qm_self->qm_calc_crc);
#endif
    /* Compare the CRCs */
    /* The CRC is zero on old images so it cannot be checked then */
    if(qm_self->qm_h_crc)
    {
	if(qm_self->qm_h_crc != qm_self->qm_calc_crc)
	    return DSK_ERR_DATAERR;			   /* Orig. DSK_ERR_NOTME */
    }

    /* Write image to file for testing */
    if(0)
    {
	FILE *tmpFile = fopen("/tmp/tmpfile.dd", "wb");
	fwrite(image, image_size, 1, tmpFile);
	fclose(tmpFile);
    }
    return errcond;
}

/************************************************
 * write run length coded data                  *
 * used by drv_qm_close                         *
 ************************************************/
static int drv_qm_write_rl(int rl, FILE * fp)
{
    unsigned char rlbuf[2];

    put_u16(rlbuf, 0, (unsigned int) rl);
    return (1 == fwrite(rlbuf, 2, 1, fp));		   /* TRUE if succesful */
}

/* Write RL coded data block */
static dsk_err_t drv_qm_dump_compressed(FILE * fp, unsigned long *pcrc, unsigned char *rd_ptr,
					size_t size)
{
    unsigned char *p;
    unsigned char a;
    int i, l, len;

    for(i = 0; i < size; i++)
	drv_qm_update_crc(pcrc, rd_ptr[i]);		   /* warming up cache */

    for(p = rd_ptr, i = 0, l = 0, len = size - 4; l < len;)
    {
	a = p[i];					   /* equals break even after 3, minimum 4 required */
	if((a == p[i + 1]) && (a == p[i + 2]) && (a == p[i + 3]))
	{
	    if(i)					   /* flush out previous non-equals */
	    {
		if(!drv_qm_write_rl(i, fp))		   /* positive length */
		    return DSK_ERR_NOTME;
		if(1 != fwrite(p, i, 1, fp))		   /* runlen unencoded data */
		    return DSK_ERR_NOTME;
		p += i;
	    }
	    for(a = *p, i = 0; (l < size) && (a == *p);)   /* find true length of equals */
	    {
		i++;
		l++;
		p++;
	    }
	    if(!drv_qm_write_rl(-i, fp))		   /* negative length */
		return DSK_ERR_NOTME;
	    if(1 != fwrite(&a, 1, 1, fp))		   /* runlen data */
		return DSK_ERR_NOTME;
	    i = 0;
	} else
	{
	    i++;
	    l++;
	}
    }
    if(i || (l < size))					   /* dump remaining buffer after end of block */
    {
	i += size - l;
	if(!drv_qm_write_rl(i, fp))			   /* unencoded rest of block */
	    return DSK_ERR_NOTME;
	if(1 != fwrite(p, i, 1, fp))			   /* runlen data */
	    return DSK_ERR_NOTME;
    }
    return DSK_ERR_OK;
}

/************************************************
 * public functions                             *
 ************************************************/
/************************************************
 * open                                         *
 ************************************************/
dsk_err_t drv_qm_open(DSK_DRIVER * self, const char *filename)
{
    FILE *fp;
    unsigned char header[QM_HEADER_SIZE];
    char *comment_buf;
    dsk_err_t errcond = DSK_ERR_OK;

    /* Create self pointer or return if wrong type */
    MAKE_CHECK_SELF;

    /* Zero some stuff */
    qm_self->qm_image = NULL;
    qm_self->qm_filename = NULL;

    /* Open file. Read only for now */
    fp = fopen(filename, "rb");
    if(!fp)
	return DSK_ERR_NOTME;

    /* Keep a copy of the filename for future writing use */
    qm_self->qm_filename = (char *) dsk_malloc(1 + strlen(filename));
    if(qm_self->qm_filename == NULL)
    {
	fclose(fp);
	return DSK_ERR_NOMEM;
    }
    strcpy(qm_self->qm_filename, filename);

    /* Load the header */
    if(1 != fread(header, QM_HEADER_SIZE, 1, fp))
    {
	errcond = DSK_ERR_NOTME;
	goto end;
    }

    /* Load the header */
    errcond = drv_qm_load_header(qm_self, header);

    /* If there's a comment, allocate a temporary buffer for it and load it. */
    if(errcond == DSK_ERR_OK && qm_self->qm_h_comment_len)
    {
	comment_buf = dsk_malloc(1 + qm_self->qm_h_comment_len);
	/* If malloc fails, ignore it - comments aren't essential */
	if(comment_buf)
	{
	    if(fseek(fp, QM_HEADER_SIZE, SEEK_SET))
		errcond = DSK_ERR_NOTME;
	    else
	    {
		if(1 != fread(comment_buf, qm_self->qm_h_comment_len, 1, fp))
		    errcond = DSK_ERR_NOTME;
		else
		{
		    comment_buf[qm_self->qm_h_comment_len] = '\0';
		    dsk_set_comment(&qm_self->qm_super, comment_buf);
		    dsk_free(comment_buf);
		}
	    }
	}
    }
    qm_self->qm_sector = qm_self->qm_h_secbase + 1;	   /* Init sector for secid function */
    /* Load the rest */
    if(errcond == DSK_ERR_OK)
    {
	errcond = drv_qm_load_image(qm_self, fp);
	if(errcond != DSK_ERR_OK)
	{
#ifdef DRV_QM_DEBUG
	    fprintf(stderr, "drv_qm_load_image returned %d\n", (int) errcond);
#endif
	}
    }

    /* Return errcond */
  end:
    /* Close the file */
    if(fp)
	fclose(fp);

    return errcond;
}

/************************************************
 * create                                       *
 ************************************************/
dsk_err_t drv_qm_create(DSK_DRIVER * self, const char *filename)
{
/* I think about this */
#undef CHK_FILE

    dsk_err_t errcond = DSK_ERR_OK;
    char def_cmt[17];

    /* Create self pointer or return if wrong type */
    MAKE_CHECK_SELF;

    /* Init some stuff */
    qm_self->qm_image = NULL;
    qm_self->qm_filename = NULL;
    qm_self->qm_h_blind = QM_BLIND_BLN;			   /* new images are always blind */
    qm_self->qm_h_interleave = 1;
    qm_self->qm_h_skew = 0;
    qm_self->qm_h_drive = QM_DRV_UNKNWN;		   /* unknown at this time */

    sprintf(def_cmt, "LibDsk v%.8s", LIBDSK_VERSION);          /* set default comment */
    dsk_set_comment(&qm_self->qm_super, def_cmt);
    qm_self->qm_h_comment_len = strlen(def_cmt) + 1;

#ifdef CHK_FILE
    FILE *fp;

    /* Check to open the file. Create new or truncate an existing file */
    fp = fopen(filename, "wb");
    if(!fp)
	return DSK_ERR_SYSERR;
    fclose(fp);						   /* Close the file immediately */
#endif

    /* Keep a copy of the filename for future writing use */
    qm_self->qm_filename = (char *) dsk_malloc(1 + strlen(filename));
    if(qm_self->qm_filename == NULL)
    {
	return DSK_ERR_NOMEM;				   /* Can't allocate a buffer */
    }
    strcpy(qm_self->qm_filename, filename);
    return errcond;
}

/************************************************
 * close                                        *
 ************************************************/
dsk_err_t drv_qm_close(DSK_DRIVER * self)
{
    FILE *fp;
    unsigned char header[QM_HEADER_SIZE];
    char *cmt;
    dsk_err_t errcond = DSK_ERR_OK;
    unsigned long crc;
    int tmp;
    int wr_cyl, wr_hd;
    size_t trk_size;
    long offset;
    time_t mod;
    struct tm *lz;

    QM_DSK_DRIVER *qm_self;
#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_close\n");
#endif
    if(self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;
    qm_self = (QM_DSK_DRIVER *) self;
    if(!qm_self->qm_filename || !qm_self->qm_image)
	return DSK_ERR_NOTRDY;

    /* Open file for write if dirty */
    if(qm_self->qm_super.dr_dirty)
    {
#ifdef DRV_QM_DEBUG
	fprintf(stderr, "qm: dirty flag %d\n", qm_self->qm_super.dr_dirty);
#endif
	fp = fopen(qm_self->qm_filename, "wb");
	if(!fp)
	{
	    return DSK_ERR_NOTME;
	}
	/* Write the header */
	memset(header, 0x00, QM_HEADER_SIZE);

	sprintf((char *) &header[QM_H_BASE], "CQ\x14");	   /* Signature */
	put_u16(header, QM_H_SECSIZE, qm_self->qm_h_sector_size);

	tmp = qm_self->qm_h_total_cyls * qm_self->qm_h_nbr_heads * qm_self->qm_h_nbr_sec_per_track;
	put_u16(header, QM_H_SECTOTL, tmp);
	put_u16(header, QM_H_SECPTRK, qm_self->qm_h_nbr_sec_per_track);
	put_u16(header, QM_H_HEADS, qm_self->qm_h_nbr_heads);

	tmp = (qm_self->qm_h_total_cyls * qm_self->qm_h_nbr_heads *
	       qm_self->qm_h_nbr_sec_per_track * qm_self->qm_h_sector_size) >> 10;
	sprintf((char *) &header[QM_H_DESCR], "%dK CQM floppy image", tmp);

	/* header[QM_H_BLIND] = (unsigned char)qm_self->qm_h_blind; */
	header[QM_H_BLIND] = QM_BLIND_BLN;		   /* always blind in this version */
	header[QM_H_DENS] = (unsigned char) qm_self->qm_h_density;
	header[QM_H_USED_CYL] = (unsigned char) qm_self->qm_h_total_cyls;
	header[QM_H_TOTL_CYL] = (unsigned char) qm_self->qm_h_total_cyls;

	memset(&header[QM_H_LABEL], ' ', QM_H_LBL_SIZE);   /* Empty volume label */
	/* alternatively, you can set a 11byte label */
	sprintf((char *) &header[QM_H_LABEL], "%.*s", QM_H_LBL_SIZE, "** NONE ** ");

        /* Processing date and time if avialable, else leave unchanged */
#ifdef HAVE_TIME_H
	mod = time(NULL);				   /* Modificaten time */
	lz = localtime(&mod);
	put_u16(header, QM_H_TIME, (unsigned int)
		(lz->tm_hour & 0x1f) << 11 | (lz->tm_min & 0x3f) << 5 | ((lz->tm_sec / 2) & 0x1f));
	put_u16(header, QM_H_DATE, (unsigned int)
		((lz->tm_year - 80) & 0x7f) << 9 | (++(lz->tm_mon) & 0x0f) << 5
		| (lz->tm_mday & 0x1f));
#endif

        /* Processing comment */
	qm_self->qm_h_comment_len = 0;
	errcond = dsk_get_comment(&qm_self->qm_super, &cmt);
        if(!errcond && cmt)                                /* if comment exist */
            if(strlen(cmt))                                /* except the null string "" */
	        qm_self->qm_h_comment_len = strlen(cmt) + 1;
	put_u16(header, QM_H_CMT_SIZE, (unsigned int) qm_self->qm_h_comment_len);

	header[QM_H_SECBASE] = (unsigned char) qm_self->qm_h_secbase;
	header[QM_H_INTLV] = (unsigned char) qm_self->qm_h_interleave;
	header[QM_H_SKEW] = (unsigned char) qm_self->qm_h_skew;
	header[QM_H_DRIVE] = (unsigned char) qm_self->qm_h_drive;

	/* write header preliminary */
	if(1 != fwrite(header, QM_HEADER_SIZE, 1, fp))
	{
	    errcond = DSK_ERR_NOTME;
	    goto end;
	}
	/* Write the comment if exist */
	if(qm_self->qm_h_comment_len)
	    if(1 != fwrite(cmt, strlen(cmt) + 1, 1, fp))
	    {
		errcond = DSK_ERR_NOTME;
		goto end;
	    }
	/* Write the image  RL coded, one run per track */
	crc = 0l;
	trk_size = (size_t) (qm_self->qm_h_nbr_sec_per_track * qm_self->qm_h_sector_size);
	for(wr_cyl = 0; wr_cyl < qm_self->qm_h_total_cyls; wr_cyl++)
	{
	    for(wr_hd = 0; wr_hd < qm_self->qm_h_nbr_heads; wr_hd++)
	    {
		offset = (wr_cyl * qm_self->qm_h_nbr_heads) + wr_hd;	/* Drive track */
		offset *= trk_size;

		if(drv_qm_dump_compressed(fp, &crc, qm_self->qm_image + offset,
					  trk_size) != DSK_ERR_OK)
		{
		    errcond = DSK_ERR_NOTME;
		    goto end;
		}
	    }
	}

#ifdef DRV_QM_DEBUG
	fprintf(stderr, "qm: CRC 0x%08lx\n", crc);
#endif
	/* Write data CRC */
	put_u16(header, QM_H_DATA_CRC, (unsigned int) crc);
	put_u16(header, QM_H_DATA_CRC + 2, (unsigned int) (crc >> 16));

	crc = 0l;					   /* Calculate header CRC */
	for(tmp = QM_H_BASE; tmp < QM_HEADER_SIZE - 1; tmp++)
	    crc += header[tmp];
	header[QM_H_HEAD_CRC] = (unsigned char) (-crc);
	if(fseek(fp, 0l, SEEK_SET))
	{
	    errcond = DSK_ERR_NOTME;
	    goto end;
	}
	/* write header final, with CRC */
	if(1 != fwrite(header, QM_HEADER_SIZE, 1, fp))
	    errcond = DSK_ERR_NOTME;

	/* Return errcond */
      end:
	/* Close the file */
	if(fp)
	    fclose(fp);
    }
    /* Free buffers */
    dsk_free(qm_self->qm_filename);
    dsk_free(qm_self->qm_image);
    return errcond;
}

/************************************************
 * read                                         *
 ************************************************/
dsk_err_t drv_qm_read(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		      void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, dsk_psect_t sector)
{
    long offset;
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!buf || !self || !geom || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_read chs=%d,%d,%d\n", cylinder, head, sector);
#endif
    if(!qm_self->qm_filename)
	return DSK_ERR_NOTRDY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    if(cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
	return DSK_ERR_SEEKFAIL;			   /* Can't access outside the image buffer */
    if((sector < geom->dg_secbase) || (sector > (geom->dg_secbase - 1 + geom->dg_sectors)))
	return DSK_ERR_NOADDR;				   /* Sector not found */

    offset = drv_qm_calc_offset(geom, cylinder, head, sector);
    memcpy(buf, qm_self->qm_image + offset, geom->dg_secsize);
    return DSK_ERR_OK;
}

/************************************************
 * write                                        *
 ************************************************/
dsk_err_t drv_qm_write(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		       const void *buf, dsk_pcyl_t cylinder, dsk_phead_t head, dsk_psect_t sector)
{
    long offset;
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!buf || !self || !geom || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_write chs=%d,%d,%d\n", cylinder, head, sector);
#endif
    if(!qm_self->qm_filename)
	return DSK_ERR_NOTRDY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    if(cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
	return DSK_ERR_SEEKFAIL;			   /* Can't access outside the image buffer */
    if((sector < geom->dg_secbase) || (sector > (geom->dg_secbase - 1 + geom->dg_sectors)))
	return DSK_ERR_NOADDR;				   /* Sector not found */

    offset = drv_qm_calc_offset(geom, cylinder, head, sector);
    memcpy(qm_self->qm_image + offset, buf, geom->dg_secsize);
    return DSK_ERR_OK;
}

/************************************************
 * format                                       *
 ************************************************/
dsk_err_t drv_qm_format(DSK_DRIVER * self, DSK_GEOMETRY * geom,
			dsk_pcyl_t cylinder, dsk_phead_t head, const DSK_FORMAT * format,
			unsigned char filler)
{
/* The "format" parameter is completely unused, since QM images
 * don't have really sector headers.
 */
    long offset;
    size_t trk_size;
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!format || !self || !geom || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_format ch=%d,%d\n", cylinder, head);
#endif
    if(!qm_self->qm_filename)
	return DSK_ERR_NOTRDY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    if(cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
	return DSK_ERR_SEEKFAIL;			   /* Can't access outside the image buffer */

    trk_size = (size_t) (qm_self->qm_h_nbr_sec_per_track * qm_self->qm_h_sector_size);
    offset = (cylinder * qm_self->qm_h_nbr_heads) + head;  /* Drive track */
    offset *= trk_size;
    memset(qm_self->qm_image + offset, filler, trk_size);
    return DSK_ERR_OK;
}

/************************************************
 * get geometry                                 *
 ************************************************/
/* Initialize the DSK_GEOMETRY structure
 * The QM format provides the physical format in its header,
 * so we can set DSK_GEOMETRY directly
 */
dsk_err_t drv_qm_getgeom(DSK_DRIVER * self, DSK_GEOMETRY * geom)
{
    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!geom || !self || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_getgeom\n");
#endif
    geom->dg_sidedness = SIDES_ALT;			   /* alway SIDE_ALT */
    /* Now override with the geometry that the image provides */
    geom->dg_cylinders = qm_self->qm_h_total_cyls;
    geom->dg_heads = qm_self->qm_h_nbr_heads;
    geom->dg_sectors = qm_self->qm_h_nbr_sec_per_track;
    geom->dg_secbase = qm_self->qm_h_secbase + 1;
    geom->dg_secsize = qm_self->qm_h_sector_size;
    /* Translate the datarate */
    switch (qm_self->qm_h_density)
    {
    case QM_DENS_DD:
	geom->dg_datarate = RATE_DD;
	break;
    case QM_DENS_HD:
	geom->dg_datarate = RATE_HD;
	break;
    case QM_DENS_ED:
	geom->dg_datarate = RATE_ED;
	break;
    default:
	geom->dg_datarate = RATE_SD;
    }
    /* Took these from drvwin32.c */
    switch (geom->dg_sectors)
    {
    case 8:
	geom->dg_rwgap = 0x2A;
	geom->dg_fmtgap = 0x50;
	break;
    case 9:
	geom->dg_rwgap = 0x2A;
	geom->dg_fmtgap = 0x52;
	break;
    case 10:
	geom->dg_rwgap = 0x0C;
	geom->dg_fmtgap = 0x17;
	break;
    case 15:
	geom->dg_rwgap = 0x1B;
	geom->dg_fmtgap = 0x50;
	break;
    case 18:
	geom->dg_rwgap = 0x1B;
	geom->dg_fmtgap = 0x50;
	break;
    default:
	geom->dg_rwgap = 0x2A;
	geom->dg_fmtgap = 0x52;
	break;
    }

    /* FIXME: What to do with interleave and skew? */

    geom->dg_fm = RECMODE_MFM;
    geom->dg_nomulti = 0;
    return DSK_ERR_OK;
}

/************************************************
 * secid                                        *
 ************************************************/
dsk_err_t drv_qm_secid(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		       dsk_pcyl_t cylinder, dsk_phead_t head, DSK_FORMAT * result)
{
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!self || !geom || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_secid ch=%d,%d\n", cylinder, head);
#endif
    if(!qm_self->qm_filename)
	return DSK_ERR_NOTRDY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    if(cylinder >= qm_self->qm_h_total_cyls || head >= qm_self->qm_h_nbr_heads)
	return DSK_ERR_SEEKFAIL;

    if(result)
    {
/* This will never happen, qm_self->qm_sector is unsigned
	if(qm_self->qm_sector < 0)
	    qm_self->qm_sector = qm_self->qm_h_secbase + 1;	// reinit 
*/
	result->fmt_cylinder = cylinder;
	result->fmt_head = head;
	result->fmt_sector = ((qm_self->qm_sector++) % qm_self->qm_h_nbr_sec_per_track)
	    + ((qm_self->qm_h_secbase + 1) & 0xFF);
	result->fmt_secsize = qm_self->qm_h_sector_size;
    }
#ifdef DRV_QM_DEBUG
    if(geom->dg_secsize != qm_self->qm_h_sector_size)
	fprintf(stderr, "secid: secsize mismatch Geometry=%d Header=%d\n", geom->dg_secsize,
		qm_self->qm_h_sector_size);
#endif
    return DSK_ERR_OK;
}

/************************************************
 * xseek                                        *
 ************************************************/
dsk_err_t drv_qm_xseek(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
		       dsk_pcyl_t cylinder, dsk_phead_t head)
{
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!self || !geom || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_xseek ch=%d,%d\n", cylinder, head);
#endif
    if(!qm_self->qm_filename)
	return DSK_ERR_NOTRDY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    if(cylinder >= geom->dg_cylinders || head >= geom->dg_heads)
	return DSK_ERR_SEEKFAIL;			   /* Can't access behind image buffer */
    return DSK_ERR_OK;					   /* OK, do nothing */
}

/************************************************
 * status                                       *
 ************************************************/
dsk_err_t drv_qm_status(DSK_DRIVER * self, const DSK_GEOMETRY * geom,
			dsk_phead_t head, unsigned char *result)
{
    dsk_err_t errcond;

    QM_DSK_DRIVER *qm_self = (QM_DSK_DRIVER *) self;
    if(!geom || !self || self->dr_class != &dc_qm)
	return DSK_ERR_BADPTR;

#ifdef DRV_QM_DEBUG
    fprintf(stderr, "drv_qm_status\n");
#endif
    if(!qm_self->qm_filename)
	*result &= ~DSK_ST3_READY;
    if(!qm_self->qm_image)				   /* first access to a new created image? */
	if(DSK_ERR_OK != (errcond = drv_qm_set_geometry(qm_self, geom)))
	    return errcond;
    return DSK_ERR_OK;
}
