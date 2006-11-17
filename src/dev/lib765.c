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
#include <libdsk.h>
#include "lib765.h"
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
  fdc->fdc_msr           = 0x80;
  fdc->fdc_st0           = 0;
  fdc->fdc_st1           = 0;
  fdc->fdc_st2           = 0;
  fdc->fdc_st3           = 0;
  fdc->fdc_curunit       = 0;
  fdc->fdc_curhead       = 0;
  fdc->fdc_specify[0]    = 0;
  fdc->fdc_specify[1]    = 0;
  (void) memset(fdc->fdc_cmd_buf, 0, sizeof(fdc->fdc_cmd_buf));
  fdc->fdc_cmd_len       = 0;
  fdc->fdc_cmd_pos       = 0;
  (void) memset(fdc->fdc_exe_buf, 0, sizeof(fdc->fdc_exe_buf));
  fdc->fdc_exe_len       = 0;
  fdc->fdc_exe_pos       = 0;
  (void) memset(fdc->fdc_res_buf, 0, sizeof(fdc->fdc_res_buf));
  fdc->fdc_res_len       = 0;
  fdc->fdc_res_pos       = 0;
  fdc->fdc_interrupting  = 0;
  fdc->fdc_isr_countdown = 0;
  fdc->fdc_write_deleted = 0;
  fdc->fdc_lastidread    = 0;
  fdc->fdc_cmd_id        = -1;
}

static int fdc_ready(FDC_765 *self, FDD_765 *fd)
{
  int rdy = fdd_ready(fd);

  if(rdy == 0) {
    self->fdc_st3 &= 0xDF;
  }
  else {
    self->fdc_st3 |= 0x20;
  }
  return(rdy);
}

/* Update the ST3 register based on a drive's status */
static void fdc_get_st3(FDC_765 *self)
{
	FDD_765 *fd = self->fdc_drive[self->fdc_curunit];
	fdc_byte value = 0;

	/* 0.3.4: Check this, it could be null! */
	if (fd->fd_vtable->fdv_drive_status)
	{
		value = (*fd->fd_vtable->fdv_drive_status)(fd);
	}
	value &= 0xF8;
	value |= (self->fdc_curhead ? 4 : 0);
	value |= (self->fdc_curunit & 3);
	self->fdc_st3 = value;
}

/* Convert an error from the fd_* routines to a value in the status registers */
static void fdc_xlt_error(FDC_765 *self, fd_err_t error)
{
	log_dprintf(4, "FDC: Error from drive: %d\n", error);
        switch(error)
        {
                case FD_E_NOTRDY: self->fdc_st0 |= 0x48; break;
		case FD_E_SEEKFAIL:
				  self->fdc_st0 |= 0x40;
				  self->fdc_st2 |= 0x02; break;
                case FD_E_NOADDR: self->fdc_st0 |= 0x40;
                                  self->fdc_st2 |= 0x01; break;
                case FD_E_NODATA: self->fdc_st0 |= 0x40;
                                  self->fdc_st1 |= 0x04; break;
		case FD_E_DATAERR:
				  self->fdc_st1 |= 0x20;
				  self->fdc_st2 |= 0x20; break;
                case FD_E_NOSECTOR:
                                  self->fdc_st0 |= 0x40;
                                  self->fdc_st1 |= 0x82; break;
                case FD_E_READONLY: 
				  self->fdc_st0 |= 0x40;
				  self->fdc_st1 |= 0x02; break;
        }

}

/* Fill out 7 result bytes
 * XXX Bytes 2,3,4,5 should change the way the real FDC does it. */
static void fdc_results_7(FDC_765 *self)
{
	self->fdc_res_buf[0] = self->fdc_st0;	/* 3 status registers */
        self->fdc_res_buf[1] = self->fdc_st1;
        self->fdc_res_buf[2] = self->fdc_st2;
        self->fdc_res_buf[3] = self->fdc_cmd_buf[2];	/* C, H, R, N */
        self->fdc_res_buf[4] = self->fdc_cmd_buf[3];
        self->fdc_res_buf[5] = self->fdc_cmd_buf[4];
        self->fdc_res_buf[6] = self->fdc_cmd_buf[5];
	self->fdc_msr = 0xD0;	/* Ready to return results */
}


/* End of result phase. Switch FDC back to idle */
static void fdc_end_result_phase(FDC_765 *self)
{
	self->fdc_msr = 0x80;
	if (self->fdc_interrupting < 3) self->fdc_interrupting = 0;
	self->fdc_cmd_id = -1;
}

/* Generate a 1-byte result phase containing ST0 */
static void fdc_error(FDC_765 *self)
{
	self->fdc_st0 = (self->fdc_st0 & 0x3F) | 0x80;
	self->fdc_msr = 0xD0;	/* Result phase */
	self->fdc_res_len = 1;
	self->fdc_res_pos = 0;
	self->fdc_res_buf[0] = self->fdc_st0;
}


/* Interrupt: Start of result phase */
static void fdc_result_interrupt(FDC_765 *self)
{
	self->fdc_isr_countdown = SHORT_TIMEOUT;
        self->fdc_interrupting  = 1;	/* Result-phase interrupt */
}


/* Interrupt: Start of execution phase */
static void fdc_exec_interrupt(FDC_765 *self)
{
        self->fdc_isr_countdown = SHORT_TIMEOUT;
        self->fdc_interrupting  = 2;    /* Execution-phase interrupt */
}

/* Compare two bytes - for the SCAN commands */
static void fdc_scan_byte(FDC_765 *self, fdc_byte fdcbyte, fdc_byte cpubyte)
{
        int cmd = self->fdc_cmd_buf[0] & 0x1F;
        if ((self->fdc_st2 & 0x0C) != 8) return;	/* Mismatched already */
        
        if ((fdcbyte == cpubyte) || (fdcbyte == 0xFF) || (cpubyte == 0xFF)) 
		return;	/* Bytes match */

        /* They differ */
        if (cmd == 17) /* equal */
        {
	    self->fdc_st2 = (self->fdc_st2 & 0xF3) | 4; /* != */
	}
        if (cmd == 25) /* low or equal */
        {
            if (fdcbyte < cpubyte) self->fdc_st2 = (self->fdc_st2 & 0xF3);
            if (fdcbyte > cpubyte) self->fdc_st2 = (self->fdc_st2 & 0xF3) | 4;
        }
        if (cmd == 30) /* high or equal */
        {
            if (fdcbyte < cpubyte) self->fdc_st2 = (self->fdc_st2 & 0xF3) | 4;
	    if (fdcbyte > cpubyte) self->fdc_st2 = (self->fdc_st2 & 0xF3);
        }
}



/* Get the drive & head from the FDC command bytes */
static void fdc_get_drive(FDC_765 *self)
{
	/* Set current drive & head in FDC struct */
	self->fdc_curunit =  self->fdc_cmd_buf[1] & 3;
	self->fdc_curhead = (self->fdc_cmd_buf[1] & 4) >> 2;

	/* Set current drive & head in FDC status regs */
	self->fdc_st0 &= 0xF8;
	self->fdc_st3 &= 0xF8;

	self->fdc_st0 |= (self->fdc_cmd_buf[1] & 7);
        self->fdc_st3 |= (self->fdc_cmd_buf[1] & 7);
}


/* End of Execution Phase in a write command. Write data. */
static void fdc_write_end(FDC_765 *self)
{
        FDD_765 *fd = self->fdc_drive[self->fdc_curunit];
	int len = (128 << self->fdc_cmd_buf[5]);
	fd_err_t rv;

	if (self->fdc_cmd_buf[8] < 255) len = self->fdc_cmd_buf[8];

	rv = fdd_write_sector(fd, self->fdc_cmd_buf[2], /* xcyl */
				 self->fdc_cmd_buf[3], /* xhd  */
				 self->fdc_curhead,    /* head */
				 self->fdc_cmd_buf[4], /* sec */
				 self->fdc_exe_buf, len,
				 self->fdc_write_deleted, 
				 self->fdc_cmd_buf[0] & 0x20,
				 self->fdc_cmd_buf[0] & 0x40,
				 self->fdc_cmd_buf[0] & 0x80);

	fdc_xlt_error(self, rv);

        fdc_results_7(self);
        fdc_result_interrupt(self);
}




/* End of Execution Phase in a format command. Format the track.   */
static void fdc_format_end(FDC_765 *self)
{
        FDD_765 *fd = self->fdc_drive[self->fdc_curunit];
	fd_err_t rv;


	rv = fdd_format_track(fd, self->fdc_curhead, /* head */
			     self->fdc_cmd_buf[3],  /* Sectors/track */
			     self->fdc_exe_buf,    /* Track template */
			     self->fdc_cmd_buf[5]); /* Filler byte */

	fdc_xlt_error(self, rv);

        fdc_results_7(self);
        fdc_result_interrupt(self);
}



/* Called when execution phase finishes */
static void fdc_end_execution_phase(FDC_765 *self)
{
        fdc_byte cmd = self->fdc_cmd_buf[0] & 0x1F;

	self->fdc_msr = 0xD0;	/* Ready to return results */

	self->fdc_res_pos = 0;

        switch(cmd)
        {
                case 17:                          /* SCAN */
                case 25:
                case 30:  fdc_results_7(self);    /* fall through */
		case 12:
                case 6:   self->fdc_res_len = 7; 
			/* Do an fdc_result_interrupt, the command has 
			 * finished. Would normally happen on buffer
		 	 * exhaustion, but if you set the terminal count
			 * then the buffer doesn't get exhausted */
			  fdc_result_interrupt(self);	/* Results ready */
			  break;  			/* READ */

		case 9:
                case 5:   fdc_write_end(self);
                          self->fdc_res_len = 7; break;  /* WRITE */

                case 13:  fdc_format_end(self);
                          self->fdc_res_len = 7; break;  /* FORMAT */
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

	self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;
	self->fdc_lastidread = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->fdc_curunit];

	self->fdc_exe_len = MAX_SECTOR_LEN;

        if (!fdc_ready(self, fd)) err = FD_E_NOTRDY;
	else err = fdd_read_track(fd, 
		self->fdc_cmd_buf[2], self->fdc_cmd_buf[3],
		self->fdc_curhead,
		self->fdc_exe_buf, 
		&self->fdc_exe_len);

	if (err) fdc_xlt_error(self, err);

	fdc_results_7(self);
	if (err && err != FD_E_DATAERR)
	{
		fdc_end_execution_phase(self);
		fdc_result_interrupt(self);
		return;
	}

        fdc_exec_interrupt(self);
	self->fdc_msr = 0xF0;	/* Ready to transfer data */
	self->fdc_exe_pos = 0;
}


/* SPECIFY */
static void fdc_specify(FDC_765 *self)
{
        self->fdc_specify[0] = self->fdc_cmd_buf[1];
        self->fdc_specify[1] = self->fdc_cmd_buf[2];
        fdc_end_result_phase(self);
}

/* SENSE DRIVE STATUS */
static void fdc_sense_drive(FDC_765 *self)
{
	fdc_get_drive(self);
	fdc_get_st3(self);
	self->fdc_res_buf[0] = self->fdc_st3;
	self->fdc_res_len    = 1;
	fdc_end_execution_phase(self);
}


/* READ DATA
 * READ DELETED DATA 
 */

static void fdc_read(FDC_765 *self, int deleted)
{
	int err;
	FDD_765 *fd;

	self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;
	self->fdc_lastidread = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->fdc_curunit];
        self->fdc_exe_len = (128 << self->fdc_cmd_buf[5]);
        if (self->fdc_cmd_buf[8] < 255) 
		self->fdc_exe_len = self->fdc_cmd_buf[8];

	memset(self->fdc_exe_buf, 0, self->fdc_exe_len);

        if (!fdc_ready(self, fd)) err = FD_E_NOTRDY;
	else err = fdd_read_sector(fd, 
		self->fdc_cmd_buf[2], self->fdc_cmd_buf[3],
		self->fdc_curhead,
		self->fdc_cmd_buf[4], self->fdc_exe_buf, 
		self->fdc_exe_len, &deleted,
		self->fdc_cmd_buf[0] & 0x20,
		self->fdc_cmd_buf[0] & 0x40,
		self->fdc_cmd_buf[0] & 0x80);

	if (err) fdc_xlt_error(self, err);
	if (deleted) self->fdc_st2 |= 0x40;

	fdc_results_7(self);
	if (err && err != FD_E_DATAERR)
	{
		fdc_end_execution_phase(self);
		fdc_result_interrupt(self);
		return;
	}

        fdc_exec_interrupt(self);
	self->fdc_msr = 0xF0;	/* Ready to transfer data */
	self->fdc_exe_pos = 0;
}

/* WRITE DATA
 * WRITE DELETED DATA 
 */

static void fdc_write(FDC_765 *self, int deleted)
{
        int err = FD_E_OK;
	FDD_765 *fd;

        self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;
        self->fdc_lastidread = 0;
	self->fdc_write_deleted = deleted;

        fdc_get_drive(self);
	fd = self->fdc_drive[self->fdc_curunit];

        self->fdc_exe_len = (128 << self->fdc_cmd_buf[5]);
        if (self->fdc_cmd_buf[8] < 255)
                self->fdc_exe_len = self->fdc_cmd_buf[8];

        memset(self->fdc_exe_buf, 0, self->fdc_exe_len);

        if (!fdc_ready(self, fd))     err = FD_E_NOTRDY;
	else if (fd && fd->fd_readonly) err = FD_E_READONLY;

	if (err) 
	{
		fdc_xlt_error(self, err);
                self->fdc_msr = 0xD0;      /* Ready to return results */
                self->fdc_res_pos = 0;
		fdc_results_7(self);
		self->fdc_res_pos = 0;
		self->fdc_res_len = 7;
		fdc_result_interrupt(self);
	}
	else
	{
		fdc_exec_interrupt(self);
		self->fdc_msr = 0xB0;	/* Ready to receive data */
		self->fdc_exe_pos = 0;
	}
}

/* RECALIBRATE */
static void fdc_recalibrate(FDC_765 *self)
{
	FDD_765 *fd;

	self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;

	fdc_get_drive(self);
	self->fdc_lastidread = 0;

	fd = self->fdc_drive[self->fdc_curunit];

	fdc_end_result_phase(self);

	self->fdc_isr_countdown = SHORT_TIMEOUT;
	self->fdc_interrupting  = 4;		/* Interrupt: End of seek */
	self->fdc_st2 &= 0xFD;
	self->fdc_st0 |= 0x20;

	/* Seek the drive to track 0 */
        if (!fdc_ready(self, fd))
        {
                self->fdc_st0 |= 0x48;          /* Drive not ready */
        }
        else if ( fdd_seek_cylinder(fd, 0) )
        {
                /* Seek failed */
                self->fdc_st2 |= 2;
                self->fdc_st0 |= 0x40;
        }
}

/* SENSE INTERRUPT STATUS */
static void fdc_sense_int(FDC_765 *self)
{
        if (self->fdc_interrupting > 2) 
		/* FDC interrupted, and is ready to return status */
        {
		fdc_byte cyl = 0;
		if (self->fdc_drive[self->fdc_curunit]) 
			cyl = self->fdc_drive[self->fdc_curunit]->fd_cylinder;

		self->fdc_res_buf[0] = self->fdc_st0;
		self->fdc_res_buf[1] = cyl;
		self->fdc_res_len = 2;
		log_dprintf(7, "SENSE INTERRUPT STATUS: Return %02x %02x\n",
					self->fdc_st0, cyl);
        }
        else    /* FDC did not interrupt, error */
        {
                self->fdc_st0 = 0x80;
		self->fdc_res_buf[0] = self->fdc_st0;
                self->fdc_res_len = 1;
		log_dprintf(7, "SENSE INTERRUPT STATUS: Return 0x80\n");
        }
	fdc_end_execution_phase(self);

	/* Drop the interrupt line */
        self->fdc_isr_countdown = 0;
        self->fdc_interrupting = 0;
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

	self->fdc_res_len = 7;
	self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;

	fdc_get_drive(self);

	fd = self->fdc_drive[self->fdc_curunit];

        if (!fdc_ready(self, fd)) 
        {
                self->fdc_st0 |= 0x48;          /* Drive not ready */
        }
	else
	{
		ret=(*fd->fd_vtable->fdv_read_id)(fd, self->fdc_curhead,
				self->fdc_lastidread++, self->fdc_cmd_buf + 2);

		if (ret == FD_E_SEEKFAIL)
		{
			self->fdc_st1 |= 1;
			self->fdc_st0 |= 0x40;
		}
		if (ret == FD_E_NOADDR) 
		{
			self->fdc_st2 |= 1;
			self->fdc_st0 |= 0x40;
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

        self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;
        self->fdc_lastidread = 0;

        fdc_get_drive(self);
        fd = self->fdc_drive[self->fdc_curunit];

        memset(self->fdc_exe_buf, 0, MAX_SECTOR_LEN);

        if (!fdc_ready(self, fd))     err = FD_E_NOTRDY;
	else if (fd && fd->fd_readonly) err = FD_E_READONLY;

	if (err) 
	{
		fdc_xlt_error(self, err);
       		self->fdc_msr = 0xD0;      /* Ready to return results */
	        self->fdc_res_pos = 0;
		fdc_results_7(self);
		fdc_result_interrupt(self);
	}
	else
	{
		fdc_exec_interrupt(self);
		self->fdc_msr = 0xB0;	/* Ready to receive data */
		self->fdc_exe_pos = 0;
		self->fdc_exe_len = 4 * self->fdc_cmd_buf[3];
	}
}

/* SEEK */
static void fdc_seek(FDC_765 *self)
{
	int cylinder = self->fdc_cmd_buf[2];
	FDD_765 *fd;

	self->fdc_st0 = self->fdc_st1 = self->fdc_st1 = 0;
	fdc_get_drive(self);
	self->fdc_lastidread = 0;

	fdc_end_result_phase(self);
	self->fdc_isr_countdown = SHORT_TIMEOUT;
	self->fdc_interrupting  = 4;	/* End of seek */
	self->fdc_st2 &= 0xFD;
	self->fdc_st0 |= 0x20;

	fd = self->fdc_drive[self->fdc_curunit];

        if (!fdc_ready(self, fd))
        {
                self->fdc_st0 |= 0x48;          /* Drive not ready */
        }
	else if ( fdd_seek_cylinder(fd, cylinder) )
	{
		/* Seek failed */
		self->fdc_st2 |= 2;
		self->fdc_st0 |= 0x40;
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

        self->fdc_st0 = self->fdc_st1 = self->fdc_st2 = 0;
        self->fdc_lastidread = 0;

        fdc_get_drive(self);

        self->fdc_exe_len = (128 << self->fdc_cmd_buf[5]);
        if (self->fdc_cmd_buf[8] < 255)
                self->fdc_exe_len = self->fdc_cmd_buf[8];

        memset(self->fdc_exe_buf, 0, self->fdc_exe_len);

        err = fdd_read_sector(self->fdc_drive[self->fdc_curunit],
                self->fdc_cmd_buf[2], self->fdc_cmd_buf[3],
                self->fdc_curhead,
                self->fdc_cmd_buf[4], self->fdc_exe_buf,
                self->fdc_exe_len, 0, 
		self->fdc_cmd_buf[0] & 0x20,
		self->fdc_cmd_buf[0] & 0x40,
		self->fdc_cmd_buf[0] & 0x80);

        if (err) fdc_xlt_error(self, err);

        fdc_results_7(self);
        if (err && err != FD_E_DATAERR)
        {
                fdc_end_execution_phase(self);
                fdc_result_interrupt(self);
                return;
        }
        fdc_exec_interrupt(self);
	self->fdc_st2 |= 8;
        self->fdc_msr = 0xB0;      /* Ready to transfer data */
	self->fdc_exe_pos = 0;
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
	for (NC = 0; NC < bytes_in_cmd[self->fdc_cmd_buf[0] & 0x1F]; NC++)
		log_dprintf(5, "%02x ", self->fdc_cmd_buf[NC]);
	log_dprintf(5, "\n");
 /* */
	/* Check if the DOR (ugh!) is being used to force us to a 
	   different drive. */

	/* Reset "seek finished" flag */
	self->fdc_st0 &= 0xBF;	 
	switch(self->fdc_cmd_buf[0] & 0x1F)
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
					self->fdc_cmd_buf[0] & 0x1F);
			fdc_error(self);	break;
	}

}

/* Make the FDC drop its "interrupt" line */
static void fdc_clear_pending_interrupt(FDC_765 *self)
{
        if (self->fdc_interrupting > 0 && self->fdc_interrupting < 4)
        {
                self->fdc_isr_countdown = 0;
                self->fdc_interrupting = 0;
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
	if (self->fdc_msr & 0x20)	/* In execution phase */
	{
		curcmd = self->fdc_cmd_buf[0] & 0x1F;
		switch(curcmd)
		{
			case 17:
			case 25:	/* SCAN commands */
			case 30:
				fdc_scan_byte(self,
					self->fdc_exe_buf[self->fdc_exe_pos],
					value);
				break;
			default:	/* WRITE commands */
				self->fdc_exe_buf[self->fdc_exe_pos] = value;
				break;
		}
		++self->fdc_exe_pos;
		--self->fdc_exe_len;
                /* If at end of exec-phase, switch to result phase */
                if (!self->fdc_exe_len)
                {
                	fdc_end_execution_phase(self);
                        fdc_result_interrupt(self);
		}
                /* Interrupt SHORT_TIMEOUT cycles from now to say that the
                 * next byte is ready */
/* [JCE 8-3-2002] Changed > to >= for xjoyce 2.1.0 */
                if (self->fdc_interrupting >= 0 &&
                    self->fdc_interrupting < 3)
                {
                	self->fdc_isr_countdown = SHORT_TIMEOUT;
		}
		return;
	}
	if (self->fdc_cmd_id < 0)	/* FDC is idle */
	{
		self->fdc_cmd_id  = value;	/* Entering command phase */
		self->fdc_cmd_pos = 0;
		self->fdc_cmd_buf[0] = value;
		self->fdc_cmd_len = bytes_in_cmd[value & 0x1F] - 1;

		if (self->fdc_cmd_len     == 0) fdc_execute(self);
		else if (self->fdc_cmd_len < 0)	fdc_error(self);
		self->fdc_msr |= 0x10;	/* In a command */
		return;
	}
	/* FDC is not idle, nor in execution phase; so it must be 
	 * accepting a multibyte command */
	self->fdc_cmd_buf[++self->fdc_cmd_pos] = value;
	--self->fdc_cmd_len;
	if (!self->fdc_cmd_len) fdc_execute(self);
}


/* Read the FDC's main data register */
fdc_byte fdc_rd_data (FDC_765 *self)
{
	log_dprintf(5, "FDC: Read main data register, value = ");

	fdc_clear_pending_interrupt(self);
       	if (self->fdc_msr & 0x80) /* Ready to output data */
	{
		fdc_byte v;
		if (self->fdc_msr & 0x20) /* In exec phase */
		{
			/* Output an exec-phase byte */
			v = self->fdc_exe_buf[self->fdc_exe_pos++];
			--self->fdc_exe_len;
			/* If at end of exec-phase, switch to result phase */
			if (!self->fdc_exe_len)
			{
				fdc_end_execution_phase(self);
				fdc_result_interrupt(self);
			}
			/* Interrupt SHORT_TIMEOUT cycles from now to say that
			 * the next byte is ready */
/* [JCE 8-3-2002] Changed > to >= for xjoyce 2.1.0 */
			if (self->fdc_interrupting >= 0 &&
		            self->fdc_interrupting < 3) 
			{
				self->fdc_isr_countdown = SHORT_TIMEOUT;
			}
			log_dprintf(7, "fdc_interrupting=%d\n", self->fdc_interrupting);
			log_dprintf(5, "%c:%02x\n", self->fdc_isr_countdown ? 'E' : 'e', v);
			return v;
		}
		/* Not in execution phase. So we must be in the result phase */
		v = self->fdc_res_buf[self->fdc_res_pos++];
		--self->fdc_res_len;
		if (self->fdc_res_len == 0) fdc_end_result_phase(self);
		log_dprintf(5, "R:%02x  (%d remaining)\n", v, self->fdc_res_len);
		return v;
	}
	/* FDC is not ready to return data! */
	log_dprintf(5, "N:%02x\n", self->fdc_msr | (1 << self->fdc_curunit));
	return self->fdc_msr | (1 << self->fdc_curunit);
}


/* Read the FDC's main control register */
fdc_byte fdc_rd_stat (FDC_765 *self)
{
	log_dprintf(5, "FDC: Read main status: %02x\n", self->fdc_msr);
	return self->fdc_msr;
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
	if (newmotor[self->fdc_curunit] == oldmotor[self->fdc_curunit]) return;

	n = newmotor[self->fdc_curunit];
	/* The status of the active drive is changing. Wait for a while, then
	 * interrupt */

	log_dprintf(5, "FDC: queued interrupt for drive motor change.\n");
	self->fdc_isr_countdown = LONGER_TIMEOUT;

	if (n) self->fdc_st0 &= 0xF7;	/* Motor -> on ; drive is ready */
	else   self->fdc_st0 |= 8;	/* Motor -> off; drive not ready*/
	fdc_get_st3(self);	/* Recalculate ST3 */

	/* FDC is doing something and the motor has stopped! */
	if ((self->fdc_msr & 0xF0) != 0x80 && (n == 0))
	{
		log_dprintf(5, "FDC: Motor stopped during command.\n");
		self->fdc_st0 |= 0xC0;
		fdc_end_execution_phase(self);
	}
}

void fdc_tick(FDC_765 *fdc)
{
  if(fdc->fdc_isr_countdown > 0) {
    if((--fdc->fdc_isr_countdown == 0) && (fdc->fdc_isr != NULL)) {
      (*fdc->fdc_isr)(fdc, 1);
    } 
  }
}
