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
  gdev_upd765_reset(upd765);
}

/**
 * GdevUPD765::reset()
 *
 * @param upd765 specifies the GdevUPD765 instance
 */
static void gdev_upd765_reset(GdevUPD765 *upd765)
{
  upd765->status = 0x80;
  upd765->motors = 0x00;
  upd765->data   = 0x00;
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
