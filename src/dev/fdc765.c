/*
 * fdc765.c - Copyright (c) 2001-2014 Olivier Poncet
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

static void gdev_fdc765_reset(GdevFDC765 *fdc765);
static void gdev_fdc765_rstat(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_wstat(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_rdata(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_wdata(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_end_of_cmd      (GdevFDC765 *fdc765);
static void gdev_fdc765_end_of_res      (GdevFDC765 *fdc765);
static void gdev_fdc765_rd_data         (GdevFDC765 *fdc765);
static void gdev_fdc765_rd_deleted_data (GdevFDC765 *fdc765);
static void gdev_fdc765_wr_data         (GdevFDC765 *fdc765);
static void gdev_fdc765_wr_deleted_data (GdevFDC765 *fdc765);
static void gdev_fdc765_rd_diagnostic   (GdevFDC765 *fdc765);
static void gdev_fdc765_rd_id           (GdevFDC765 *fdc765);
static void gdev_fdc765_wr_id           (GdevFDC765 *fdc765);
static void gdev_fdc765_scan_equ        (GdevFDC765 *fdc765);
static void gdev_fdc765_scan_lo_or_equ  (GdevFDC765 *fdc765);
static void gdev_fdc765_scan_hi_or_equ  (GdevFDC765 *fdc765);
static void gdev_fdc765_recalibrate     (GdevFDC765 *fdc765);
static void gdev_fdc765_sense_int_status(GdevFDC765 *fdc765);
static void gdev_fdc765_specify         (GdevFDC765 *fdc765);
static void gdev_fdc765_sense_drv_status(GdevFDC765 *fdc765);
static void gdev_fdc765_seek            (GdevFDC765 *fdc765);
static void gdev_fdc765_invalid_cmd     (GdevFDC765 *fdc765);

enum {
  FDC_IDLE,
  FDC_RD_DATA,
  FDC_RD_DELETED_DATA,
  FDC_WR_DATA,
  FDC_WR_DELETED_DATA,
  FDC_RD_DIAGNOSTIC,
  FDC_RD_ID,
  FDC_WR_ID,
  FDC_SCAN_EQU,
  FDC_SCAN_LO_OR_EQU,
  FDC_SCAN_HI_OR_EQU,
  FDC_RECALIBRATE,
  FDC_SENSE_INT_STATUS,
  FDC_SPECIFY,
  FDC_SENSE_DRV_STATUS,
  FDC_SEEK,
  FDC_INVALID_CMD
};

static struct {
  int command;
  int cmd_len;
  int res_len;
} fdc_cmd[32] = {
  { FDC_INVALID_CMD,      1, 1 }, /* [ 00 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 01 ]                        */
  { FDC_RD_DIAGNOSTIC,    9, 7 }, /* [ 02 ] RD DIAGNOSTIC          */
  { FDC_SPECIFY,          3, 1 }, /* [ 03 ] SPECIFY                */
  { FDC_SENSE_DRV_STATUS, 2, 1 }, /* [ 04 ] SENSE DRIVE STATUS     */
  { FDC_WR_DATA,          9, 7 }, /* [ 05 ] WR DATA                */
  { FDC_RD_DATA,          9, 7 }, /* [ 06 ] RD DATA                */
  { FDC_RECALIBRATE,      2, 1 }, /* [ 07 ] RECALIBRATE            */
  { FDC_SENSE_INT_STATUS, 1, 2 }, /* [ 08 ] SENSE INTERRUPT STATUS */
  { FDC_WR_DELETED_DATA,  9, 7 }, /* [ 09 ] WR DELETED DATA        */
  { FDC_RD_ID,            2, 7 }, /* [ 00 ] RD ID                  */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 11 ]                        */
  { FDC_RD_DELETED_DATA,  9, 7 }, /* [ 12 ] RD DELETED DATA        */
  { FDC_WR_ID,            6, 7 }, /* [ 13 ] WR ID                  */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 14 ]                        */
  { FDC_SEEK,             3, 1 }, /* [ 15 ] SEEK                   */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 16 ]                        */
  { FDC_SCAN_EQU,         9, 7 }, /* [ 17 ] SCAN EQUAL             */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 18 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 19 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 20 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 21 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 22 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 23 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 24 ]                        */
  { FDC_SCAN_LO_OR_EQU,   9, 7 }, /* [ 25 ] SCAN LOW OR EQUAL      */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 26 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 27 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 28 ]                        */
  { FDC_SCAN_HI_OR_EQU,   9, 7 }, /* [ 29 ] SCAN HIGH OR EQUAL     */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 30 ]                        */
  { FDC_INVALID_CMD,      1, 1 }, /* [ 31 ]                        */
};

static gboolean myfdc = FALSE;

G_DEFINE_TYPE(GdevFDC765, gdev_fdc765, G_TYPE_OBJECT)

/**
 * GdevFDC765::class_init()
 *
 * @param fdc765_class specifies the GdevFDC765 class
 */
static void gdev_fdc765_class_init(GdevFDC765Class *fdc765_class)
{
  fdc765_class->reset = gdev_fdc765_reset;
  fdc765_class->rstat = gdev_fdc765_rstat;
  fdc765_class->wstat = gdev_fdc765_wstat;
  fdc765_class->rdata = gdev_fdc765_rdata;
  fdc765_class->wdata = gdev_fdc765_wdata;
  g_type_class_add_private(fdc765_class, sizeof(FDC_765));
}

/**
 * GdevFDC765::init()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_init(GdevFDC765 *fdc765)
{
  fdc765->upd765 = NULL;
  fdc765->impl   = G_TYPE_INSTANCE_GET_PRIVATE(fdc765, GDEV_TYPE_FDC765, FDC_765);
  fdc_init_impl(fdc765->impl);
  gdev_fdc765_reset(fdc765);
}

/**
 * GdevFDC765::reset()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_reset(GdevFDC765 *fdc765)
{
  fdc765->unit_id   = 0x00;
  fdc765->head_id   = 0x00;
  fdc765->reg.cmd   = 0x00;
  fdc765->reg.msr   = 0x80;
  fdc765->reg.st0   = 0x00;
  fdc765->reg.st1   = 0x00;
  fdc765->reg.st2   = 0x00;
  fdc765->reg.st3   = 0x00;
  fdc765->reg.srt   = 0x00;
  fdc765->reg.hlt   = 0x00;
  fdc765->reg.hut   = 0x00;
  fdc765->reg.ndm   = 0x01;
  fdc765->cmd.len   = 0;
  fdc765->cmd.pos   = 0;
  fdc765->exe.len   = 0;
  fdc765->exe.pos   = 0;
  fdc765->res.len   = 0;
  fdc765->res.pos   = 0;
  fdc765->isr.state = 0;
  fdc765->isr.count = 0;
}

/**
 * GdevFDC765::rstat()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_rstat(GdevFDC765 *fdc765, guint8 *busptr)
{
  if((fdc765->impl != NULL) && (myfdc == FALSE)) {
    *busptr = fdc_rd_stat(fdc765->impl);
#ifdef DEBUG_FDC
    (void) printf("CPU <-- FDC : MSR=0x%02x\n", *busptr);
#endif
  }
  else {
    *busptr = fdc765->reg.msr;
#ifdef DEBUG_FDC
    (void) printf("CPU <-- FDC : MSR=0x%02x\n", *busptr);
#endif
  }
}

/**
 * GdevFDC765::wstat()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_wstat(GdevFDC765 *fdc765, guint8 *busptr)
{
}

/**
 * GdevFDC765::rdata()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_rdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if((fdc765->impl != NULL) && (myfdc == FALSE)) {
    *busptr = fdc_rd_data(fdc765->impl);
#ifdef DEBUG_FDC
    (void) printf("CPU <-- FDC : DAT=0x%02x\n", *busptr);
#endif
  }
  else {
    if(fdc765->reg.cmd == 0) {
      /* XXX */
    }
    if(fdc765->reg.cmd != 0) {
      fdc765->reg.msr = 0x90; /* 10010000: RDY + BSY */
      *busptr = fdc765->res.buf[fdc765->res.pos];
#ifdef DEBUG_FDC
      (void) printf("CPU <-- FDC : DAT=0x%02x\n", *busptr);
#endif
      if(++fdc765->res.pos >= fdc765->res.len) {
        gdev_fdc765_end_of_res(fdc765);
      }
    }
    else {
      fdc765->reg.msr = 0x80; /* 10000000: RDY */
    }
  }
}

/**
 * GdevFDC765::wdata()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_wdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if((fdc765->impl != NULL) && (myfdc == FALSE)) {
    fdc_wr_data(fdc765->impl, *busptr);
#ifdef DEBUG_FDC
    (void) printf("CPU --> FDC : DAT=0x%02x\n", *busptr);
#endif
  }
  else {
    if(fdc765->reg.cmd == 0) {
      fdc765->reg.cmd = fdc_cmd[*busptr & 31].command;
      fdc765->cmd.len = fdc_cmd[*busptr & 31].cmd_len;
      fdc765->cmd.pos = 0;
      fdc765->res.len = fdc_cmd[*busptr & 31].res_len;
      fdc765->res.pos = 0;
    }
    if(fdc765->reg.cmd != 0) {
      fdc765->reg.msr = 0x90; /* 10010000: RDY + BSY */
      fdc765->cmd.buf[fdc765->cmd.pos] = *busptr;
#ifdef DEBUG_FDC
      (void) printf("CPU --> FDC : DAT=0x%02x\n", *busptr);
#endif
      if(++fdc765->cmd.pos >= fdc765->cmd.len) {
        gdev_fdc765_end_of_cmd(fdc765);
      }
    }
    else {
      fdc765->reg.msr = 0x80; /* 10000000: RDY */
    }
  }
}

/**
 * GdevFDC765::end_of_cmd()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_end_of_cmd(GdevFDC765 *fdc765)
{
  switch(fdc765->reg.cmd) {
    case FDC_RD_DATA:          gdev_fdc765_rd_data         (fdc765); break;
    case FDC_RD_DELETED_DATA:  gdev_fdc765_rd_deleted_data (fdc765); break;
    case FDC_WR_DATA:          gdev_fdc765_wr_data         (fdc765); break;
    case FDC_WR_DELETED_DATA:  gdev_fdc765_wr_deleted_data (fdc765); break;
    case FDC_RD_DIAGNOSTIC:    gdev_fdc765_rd_diagnostic   (fdc765); break;
    case FDC_RD_ID:            gdev_fdc765_rd_id           (fdc765); break;
    case FDC_WR_ID:            gdev_fdc765_wr_id           (fdc765); break;
    case FDC_SCAN_EQU:         gdev_fdc765_scan_equ        (fdc765); break;
    case FDC_SCAN_LO_OR_EQU:   gdev_fdc765_scan_lo_or_equ  (fdc765); break;
    case FDC_SCAN_HI_OR_EQU:   gdev_fdc765_scan_hi_or_equ  (fdc765); break;
    case FDC_RECALIBRATE:      gdev_fdc765_recalibrate     (fdc765); break;
    case FDC_SENSE_INT_STATUS: gdev_fdc765_sense_int_status(fdc765); break;
    case FDC_SPECIFY:          gdev_fdc765_specify         (fdc765); break;
    case FDC_SENSE_DRV_STATUS: gdev_fdc765_sense_drv_status(fdc765); break;
    case FDC_SEEK:             gdev_fdc765_seek            (fdc765); break;
    default:                   gdev_fdc765_invalid_cmd     (fdc765); break;
  }
  fdc765->cmd.len = 0;
  fdc765->cmd.pos = 0;
}

/**
 * GdevFDC765::end_of_res()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_end_of_res(GdevFDC765 *fdc765)
{
  fdc765->reg.msr = 0x80;
  fdc765->res.len = 0;
  fdc765->res.pos = 0;
}

/**
 * GdevFDC765::rd_data()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_rd_data(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_rd_data\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::rd_deleted_data()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_rd_deleted_data(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_rd_deleted_data\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::wr_data()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_wr_data(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_wr_data\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::wr_deleted_data()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_wr_deleted_data(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_wr_deleted_data\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::rd_diagnostic()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_rd_diagnostic(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_rd_diagnostic\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::rd_id()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_rd_id(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_rd_id\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::wr_id()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_wr_id(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_wr_id\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::scan_equ()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_scan_equ(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_scan_equ\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::scan_lo_or_equ()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_scan_lo_or_equ(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_scan_lo_or_equ\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::scan_hi_or_equ()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_scan_hi_or_equ(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_scan_hi_or_equ\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::recalibrate()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_recalibrate(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  fdc765->reg.cmd = 0x00;
  fdc765->reg.msr = 0x80 | (1 << fdc765->unit_id);
  fdc765->reg.st0 = 0x20;
  fdc765->reg.st1 = 0x00;
  fdc765->reg.st2 = 0x00;
  fdc765->reg.st3 = 0x00;
  (void) fdd_seek_cylinder(fd, 0);
  (void) fprintf(stdout, "gdev_fdc765_recalibrate\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::sense_int_status()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_sense_int_status(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;
  int ready = 0;

  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
    if((fd->fd_vtable != NULL) && (fd->fd_vtable->fdv_ready != NULL)) {
      ready = (*fd->fd_vtable->fdv_ready)(fd);
    }
  }
  fdc765->reg.msr    = 0xd0 | (fdc765->reg.msr & 0x1f); /* RDY + DIR[FDC=>CPU]  */
  fdc765->reg.st0   |= (ready & 1) << 3;
  fdc765->res.buf[0] = fdc765->reg.st0;
  fdc765->res.buf[1] = fd->fd_cylinder;
  (void) fprintf(stdout, "gdev_fdc765_sense_int_status\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::specify()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_specify(GdevFDC765 *fdc765)
{
  fdc765->reg.cmd = 0x00;
  fdc765->reg.msr = 0x80;
  fdc765->reg.st0 = 0x00;
  fdc765->reg.st1 = 0x00;
  fdc765->reg.st2 = 0x00;
  fdc765->reg.st3 = 0x00;
  fdc765->reg.srt = (fdc765->cmd.buf[0] & 0xf0) >> 4;
  fdc765->reg.hlt = (fdc765->cmd.buf[1] & 0xfe) >> 1;
  fdc765->reg.hut = (fdc765->cmd.buf[0] & 0x0f) >> 0;
  fdc765->reg.ndm = (fdc765->cmd.buf[1] & 0x01) >> 0;
  (void) fprintf(stdout, "gdev_fdc765_specify\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::sense_drv_status()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_sense_drv_status(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_sense_drv_status\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::seek()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_seek(GdevFDC765 *fdc765)
{
  FDD_765 *fd = NULL;

  fdc765->unit_id = (fdc765->cmd.buf[1] & 0x03) >> 0;
  fdc765->head_id = (fdc765->cmd.buf[1] & 0x04) >> 2;
  if((fdc765->upd765 != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id] != NULL)
  && (fdc765->upd765->fdd[fdc765->unit_id]->impl != NULL)) {
    fd = fdc765->upd765->fdd[fdc765->unit_id]->impl;
  }
  (void) fprintf(stdout, "gdev_fdc765_seek\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::invalid_cmd()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 */
static void gdev_fdc765_invalid_cmd(GdevFDC765 *fdc765)
{
  fdc765->reg.msr    = 0xc0;                      /* RDY + DIR[FDC=>CPU]  */
  fdc765->reg.st0    = fdc765->res.buf[0] = 0x80; /* Invalid Command [IC] */
  fdc765->reg.st1    = fdc765->res.buf[1] = 0x00; /* ST1 resetted         */
  fdc765->reg.st2    = fdc765->res.buf[2] = 0x00; /* ST2 resetted         */
  fdc765->reg.st3    = fdc765->res.buf[3] = 0x00; /* ST3 resetted         */
  (void) fprintf(stdout, "gdev_fdc765_invalid_cmd\n");
  (void) fflush(stdout);
}

/**
 * GdevFDC765::new()
 *
 * @return the GdevFDC765 instance
 */
GdevFDC765 *gdev_fdc765_new(void)
{
  return(g_object_new(GDEV_TYPE_FDC765, NULL));
}
