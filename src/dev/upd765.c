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
#include <libdsk.h>
#include <765.h>
#include "upd765.h"

static void gdev_upd765_reset(GdevUPD765 *upd765);
static void gdev_upd765_clock(GdevUPD765 *upd765);

G_DEFINE_TYPE(GdevUPD765, gdev_upd765, GDEV_TYPE_DEVICE)

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
  upd765->fdc    = NULL; /* Floppy Disc Controller */
  upd765->fdd[0] = NULL; /* Floppy Disc Drive #0   */
  upd765->fdd[1] = NULL; /* Floppy Disc Drive #1   */
  upd765->fdd[2] = NULL; /* Floppy Disc Drive #2   */
  upd765->fdd[3] = NULL; /* Floppy Disc Drive #3   */
  gdev_upd765_reset(upd765);
}

/**
 * GdevUPD765::reset()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_reset(GdevUPD765 *upd765)
{
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
 * GdevUPD765::new()
 *
 * @return the GdevUPD765 instance
 */
GdevUPD765 *gdev_upd765_new(void)
{
  return(g_object_new(GDEV_TYPE_UPD765, NULL));
}

/**
 * GdevUPD765::set_fdc()
 *
 * @param upd765 specifies the GdevUPD765 instance
 * @param fdc765 specifies the GdevFDC765 instance
 */
void gdev_upd765_set_fdc(GdevUPD765 *upd765, GdevFDC765 *fdc765)
{
  upd765->fdc    = fdc765;
  fdc765->upd765 = upd765;
}

/**
 * GdevUPD765::set_fdd()
 *
 * @param upd765 specifies the GdevUPD765 instance
 * @param fdd765 specifies the GdevFDD765 instance
 */
void gdev_upd765_set_fdd(GdevUPD765 *upd765, GdevFDD765 *fdd765, guint8 drive)
{
  upd765->fdd[drive] = fdd765;
  fdd765->upd765     = upd765;
  fdc_setdrive((FDC_PTR) upd765->fdc->impl, drive, (FDRV_PTR) fdd765->impl);
}

/**
 * GdevUPD765::set_motor()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
void gdev_upd765_set_motor(GdevUPD765 *upd765, guint8 data)
{
  fdc_set_motor((FDC_PTR) upd765->fdc->impl, data);
}
