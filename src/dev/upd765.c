/*
 * upd765.c - Copyright (c) 2001, 2006 Olivier Poncet
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

static void gdev_upd765_reset(GdevUPD765 *upd765);
static void gdev_upd765_clock(GdevUPD765 *upd765);

G_DEFINE_TYPE(GdevUPD765, gdev_upd765, GDEV_TYPE_DEVICE)

enum {
  FDC_IDLE_STATE,
  FDC_CMMD_PHASE,
  FDC_EXEC_PHASE,
  FDC_RSLT_PHASE,
};

enum {
  FDC_CMD_INVALID,
  FDC_CMD_RD_DATA,
  FDC_CMD_RD_DELETED_DATA,
  FDC_CMD_WR_DATA,
  FDC_CMD_WR_DELETED_DATA,
  FDC_CMD_RD_DIAGNOSTIC,
  FDC_CMD_RD_ID,
  FDC_CMD_WR_ID,
  FDC_CMD_SCAN_EQU,
  FDC_CMD_SCAN_LO_OR_EQU,
  FDC_CMD_SCAN_HI_OR_EQU,
  FDC_CMD_RECALIBRATE,
  FDC_CMD_SENSE_INTERRUPT_STATUS,
  FDC_CMD_SPECIFY,
  FDC_CMD_SENSE_DRIVE_STATUS,
  FDC_CMD_SEEK,
  FDC_CMD_VERSION
};

static struct {
  int cmd_id;
  int cmd_len;
} fdc_cmd[32] = {
  { FDC_CMD_INVALID,                0 }, /* 0x00 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x01 = [ *** INVALID CMD *** ] */
  { FDC_CMD_RD_DIAGNOSTIC,          9 }, /* 0x02 = RD DIAGNOSTIC           */
  { FDC_CMD_SPECIFY,                3 }, /* 0x03 = SPECIFY                 */
  { FDC_CMD_SENSE_DRIVE_STATUS,     2 }, /* 0x04 = SENSE DRIVE STATUS      */
  { FDC_CMD_WR_DATA,                9 }, /* 0x05 = WR DATA                 */
  { FDC_CMD_RD_DATA,                9 }, /* 0x06 = RD DATA                 */
  { FDC_CMD_RECALIBRATE,            2 }, /* 0x07 = RECALIBRATE             */
  { FDC_CMD_SENSE_INTERRUPT_STATUS, 1 }, /* 0x08 = SENSE INTERRUPT STATUS  */
  { FDC_CMD_WR_DELETED_DATA,        9 }, /* 0x09 = WR DELETED DATA         */
  { FDC_CMD_RD_ID,                  2 }, /* 0x0a = RD ID                   */
  { FDC_CMD_INVALID,                0 }, /* 0x0b = [ *** INVALID CMD *** ] */
  { FDC_CMD_RD_DELETED_DATA,        9 }, /* 0x0c = RD DELETED DATA         */
  { FDC_CMD_WR_ID,                  6 }, /* 0x0d = WR ID                   */
  { FDC_CMD_INVALID,                0 }, /* 0x0e = [ *** INVALID CMD *** ] */
  { FDC_CMD_SEEK,                   3 }, /* 0x0f = SEEK                    */
  { FDC_CMD_VERSION,                1 }, /* 0x10 = VERSION                 */
  { FDC_CMD_SCAN_EQU,               9 }, /* 0x11 = SCAN EQUAL              */
  { FDC_CMD_INVALID,                0 }, /* 0x12 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x13 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x14 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x15 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x16 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x17 = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x18 = [ *** INVALID CMD *** ] */
  { FDC_CMD_SCAN_LO_OR_EQU,         9 }, /* 0x19 = SCAN LOW OR EQUAL       */
  { FDC_CMD_INVALID,                0 }, /* 0x1a = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x1b = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x1c = [ *** INVALID CMD *** ] */
  { FDC_CMD_SCAN_HI_OR_EQU,         9 }, /* 0x1d = SCAN HIGH OR EQUAL      */
  { FDC_CMD_INVALID,                0 }, /* 0x1e = [ *** INVALID CMD *** ] */
  { FDC_CMD_INVALID,                0 }, /* 0x1f = [ *** INVALID CMD *** ] */
};

/**
 * GdevUPD765::class_init()
 *
 * @param upd765_class specifies the GdevUPD765 class
 */
static void gdev_upd765_class_init(GdevUPD765Class *upd765_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(upd765_class);

  device_class->reset = (GdevDeviceProc) gdev_upd765_reset;
  device_class->clock = (GdevDeviceProc) gdev_upd765_clock;
}

/**
 * GdevUPD765::init()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_init(GdevUPD765 *upd765)
{
  upd765->fdd[0] = NULL;
  upd765->fdd[1] = NULL;
  upd765->fdd[2] = NULL;
  upd765->fdd[3] = NULL;
  gdev_upd765_reset(upd765);
}

/**
 * GdevUPD765::reset()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_reset(GdevUPD765 *upd765)
{
  upd765->cur_fdd    = 0x00;
  upd765->reg_msr    = 0x80;
  upd765->reg_st0    = 0x00;
  upd765->reg_st1    = 0x00;
  upd765->reg_st2    = 0x00;
  upd765->reg_st3    = 0x00;
  upd765->cmd_buflen = 0x00;
  upd765->cmd_bufpos = 0x00;
  upd765->res_buflen = 0x00;
  upd765->res_bufpos = 0x00;
  upd765->state      = 0x00;
  upd765->data       = 0x00;
}

/**
 * GdevUPD765::clock()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_clock(GdevUPD765 *upd765)
{
}

/**
 * GdevUPD765::decode()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_decode(GdevUPD765 *upd765, guint8 data)
{
}

/**
 * GdevUPD765::new()
 *
 * @return the GdevUPD765 instance
 */
GdevUPD765 *gdev_upd765_new(void)
{
  return(g_object_new(GDEV_TYPE_UPD765, NULL));
}

/**
 * GdevUPD765::rd_ctrl()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
guint8 gdev_upd765_rd_ctrl(GdevUPD765 *upd765)
{
  return(upd765->reg_msr);
}

/**
 * GdevUPD765::wr_ctrl()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
void gdev_upd765_wr_ctrl(GdevUPD765 *upd765, guint8 data)
{
}

/**
 * GdevUPD765::rd_data()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
guint8 gdev_upd765_rd_data(GdevUPD765 *upd765)
{
  return(upd765->data);
}

/**
 * GdevUPD765::wr_data()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
void gdev_upd765_wr_data(GdevUPD765 *upd765, guint8 data)
{
  switch(upd765->state) {
    case FDC_IDLE_STATE:
      break;
    case FDC_CMMD_PHASE:
      break;
    case FDC_EXEC_PHASE:
      break;
    case FDC_RSLT_PHASE:
      break;
  }
  upd765->data = data;
}
