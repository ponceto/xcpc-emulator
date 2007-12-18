/* lib765.c - Copyright (c) 2002, 2003, 2004 John Elliott - 2005 Philip Kendall
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "upd765.h"

static void log_dprintf(int debuglevel, char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  if(debuglevel <= 1) {
    (void) fprintf(stderr, "lib765 level:%d error: ", debuglevel);
    (void) vfprintf(stderr, fmt, ap); (void) fputc('\n', stderr);
  }
  va_end(ap);
}

/*
 * ############################################################################
 * DSK implementation
 * ############################################################################
 */

static fd_err_t fdl_xlt_error(dsk_err_t err)
{
  switch(err) {
    case DSK_ERR_OK:       return FD_E_OK;
    case DSK_ERR_SEEKFAIL: return FD_E_SEEKFAIL;
    case DSK_ERR_NOADDR:   return FD_E_NOADDR;
    case DSK_ERR_NODATA:   return FD_E_NODATA;
    case DSK_ERR_DATAERR:  return FD_E_DATAERR;
    case DSK_ERR_NOTRDY:   return FD_E_NOTRDY;
    case DSK_ERR_RDONLY:   return FD_E_READONLY;
  }
  return FD_E_NOSECTOR;
}

static int fdl_regeom(FDD_765 *fdd)
{
  dsk_err_t err = dsk_getgeom(fdd->fdl_diskp, &fdd->fdl_diskg);

  if((err != DSK_ERR_OK)
  && (err != DSK_ERR_NOADDR)
  && (err != DSK_ERR_NODATA)
  && (err != DSK_ERR_BADFMT)) {
    log_dprintf(0, "Could not get geometry for %s: %s.\n", fdd->fdl_filename, dsk_strerror(err));
    if(fdd->fdl_diskp != NULL) {
      dsk_close(&fdd->fdl_diskp);
      fdd->fdl_filename[0] = 0;
      fdd->fdl_type        = NULL;
      fdd->fdl_compress    = NULL;
      fdd->fdl_diskp       = NULL;
    }
  }
  return(err);
}

static int fdl_ready(FDD_765 *fdd)
{
  dsk_err_t err;

  if(fdd->fd_motor == 0)        return(0);
  if(fdd->fdl_diskp != NULL)    return(1);
  if(fdd->fdl_filename[0] == 0) return(0);

  err = dsk_open(&fdd->fdl_diskp, fdd->fdl_filename, fdd->fdl_type, fdd->fdl_compress);
  if((err != DSK_ERR_OK) || (fdd->fdl_diskp == NULL)) {
    log_dprintf(0, "Could not open %s: %s.\n", fdd->fdl_filename, dsk_strerror(err));
    if(fdd->fdl_diskp != NULL) {
      dsk_close(&fdd->fdl_diskp);
      fdd->fdl_filename[0] = 0;
      fdd->fdl_type        = NULL;
      fdd->fdl_compress    = NULL;
      fdd->fdl_diskp       = NULL;
    }
    return(0);
  }
  return(fdl_regeom(fdd) == 0);
}

static void fdl_eject(FDD_765 *fd)
{
  if(fd->fdl_diskp) {
    dsk_close(&fd->fdl_diskp);
    fd->fdl_filename[0] = 0;
    fd->fdl_type        = NULL;
    fd->fdl_compress    = NULL;
    fd->fdl_diskp       = NULL;
  }
}

static fd_err_t fdl_seek_cylinder(FDD_765 *fd, int cylinder)
{
	int req_cyl = cylinder;
	dsk_err_t err;

	log_dprintf(4, "fdl_seek_cylinder: cylinder=%d\n",cylinder);

	if (!fd->fdl_diskp) return FD_E_NOTRDY;

	log_dprintf(6, "fdl_seek_cylinder: image open OK\n");

	err = dsk_pseek(fd->fdl_diskp, &fd->fdl_diskg, cylinder, 0);
	if (err == DSK_ERR_NOTIMPL || err == DSK_ERR_OK)
	{
		log_dprintf(6, "fdl_seek_cylinder: OK\n");
		fd->fd_cylinder = req_cyl;
		return 0;
	}
	log_dprintf(6, "fdl_seek_cylinder: fails, LIBDSK error %d\n", err);
	/* Check if the DSK image goes out to the correct cylinder */
	return fdl_xlt_error(err);
}

/* Read a sector ID from the current track */
static fd_err_t fdl_read_id(FDD_765 *fd, int head, int sector, fdc_byte *buf)
{
	dsk_err_t err;
	DSK_FORMAT fmt;

	log_dprintf(4, "fdl_read_id: head=%d\n", head);
	if (!fd->fdl_diskp) return FD_E_NOTRDY;
	err = dsk_psecid(fd->fdl_diskp, &fd->fdl_diskg, fd->fd_cylinder,
			 head, &fmt);
	if (err == DSK_ERR_NOTIMPL)
	{
		buf[0] = fd->fd_cylinder;
		buf[1] = head;
		buf[2] = sector;
		buf[3] = dsk_get_psh(fd->fdl_diskg.dg_secsize);
		return 0;
	}
	if (err) return fdl_xlt_error(err);

	buf[0] = fmt.fmt_cylinder;
	buf[1] = fmt.fmt_head;
	buf[2] = fmt.fmt_sector;
	buf[3] = dsk_get_psh(fmt.fmt_secsize);
	return 0;
}


/* Read a sector */
static fd_err_t fdl_read_sector(FDD_765 *fd, int xcylinder, int xhead, 
		int head,  int sector, fdc_byte *buf, int len, int *deleted,
		int skip_deleted, int mfm, int multi)
{
	dsk_err_t err;

	log_dprintf(4, "fdl_read_sector: cyl=%d xc=%d xh=%d h=%d s=%d len=%d\n", 
			fd->fd_cylinder, xcylinder, xhead, head, sector, len);
	if (!fd->fdl_diskp) return FD_E_NOTRDY;

	fd->fdl_diskg.dg_noskip  = skip_deleted ? 0 : 1;
	fd->fdl_diskg.dg_fm      = mfm ? 0 : 1;
	fd->fdl_diskg.dg_nomulti = multi ? 0 : 1;

	err = dsk_xread(fd->fdl_diskp, &fd->fdl_diskg, buf,
		fd->fd_cylinder, head, xcylinder, xhead, sector, len, deleted);

	if (err == DSK_ERR_NOTIMPL)
	{
/* lib765 v0.3.2: If 'deleted' is passed but points to zero, treat it as if
 * 'deleted' is not passed at all. */
		if (deleted && *deleted) return FD_E_NOADDR;
		err = dsk_pread(fd->fdl_diskp, &fd->fdl_diskg, buf,
			fd->fd_cylinder, head, sector);
	}
	return fdl_xlt_error(err);
}

/* Read a track */
static fd_err_t fdl_read_track(FDD_765 *fd, int xcylinder, int xhead,
                int head,  fdc_byte *buf, int *len)
{
	fd_err_t err; 

	log_dprintf(4, "fdl_read_track: xc=%d xh=%d h=%d\n", 
			xcylinder, xhead, head);
	if (!fd->fdl_diskp) return FD_E_NOTRDY;

	err = dsk_xtread(fd->fdl_diskp, &fd->fdl_diskg, buf,
			fd->fd_cylinder, head, xcylinder, xhead);
	return fdl_xlt_error(err);
}



/* Write a sector */
static fd_err_t fdl_write_sector(FDD_765 *fd, int xcylinder, int xhead, 
		int head,  int sector, fdc_byte *buf, int len, int deleted, 
		int skip_deleted, int mfm, int multi)
{
	dsk_err_t err;

	log_dprintf(4, "fdl_write_sector: xc=%d xh=%d h=%d s=%d\n", 
			xcylinder, xhead, head, sector);
	if (!fd->fdl_diskp) return FD_E_NOTRDY;

	fd->fdl_diskg.dg_noskip  = skip_deleted ? 0 : 1;
/* lib765 0.3.3: Oops. Get the FM/MFM flag round the right way. */
	fd->fdl_diskg.dg_fm      = mfm ? 0 : 1;
	fd->fdl_diskg.dg_nomulti = multi ? 0 : 1;
	err = dsk_xwrite(fd->fdl_diskp, &fd->fdl_diskg, buf,
		fd->fd_cylinder, head, xcylinder, xhead, sector, len, deleted);
	if (err == DSK_ERR_NOTIMPL)
	{
		if (deleted) return FD_E_NOADDR;

		err = dsk_pwrite(fd->fdl_diskp, &fd->fdl_diskg, buf,
			fd->fd_cylinder, head, sector);
	}
	return fdl_xlt_error(err);
}

/* Format a track on a DSK. Can grow the DSK file. */
static fd_err_t fdl_format_track(FDD_765 *fd, int head,
                int sectors, fdc_byte *track, fdc_byte filler)
{
	int n, os;
	dsk_err_t err;
	DSK_FORMAT *formbuf;

	log_dprintf(4, "fdl_format_track: cyl=%d h=%d s=%d\n", 
			fd->fd_cylinder, head, sectors);
	if (!fd->fdl_diskp) return FD_E_NOTRDY;

	formbuf = malloc(sectors * sizeof(DSK_FORMAT));
	if (!formbuf) return FD_E_READONLY;

	for (n = 0; n < sectors; n++)
	{
		formbuf[n].fmt_cylinder = track[n * 4];
		formbuf[n].fmt_head     = track[n * 4 + 1];
		formbuf[n].fmt_sector   = track[n * 4 + 2];
		formbuf[n].fmt_secsize  = 128 << track[n * 4 + 3];
	}
	os = fd->fdl_diskg.dg_sectors;
	fd->fdl_diskg.dg_sectors = sectors;
	err = dsk_pformat(fd->fdl_diskp, &fd->fdl_diskg, fd->fd_cylinder,
			head, formbuf, filler);
	fd->fdl_diskg.dg_sectors = os;

	free(formbuf);
	if (fd->fd_cylinder == 0 && !fd->fd_cylinder) fdl_regeom(fd);
	if (!err) 
	{
		return 0;
	}
	return fdl_xlt_error(err);
}

static fdc_byte fdl_drive_status(FDD_765 *fd)
{
	fdc_byte st;
	dsk_err_t err;

        if (fd->fdl_diskp)
	{
		err = dsk_drive_status(fd->fdl_diskp, &fd->fdl_diskg, 0, &st);
	}
	else 
	{
		st = 0;
		if (fdl_ready(fd)) st = DSK_ST3_READY;
	}

	/* 5.25" drives don't report read-only when they're not ready */
	if (fd->fd_type == FD_525)
	{
		if ((st & (DSK_ST3_RO | DSK_ST3_READY)) == DSK_ST3_RO)
		{
			st &= ~DSK_ST3_RO;
		}
	}
	else
	/* 3" and 3.5" drives always report read-only when not ready */
	{
		if (!(st & DSK_ST3_READY)) st |= DSK_ST3_RO;
		if (fd->fd_readonly)       st |= DSK_ST3_RO;
	}

	if (fd->fd_cylinder == 0  ) st |= DSK_ST3_TRACK0;  /* Track 0   */

	if (fd->fd_type == FD_35)		/* 3.5" does not give track 0
					         * if motor is off */
	{
		if (! fd->fd_motor ) st &= ~DSK_ST3_TRACK0;
	}
	if (fd->fd_heads > 1) st |= DSK_ST3_DSDRIVE;	/* Double sided */
	if (!fd->fd_motor) st &= ~DSK_ST3_READY;	/* Motor is not running */

	return st;
}

static FDD_765_VTABLE fdv_libdsk = {
  fdl_seek_cylinder,
  fdl_read_id,
  fdl_read_sector,
  fdl_read_track,
  fdl_write_sector,
  fdl_format_track,
  fdl_drive_status,
  fdl_ready,
  fdl_eject
};

/*
 * ############################################################################
 * FDD implementation
 * ############################################################################
 */

void fdd_init_impl(FDD_765 *fdd)
{
  (void) memset(fdd, 0, sizeof(FDD_765));
  fdd->fd_vtable    = &fdv_libdsk;
  fdd->fd_type      = FD_NONE;
  fdd->fd_heads     = 0;
  fdd->fd_cylinders = 0;
  fdd->fd_readonly  = 0;
  fdd->fd_motor     = 0;
  fdd->fd_cylinder  = 0;
  fdd_reset_impl(fdd);
}

void fdd_reset_impl(FDD_765 *fdd)
{
  fdd->fdl_filename[0] = 0;
  fdd->fdl_type        = NULL;
  fdd->fdl_compress    = NULL;
  fdd->fdl_diskp       = NULL;
}

fd_err_t fdd_seek_cylinder(FDD_765 *fdd, int cylinder)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_seek_cylinder != NULL)) {
    return((*fdd->fd_vtable->fdv_seek_cylinder)(fdd, cylinder));
  }
  return(FD_E_NOTRDY);
}

fd_err_t fdd_read_id(FDD_765 *fdd, int head, int sector, fdc_byte *buf)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_read_id != NULL)) {
    return((*fdd->fd_vtable->fdv_read_id)(fdd, head, sector, buf));
  }
  return(FD_E_NOTRDY);
}

fd_err_t fdd_read_sector(FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int *deleted, int skip_deleted, int mfm, int multi)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_read_sector != NULL)) {
    return((*fdd->fd_vtable->fdv_read_sector)(fdd, xcylinder, xhead, head, sector, buf, len, deleted, skip_deleted, mfm, multi));
  }
  return(FD_E_NOTRDY);
}

fd_err_t fdd_read_track(FDD_765 *fdd, int xcylinder, int xhead, int head, fdc_byte *buf, int *len)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_read_track != NULL)) {
    return((*fdd->fd_vtable->fdv_read_track)(fdd, xcylinder, xhead, head, buf, len));
  }
  return(FD_E_NOTRDY);
}

fd_err_t fdd_write_sector(FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int deleted, int skip_deleted, int mfm, int multi)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_write_sector != NULL)) {
    return((*fdd->fd_vtable->fdv_write_sector)(fdd, xcylinder, xhead, head, sector, buf, len, deleted, skip_deleted, mfm, multi));
  }
  return(FD_E_NOTRDY);
}

fd_err_t fdd_format_track(FDD_765 *fdd, int head, int sectors, fdc_byte *track, fdc_byte filler)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_format_track != NULL)) {
    return((*fdd->fd_vtable->fdv_format_track)(fdd, head, sectors, track, filler));
  }
  return(FD_E_NOTRDY);
}

int fdd_ready(FDD_765 *fdd)
{
  if((fdd != NULL)
  && (fdd->fd_vtable != NULL)
  && (fdd->fd_vtable->fdv_ready != NULL)) {
    return((*fdd->fd_vtable->fdv_ready)(fdd));
  }
  return(0);
}

/*
 * ############################################################################
 * FDC implementation
 * ############################################################################
 */

static int bytes_in_cmd[32] = {
  1, /*  0 = none                   */
  1, /*  1 = none                   */
  9, /*  2 = READ TRACK             */
  3, /*  3 = SPECIFY                */
  2, /*  4 = SENSE DRIVE STATUS     */
  9, /*  5 = WRITE DATA             */
  9, /*  6 = READ DATA              */
  2, /*  7 = RECALIBRATE            */
  1, /*  8 = SENSE INTERRUPT STATUS */
  9, /*  9 = WRITE DELETED DATA     */
  2, /* 10 = READ SECTOR ID         */
  1, /* 11 = none                   */
  9, /* 12 = READ DELETED DATA      */
  6, /* 13 = FORMAT A TRACK         */
  1, /* 14 = none                   */
  3, /* 15 = SEEK                   */
  1, /* 16 = none                   */
  9, /* 17 = SCAN EQUAL             */
  1, /* 18 = none                   */
  1, /* 19 = none                   */
  1, /* 20 = none                   */
  1, /* 21 = none                   */
  1, /* 22 = none                   */
  1, /* 23 = none                   */
  1, /* 24 = none                   */
  9, /* 25 = SCAN LOW OR EQUAL      */
  1, /* 26 = none                   */
  1, /* 27 = none                   */
  1, /* 28 = none                   */
  1, /* 29 = none                   */
  9, /* 30 = SCAN HIGH OR EQUAL     */
  1  /* 31 = none                   */
};

void fdc_init_impl(FDC_765 *fdc)
{
  (void) memset(fdc, 0, sizeof(FDC_765));
  fdc->fdc_isr      = NULL;
  fdc->fdc_drive[0] = NULL;
  fdc->fdc_drive[1] = NULL;
  fdc->fdc_drive[2] = NULL;
  fdc->fdc_drive[3] = NULL;
  fdc_reset_impl(fdc);
}

void fdc_reset_impl(FDC_765 *fdc)
{
  fdc->reg.msr           = 0x80;
  fdc->reg.st0           = 0;
  fdc->reg.st1           = 0;
  fdc->reg.st2           = 0;
  fdc->reg.st3           = 0;
  fdc->reg.srt           = 0x00;
  fdc->reg.hlt           = 0x00;
  fdc->reg.hut           = 0x00;
  fdc->reg.ndm           = 0x01;
  fdc->unit_id           = 0;
  fdc->head_id           = 0;
  (void) memset(fdc->cmd.buf, 0, sizeof(fdc->cmd.buf));
  fdc->cmd.len           = 0;
  fdc->cmd.pos           = 0;
  (void) memset(fdc->exe.buf, 0, sizeof(fdc->exe.buf));
  fdc->exe.len           = 0;
  fdc->exe.pos           = 0;
  (void) memset(fdc->res.buf, 0, sizeof(fdc->res.buf));
  fdc->res.len           = 0;
  fdc->res.pos           = 0;
  fdc->isr.state  = 0;
  fdc->isr.count = 0;
  fdc->fdc_write_deleted = 0;
  fdc->fdc_lastidread    = 0;
  fdc->reg.cmd           = -1;
}

static int fdc_ready(FDC_765 *self, FDD_765 *fd)
{
  int rdy = fdd_ready(fd);

  if(rdy == 0) {
    self->reg.st3 &= 0xDF;
  }
  else {
    self->reg.st3 |= 0x20;
  }
  return(rdy);
}

/* Update the ST3 register based on a drive's status */
static void fdc_get_st3(FDC_765 *self)
{
	FDD_765 *fd = self->fdc_drive[self->unit_id];
	fdc_byte value = 0;

	/* 0.3.4: Check this, it could be null! */
	if (fd->fd_vtable->fdv_drive_status)
	{
		value = (*fd->fd_vtable->fdv_drive_status)(fd);
	}
	value &= 0xF8;
	value |= (self->head_id ? 4 : 0);
	value |= (self->unit_id & 3);
	self->reg.st3 = value;
}

/* Convert an error from the fd_* routines to a value in the status registers */
static void fdc_xlt_error(FDC_765 *self, fd_err_t error)
{
	log_dprintf(4, "FDC: Error from drive: %d\n", error);
        switch(error)
        {
                case FD_E_NOTRDY: self->reg.st0 |= 0x48; break;
		case FD_E_SEEKFAIL:
				  self->reg.st0 |= 0x40;
				  self->reg.st2 |= 0x02; break;
                case FD_E_NOADDR: self->reg.st0 |= 0x40;
                                  self->reg.st2 |= 0x01; break;
                case FD_E_NODATA: self->reg.st0 |= 0x40;
                                  self->reg.st1 |= 0x04; break;
		case FD_E_DATAERR:
				  self->reg.st1 |= 0x20;
				  self->reg.st2 |= 0x20; break;
                case FD_E_NOSECTOR:
                                  self->reg.st0 |= 0x40;
                                  self->reg.st1 |= 0x82; break;
                case FD_E_READONLY: 
				  self->reg.st0 |= 0x40;
				  self->reg.st1 |= 0x02; break;
        }

}

/* Fill out 7 result bytes
 * XXX Bytes 2,3,4,5 should change the way the real FDC does it. */
static void fdc_results_7(FDC_765 *self)
{
	self->res.buf[0] = self->reg.st0;	/* 3 status registers */
        self->res.buf[1] = self->reg.st1;
        self->res.buf[2] = self->reg.st2;
        self->res.buf[3] = self->cmd.buf[2];	/* C, H, R, N */
        self->res.buf[4] = self->cmd.buf[3];
        self->res.buf[5] = self->cmd.buf[4];
        self->res.buf[6] = self->cmd.buf[5];
	self->reg.msr = 0xD0;	/* Ready to return results */
}


/* End of result phase. Switch FDC back to idle */
static void fdc_end_result_phase(FDC_765 *self)
{
	self->reg.msr = 0x80;
	if (self->isr.state < 3) self->isr.state = 0;
	self->reg.cmd = -1;
}

/* Generate a 1-byte result phase containing ST0 */
static void fdc_error(FDC_765 *self)
{
	self->reg.st0 = (self->reg.st0 & 0x3F) | 0x80;
	self->reg.msr = 0xD0;	/* Result phase */
	self->res.len = 1;
	self->res.pos = 0;
	self->res.buf[0] = self->reg.st0;
}


/* Interrupt: Start of result phase */
static void fdc_result_interrupt(FDC_765 *self)
{
	self->isr.count = SHORT_TIMEOUT;
        self->isr.state  = 1;	/* Result-phase interrupt */
}


/* Interrupt: Start of execution phase */
static void fdc_exec_interrupt(FDC_765 *self)
{
        self->isr.count = SHORT_TIMEOUT;
        self->isr.state  = 2;    /* Execution-phase interrupt */
}

/* Compare two bytes - for the SCAN commands */
static void fdc_scan_byte(FDC_765 *self, fdc_byte fdcbyte, fdc_byte cpubyte)
{
        int cmd = self->cmd.buf[0] & 0x1F;
        if ((self->reg.st2 & 0x0C) != 8) return;	/* Mismatched already */
        
        if ((fdcbyte == cpubyte) || (fdcbyte == 0xFF) || (cpubyte == 0xFF)) 
		return;	/* Bytes match */

        /* They differ */
        if (cmd == 17) /* equal */
        {
	    self->reg.st2 = (self->reg.st2 & 0xF3) | 4; /* != */
	}
        if (cmd == 25) /* low or equal */
        {
            if (fdcbyte < cpubyte) self->reg.st2 = (self->reg.st2 & 0xF3);
            if (fdcbyte > cpubyte) self->reg.st2 = (self->reg.st2 & 0xF3) | 4;
        }
        if (cmd == 30) /* high or equal */
        {
            if (fdcbyte < cpubyte) self->reg.st2 = (self->reg.st2 & 0xF3) | 4;
	    if (fdcbyte > cpubyte) self->reg.st2 = (self->reg.st2 & 0xF3);
        }
}



/* Get the drive & head from the FDC command bytes */
static void fdc_get_drive(FDC_765 *self)
{
	/* Set current drive & head in FDC struct */
	self->unit_id =  self->cmd.buf[1] & 3;
	self->head_id = (self->cmd.buf[1] & 4) >> 2;

	/* Set current drive & head in FDC status regs */
	self->reg.st0 &= 0xF8;
	self->reg.st3 &= 0xF8;

	self->reg.st0 |= (self->cmd.buf[1] & 7);
        self->reg.st3 |= (self->cmd.buf[1] & 7);
}


/* End of Execution Phase in a write command. Write data. */
static void fdc_write_end(FDC_765 *self)
{
        FDD_765 *fd = self->fdc_drive[self->unit_id];
	int len = (128 << self->cmd.buf[5]);
	fd_err_t rv;

	if (self->cmd.buf[8] < 255) len = self->cmd.buf[8];

	rv = fdd_write_sector(fd, self->cmd.buf[2], /* xcyl */
				 self->cmd.buf[3], /* xhd  */
				 self->head_id,    /* head */
				 self->cmd.buf[4], /* sec */
				 self->exe.buf, len,
				 self->fdc_write_deleted, 
				 self->cmd.buf[0] & 0x20,
				 self->cmd.buf[0] & 0x40,
				 self->cmd.buf[0] & 0x80);

	fdc_xlt_error(self, rv);

        fdc_results_7(self);
        fdc_result_interrupt(self);
}




/* End of Execution Phase in a format command. Format the track.   */
static void fdc_format_end(FDC_765 *self)
{
        FDD_765 *fd = self->fdc_drive[self->unit_id];
	fd_err_t rv;


	rv = fdd_format_track(fd, self->head_id, /* head */
			     self->cmd.buf[3],  /* Sectors/track */
			     self->exe.buf,    /* Track template */
			     self->cmd.buf[5]); /* Filler byte */

	fdc_xlt_error(self, rv);

        fdc_results_7(self);
        fdc_result_interrupt(self);
}



/* Called when execution phase finishes */
static void fdc_end_execution_phase(FDC_765 *self)
{
        fdc_byte cmd = self->cmd.buf[0] & 0x1F;

	self->reg.msr = 0xD0;	/* Ready to return results */

	self->res.pos = 0;

        switch(cmd)
        {
                case 17:                          /* SCAN */
                case 25:
                case 30:  fdc_results_7(self);    /* fall through */
		case 12:
                case 6:   self->res.len = 7; 
			/* Do an fdc_result_interrupt, the command has 
			 * finished. Would normally happen on buffer
		 	 * exhaustion, but if you set the terminal count
			 * then the buffer doesn't get exhausted */
			  fdc_result_interrupt(self);	/* Results ready */
			  break;  			/* READ */

		case 9:
                case 5:   fdc_write_end(self);
                          self->res.len = 7; break;  /* WRITE */

                case 13:  fdc_format_end(self);
                          self->res.len = 7; break;  /* FORMAT */
        }
}


/****************************************************************
 *                    FDC COMMAND HANDLERS                      *
 ****************************************************************/


/* READ TRACK */

static void fdc_read_track(FDC_765 *self)
{
	int err;
	FDD_765 *fd;

	self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;
	self->fdc_lastidread = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->unit_id];

	self->exe.len = MAX_SECTOR_LEN;

        if (!fdc_ready(self, fd)) err = FD_E_NOTRDY;
	else err = fdd_read_track(fd, 
		self->cmd.buf[2], self->cmd.buf[3],
		self->head_id,
		self->exe.buf, 
		&self->exe.len);

	if (err) fdc_xlt_error(self, err);

	fdc_results_7(self);
	if (err && err != FD_E_DATAERR)
	{
		fdc_end_execution_phase(self);
		fdc_result_interrupt(self);
		return;
	}

        fdc_exec_interrupt(self);
	self->reg.msr = 0xF0;	/* Ready to transfer data */
	self->exe.pos = 0;
}


/* SPECIFY */
static void fdc_specify(FDC_765 *self)
{
  self->reg.srt = (self->cmd.buf[0] & 0xf0) >> 4;
  self->reg.hlt = (self->cmd.buf[1] & 0xfe) >> 1;
  self->reg.hut = (self->cmd.buf[0] & 0x0f) >> 0;
  self->reg.ndm = (self->cmd.buf[1] & 0x01) >> 0;
  fdc_end_result_phase(self);
}

/* SENSE DRIVE STATUS */
static void fdc_sense_drive(FDC_765 *self)
{
	fdc_get_drive(self);
	fdc_get_st3(self);
	self->res.buf[0] = self->reg.st3;
	self->res.len    = 1;
	fdc_end_execution_phase(self);
}


/* READ DATA
 * READ DELETED DATA 
 */

static void fdc_read(FDC_765 *self, int deleted)
{
	int err;
	FDD_765 *fd;

	self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;
	self->fdc_lastidread = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->unit_id];
        self->exe.len = (128 << self->cmd.buf[5]);
        if (self->cmd.buf[8] < 255) 
		self->exe.len = self->cmd.buf[8];

	memset(self->exe.buf, 0, self->exe.len);

        if (!fdc_ready(self, fd)) err = FD_E_NOTRDY;
	else err = fdd_read_sector(fd, 
		self->cmd.buf[2], self->cmd.buf[3],
		self->head_id,
		self->cmd.buf[4], self->exe.buf, 
		self->exe.len, &deleted,
		self->cmd.buf[0] & 0x20,
		self->cmd.buf[0] & 0x40,
		self->cmd.buf[0] & 0x80);

	if (err) fdc_xlt_error(self, err);
	if (deleted) self->reg.st2 |= 0x40;

	fdc_results_7(self);
	if (err && err != FD_E_DATAERR)
	{
		fdc_end_execution_phase(self);
		fdc_result_interrupt(self);
		return;
	}

        fdc_exec_interrupt(self);
	self->reg.msr = 0xF0;	/* Ready to transfer data */
	self->exe.pos = 0;
}

/* WRITE DATA
 * WRITE DELETED DATA 
 */

static void fdc_write(FDC_765 *self, int deleted)
{
        int err = FD_E_OK;
	FDD_765 *fd;

        self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;
        self->fdc_lastidread = 0;
	self->fdc_write_deleted = deleted;

        fdc_get_drive(self);
	fd = self->fdc_drive[self->unit_id];

        self->exe.len = (128 << self->cmd.buf[5]);
        if (self->cmd.buf[8] < 255)
                self->exe.len = self->cmd.buf[8];

        memset(self->exe.buf, 0, self->exe.len);

        if (!fdc_ready(self, fd))     err = FD_E_NOTRDY;
	else if (fd && fd->fd_readonly) err = FD_E_READONLY;

	if (err) 
	{
		fdc_xlt_error(self, err);
                self->reg.msr = 0xD0;      /* Ready to return results */
                self->res.pos = 0;
		fdc_results_7(self);
		self->res.pos = 0;
		self->res.len = 7;
		fdc_result_interrupt(self);
	}
	else
	{
		fdc_exec_interrupt(self);
		self->reg.msr = 0xB0;	/* Ready to receive data */
		self->exe.pos = 0;
	}
}

/* RECALIBRATE */
static void fdc_recalibrate(FDC_765 *self)
{
	FDD_765 *fd;

	self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;

	fdc_get_drive(self);
	self->fdc_lastidread = 0;

	fd = self->fdc_drive[self->unit_id];

	fdc_end_result_phase(self);

	self->isr.count = SHORT_TIMEOUT;
	self->isr.state  = 4;		/* Interrupt: End of seek */
	self->reg.st2 &= 0xFD;
	self->reg.st0 |= 0x20;

	/* Seek the drive to track 0 */
        if (!fdc_ready(self, fd))
        {
                self->reg.st0 |= 0x48;          /* Drive not ready */
        }
        else if ( fdd_seek_cylinder(fd, 0) )
        {
                /* Seek failed */
                self->reg.st2 |= 2;
                self->reg.st0 |= 0x40;
        }
}

/* SENSE INTERRUPT STATUS */
static void fdc_sense_int(FDC_765 *self)
{
        if (self->isr.state > 2) 
		/* FDC interrupted, and is ready to return status */
        {
		fdc_byte cyl = 0;
		if (self->fdc_drive[self->unit_id]) 
			cyl = self->fdc_drive[self->unit_id]->fd_cylinder;

		self->res.buf[0] = self->reg.st0;
		self->res.buf[1] = cyl;
		self->res.len = 2;
		log_dprintf(7, "SENSE INTERRUPT STATUS: Return %02x %02x\n",
					self->reg.st0, cyl);
        }
        else    /* FDC did not interrupt, error */
        {
                self->reg.st0 = 0x80;
		self->res.buf[0] = self->reg.st0;
                self->res.len = 1;
		log_dprintf(7, "SENSE INTERRUPT STATUS: Return 0x80\n");
        }
	fdc_end_execution_phase(self);

	/* Drop the interrupt line */
        self->isr.count = 0;
        self->isr.state = 0;
        if (self->fdc_isr)
        {
		(*self->fdc_isr)(self, 0);
        }

}


/* READ SECTOR ID */
static void fdc_read_id(FDC_765 *self)
{
        FDD_765 *fd;
	int ret;

	self->res.len = 7;
	self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->unit_id];

        if (!fdc_ready(self, fd)) 
        {
                self->reg.st0 |= 0x48;          /* Drive not ready */
        }
	else
	{
		ret=(*fd->fd_vtable->fdv_read_id)(fd, self->head_id,
				self->fdc_lastidread++, self->cmd.buf + 2);

		if (ret == FD_E_SEEKFAIL)
		{
			self->reg.st1 |= 1;
			self->reg.st0 |= 0x40;
		}
		if (ret == FD_E_NOADDR) 
		{
			self->reg.st2 |= 1;
			self->reg.st0 |= 0x40;
		}
	}
	fdc_results_7(self);
	fdc_result_interrupt(self);
	fdc_end_execution_phase(self);
}



/* FORMAT TRACK */

static void fdc_format(FDC_765 *self)
{
        int err = FD_E_OK;
	FDD_765 *fd;

        self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;
        self->fdc_lastidread = 0;

        fdc_get_drive(self);
        fd = self->fdc_drive[self->unit_id];

        memset(self->exe.buf, 0, MAX_SECTOR_LEN);

        if (!fdc_ready(self, fd))     err = FD_E_NOTRDY;
	else if (fd && fd->fd_readonly) err = FD_E_READONLY;

	if (err) 
	{
		fdc_xlt_error(self, err);
       		self->reg.msr = 0xD0;      /* Ready to return results */
	        self->res.pos = 0;
		fdc_results_7(self);
		fdc_result_interrupt(self);
	}
	else
	{
		fdc_exec_interrupt(self);
		self->reg.msr = 0xB0;	/* Ready to receive data */
		self->exe.pos = 0;
		self->exe.len = 4 * self->cmd.buf[3];
	}
}

/* SEEK */
static void fdc_seek(FDC_765 *self)
{
	int cylinder = self->cmd.buf[2];
	FDD_765 *fd;

	self->reg.st0 = self->reg.st1 = self->reg.st1 = 0;
	fdc_get_drive(self);
	self->fdc_lastidread = 0;

	fdc_end_result_phase(self);
	self->isr.count = SHORT_TIMEOUT;
	self->isr.state  = 4;	/* End of seek */
	self->reg.st2 &= 0xFD;
	self->reg.st0 |= 0x20;

	fd = self->fdc_drive[self->unit_id];

        if (!fdc_ready(self, fd))
        {
                self->reg.st0 |= 0x48;          /* Drive not ready */
        }
	else if ( fdd_seek_cylinder(fd, cylinder) )
	{
		/* Seek failed */
		self->reg.st2 |= 2;
		self->reg.st0 |= 0x40;
	}
}

/* SCAN EQUAL
 * SCAN LOW OR EQUAL
 * SCAN HIGH OR EQUAL
 */

static void fdc_scan(FDC_765 *self)
{
        int err;

	/* Load the sector we were working on */

        self->reg.st0 = self->reg.st1 = self->reg.st2 = 0;
        self->fdc_lastidread = 0;

        fdc_get_drive(self);

        self->exe.len = (128 << self->cmd.buf[5]);
        if (self->cmd.buf[8] < 255)
                self->exe.len = self->cmd.buf[8];

        memset(self->exe.buf, 0, self->exe.len);

        err = fdd_read_sector(self->fdc_drive[self->unit_id],
                self->cmd.buf[2], self->cmd.buf[3],
                self->head_id,
                self->cmd.buf[4], self->exe.buf,
                self->exe.len, 0, 
		self->cmd.buf[0] & 0x20,
		self->cmd.buf[0] & 0x40,
		self->cmd.buf[0] & 0x80);

        if (err) fdc_xlt_error(self, err);

        fdc_results_7(self);
        if (err && err != FD_E_DATAERR)
        {
                fdc_end_execution_phase(self);
                fdc_result_interrupt(self);
                return;
        }
        fdc_exec_interrupt(self);
	self->reg.st2 |= 8;
        self->reg.msr = 0xB0;      /* Ready to transfer data */
	self->exe.pos = 0;
}


/* --- Main FDC dispatcher --- */
static void fdc_execute(FDC_765 *self)
{
/* This code to dump out FDC commands as they are received is very useful
 * in debugging
 *
 * */
	int NC;
	log_dprintf(5, "FDC: ");
	for (NC = 0; NC < bytes_in_cmd[self->cmd.buf[0] & 0x1F]; NC++)
		log_dprintf(5, "%02x ", self->cmd.buf[NC]);
	log_dprintf(5, "\n");
 /* */
	/* Check if the DOR (ugh!) is being used to force us to a 
	   different drive. */

	/* Reset "seek finished" flag */
	self->reg.st0 &= 0xBF;	 
	switch(self->cmd.buf[0] & 0x1F)
	{
		case 2: fdc_read_track(self);	break;	/* 2: READ TRACK */
		case 3: fdc_specify(self);	break;	/* 3: SPECIFY */
		case 4:	fdc_sense_drive(self);	break;	/* 4: SENSE DRV STATUS*/
		case 5:	fdc_write(self, 0);	break;	/* 5: WRITE */
		case 6: fdc_read(self, 0);	break;	/* 6: READ */
		case 7:	fdc_recalibrate(self);	break;	/* 7: RECALIBRATE */
		case 8: fdc_sense_int(self);	break;	/* 8: SENSE INT STATUS*/
		case 9: fdc_write(self, 1);	break;	/* 9: WRITE DELETED */
		case 10:fdc_read_id(self);	break;	/*10: READ ID */
		case 12:fdc_read(self, 1);	break;	/*12: READ DELETED */
		case 13:fdc_format(self);	break;	/*13: FORMAT TRACK */
		case 15:fdc_seek(self);		break;	/*15: SEEK */
		case 17:				/*17: SCAN EQUAL */
		case 25:				/*25: SCAN LOW/EQUAL */
		case 30:fdc_scan(self);		break;	/*30: SCAN HIGH/EQUAL*/
		default:
			log_dprintf(2, "Unknown FDC command %d\n", 
					self->cmd.buf[0] & 0x1F);
			fdc_error(self);	break;
	}

}

/* Make the FDC drop its "interrupt" line */
static void fdc_clear_pending_interrupt(FDC_765 *self)
{
        if (self->isr.state > 0 && self->isr.state < 4)
        {
                self->isr.count = 0;
                self->isr.state = 0;
                if (self->fdc_isr)
                {
                        (*self->fdc_isr)(self, 0);
                }
        }
}


/* Write a byte to the FDC's "data" register */
void fdc_wr_data(FDC_765 *self, fdc_byte value)
{
	fdc_byte curcmd;

	fdc_clear_pending_interrupt(self);
	if (self->reg.msr & 0x20)	/* In execution phase */
	{
		curcmd = self->cmd.buf[0] & 0x1F;
		switch(curcmd)
		{
			case 17:
			case 25:	/* SCAN commands */
			case 30:
				fdc_scan_byte(self,
					self->exe.buf[self->exe.pos],
					value);
				break;
			default:	/* WRITE commands */
				self->exe.buf[self->exe.pos] = value;
				break;
		}
		++self->exe.pos;
		--self->exe.len;
                /* If at end of exec-phase, switch to result phase */
                if (!self->exe.len)
                {
                	fdc_end_execution_phase(self);
                        fdc_result_interrupt(self);
		}
                /* Interrupt SHORT_TIMEOUT cycles from now to say that the
                 * next byte is ready */
/* [JCE 8-3-2002] Changed > to >= for xjoyce 2.1.0 */
                if (self->isr.state >= 0 &&
                    self->isr.state < 3)
                {
                	self->isr.count = SHORT_TIMEOUT;
		}
		return;
	}
	if (self->reg.cmd < 0)	/* FDC is idle */
	{
		self->reg.cmd  = value;	/* Entering command phase */
		self->cmd.pos = 0;
		self->cmd.buf[0] = value;
		self->cmd.len = bytes_in_cmd[value & 0x1F] - 1;

		if (self->cmd.len     == 0) fdc_execute(self);
		else if (self->cmd.len < 0)	fdc_error(self);
		self->reg.msr |= 0x10;	/* In a command */
		return;
	}
	/* FDC is not idle, nor in execution phase; so it must be 
	 * accepting a multibyte command */
	self->cmd.buf[++self->cmd.pos] = value;
	--self->cmd.len;
	if (!self->cmd.len) fdc_execute(self);
}


/* Read the FDC's main data register */
fdc_byte fdc_rd_data (FDC_765 *self)
{
	log_dprintf(5, "FDC: Read main data register, value = ");

	fdc_clear_pending_interrupt(self);
       	if (self->reg.msr & 0x80) /* Ready to output data */
	{
		fdc_byte v;
		if (self->reg.msr & 0x20) /* In exec phase */
		{
			/* Output an exec-phase byte */
			v = self->exe.buf[self->exe.pos++];
			--self->exe.len;
			/* If at end of exec-phase, switch to result phase */
			if (!self->exe.len)
			{
				fdc_end_execution_phase(self);
				fdc_result_interrupt(self);
			}
			/* Interrupt SHORT_TIMEOUT cycles from now to say that
			 * the next byte is ready */
/* [JCE 8-3-2002] Changed > to >= for xjoyce 2.1.0 */
			if (self->isr.state >= 0 &&
		            self->isr.state < 3) 
			{
				self->isr.count = SHORT_TIMEOUT;
			}
			log_dprintf(7, "isr.state=%d\n", self->isr.state);
			log_dprintf(5, "%c:%02x\n", self->isr.count ? 'E' : 'e', v);
			return v;
		}
		/* Not in execution phase. So we must be in the result phase */
		v = self->res.buf[self->res.pos++];
		--self->res.len;
		if (self->res.len == 0) fdc_end_result_phase(self);
		log_dprintf(5, "R:%02x  (%d remaining)\n", v, self->res.len);
		return v;
	}
	/* FDC is not ready to return data! */
	log_dprintf(5, "N:%02x\n", self->reg.msr | (1 << self->unit_id));
	return self->reg.msr | (1 << self->unit_id);
}


/* Read the FDC's main control register */
fdc_byte fdc_rd_stat (FDC_765 *self)
{
	log_dprintf(5, "FDC: Read main status: %02x\n", self->reg.msr);
	return self->reg.msr;
}

/* Start or stop drive motors */
void fdc_set_motor(FDC_765 *self, fdc_byte state)
{
	int oldmotor[4], newmotor[4];
	int n;

	log_dprintf(3, "FDC: Setting motor states to %d %d %d %d\n",
			(state & 1) >> 0, (state & 2) >> 1,
			(state & 4) >> 2, (state & 8) >> 3);
	/* Save the old motor states */
	for (n = 0; n < 4; n++) 
	    if (self->fdc_drive[n]) 
		 oldmotor[n] = self->fdc_drive[n]->fd_motor;
	    else oldmotor[n] = 0;

	/* Now start/stop the motors as appropriate. Note that these are
	 * the real drives  */
	if (self->fdc_drive[0]) self->fdc_drive[0]->fd_motor = ((state & 0x01) ? 1: 0);
        if (self->fdc_drive[1]) self->fdc_drive[1]->fd_motor = ((state & 0x02) ? 1: 0);
        if (self->fdc_drive[2]) self->fdc_drive[2]->fd_motor = ((state & 0x04) ? 1: 0);
        if (self->fdc_drive[3]) self->fdc_drive[3]->fd_motor = ((state & 0x08) ? 1: 0);

	/* Now get the new motor states */
        for (n = 0; n < 4; n++) 
            if (self->fdc_drive[n]) 
                 newmotor[n] = self->fdc_drive[n]->fd_motor;
            else newmotor[n] = 0;

	/* If motor of active drive hasn't changed, return */
	if (newmotor[self->unit_id] == oldmotor[self->unit_id]) return;

	n = newmotor[self->unit_id];
	/* The status of the active drive is changing. Wait for a while, then
	 * interrupt */

	log_dprintf(5, "FDC: queued interrupt for drive motor change.\n");
	self->isr.count = LONGER_TIMEOUT;

	if (n) self->reg.st0 &= 0xF7;	/* Motor -> on ; drive is ready */
	else   self->reg.st0 |= 8;	/* Motor -> off; drive not ready*/
	fdc_get_st3(self);	/* Recalculate ST3 */

	/* FDC is doing something and the motor has stopped! */
	if ((self->reg.msr & 0xF0) != 0x80 && (n == 0))
	{
		log_dprintf(5, "FDC: Motor stopped during command.\n");
		self->reg.st0 |= 0xC0;
		fdc_end_execution_phase(self);
	}
}

void fdc_tick(FDC_765 *fdc)
{
  if(fdc->isr.count > 0) {
    if((--fdc->isr.count == 0) && (fdc->fdc_isr != NULL)) {
      (*fdc->fdc_isr)(fdc, 1);
    } 
  }
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Moved here from libdsk.h; there's no need for it to be public */
typedef struct dsk_driver
{
        struct drv_class     *dr_class;
        struct compress_data *dr_compress;      /* Compressed? */
	struct remote_data   *dr_remote;	/* Remote, if any */
	char *dr_comment;	/* Comment, if any */
/*        int dr_forcehead;     Force drive to use head 0 or head 1
 *        			Moved to Linux floppy driver; it's the only one
 *        			that supports it. */
	int dr_dirty;		/* Has this device been written to? 
				 * Set to 1 by writes and formats */
	unsigned dr_retry_count; /* Number of times to retry if error */	
} DSK_DRIVER;



/* Functions a driver must provide. If you are implementing a driver, 
 * create instances of these functions and have pointers to them (see
 * eg. drvposix.c) */

typedef struct drv_class
{
	/* Variables */
	size_t	dc_selfsize;	/* Size of the DSK_DRIVER subclass to be
				 * malloced and zeroed before we enter 
				 * dc_open */
	char   *dc_drvname;	/* Short driver name, as used by eg. the 
				 * -itype and -otype arguments in DSKTRANS. */
	char   *dc_description;	/* Human-readable description of driver */

	/* Functions: */

	/* Open an existing disc image. Return DSK_ERR_OK if successful.
	 * Return DSK_ERR_NOTME if the image cannot be handled by this driver.
	 * Return other errors only if you want to stop other drivers having 
	 * a go. */ 
	dsk_err_t (*dc_open )(DSK_DRIVER *self, const char *filename);
	/* Create a new disc image. Return DSK_ERR_OK if successful, any
	 * other errors if failed. For floppy drives, does the same as
	 * dc_open */
	dsk_err_t (*dc_creat)(DSK_DRIVER *self, const char *filename);
	/* Close disc image / drive */
	dsk_err_t (*dc_close)(DSK_DRIVER *self);

	/* Read a physical sector */
	dsk_err_t (*dc_read)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_psect_t sector);
	/* Write a physical sector */
	dsk_err_t (*dc_write)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      const void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_psect_t sector);

	/* Format a track */
	dsk_err_t (*dc_format)(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
	/* Get geometry. Only provide this function if your disc image
	 * does not allow a program to set its own disc geometry; if it
	 * /does/, then use the DSK_GEOMETRY argument passed to the other
	 * functions (drvlinux.c is a good example here). 
	 * The two drivers which do this are the Windows NT one, because
	 * Windows NT uses an arbitrary geometry which can't be overridden;
	 * and the MYZ80 one, which has a single fixed geometry.
	 * Return DSK_ERR_NOTME to fall back to the default geometry
	 * probe; other values indicate immediate success or failure. */
	dsk_err_t (*dc_getgeom)(DSK_DRIVER *self, DSK_GEOMETRY *geom);

	/* Read the ID of a random sector on a certain track/head, and 
	 * put it in "result". This function is primarily used to test for
	 * discs in CPC format (which have oddly-numbered physical sectors); 
	 * drivers such as NT and POSIX can't implement it, and so don't. */
	dsk_err_t (*dc_secid)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
	/* Seek to a track */
	dsk_err_t (*dc_xseek)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head);
	/* Get drive status */
	dsk_err_t (*dc_status)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_phead_t head, unsigned char *result);
	/* Read a sector whose ID doesn't match its location on disc */
	dsk_err_t (*dc_xread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, 
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int *deleted);
	/* Write a sector whose ID doesn't match its location on disc */
	dsk_err_t (*dc_xwrite)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      const void *buf,
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int deleted);
	/* Read a track (8272 READ TRACK command) */
	dsk_err_t (*dc_tread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head);
	/* Read a track: Version where the sector ID doesn't necessarily
	 * match */
	dsk_err_t (*dc_xtread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_pcyl_t cyl_expected,
			      dsk_phead_t head_expected);

	/* List driver-specific options */
	dsk_err_t (*dc_option_enum)(DSK_DRIVER *self, int idx, char **optname);

	/* Set a driver-specific option */
	dsk_err_t (*dc_option_set)(DSK_DRIVER *self, const char *optname, int value);
	/* Get a driver-specific option */
	dsk_err_t (*dc_option_get)(DSK_DRIVER *self, const char *optname, int *value);
	/* Read headers for an entire track at once */
	dsk_err_t (*dc_trackids)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                dsk_psect_t *count, DSK_FORMAT **result);

	/* Read raw track, including sector headers */
	dsk_err_t (*dc_rtread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			void *buf, dsk_pcyl_t cylinder,  dsk_phead_t head, 
			int reserved, size_t *bufsize);
} DRV_CLASS;

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifndef DISABLE_FLOPPY
# ifdef HAVE_LINUX_FD_H
#  include "linux/fd.h"
#  ifdef HAVE_LINUX_FDREG_H
#   define LINUXFLOPPY 
#   include "linux/fdreg.h"
#  endif
# endif

# ifdef HAVE_WINDOWS_H 
#  include <windows.h>
# endif

#ifdef HAVE_WINIOCTL_H
#  define WIN32FLOPPY 
#  define NTWDMFLOPPY 
#  include <winioctl.h>
# endif
#endif

/* See if we have any floppy drivers that take parameters of the form A: */
#ifdef WIN32FLOPPY 
# define ANYFLOPPY
#endif
#ifdef WIN16FLOPPY 
# define ANYFLOPPY
#endif
#ifdef DOS32FLOPPY 
# define ANYFLOPPY
#endif
#ifdef DOS16FLOPPY 
# define ANYFLOPPY
#endif

#if defined(HAVE_UNISTD_H)
#define HAVE_RCPMFS 1
#elif defined(_WIN32)
#define HAVE_RCPMFS 1
#elif defined(HAVE_DIR_H)
#define HAVE_RCPMFS 1
#endif


/* Initialise custom formats */
dsk_err_t dg_custom_init(void);
const char *dg_homedir(void);
const char *dg_sharedir(void);
dsk_err_t dg_parseline(char *linebuf, DSK_GEOMETRY *dg, char *description);
dsk_err_t dg_parse(FILE *fp, DSK_GEOMETRY *dg, char *description);
dsk_err_t dg_store(FILE *fp, DSK_GEOMETRY *dg, char *description);
/* The default geometry probe; driver geometry probes can call it */
dsk_err_t dsk_defgetgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);

#ifdef AUTOSHARE
# define Q2(x) Q1(x)
# define Q1(x) #x
# define SHAREDIR Q2(AUTOSHARE)
#else
# define SHAREDIR NULL
#endif

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Declarations for the CPCEMU driver */

typedef struct
{
        DSK_DRIVER cpc_super;
        FILE *cpc_fp;
	int   cpc_readonly;
	dsk_psect_t   cpc_sector;	/* Last sector used for DD READ ID */
	unsigned char cpc_dskhead[256];	/* DSK header */
	unsigned char cpc_trkhead[256];	/* Track header */
	unsigned char cpc_status[4];	/* Dummy FDC status regs ST0-ST3: Read */
	int	      cpc_statusw[4];	/* Dummy FDC status regs ST0-ST3: Write */
} CPCEMU_DSK_DRIVER;

/* v0.9.0: Use subclassing to create separate drivers for normal and 
 * extended .DSK images. This way we can create extended images by 
 * using "-type edsk" or similar */
dsk_err_t cpcext_open(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcext_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_open(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t cpcemu_close(DSK_DRIVER *self);
dsk_err_t cpcemu_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t cpcemu_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t cpcemu_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t cpcemu_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t cpcemu_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t cpcemu_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t cpcemu_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t cpcemu_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result);
dsk_err_t cpcemu_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result);

dsk_err_t cpcemu_option_enum(DSK_DRIVER *self, int idx, char **optname);

dsk_err_t cpcemu_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t cpcemu_option_get(DSK_DRIVER *self, const char *optname, int *value);

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2005  John Elliott <jce@seasip.demon.co.uk>       *
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
		case 1: if (!geom->dg_fm) return DSK_ERR_NOADDR;
			break;
		case 2: if (geom->dg_fm) return DSK_ERR_NOADDR;
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
		if (self->cpc_sector >= 0 && (int)self->cpc_sector < maxsec)
		{
/* Calculate the offset of the current sector */
			for (n = 0; n < (int)self->cpc_sector; n++)
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
		if (self->cpc_sector >= 0 && (int)self->cpc_sector < maxsec)
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
				head, sector, geom->dg_secsize, 0);
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
	return cpcemu_xwrite(self, geom, buf, cylinder, head, cylinder,head,
				sector, geom->dg_secsize, 0);
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
	cpc_self->cpc_trkhead[0x13] = (geom->dg_fm) ? 1 : 2;
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

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2  John Elliott <jce@seasip.demon.co.uk>          *
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

/* This file lists all possible types of floppy driver in libdsk. Order does
 * not matter in this file. */

extern DRV_CLASS dc_cpcemu;	/* CPCEMU DSK driver */
extern DRV_CLASS dc_cpcext;	/* CPCEMU DSK driver: create in ext. format */
extern DRV_CLASS dc_dqk;	/* Compressed CPCEMU driver */
extern DRV_CLASS dc_posix;	/* POSIX driver */
extern DRV_CLASS dc_nwasp;	/* NanoWasp driver */
extern DRV_CLASS dc_myz80;	/* MYZ80 driver */
extern DRV_CLASS dc_cfi;	/* CFI driver */
extern DRV_CLASS dc_adisk;	/* APRIDISK driver */
extern DRV_CLASS dc_qm;		/* CopyQM driver */
extern DRV_CLASS dc_tele;	/* Teledisk driver */
extern DRV_CLASS dc_logical;	/* Raw, in logical sector order */
extern DRV_CLASS dc_rcpmfs;	/* Reverse-CP/MFS driver */
extern DRV_CLASS dc_remote;	/* All remote drivers */
#ifdef LINUXFLOPPY
extern DRV_CLASS dc_linux;	/* Linux driver */
#endif
#ifdef NTWDMFLOPPY
extern DRV_CLASS dc_ntwdm;	/* NT WDM driver */
#endif
#ifdef WIN32FLOPPY
extern DRV_CLASS dc_win32;	/* Win32 driver */
#endif
#ifdef WIN16FLOPPY
extern DRV_CLASS dc_win16;	/* Win16 driver */
#endif
#ifdef DOS32FLOPPY
extern DRV_CLASS dc_dos32;	/* DOS32 driver */
extern DRV_CLASS dc_dosint25;	/* DOS (INT 25h) driver */
#endif
#ifdef DOS16FLOPPY
extern DRV_CLASS dc_dos16;	/* DOS16 driver */
extern DRV_CLASS dc_dosint25;	/* DOS (INT 25h) driver */
#endif

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* LibDsk generalised compression support */

/* LibDsk compression works by creating an uncompressed copy of the 
 * compressed file, and passing the name of that file through to the 
 * driver.
 *
 * In fact, this generalised compress/decompress might come in useful in
 * other ways. I'll try to minimise dependencies on the rest of LibDsk.
 */

typedef struct compress_data
{
	char *cd_cfilename;	/* Filename of compressed file */
	char *cd_ufilename;	/* Filename of temporary uncompressed file */
	int cd_readonly;	/* Compressed file is read-only */
	struct compress_class *cd_class;	
} COMPRESS_DATA;


typedef struct compress_class
{
	size_t cc_selfsize;	/* Size of data associated with this class */
/* Name and description are not used for anything yet, but are present by 
 * analogy with drv_class */
	char *cc_name;		/* Compression scheme short name */
	char *cc_description;	/* Compression scheme description */

/* Functions: */

	/* Open and decompress a compressed file. Returns DSK_ERR_OK if 
	 * the file could be decompressed; DSK_ERR_NOTME if the file is
	 * not compressed; DSK_ERR_COMPRESS if the compressed file is
	 * damaged. The filenames to use are stored in "self". */
	dsk_err_t (*cc_open)(COMPRESS_DATA *self);

	/* Record that a compressed file will be created. Usually a no-op.  */
	dsk_err_t (*cc_creat)(COMPRESS_DATA *self);

	/* Close and compress the file. Returns DSK_ERR_OK if the file 
	 * could be closed, other errors if not. */
	dsk_err_t (*cc_commit)(COMPRESS_DATA *self);

	/* Close file, but don't bother re-compressing it, because 
	 * it wasn't changed. Usually a no-op. */
	dsk_err_t (*cc_abort)(COMPRESS_DATA *self);
} COMPRESS_CLASS;


/* See if a file is compressed. If the file is not compressed, (*cd) will
 * be set to NULL and DSK_ERR_NOTME will be returned. If the file *is*
 * compressed, (*cd) will be set to a new COMPRESS_DATA object.  */
dsk_err_t comp_open(COMPRESS_DATA **cd, const char *filename, const char *type);

/* Create a compressed file. If type is NULL (uncompressed) this returns 
 * dsk_err_ok with *cd = NULL */
dsk_err_t comp_creat(COMPRESS_DATA **cd, const char *filename, const char *type);

/* Close compressed file */
dsk_err_t comp_commit(COMPRESS_DATA **cd);
/* Abandon compressed file */
dsk_err_t comp_abort(COMPRESS_DATA **cd);

/* Open the compressed file to read (cd->cd_cfilename) */
dsk_err_t comp_fopen(COMPRESS_DATA *self, FILE **pfp);

/* Create a temporary file to decompress into. cd->cd_ufilename will be set 
 * to its name. */
dsk_err_t comp_mktemp(COMPRESS_DATA *cd, FILE **pfp);


dsk_err_t comp_type_enum(int index, char **compname);
const char *comp_name(COMPRESS_DATA *self);
const char *comp_desc(COMPRESS_DATA *self);

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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
/* strerror() for all possible errors */

#include <string.h>
#include <errno.h>

LDPUBLIC32 char * LDPUBLIC16 dsk_strerror(dsk_err_t err)
{
	switch(err)
	{
		case DSK_ERR_OK:	return "No error.";
		case DSK_ERR_BADPTR:	return "Bad pointer passed to libdsk.";
		case DSK_ERR_DIVZERO:	return "Division by zero.";
		case DSK_ERR_BADPARM:	return "Bad parameter";
		case DSK_ERR_NODRVR:	return "Requested driver not found";
		case DSK_ERR_NOTME:	return "Disc rejected by driver.";
		case DSK_ERR_SYSERR:	return strerror(errno);
		case DSK_ERR_NOMEM:	return "Out of memory.";
		case DSK_ERR_NOTIMPL:	return "Driver does not support this function.";
		case DSK_ERR_MISMATCH:	return "Sector on disc does not match buffer.";
		case DSK_ERR_NOTRDY:	return "Drive not ready.";
		case DSK_ERR_RDONLY:	return "Disc is read-only.";
		case DSK_ERR_SEEKFAIL:	return "Seek fail.";
		case DSK_ERR_DATAERR:	return "Data error.";
		case DSK_ERR_NODATA:	return "No data.";
		case DSK_ERR_NOADDR:	return "Missing address mark.";
		case DSK_ERR_BADFMT:	return "Bad format.";
                case DSK_ERR_CHANGED:	return "Disc changed.";
		case DSK_ERR_ECHECK:	return "Equipment check.";
		case DSK_ERR_OVERRUN:	return "Overrun.";
		case DSK_ERR_ACCESS:	return "Access denied.";
                case DSK_ERR_CTRLR:	return "Controller failed.";
                case DSK_ERR_COMPRESS:	return "Compressed file is corrupt.";
		case DSK_ERR_RPC:	return "Invalid RPC packet.";
		case DSK_ERR_BADOPT:	return "Requested driver-specific feature not available.";
		case DSK_ERR_BADVAL:	return "Bad value for driver-specific feature.";
		case DSK_ERR_ABORT:	return "Operation was cancelled by user.";
		case DSK_ERR_TIMEOUT:	return "Timed out waiting for response.";
		case DSK_ERR_UNKRPC:	return "RPC server did not recognise function.";
		case DSK_ERR_BADMEDIA:	return "Disc is not suitable for this operation";
		case DSK_ERR_UNKNOWN:	return "Controller returned unknown error.";
 	}
	return "Unknown error.";
}


#ifdef TRACE_MALLOCS
#include <stdio.h>
#include <assert.h>
void *dsk_malloc(size_t size)
{
	void *ptr;

	assert(size);	
	printf("malloc(%d) ", size);
	fflush(stdout);
	ptr = malloc(size);
	printf("returns %x\n", (int)ptr);
	return ptr;
}


void  dsk_free(void *ptr)
{
	printf("free(%x)\n", (int)ptr);
	free(ptr);
}

#endif

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

static DRV_CLASS *classes[] = 
{
    &dc_cpcemu, /* Normal .DSK */
    &dc_cpcext, /* Extended .DSK */
	NULL
};




static void dr_construct(DSK_DRIVER *self, DRV_CLASS *dc)
{
	memset(self, 0, dc->dc_selfsize);
/*	self->dr_forcehead = -1; 	 */
	self->dr_class = dc;
	self->dr_dirty = 0;
	self->dr_compress = NULL;
	self->dr_retry_count = 1;
}


/* Attempt to create a DSK file with driver number <ndrv> */
static dsk_err_t dsk_icreat(DSK_DRIVER **self, const char *filename, int ndrv, COMPRESS_DATA *cd)
{
	DRV_CLASS *dc = classes[ndrv];
	dsk_err_t err;

	if (!dc) return DSK_ERR_BADPTR;
	
	(*self) = dsk_malloc(dc->dc_selfsize);
	if (!*self) return DSK_ERR_NOMEM;
	dr_construct(*self, dc);

	err = (dc->dc_creat)(*self, filename);
	if (err == DSK_ERR_OK) 
	{
		(*self)->dr_compress = cd;
		return err;
	}
	dsk_free (*self);
	*self = NULL;
	return err;
}


/* Attempt to open a DSK file with driver <ndrv> */
static dsk_err_t dsk_iopen(DSK_DRIVER **self, const char *filename, int ndrv, COMPRESS_DATA *cd)
{
	DRV_CLASS *dc = classes[ndrv];
	dsk_err_t err;

	/* If we're handling compressed data, use the temporary uncompressed file */
	if (cd) filename = cd->cd_ufilename;

	if (!dc) return DSK_ERR_BADPTR;
	
	(*self) = dsk_malloc(dc->dc_selfsize);
	if (!*self) return DSK_ERR_NOMEM;
	dr_construct(*self, dc);

	err = (dc->dc_open)(*self, filename);
	if (err == DSK_ERR_OK) 
	{
		(*self)->dr_compress = cd;
		return err;
	}
	dsk_free (*self);
	*self = NULL;
	return err;
}

/* Create a disc file, of type "type" with name "filename". */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_creat(DSK_DRIVER **self, const char *filename, const char *type,
			const char *compress)
{
	int ndrv;
	dsk_err_t e;
	COMPRESS_DATA *cd = NULL;

	if (!self || !filename || !type) return DSK_ERR_BADPTR;

	dg_custom_init();
	if (compress)
	{	
		e = comp_creat(&cd, filename, compress);
		if (e) return e;
		if (cd) filename = cd->cd_ufilename;
	}

	for (ndrv = 0; classes[ndrv]; ndrv++)
	{
		if (!strcmp(type, classes[ndrv]->dc_drvname))
		{
			e = dsk_icreat(self, filename, ndrv, cd);
			if (e != DSK_ERR_OK && cd) comp_abort(&cd);
			return e;
		}
	}
	if (cd) comp_abort(&cd);
	return DSK_ERR_NODRVR;
}



/* Close a DSK file. Frees the pointer and sets it to null. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_open(DSK_DRIVER **self, const char *filename, const char *type,
			const char *compress)
{
	int ndrv;
	dsk_err_t e;
	COMPRESS_DATA *cd;

	if (!self || !filename) return DSK_ERR_BADPTR;

	dg_custom_init();

	/* See if it's compressed */
	e = comp_open(&cd, filename, compress);
	if (e != DSK_ERR_OK && e != DSK_ERR_NOTME) return e;
	
	if (type)
	{
		for (ndrv = 0; classes[ndrv]; ndrv++)
		{
			if (!strcmp(type, classes[ndrv]->dc_drvname))
			{
				e = dsk_iopen(self, filename, ndrv, cd);
				if (e && cd) comp_abort(&cd);
				return e;
			}
		}
		if (cd) comp_abort(&cd);
		return DSK_ERR_NODRVR;
	}
	for (ndrv = 0; classes[ndrv]; ndrv++)
	{
		e = dsk_iopen(self, filename, ndrv, cd);
		if (e != DSK_ERR_NOTME) 
		{
			if (e != DSK_ERR_OK && cd) comp_abort(&cd);
			return e;
		}
	}	
	if (cd) comp_abort(&cd);
	return DSK_ERR_NOTME;
}

/* Close a DSK file. Frees the pointer and sets it to null. */

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_close(DSK_DRIVER **self)
{
	dsk_err_t e, e2;
	COMPRESS_DATA *dc;

	if (!self || (!(*self)) || (!(*self)->dr_class))    return DSK_ERR_BADPTR;

	e = ((*self)->dr_class->dc_close)(*self);

	dc = (*self)->dr_compress;
	if (dc)
	{
		if ((*self)->dr_dirty) e2 = comp_commit(&dc); 
		else		       e2 = comp_abort (&dc);

		if (!e) e = e2;
	}
	dsk_free (*self);
	*self = NULL;
	return e;
}

/* If "index" is in range, returns the n'th driver name in (*drvname).
 * Else sets (*drvname) to null. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_type_enum(int index, char **drvname)
{
	int ndrv;

	if (!drvname) return DSK_ERR_BADPTR;

	for (ndrv = 0; classes[ndrv]; ndrv++)
	{
		if (index == ndrv)
		{
			*drvname = classes[ndrv]->dc_drvname;
			return DSK_ERR_OK;
		}
	}
	*drvname = NULL;
	return DSK_ERR_NODRVR;
}

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_comp_enum(int index, char **compname)
{
	return comp_type_enum(index, compname);
}



LDPUBLIC32 unsigned char LDPUBLIC16 dsk_get_psh(size_t secsize)
{
	unsigned char psh;

	for (psh = 0; secsize > 128; secsize /= 2) psh++;
	return psh;
}


LDPUBLIC32 const char * LDPUBLIC16 dsk_drvname(DSK_DRIVER *self)
{
	if (!self || !self->dr_class || !self->dr_class->dc_drvname)
		return "(null)";
	return self->dr_class->dc_drvname;
}


LDPUBLIC32 const char * LDPUBLIC16 dsk_drvdesc(DSK_DRIVER *self)
{
	if (!self || !self->dr_class || !self->dr_class->dc_description)
		return "(null)";
	return self->dr_class->dc_description;
}



LDPUBLIC32 const char * LDPUBLIC16 dsk_compname(DSK_DRIVER *self)
{
        if (!self) return "(null)";
	if (!self->dr_compress) return NULL;
	
	return comp_name(self->dr_compress);	
}


LDPUBLIC32 const char * LDPUBLIC16 dsk_compdesc(DSK_DRIVER *self)
{
        if (!self) return "(null)";
	if (!self->dr_compress) return "Not compressed";
	
	return comp_desc(self->dr_compress);	
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Get the status (ST3) of a drive.  */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_drive_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
                                dsk_phead_t head, unsigned char *status)
{
	DRV_CLASS *dc;
	dsk_err_t err;
	unsigned char ro = 0;

	if (!self || !geom || !status || !self->dr_class) return DSK_ERR_BADPTR;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
		ro = DSK_ST3_RO;

	/* Generate a default status. If the driver doesn't provide this 
	 * function, the default status will be used. */

	*status = DSK_ST3_READY | ro;
	if (geom->dg_heads > 1) *status |= DSK_ST3_DSDRIVE;	
	if (head)               *status |= DSK_ST3_HEAD1;
	
	dc = self->dr_class;
        if (!dc->dc_status) return DSK_ERR_OK;
	err = (dc->dc_status)(self,geom,head,status);	
	
	*status |= ro;
	return err;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	/* LDTRACE(("dsk_pread (%d,%d,%d)\n", cylinder, head, sector)); */
        if (!dc->dc_read) return DSK_ERR_NOTIMPL;
	for (n = 0; n < self->dr_retry_count; n++)
	{
		e = (dc->dc_read)(self,geom,buf,cylinder,head,sector);	
/* 		LDTRACE(("  err=%d\n", e)); */
		if (!DSK_TRANSIENT_ERROR(e)) return e; 
	}
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_lsect_t sector)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_psect_t s;
        dsk_err_t e;
 
        e = dg_ls2ps(geom, sector, &c, &h, &s);
        if (e != DSK_ERR_OK) return e;
        return dsk_pread(self, geom, buf, c, h, s);
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf, 
			dsk_pcyl_t cylinder,   dsk_phead_t head, 
			dsk_pcyl_t cyl_expect, dsk_phead_t head_expect, 
			dsk_psect_t sector, size_t sector_len, int *deleted)
{
	DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n;
	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

/*	LDTRACE(("dsk_xread (%d,%d,%d) (%d,%d)\n", cylinder, head, sector,
				cyl_expect, head_expect)); */
        if (!dc->dc_xread) 
	{
		return DSK_ERR_NOTIMPL;
	}
	for (n = 0; n < self->dr_retry_count; n++)
	{
		e = (dc->dc_xread)(self,geom,buf,cylinder,head,
			cyl_expect, head_expect, sector, sector_len, deleted);	
		/* LDTRACE(("  err=%d\n", e)); */
		if (!DSK_TRANSIENT_ERROR(e)) return e;
	}
	return e;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
    DRV_CLASS *dc;
    dsk_err_t e = DSK_ERR_UNKNOWN;
    unsigned n;

    if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

    dc = self->dr_class;

    if (self && self->dr_compress && self->dr_compress->cd_readonly)
        return DSK_ERR_RDONLY;

    if (!dc->dc_write) return DSK_ERR_NOTIMPL;
    for (n = 0; n < self->dr_retry_count; n++)
    {
        e = (dc->dc_write)(self,geom,buf,cylinder,head,sector); 
        if (e == DSK_ERR_OK) self->dr_dirty = 1;
        if (!DSK_TRANSIENT_ERROR(e)) return e;
    }
    return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_lsect_t sector)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_psect_t s;
        dsk_err_t e;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;
 
        e = dg_ls2ps(geom, sector, &c, &h, &s);
        if (e != DSK_ERR_OK) return e;
        e = dsk_pwrite(self, geom, buf, c, h, s);
    if (e == DSK_ERR_OK) self->dr_dirty = 1;
    return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
            const void *buf, 
                        dsk_pcyl_t cylinder,   dsk_phead_t head,
                        dsk_pcyl_t cyl_expect, dsk_phead_t head_expect,
                        dsk_psect_t sector, size_t sector_len, int deleted)
{
        DRV_CLASS *dc;
    dsk_err_t err = DSK_ERR_UNKNOWN;
    unsigned n;

        if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;

        if (!dc->dc_xwrite) return DSK_ERR_NOTIMPL;
    for (n = 0; n < self->dr_retry_count; n++)
    {
            err = (dc->dc_xwrite)(self,geom,buf,cylinder,head, cyl_expect, 
                head_expect, sector, sector_len, deleted);
        if (err == DSK_ERR_OK) self->dr_dirty = 1;
        if (!DSK_TRANSIENT_ERROR(err)) return err;
    }
    return err;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Wrapper functions for FORMAT calls */

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pformat(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
        DRV_CLASS *dc;
	dsk_err_t e = DSK_ERR_UNKNOWN;
	unsigned n;

        if (!self || !geom || !format || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;

        if (!dc->dc_format) return DSK_ERR_NOTIMPL;
	for (n = 0; n < self->dr_retry_count; n++)
	{
	        e = (dc->dc_format)(self,geom,cylinder,head,format,filler);      
		if (!DSK_TRANSIENT_ERROR(e)) break;
	}
	if (e == DSK_ERR_OK) self->dr_dirty = 1;
	return e;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lformat(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, const DSK_FORMAT *format, 
                                unsigned char filler)
{
        dsk_err_t e;
        dsk_pcyl_t c;
        dsk_phead_t h;

        if (self && self->dr_compress && self->dr_compress->cd_readonly)
                return DSK_ERR_RDONLY;
 
        e = dg_lt2pt(geom, track, &c, &h);
        if (e != DSK_ERR_OK) return e;
        e = dsk_pformat(self, geom, c, h, format, filler);                          
	if (e == DSK_ERR_OK) self->dr_dirty = 1;
	return e;
}


static DSK_FORMAT * dsk_formauto(const DSK_GEOMETRY *dg, 
				dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned int ns;
	DSK_FORMAT *fmt = calloc(dg->dg_sectors, sizeof(DSK_FORMAT));

	if (!fmt) return NULL;
	for (ns = 0; ns < dg->dg_sectors; ns++)
	{
		fmt[ns].fmt_cylinder = cylinder;
		fmt[ns].fmt_head     = head;
		fmt[ns].fmt_sector   = dg->dg_secbase + ns;
		fmt[ns].fmt_secsize  = dg->dg_secsize;
	}
	return fmt;
}

/* Auto-format: generates the sector headers from "geom" */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_apform(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                unsigned char filler)
{
	DSK_FORMAT *fmt;
	dsk_err_t err;

	if (!geom) return DSK_ERR_BADPTR;

	fmt = dsk_formauto(geom, cylinder, head);
	if (!fmt)  return DSK_ERR_NOMEM;
	err = dsk_pformat(self,geom,cylinder,head,fmt,filler);
	free(fmt);
	return err;
}



LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_alform(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, unsigned char filler)
{
	dsk_pcyl_t cylinder; 
	dsk_phead_t head;
	DSK_FORMAT *fmt;
	dsk_err_t err;

	err = dg_lt2pt(geom, track, &cylinder, &head); 
	if (err) return err;

	fmt = dsk_formauto(geom, cylinder, head);
	if (!fmt)  return DSK_ERR_NOMEM;
	err = dsk_lformat(self,geom,track,fmt,filler);
	free(fmt);
	return err;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Seek to a track */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	DRV_CLASS *dc;
	if (!self || !geom || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (!dc->dc_xseek) return DSK_ERR_NOTIMPL;
	return (dc->dc_xseek)(self,geom,cylinder,head);	

}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_ltrack_t track)
{
        dsk_err_t e;
        dsk_pcyl_t cyl;
        dsk_phead_t head;
 
        e = dg_lt2pt(geom, track, &cyl, &head);
        if (e) return e;
        return dsk_pseek(self, geom, cyl, head);
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Disc geometry probe and related code */

/* Probe the geometry of a disc. This will use the boot sector or the
 * driver's own probe */

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
        DRV_CLASS *dc; 
	dsk_err_t e;

        if (!self || !geom || !self->dr_class) return DSK_ERR_BADPTR;

	/* Check if the driver has overridden this function. If it has,
	 * then use its geometry probe, which is probably more limited. */
	dc = self->dr_class; 
	memset(geom, 0, sizeof(*geom));

	if (dc->dc_getgeom)
	{
		e = (dc->dc_getgeom)(self, geom);
		if (e != DSK_ERR_NOTME && e != DSK_ERR_NOTIMPL) return e;	
	}	
	return dsk_defgetgeom(self, geom);
}


	
/* Probe the geometry of a disc. This will always use the boot sector. */
dsk_err_t dsk_defgetgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
        DRV_CLASS *dc; 
	DSK_FORMAT secid;
	dsk_err_t e;
	unsigned char *secbuf;
	unsigned long dsksize;
	dsk_rate_t oldrate;

        if (!self || !geom || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class; 
	memset(geom, 0, sizeof(*geom));

	/* Switch to a minimal format */
	e = dg_stdformat(geom, FMT_180K, NULL, NULL);
	if (e) return e;
	/* Allocate buffer for boot sector (512 bytes) */
	secbuf = dsk_malloc(geom->dg_secsize);
	if (!secbuf) return DSK_ERR_NOMEM;


	/* Check for CPC6128 type discs. Also probe the data rate; if we get a 
	 * missing address mark, then the data rate is wrong.
	 */ 
	e = dg_stdformat(geom, FMT_180K, NULL, NULL);
	if (e) return e;
	e = dsk_lsecid(self, geom, 0, &secid);
	/* Check for HD discs */
	if (e == DSK_ERR_NOADDR)
	{
		geom->dg_datarate = RATE_HD;
		e = dsk_lsecid(self, geom, 0, &secid);
	}
	/* Check for DD 5.25" disc in HD 5.25" drive */
	if (e == DSK_ERR_NOADDR)
	{
		geom->dg_datarate = RATE_DD;
		e = dsk_lsecid(self, geom, 0, &secid);
	}
	/* Check for BBC micro DFS discs (FM encoded) */
	if (e == DSK_ERR_NOADDR)
	{
		e = dg_stdformat(geom, FMT_BBC100, NULL, NULL);
		if (!e) e = dsk_lsecid(self, geom, 0, &secid);
	}
	if (!e)	/* We could get the sector ID */
	{
		if ((secid.fmt_sector & 0xC0) == 0x40) 	/* CPC system */
		{
			dsk_free(secbuf);
			return dg_stdformat(geom, FMT_CPCSYS, NULL, NULL);
		}
		if ((secid.fmt_sector & 0xC0) == 0xC0)	/* CPC data */
		{
			dsk_free(secbuf);
			return dg_stdformat(geom, FMT_CPCDATA, NULL, NULL);
		}
		/* [v0.6.0] Handle discs with non-512 byte sectors */
		if (secid.fmt_secsize == 256)
		{
			if (geom->dg_fm)	/* BBC Micro FM floppy */
			{
				unsigned int tot_sectors;
				e = dsk_lread(self, geom, secbuf, 1);

				tot_sectors = secbuf[7] + 256 * (secbuf[6] & 3);
			
/* If disc is FM recorded but does not have 400 or 800 sectors, fail. */	
				if (e == DSK_ERR_OK && tot_sectors != 400 && tot_sectors != 800) e = DSK_ERR_BADFMT; 

				geom->dg_cylinders = tot_sectors / (geom->dg_heads * geom->dg_sectors);	
				dsk_free(secbuf);
				return e;
			}
			else	/* MFM */
			{
				e = dg_stdformat(geom, FMT_ACORN160, NULL, NULL);
				if (!e) e = dsk_lread(self, geom, secbuf, 0);
				if (e)
				{
					dsk_free(secbuf);
					return DSK_ERR_BADFMT;
				}
				/* Acorn ADFS discs have a size in sectors at 0xFC in the
				 * first sector */
				dsksize = secbuf[0xFC] + 256 * secbuf[0xFD] +
					65536L * secbuf[0xFE];
				dsk_free(secbuf);
				if (dsksize ==  640) return dg_stdformat(geom, FMT_ACORN160, NULL, NULL);
				if (dsksize == 1280) return dg_stdformat(geom, FMT_ACORN320, NULL, NULL);
				if (dsksize == 2560) return dg_stdformat(geom, FMT_ACORN640, NULL, NULL);
				/* The DOS Plus boot floppy has 2720 here for
				 * some reason */
				if (dsksize == 2720) return dg_stdformat(geom, FMT_ACORN640, NULL, NULL);
				return DSK_ERR_BADFMT;
			}
		}
		if (secid.fmt_secsize == 1024)
		{
			/* Save the data rate, which we know to be correct */
			dsk_rate_t rate = geom->dg_datarate;

			dsk_free(secbuf);
			/* Switch to a format with 1k sectors */
			if (geom->dg_datarate == RATE_HD)
				e = dg_stdformat(geom, FMT_ACORN1600, NULL, NULL);	
			else	e = dg_stdformat(geom, FMT_ACORN800, NULL, NULL);
			if (e) return e;
			/* And restore it. */
			geom->dg_datarate = rate;
			/* Allocate buffer for boot sector (1k bytes) */
			secbuf = dsk_malloc(geom->dg_secsize);
			if (!secbuf) return DSK_ERR_NOMEM;
			e = dsk_lread(self, geom, secbuf, 0);
			if (!e)
			{
				dsksize = secbuf[0xFC] + 256 * secbuf[0xFD] +
					65536L * secbuf[0xFE];
				/* Check for 1600k-format */
				if (geom->dg_datarate == RATE_HD)
				{
				/* XXX Need a better check for Acorn 1600k */
					dsk_free(secbuf);
					return DSK_ERR_OK;
				}
				/* Check for D-format magic */
				if (dsksize == 3200) 
				{
					dsk_free(secbuf);
					return DSK_ERR_OK;
				}
				/* Check for E-format magic */
				if (secbuf[4] == 10 && secbuf[5] == 5 &&
				    secbuf[6] == 2  && secbuf[7] == 2)
				{
					dsk_free(secbuf);
					return DSK_ERR_OK;
				}
			}
			/* Check for DOS Plus magic. DOS Plus has sectors
			 * based at 1, not 0. */
			geom->dg_secbase = 1;
			e = dsk_lread(self, geom, secbuf, 0);
			if (!e)
			{
				if (secbuf[0] == 0xFD && 
				    secbuf[1] == 0xFF && 
				    secbuf[2] == 0xFF)
				{
					dsk_free(secbuf);
					return DSK_ERR_OK;
				}
			}
			dsk_free(secbuf);
			return DSK_ERR_BADFMT;
		}	
		/* Can't handle other discs with non-512 sector sizes. */
		if ((secid.fmt_secsize != 512))
		{
			dsk_free(secbuf);
			return DSK_ERR_BADFMT;
		}
	}
	/* If the driver couldn't do a READ ID call, then ignore it */
	if (e == DSK_ERR_NOTIMPL) e = DSK_ERR_OK;
	/* Try to ID the disc from its boot sector */
	if (!e) e = dsk_lread(self, geom, secbuf, 0);
	if (e) 
	{ 	
		dsk_free(secbuf);
		return e; 
	}

	/* Save the data rate, because what we have is right, and what's
	 * in the sector might not be. */
	oldrate = geom->dg_datarate;	
	/* We have the sector. Let's try to guess what it is */
	e = dg_dosgeom(geom, secbuf);	
	if (e == DSK_ERR_BADFMT) e = dg_pcwgeom(geom, secbuf);
	if (e == DSK_ERR_BADFMT) e = dg_aprigeom(geom, secbuf);
	if (e == DSK_ERR_BADFMT) e = dg_cpm86geom(geom, secbuf);
	geom->dg_datarate = oldrate;
	
	dsk_free(secbuf);
	return e;
}


/* Interpret a DOS superblock */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_dosgeom(DSK_GEOMETRY *self, const unsigned char *bootsect)
{
	dsk_lsect_t lsmax;

	if (!self || !bootsect) return DSK_ERR_BADPTR;

/* If the boot sector starts 0xE9 or 0xEB, it's DOS. If it starts with
 * three zeroes, it's Atari. 
 *  In particular, we have to be careful not to try to identify a 
 * PCW 180k floppy, which starts 0x00 0x00 0x28 0x09 */

	if (bootsect[0] != 0xE9 && bootsect[0] != 0xEB)
	{
/* However, the Mini Office distribution floppies for the Atari have only 
 * two zeroes. So if bytes 0B 0C 15 and 1B look something like a BPB, 
 * allow them. This should be sufficient to reject PCW diskettes */
		if (bootsect[0x0b] != 0   || bootsect[0x0c] != 2 || 
		    bootsect[0x15] < 0xF8 || bootsect[0x1b] != 0)
		{ 
			if (bootsect[0] || bootsect[1] || bootsect[2]) 
				return DSK_ERR_BADFMT;
		}
	}

	/* Reject fake DOS bootsectors created by 144FEAT */ 	
	if (bootsect[511] == 144 || bootsect[511] == 72 || bootsect[511] == 12)
		return DSK_ERR_BADFMT;

	self->dg_secsize   = bootsect[11] + 256 * bootsect[12];
	if ((self->dg_secsize % 128) || (self->dg_secsize == 0)) 
/* Possible Apricot bootdisk if sector size is 0, or not a multiple of 128 */ 
/* 		self->dg_secsize = 512; */
		return DSK_ERR_BADFMT; 
	self->dg_secbase   = 1;
	self->dg_heads     = bootsect[26] + 256 * bootsect[27];
	self->dg_sectors   = bootsect[24] + 256 * bootsect[25];
	if (!self->dg_heads || !self->dg_sectors) return DSK_ERR_BADFMT;
	lsmax = bootsect[19] + 256 * bootsect[20];
	lsmax /= self->dg_heads;
	lsmax /= self->dg_sectors;
	self->dg_cylinders = (dsk_pcyl_t)lsmax; 
	/* DOS boot sector doesn't store the data rate. We guess that if there are >12
	 * sectors per track, it must have used high density to get them all in */
	self->dg_datarate  = (self->dg_sectors >= 12) ? RATE_HD : RATE_SD;
	/* Similarly it doesn't store the gap lengths: */
	switch(self->dg_sectors)
	{
		case 8:  self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x50; break;
		case 9:  self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x52; break;
		case 10: self->dg_rwgap = 0x0C; self->dg_fmtgap = 0x17; break;
		case 15: self->dg_rwgap = 0x1B; self->dg_fmtgap = 0x50; break;
		case 18: self->dg_rwgap = 0x1B; self->dg_fmtgap = 0x50; break;
		default: self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x52; break;
	}
	self->dg_fm      = 0;
	self->dg_nomulti = 0;

	return DSK_ERR_OK;
}


/* Interpret a PCW superblock */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_pcwgeom(DSK_GEOMETRY *dg, const unsigned char *bootsec)
{
	static unsigned char defsec[10] = { 0, 0, 40, 9, 2, 1, 3, 2, 42, 82 };
	static unsigned char alle5[10]  = { 0xE5, 0xE5, 0xE5, 0xE5, 0xE5,
					    0xE5, 0xE5, 0xE5, 0xE5, 0xE5 };

	/* Treat all 0xE5s as 180k */
	if (!memcmp(bootsec, alle5, 10)) bootsec = defsec;
	/* Check for PCW16 boot/root format */
	if (bootsec[0] == 0xE9 || bootsec[0] == 0xEA)
	{
		if (memcmp(bootsec + 0x2B, "CP/M", 4) ||
		    memcmp(bootsec + 0x33, "DSK", 3)  ||
		    memcmp(bootsec + 0x7C, "CP/M", 4)) return DSK_ERR_BADFMT;
		/* Detected PCW16 boot+root, disc spec at 80h */
		bootsec += 0x80;
	}
	if (bootsec[0] != 3 && bootsec[0] != 0) return DSK_ERR_BADFMT;

	switch(bootsec[1] & 3)
	{
		case 0: dg->dg_heads = 1; dg->dg_sidedness = SIDES_ALT; break;
		case 1: dg->dg_heads = 2; dg->dg_sidedness = SIDES_ALT; break;
		case 2: dg->dg_heads = 2; dg->dg_sidedness = SIDES_OUTBACK; break;
		default: return DSK_ERR_BADFMT;
	}
	dg->dg_cylinders = bootsec[2];
	dg->dg_sectors   = bootsec[3];
	/* Zeroes here may mean an Apricot superblock */
	if (!dg->dg_cylinders || !dg->dg_sectors) return DSK_ERR_BADFMT;
	dg->dg_secbase   = 1;
	dg->dg_secsize   = 128;
	/* My PCW16 extension to the PCW superblock encodes data rate. Fancy that. */
	dg->dg_datarate  = (bootsec[1] & 0x40) ? RATE_HD : RATE_SD;
	dg->dg_fm      = 0;
	dg->dg_nomulti = 0;
	dg->dg_rwgap   = bootsec[8];
	dg->dg_fmtgap  = bootsec[9];
   dg->dg_secsize = 128 << bootsec[4];
   return DSK_ERR_OK;
}

/* Interpret a CP/M86 (floppy) superblock */
LDPUBLIC32 dsk_err_t LDPUBLIC16  dg_cpm86geom(DSK_GEOMETRY *dg, const unsigned char *bootsec)
{
	switch(bootsec[511])
	{
		case 0x00: return dg_stdformat(dg, FMT_160K, NULL, NULL);
		case 0x01: return dg_stdformat(dg, FMT_320K, NULL, NULL);
		case 0x0C: return dg_stdformat(dg, FMT_1200F, NULL, NULL);
		case 0x40:
		case 0x10: return dg_stdformat(dg, FMT_360K, NULL, NULL);
		case 0x11: return dg_stdformat(dg, FMT_720K, NULL, NULL);
		case 0x48: return dg_stdformat(dg, FMT_720F, NULL, NULL);
		case 0x90: return dg_stdformat(dg, FMT_1440F, NULL, NULL);
	}
	return DSK_ERR_BADFMT;
}



/* Interpret an Apricot superblock */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_aprigeom(DSK_GEOMETRY *self, const unsigned char *bootsect)
{
	int n;

	if (!self || !bootsect) return DSK_ERR_BADPTR;

/* Check that the first 8 bytes are ASCII (OEM label) or all zeroes */
	for (n = 0; n < 8; n++) 
		if (bootsect[n] != 0 && (bootsect[n] < 0x20 || bootsect[n] > 0x7E))
			return DSK_ERR_BADFMT;

	/* Sector size */
	self->dg_secsize   = bootsect[0x0E] + 256 * bootsect[0x0F];
	self->dg_secbase   = 1;
	self->dg_heads     = bootsect[0x16];
	self->dg_sectors   = bootsect[0x10] + 256 * bootsect[0x11];
	if (!self->dg_heads || !self->dg_sectors) return DSK_ERR_BADFMT;
	self->dg_cylinders = bootsect[0x12] + 256 * bootsect[0x13];
	/* Sector doesn't store the data rate. We guess that if there are >12
	 * sectors per track, it must have used high density to get them all in */
	self->dg_datarate  = (self->dg_sectors >= 12) ? RATE_HD : RATE_SD;
	/* Similarly it doesn't store the gap lengths: */
	switch(self->dg_sectors)
	{
		case 8:  self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x50; break;
		case 9:  self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x52; break;
		case 10: self->dg_rwgap = 0x0C; self->dg_fmtgap = 0x17; break;
		case 15: self->dg_rwgap = 0x1B; self->dg_fmtgap = 0x50; break;
		case 18: self->dg_rwgap = 0x1B; self->dg_fmtgap = 0x50; break;
		default: self->dg_rwgap = 0x2A; self->dg_fmtgap = 0x52; break;
	}
	self->dg_fm      = 0;
	self->dg_nomulti = 0;

	return DSK_ERR_OK;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_ptread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	unsigned int sec;
	dsk_err_t err;
	unsigned char *b;
	DRV_CLASS *dc;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	if (dc->dc_tread) 
	{
		err = (dc->dc_tread)(self,geom,buf,cylinder,head);	
		if (err != DSK_ERR_NOTIMPL) return err;
	}

	b = (unsigned char *)buf;

	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		err = dsk_pread(self, geom, b + sec * geom->dg_secsize,
			cylinder, head, sec + geom->dg_secbase);
		if (err) return err;
	}
	return DSK_ERR_OK;
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_ltread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf, 
			dsk_ltrack_t track)
{
        dsk_pcyl_t  c;
        dsk_phead_t h;
        dsk_err_t e;
 
        e = dg_lt2pt(geom, track, &c, &h);
        if (e != DSK_ERR_OK) return e;
        return dsk_ptread(self, geom, buf, c, h);
}

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
				dsk_pcyl_t cyl_expected, 
				dsk_phead_t head_expected)
{
	unsigned sec;
	dsk_err_t err;
	unsigned char *b;
	DRV_CLASS *dc;

	if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

	if (dc->dc_xtread) 
	{
		err = (dc->dc_xtread)(self,geom,buf,cylinder,head,
				cyl_expected, head_expected);	
		if (err != DSK_ERR_NOTIMPL) return err;
	}

	b = (unsigned char *)buf;

	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		err = dsk_xread(self, geom, b + sec * geom->dg_secsize,
			cylinder, head, 
			cyl_expected, head_expected, sec + geom->dg_secbase,
			geom->dg_secsize, 0);
		if (err) return err;
	}
	return DSK_ERR_OK;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Read a random sector header from current track */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_psecid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	DRV_CLASS *dc;
	if (!self || !geom || !result || !self->dr_class) return DSK_ERR_BADPTR;

	dc = self->dr_class;

        if (!dc->dc_secid) return DSK_ERR_NOTIMPL;
	return (dc->dc_secid)(self,geom,cylinder,head,result);	

}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lsecid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_ltrack_t track, DSK_FORMAT *result)
{
        dsk_err_t e;
        dsk_pcyl_t cyl;
        dsk_psect_t sec;
 
        e = dg_lt2pt(geom, track, &cyl, &sec);
        if (e) return e;
        return dsk_psecid(self, geom, cyl, sec, result);   
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <jce@seasip.demon.co.uk>            *
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

/* Functions to convert logical <--> physical tracks/sectors */

/* Convert physical sector to logical */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_ps2ls(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head, dsk_psect_t sec,
	/* out */	dsk_lsect_t *logical)
{
	dsk_ltrack_t track;
	dsk_lsect_t sector;
	dsk_err_t e;

	e = dg_pt2lt(self, cyl, head, &track);
   if (e) return e;

	if (sec  < self->dg_secbase || 
	    sec  >= self->dg_secbase + self->dg_sectors) return DSK_ERR_BADPTR;

	sector = track * self->dg_sectors;
	sector += (sec - self->dg_secbase);

	if (logical) *logical = sector;

	return DSK_ERR_OK;
}

/* Convert logical sector to physical */
LDPUBLIC32 dsk_err_t LDPUBLIC16  dg_ls2ps(const DSK_GEOMETRY *self, 
	/* in */	dsk_lsect_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head, dsk_psect_t *sec)
{
	if (!self) return DSK_ERR_BADPTR;
	if (!self->dg_sectors || !self->dg_heads) return DSK_ERR_DIVZERO;

	if (logical >= self->dg_cylinders * self->dg_heads * self->dg_sectors)
		return DSK_ERR_BADPARM;

	if (sec) *sec = (dsk_psect_t)((logical % self->dg_sectors) + self->dg_secbase);

	logical /= self->dg_sectors;
	return dg_lt2pt(self, (dsk_ltrack_t)logical, cyl, head);
}



 
/* Convert physical cyl/head to logical track */
LDPUBLIC32 dsk_err_t LDPUBLIC16  dg_pt2lt(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head,
	/* out */	dsk_ltrack_t *logical)
{
	dsk_ltrack_t track = 0;

	if (!self          ) return DSK_ERR_BADPTR;
	if (!self->dg_heads) return DSK_ERR_DIVZERO;

	if (head >= self->dg_heads ||
	    cyl  >= self->dg_cylinders) return DSK_ERR_BADPARM;

	switch(self->dg_sidedness)
	{
		case SIDES_ALT:		track = (cyl * self->dg_heads) + head; break;
		case SIDES_OUTBACK:	if (self->dg_heads > 2) return DSK_ERR_BADPARM;
					if (!head)	track = cyl;
					else		track = (2 * self->dg_cylinders) - (1 + cyl); 
					break;
		case SIDES_OUTOUT:	track = (head * self->dg_cylinders) + cyl;
					break;
	}

	if (logical) *logical = track;

	return DSK_ERR_OK;
}

/* Convert logical track to physical */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_lt2pt(const DSK_GEOMETRY *self, 
	/* in */	dsk_ltrack_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head)
{
	dsk_pcyl_t c = 0;
	dsk_phead_t h = 0;

	if (!self) return DSK_ERR_BADPTR;
	if (!self->dg_heads) return DSK_ERR_DIVZERO;

	if (logical >= self->dg_cylinders * self->dg_heads)
		return DSK_ERR_BADPARM;

	switch(self->dg_sidedness)
	{
		case SIDES_ALT:		c = (logical / self->dg_heads); 
					h = (logical % self->dg_heads); 
					break;
		case SIDES_OUTBACK:	if (self->dg_heads > 2) return DSK_ERR_BADPARM;
					if (logical < self->dg_cylinders)
					{
						c = logical;
						h = 0;
					}
					else
					{
						c = (2 * self->dg_cylinders) - (1 + logical); 
						h = 1;
					}
					break;
		case SIDES_OUTOUT:	c = (logical % self->dg_cylinders);
					h = (logical / self->dg_cylinders);
					break;
	}
	if (cyl)  *cyl  = c;
	if (head) *head = h;
	return DSK_ERR_OK;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2, 2005  John Elliott <jce@seasip.demon.co.uk>    *
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

/* Standard disc geometries. These are used 
 * (i)  when logging in a disc, if the superblock is recognised
 * (ii) when formatting */

typedef struct dsk_namedgeom
{
    dsk_cchar_t name;
    DSK_GEOMETRY dg;
    dsk_cchar_t desc;
    struct dsk_namedgeom *next;
} DSK_NAMEDGEOM;

/* These must match the order of the entries in dsk_format_t in libdsk.h */

static DSK_NAMEDGEOM stdg[] = 
{
        /*    sidedness cyl hd sec  psn  sz   rate    rwgap  fmtgap  fm  nomulti*/
{"pcw180",  { SIDES_ALT,     40, 1, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "PCW / IBM 180k" }, /* 180k */
{"cpcsys",  { SIDES_ALT,     40, 1, 9, 0x41, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "CPC System" }, /* CPC system */
{"cpcdata", { SIDES_ALT,     40, 1, 9, 0xC1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "CPC Data" }, /* CPC data */
{"pcw720",  { SIDES_ALT,     80, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "PCW / IBM 720k" }, /* 720k */
{"pcw1440", { SIDES_ALT,     80, 2,18,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "PcW16 / IBM 1440k "}, /* 1.4M */
{"ibm160",  { SIDES_ALT,     40, 1, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 160k (CP/M-86 / DOSPLUS)" }, /* 160k */
/* This was commented out in libdsk-1.1.3, but I can't remember why. Bring it 
 * back. */
{"ibm320",  { SIDES_ALT,     40, 2, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 320k (CP/M-86 / DOSPLUS)" }, /* 320k */
{"pcpm320", { SIDES_OUTBACK, 40, 2, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 320k (CP/M-86 / DOSPLUS)" }, /* 320k */
{"ibm360",  { SIDES_ALT,     40, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "IBM 360k (DOSPLUS)", }, /* 360k */
{"ibm720",  { SIDES_OUTBACK, 80, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "IBM 720k (144FEAT)", }, /* 720k 144FEAT */
{"ibm1200", { SIDES_OUTBACK, 80, 2,15,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "IBM 1.2M (144FEAT)", }, /* 1.2M 144FEAT */
{"ibm1440", { SIDES_OUTBACK, 80, 2,18,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "IBM 1.4M (144FEAT)", }, /* 1.4M 144FEAT */
{"acorn160",    { SIDES_OUTOUT,  40, 1,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 160k" }, /* Acorn 160k */
{"acorn320",    { SIDES_OUTOUT,  80, 1,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 320k" }, /* Acorn 320k */
{"acorn640",    { SIDES_OUTOUT,  80, 2,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 640k" }, /* Acorn 640k */
{"acorn800",    { SIDES_ALT,     80, 2, 5,    0,1024, RATE_SD, 0x04, 0x05,   0,  0 }, "Acorn 800k" }, /* Acorn 800k */
{"acorn1600",   { SIDES_ALT,     80, 2,10,    0,1024, RATE_HD, 0x04, 0x05,   0,  0 }, "Acorn 1600k" }, /* Acorn 1600k */
{"pcw800",  { SIDES_ALT,     80, 2,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "PCW 800k" }, /* 800k */
{"pcw200",  { SIDES_ALT,     40, 1,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "PCW 200k" }, /* 200k */
{"bbc100",  { SIDES_ALT,     40, 1,10,    0, 256, RATE_SD, 0x2A, 0x50,   1,  0 }, "BBC 100k" }, /* 100k */
{"bbc200",  { SIDES_ALT,     80, 1,10,    0, 256, RATE_SD, 0x2A, 0x50,   1,  0 }, "BBC 200k" }, /* 200k */
{"mbee400", { SIDES_ALT,     40, 1,10,    0, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "Microbee 400k" }, /* 400k */
{"mgt800",  { SIDES_OUTOUT,  80, 2,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "MGT 800k" }, /* MGT 800k */
{"trdos640",{ SIDES_ALT,     80, 2,16,    1, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "TR-DOS 640k" }, /* TR-DOS 640k */

/* Geometries below this line don't appear in dsk_format_t and can be accessed
 * only by name. */

{"myz80",   { SIDES_ALT,     64, 1,128,   0,1024, RATE_ED, 0x2A, 0x52,   0,  0 }, "MYZ80 8Mb" }, /* MYZ80 8Mb */
};


#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#include <io.h>
#include <objidl.h>

static void dg_shell_folder(int csidl, char *buf)
{
    LPMALLOC pMalloc;
    LPITEMIDLIST pidl;
    char *cwd;
        char result[PATH_MAX]; 

    cwd = getcwd(result, PATH_MAX);
    if (cwd == NULL) strcpy(result, ".");
    strcpy(buf, result);

    /* Get the shell's allocator */
    if (!SUCCEEDED(SHGetMalloc(&pMalloc))) return;
    /* Get the PIDL for the folder in question */
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
    {
        if (SHGetPathFromIDList(pidl, result))
            strcpy(buf, result);
        (pMalloc->lpVtbl->Free)(pMalloc, pidl);
    }
    /* Release the shell's allocator */
    (pMalloc->lpVtbl->Release)(pMalloc);
}

const char *dg_homedir(void)
{
    HKEY hk;
    DWORD dws;
    static char buf[PATH_MAX];
    char *t;
    long l;

    l = RegOpenKeyEx(HKEY_CURRENT_USER, 
        "Software\\jce@seasip\\LibDsk", 0, KEY_READ, &hk);
    if (l == ERROR_SUCCESS)
    {
        dws = PATH_MAX;
        l = RegQueryValueEx(hk, "HomeDir", NULL, NULL, 
                (BYTE *)buf, &dws); 
        RegCloseKey(hk);
        if (l == ERROR_SUCCESS) return buf;
    }
    dg_shell_folder(CSIDL_PERSONAL, buf);
    
    while ((t = strchr(buf, '\\'))) *t = '/';

    /* Ensure path ends with a slash */
    if (buf[strlen(buf)-1] != '/') strcat(buf, "/");

    return buf;
}
#elif defined(HAVE_PWD_H) && defined(HAVE_UNISTD_H)

#include <unistd.h>
#include <pwd.h>

const char *dg_homedir(void)
{
    char *s;
    static char buf[PATH_MAX];
    struct passwd *pw;
    int uid = getuid();

    s = getenv("HOME");
    if (s)
    {
        strcpy(buf, s);
        /* Ensure path ends with a slash */

        if (buf[strlen(buf)-1] != '/') strcat(buf, "/");
        return buf;
    }
    else
    {
        setpwent();
        while ((pw = getpwent()))
        {
            if (pw->pw_uid == uid)
            {
                strcpy(buf, pw->pw_dir);
                if ( buf[strlen(buf)-1] != '/') strcat(buf, "/");
                endpwent();
                return buf;
            }
        }
        endpwent();
    }
    return NULL;
}
#else

const char *dg_homedir(void)
{
    static char buf[PATH_MAX];
    char *s = getenv("HOME");

    if (!s) return NULL;
    strcpy(buf, s);
    /* Ensure path ends with a slash */
    if (s && buf[strlen(buf)-1] != '/') strcat(buf, "/");

    return buf;
}
#endif



#ifdef HAVE_WINDOWS_H

const char *dg_sharedir(void)
{
    static char buf[PATH_MAX + 8];
    char *t;
#ifdef _WIN32   /* Win16 doesn't have HKLM */
    long l;
    HKEY hk;
    DWORD dws = sizeof(buf)-1;

    l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        "Software\\jce@seasip\\LibDsk", 0, KEY_READ, &hk);
    if (l == ERROR_SUCCESS)
    {                                             
        dws = PATH_MAX;
        l = RegQueryValueEx(hk, "ShareDir", NULL, NULL, 
                (BYTE *)buf, &dws); 
        RegCloseKey(hk);
        if (l == ERROR_SUCCESS) return buf;
    }
#endif
    GetModuleFileName((HINSTANCE)NULL, buf, PATH_MAX);
    while ((t = strchr(buf, '\\'))) *t = '/';
    for (t = buf + strlen(buf); t >= buf; t--)
    {
        if (t[0] == ':' || t[0] == '/')
        {
            t[1] = 0;
            break;
        }
    }
    strcat(buf, "share/");
    return buf;
}             

#else /* def HAVE_WINDOWS_H */

const char *dg_sharedir()
{
    static char buf[PATH_MAX];
    char *s = getenv("LIBDSK");

#ifdef AUTOSHARE
    if (SHAREDIR)
    {
        return SHAREDIR "/";
    }
#endif

    if (!s) return NULL;
    strcpy(buf, s);
    /* Ensure path ends with a slash */
    if (s && buf[strlen(buf)-1] != '/') strcat(buf, "/");
    return buf;
}

#endif /* def HAVE_WINDOWS_H */

static DSK_NAMEDGEOM *customgeom = NULL;


dsk_err_t dg_parse(FILE *fp, DSK_GEOMETRY *dg, char *description)
{
    char linebuf[160];
    dsk_err_t err;

    /* Init geometry to default values */
    memcpy(dg, &stdg[0].dg, sizeof(*dg));

    while (fgets(linebuf, sizeof(linebuf), fp))
    {
        /* Start of next section */
        if (linebuf[0] == '[') return DSK_ERR_OK;
        err = dg_parseline(linebuf, dg, description);
        if (err) return err;
    }
    return DSK_ERR_OK;
}

dsk_err_t dg_parseline(char *linebuf, DSK_GEOMETRY *dg, char *description)
{
    char *s;
    char *value;
/* Drop comments and newlines */

    s = strchr(linebuf, ';'); if (s) *s = 0;
    s = strchr(linebuf, '#'); if (s) *s = 0;
    s = strchr(linebuf, '\n'); if (s) *s = 0;

    value = strchr(linebuf, '=');
    if (!value) return DSK_ERR_OK;
    *value = 0;
    ++value;

    /* Chop variable name at first space */
    s = strchr(linebuf, ' '); 
    if (s) *s = 0;
    /* Skip leading spaces in the value */
    while (*value == ' ') ++value;

/* Make the variable name case-insensitive */
    for (s = linebuf; s[0]; s++)
    {
        *s = tolower(*s);
    }
    if (!strcmp(linebuf, "description"))
    {
        if (description) 
            strcpy(description, value);
    }
    if (!strcmp(linebuf, "sidedness") || !strcmp(linebuf, "sides"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "alt"))     
            dg->dg_sidedness = SIDES_ALT;
        if (!strcmp(value, "outback")) 
            dg->dg_sidedness = SIDES_OUTBACK;
        if (!strcmp(value, "outout"))  
            dg->dg_sidedness = SIDES_OUTOUT;
    }
    if (!strcmp(linebuf, "cylinders") && atoi(value))
        dg->dg_cylinders = atoi(value);
    if (!strcmp(linebuf, "heads") && atoi(value))
        dg->dg_heads     = atoi(value);
    if (!strcmp(linebuf, "sectors") && atoi(value))
        dg->dg_sectors   = atoi(value);
    if (!strcmp(linebuf, "secbase"))
        dg->dg_secbase   = atoi(value);
    if (!strcmp(linebuf, "secsize") && atoi(value))
        dg->dg_secsize   = atoi(value);
    if (!strcmp(linebuf, "datarate"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "hd")) dg->dg_datarate = RATE_HD;
        if (!strcmp(value, "dd")) dg->dg_datarate = RATE_DD;
        if (!strcmp(value, "sd")) dg->dg_datarate = RATE_SD;
        if (!strcmp(value, "ed")) dg->dg_datarate = RATE_ED;
    }
    if (!strcmp(linebuf, "rwgap") && atoi(value))
        dg->dg_rwgap  = atoi(value);
    if (!strcmp(linebuf, "fmtgap") && atoi(value))
        dg->dg_fmtgap  = atoi(value);
    if (!strcmp(linebuf, "fm"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) dg->dg_fm = 1;
        if (!strcmp(value, "n")) dg->dg_fm = 0;
    }
    if (!strcmp(linebuf, "multitrack"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) dg->dg_nomulti = 0;
        if (!strcmp(value, "n")) dg->dg_nomulti = 1;
    }
    if (!strcmp(linebuf, "skipdeleted"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) dg->dg_noskip = 0;
        if (!strcmp(value, "n")) dg->dg_noskip = 1;
    }
    return DSK_ERR_OK;
}



dsk_err_t dg_store(FILE *fp, DSK_GEOMETRY *dg, char *description)
{
    if (description) fprintf(fp, "description=%s\n", description);
    switch(dg->dg_sidedness)
    {
        case SIDES_ALT:     fprintf(fp, "sides=alt\n");     break;
        case SIDES_OUTOUT:  fprintf(fp, "sides=outback\n"); break;
        case SIDES_OUTBACK: fprintf(fp, "sides=outout\n");  break;
        }
    fprintf(fp, "cylinders=%d\n", dg->dg_cylinders);
    fprintf(fp, "heads=%d\n",     dg->dg_heads);
    fprintf(fp, "sectors=%d\n",   dg->dg_sectors);
    fprintf(fp, "secbase=%d\n",   dg->dg_secbase);
    fprintf(fp, "secsize=%ld\n",  (long)dg->dg_secsize);
    switch(dg->dg_datarate)
    {
        case RATE_HD: fprintf(fp, "datarate=HD\n"); break;
        case RATE_DD: fprintf(fp, "datarate=DD\n"); break;
        case RATE_SD: fprintf(fp, "datarate=SD\n"); break;
        case RATE_ED: fprintf(fp, "datarate=ED\n"); break;
    }
    fprintf(fp, "rwgap=%d\n", dg->dg_rwgap);
    fprintf(fp, "fmtgap=%d\n", dg->dg_fmtgap);
    fprintf(fp, "fm=%c\n", dg->dg_fm ? 'Y' : 'N');
    fprintf(fp, "multitrack=%c\n", dg->dg_nomulti ? 'N' : 'Y');
    fprintf(fp, "skipdeleted=%c\n", dg->dg_noskip ? 'N' : 'Y');
    return DSK_ERR_OK;
}


static dsk_err_t dg_parse_file(FILE *fp)
{
    DSK_NAMEDGEOM ng, *pg;
    char linebuf[160];
    char formname[160];
    char formdesc[160];
    char *s;
    long pos;
    dsk_err_t err;

    while (fgets(linebuf, sizeof(linebuf), fp))
    {
        formdesc[0] = 0;
/* Drop comments and newlines */
        s = strchr(linebuf, ';'); if (s) *s = 0;
        s = strchr(linebuf, '#'); if (s) *s = 0;
        s = strchr(linebuf, '\n'); if (s) *s = 0;

        if (linebuf[0] != '[') continue;
/* Format names cannot start with "-", so any section beginning "[-" is 
 * going to be something other than a format. */
	if (linebuf[1] == '-') continue;
        strcpy(formname, linebuf+1);
        s = strchr(formname, ']');
        if (s) *s = 0;
        pos = ftell(fp);
        err = dg_parse(fp, &ng.dg, formdesc);
        if (err) return err;
        fseek(fp, pos, SEEK_SET);   

        pg = dsk_malloc(sizeof(ng) + 2 + 
                strlen(formdesc) + strlen(formname));
        if (!pg) return DSK_ERR_NOMEM;
        memcpy(pg, &ng, sizeof(ng));

        pg->name = ((char *)pg) + sizeof(ng);
        pg->desc = ((char *)pg) + sizeof(ng) + 1 + strlen(formname);
        strcpy((char *)pg->name, formname);
        strcpy((char *)pg->desc, formdesc);
        pg->next = customgeom;
        customgeom = pg;
    }
    return DSK_ERR_OK;
}

dsk_err_t dg_custom_init(void)
{
    const char *path;
    char buf[2 * PATH_MAX];
    FILE *fp;
    dsk_err_t err;

    static int custom_inited = 0;

    if (custom_inited < 1)
    {
        path = dg_sharedir();
        if (path)
        {
            sprintf(buf, "%s%s", path, "libdskrc");
            fp = fopen(buf, "r");
            if (fp)
            {
                err = dg_parse_file(fp);
                fclose(fp);
                if (err) return err;
            }
        }
        custom_inited = 1;
    }
    if (custom_inited < 2)
    {
        path = dg_homedir();
        if (path)
        {
            sprintf(buf, "%s%s", path, ".libdskrc");
            fp = fopen(buf, "r");
            if (fp)
            {
                err = dg_parse_file(fp);
                fclose(fp);
                if (err) return err;
            }       
        }
        custom_inited = 2;
    }
    return DSK_ERR_OK;
}



/* Initialise a DSK_GEOMETRY with a standard format */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_stdformat(DSK_GEOMETRY *self, dsk_format_t formatid,
            dsk_cchar_t *fname, dsk_cchar_t *fdesc)
{
    int idx = (formatid - FMT_180K);

    dg_custom_init();

/* If index is out of range in the standard set, search the custom set */
    if (idx >= (sizeof(stdg)/sizeof(stdg[0]))  ) 
    {
        DSK_NAMEDGEOM *cg = customgeom;
        idx -= (sizeof(stdg) / sizeof(stdg[0]));

        while (idx)
        {
            if (!cg) return DSK_ERR_BADFMT;
            cg = cg->next;
            --idx;
        }
        if (!cg) return DSK_ERR_BADFMT;

        if (self) memcpy(self, &cg->dg, sizeof(*self));
        if (fname) *fname = cg->name;
        if (fdesc) *fdesc = cg->desc;
        return DSK_ERR_OK;
    }
/* Search the standard set */
    if (self) memcpy(self, &stdg[idx].dg, sizeof(*self));
    if (fname) *fname = stdg[idx].name;
    if (fdesc) *fdesc = stdg[idx].desc;
    return DSK_ERR_OK;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2002  John Elliott <jce@seasip.demon.co.uk>            *
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

/* LibDsk generalised compression support */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#define TMPDIR "/tmp"

static COMPRESS_CLASS *comp_classes[] =
{
    NULL
};


static dsk_err_t comp_construct(COMPRESS_DATA *cd, const char *filename)
{
    cd->cd_cfilename = dsk_malloc(1 + strlen(filename));    
    if (!cd->cd_cfilename) return DSK_ERR_NOMEM;
    strcpy(cd->cd_cfilename, filename);
    cd->cd_ufilename = NULL;
    cd->cd_readonly = 0;
    return DSK_ERR_OK;
}

void comp_free(COMPRESS_DATA *cd)
{
    if (!cd) return;
    if (cd->cd_cfilename) free(cd->cd_cfilename);
    if (cd->cd_ufilename) free(cd->cd_ufilename);
    free(cd);
}


/* See if a file can be decompressed */
static dsk_err_t comp_iopen(COMPRESS_DATA **cd, const char *filename, int nc)
{
        COMPRESS_CLASS *cc = comp_classes[nc];
        dsk_err_t err;

        if (!cc) return DSK_ERR_BADPTR;

        (*cd) = dsk_malloc(cc->cc_selfsize);
        if (!*cd) return DSK_ERR_NOMEM;
    memset((*cd), 0, cc->cc_selfsize);
        err = comp_construct(*cd, filename);
    (*cd)->cd_class = cc;
    if (err == DSK_ERR_OK) 
    {
        char *s = dsk_malloc(strlen(cc->cc_description) + 50);
        if (s) 
        {
            sprintf(s, "Checking compression: %s...", cc->cc_description);
            dsk_report(s);
            dsk_free(s);
        }
        else    dsk_report("Checking compression...");
        err = (cc->cc_open)(*cd);
        dsk_report_end();
    }
        if (err == DSK_ERR_OK) return err;
        comp_free (*cd);
        *cd = NULL;
        return err;
}

/* See if a file can be decompressed */
static dsk_err_t comp_icreat(COMPRESS_DATA **cd, const char *filename, int nc)
{
        COMPRESS_CLASS *cc = comp_classes[nc];
        dsk_err_t err;
    FILE *fp;

        if (!cc) return DSK_ERR_BADPTR;

        (*cd) = dsk_malloc(cc->cc_selfsize);
        if (!*cd) return DSK_ERR_NOMEM;
    memset((*cd), 0, cc->cc_selfsize);
        err = comp_construct(*cd, filename);
    (*cd)->cd_class = cc;
    if (err == DSK_ERR_OK) err = (cc->cc_creat)(*cd);
/* Stake out our claim to the temporary file. */
        if (err == DSK_ERR_OK) err = comp_mktemp(*cd, &fp);
    if (fp) fclose(fp);
    if (err == DSK_ERR_OK) return err;
    
        comp_free (*cd);
        *cd = NULL;
        return err;
}



dsk_err_t comp_open(COMPRESS_DATA **cd, const char *filename, const char *type)
{
    int nc;
    dsk_err_t e;
    struct stat st;

    if (!cd || !filename) return DSK_ERR_BADPTR;
    *cd = NULL;

/* Do not attempt to decompress directories */  
    if (stat(filename, &st)) return DSK_ERR_NOTME;
    if (S_ISDIR(st.st_mode)) return DSK_ERR_NOTME;

#ifdef LINUXFLOPPY 
/* Do not try to decompress a floppy drive. The reason for this is: If the 
 * floppy has a format Linux can't detect, repeatedly failing to open /dev/fd0
 * as we check for each compression type causes long delays */
        if (S_ISBLK(st.st_mode) && (major(st.st_rdev) == 2)) return DSK_ERR_NOTME;
#endif
/* Same shortcut for Windows / DOS */
#ifdef ANYFLOPPY
        if (strlen(filename) == 2 && filename[1] == ':') return DSK_ERR_NOTME;
#endif
    
    if (type)
    {
        for (nc = 0; comp_classes[nc]; nc++)
        {
            if (!strcmp(type, comp_classes[nc]->cc_name))
                return comp_iopen(cd, filename, nc);
        }
        return DSK_ERR_NODRVR;
    }
    for (nc = 0; comp_classes[nc]; nc++)
    {
        e = comp_iopen(cd, filename, nc);
        if (e != DSK_ERR_NOTME) return e;
    }   
    return DSK_ERR_NOTME;
}

dsk_err_t comp_creat(COMPRESS_DATA **cd, const char *filename, const char *type)
{
    int nc;

    if (!type)  /* Uncompressed */
    {
        *cd = NULL;
        return DSK_ERR_OK;
    }

    if (!cd || !filename) return DSK_ERR_BADPTR;
    
    for (nc = 0; comp_classes[nc]; nc++)
    {
        if (!strcmp(type, comp_classes[nc]->cc_name))
            return comp_icreat(cd, filename, nc);
    }
    return DSK_ERR_NODRVR;
}



dsk_err_t comp_commit(COMPRESS_DATA **self)
{
    dsk_err_t e;

    if (!self || (!(*self)) || (!(*self)->cd_class))    return DSK_ERR_BADPTR;

    dsk_report("Compressing...");
    e = ((*self)->cd_class->cc_commit)(*self);
    dsk_report_end();

    if ((*self)->cd_ufilename) remove((*self)->cd_ufilename);
    comp_free (*self);
    *self = NULL;
    return e;
}

dsk_err_t comp_abort(COMPRESS_DATA **self)
{
    dsk_err_t e;

    if (!self || (!(*self)) || (!(*self)->cd_class))    return DSK_ERR_BADPTR;

    e = ((*self)->cd_class->cc_abort)(*self);

    if ((*self)->cd_ufilename) remove((*self)->cd_ufilename);
    comp_free (*self);
    *self = NULL;
    return e;
}


/* If "index" is in range, returns the n'th driver name in (*drvname).
 * Else sets (*drvname) to null. */
dsk_err_t comp_type_enum(int index, char **compname)
{
    int nc;

    if (!compname) return DSK_ERR_BADPTR;

    for (nc = 0; comp_classes[nc]; nc++)
    {
        if (index == nc)
        {
            *compname = comp_classes[nc]->cc_name;
            return DSK_ERR_OK;
        }
    }
    *compname = NULL;
    return DSK_ERR_NODRVR;
}


const char *comp_name(COMPRESS_DATA *self)
{
    if (!self || !self->cd_class || !self->cd_class->cc_name)
        return "(null)";
    return self->cd_class->cc_name;
}


const char *comp_desc(COMPRESS_DATA *self)
{
    if (!self || !self->cd_class || !self->cd_class->cc_description)
        return "(null)";
    return self->cd_class->cc_description;
}



dsk_err_t comp_fopen(COMPRESS_DATA *self, FILE **fp)
{
        *fp = fopen(self->cd_cfilename, "r+b");
        if (!(*fp))
        {
                self->cd_readonly = 1;
                (*fp) = fopen(self->cd_cfilename, "rb");
        }
        if (!(*fp)) return DSK_ERR_SYSERR;
    return DSK_ERR_OK;
}
    
    
dsk_err_t comp_mktemp(COMPRESS_DATA *self, FILE **fp)
{
    char *tdir;
    int fd;
    char tmpdir[PATH_MAX];

    self->cd_ufilename = dsk_malloc(PATH_MAX);

/* Win32: Create temp file using GetTempFileName() */
#ifdef HAVE_GETTEMPFILENAME
        GetTempPath(PATH_MAX, tmpdir);
        if (!GetTempFileName(tmpdir, "dsk", 0, self->cd_ufilename))
    {
        dsk_free(self->cd_ufilename);
        self->cd_ufilename = NULL;
        return DSK_ERR_SYSERR;
    }
        *fp = fopen(self->cd_ufilename, "wb");
        (void)fd;
        (void)tdir;
#else /* def HAVE_GETTEMPFILENAME */

/* Modern Unixes: Use mkstemp() */
#ifdef HAVE_MKSTEMP
    tdir = getenv("TMPDIR");
    if (tdir) sprintf(tmpdir, "%s/libdskdXXXXXXXX", tdir);
    else      sprintf(tmpdir, TMPDIR "/libdskXXXXXXXX");

    fd = mkstemp(tmpdir);
    *fp = NULL;
    if (fd != -1) fp[0] = fdopen(fd, "wb");
    strcpy(self->cd_ufilename, tmpdir);
#else   /* def HAVE_MKSTEMP */
/* Old Unixes, old xenix clones such as DOS */ 
    tdir = getenv("TMP");
    if (!tdir) tdir = getenv("TEMP");
    if (tdir) sprintf(tmpdir, "%s/LDXXXXXX", tdir);
    else      sprintf(tmpdir, "./LDXXXXXX");

    strcpy(self->cd_ufilename, mktemp(tmpdir));
    fp[0] = fopen(self->cd_ufilename, "wb");
    (void)fd;
#endif /* HAVE_MKSTEMP */                  
#endif /* HAVE_GETTEMPFILENAME */
    if (!*fp)   
    {
        dsk_free(self->cd_ufilename);
        self->cd_ufilename = NULL;
        return DSK_ERR_SYSERR;
    }
    return DSK_ERR_OK;
}

/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2003  John Elliott <jce@seasip.demon.co.uk>            *
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

static DSK_REPORTFUNC st_repfunc;
static DSK_REPORTEND  st_repend;

/* Register callbacks for LibDsk functions to display information on the
 *  * screen. */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_set(DSK_REPORTFUNC report, 
                                              DSK_REPORTEND  repend)
{
	st_repfunc = report;
	st_repend  = repend;
}


/* Retrieve the values of the callbacks */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_get(DSK_REPORTFUNC *report, 
                                              DSK_REPORTEND  *repend)
{
	if (report) *report = st_repfunc;
	if (repend) *repend = st_repend;
}


LDPUBLIC32 void LDPUBLIC16 dsk_report(const char *s)
{
	if (st_repfunc) (*st_repfunc)(s);
}


LDPUBLIC32 void LDPUBLIC16 dsk_report_end()
{
	if (st_repend) (*st_repend)();
}

