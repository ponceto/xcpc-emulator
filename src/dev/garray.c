/*
 * garray.c - Copyright (c) 2001-2013 Olivier Poncet
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
#include "garray.h"

static void gdev_garray_reset(GdevGArray *garray);
static void gdev_garray_clock(GdevGArray *garray);

G_DEFINE_TYPE(GdevGArray, gdev_garray, GDEV_TYPE_DEVICE)

/**
 * GdevGArray::class_init()
 *
 * @param garray_class specifies the GdevGArray class
 */
static void gdev_garray_class_init(GdevGArrayClass *garray_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(garray_class);

  device_class->reset = (GdevDeviceProc) gdev_garray_reset;
  device_class->clock = (GdevDeviceProc) gdev_garray_clock;
}

/**
 * GdevGArray::init()
 *
 * @param garray specifies the GdevGArray instance
 */
static void gdev_garray_init(GdevGArray *garray)
{
  int ix;

  for(ix = 0; ix < 256; ix++) {
    garray->mode0[ix] = ((ix & 0x80) >> 7) | ((ix & 0x08) >> 2)
                      | ((ix & 0x20) >> 3) | ((ix & 0x02) << 2)
                      | ((ix & 0x40) >> 2) | ((ix & 0x04) << 3)
                      | ((ix & 0x10) << 2) | ((ix & 0x01) << 7);
    garray->mode1[ix] = ((ix & 0x80) >> 7) | ((ix & 0x08) >> 2)
                      | ((ix & 0x40) >> 4) | ((ix & 0x04) << 1)
                      | ((ix & 0x20) >> 1) | ((ix & 0x02) << 4)
                      | ((ix & 0x10) << 2) | ((ix & 0x01) << 7);
    garray->mode2[ix] = ((ix & 0x80) >> 7) | ((ix & 0x40) >> 5)
                      | ((ix & 0x20) >> 3) | ((ix & 0x10) >> 1)
                      | ((ix & 0x08) << 1) | ((ix & 0x04) << 3)
                      | ((ix & 0x02) << 5) | ((ix & 0x01) << 7);
  }
  gdev_garray_reset(garray);
}

/**
 * GdevGArray::reset()
 *
 * @param garray specifies the GdevGArray instance
 */
static void gdev_garray_reset(GdevGArray *garray)
{
  int ix;

  garray->pen = 0x00;
  for(ix = 0; ix < 17; ix++) {
    garray->ink[ix] = 0x00;
  }
  garray->rom_cfg = 0x00;
  garray->ram_cfg = 0x00;
  garray->counter = 0x00;
  garray->delayed = 0x00;
}

/**
 * GdevGArray::clock()
 *
 * @param garray specifies the GdevGArray instance
 */
static void gdev_garray_clock(GdevGArray *garray)
{
}

/**
 * GdevGArray::new()
 *
 * @return the GdevGArray instance
 */
GdevGArray *gdev_garray_new(void)
{
  return(g_object_new(GDEV_TYPE_GARRAY, NULL));
}
