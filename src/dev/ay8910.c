/*
 * ay8910.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "ay8910.h"

static void gdev_ay8910_reset(GdevAY8910 *ay8910);
static void gdev_ay8910_clock(GdevAY8910 *ay8910);

G_DEFINE_TYPE(GdevAY8910, gdev_ay8910, GDEV_TYPE_DEVICE)

/**
 * GdevAY8910::class_init()
 *
 * @param ay8910_class specifies the GdevAY8910 class
 */
static void gdev_ay8910_class_init(GdevAY8910Class *ay8910_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(ay8910_class);

  device_class->reset = (GdevDeviceProc) gdev_ay8910_reset;
  device_class->clock = (GdevDeviceProc) gdev_ay8910_clock;
}

/**
 * GdevAY8910::init()
 *
 * @param ay8910 specifies the GdevAY8910 instance
 */
static void gdev_ay8910_init(GdevAY8910 *ay8910)
{
  gdev_ay8910_reset(ay8910);
}

/**
 * GdevAY8910::reset()
 *
 * @param ay8910 specifies the GdevAY8910 instance
 */
static void gdev_ay8910_reset(GdevAY8910 *ay8910)
{
  ay8910->addr_reg     = 0x00;
  ay8910->reg_file[ 0] = 0x00;
  ay8910->reg_file[ 1] = 0x00;
  ay8910->reg_file[ 2] = 0x00;
  ay8910->reg_file[ 3] = 0x00;
  ay8910->reg_file[ 4] = 0x00;
  ay8910->reg_file[ 5] = 0x00;
  ay8910->reg_file[ 6] = 0x00;
  ay8910->reg_file[ 7] = 0x00;
  ay8910->reg_file[ 8] = 0x00;
  ay8910->reg_file[ 9] = 0x00;
  ay8910->reg_file[10] = 0x00;
  ay8910->reg_file[11] = 0x00;
  ay8910->reg_file[12] = 0x00;
  ay8910->reg_file[13] = 0x00;
  ay8910->reg_file[14] = 0x00;
  ay8910->reg_file[15] = 0x00;
}

/**
 * GdevAY8910::clock()
 *
 * @param ay8910 specifies the GdevAY8910 instance
 */
static void gdev_ay8910_clock(GdevAY8910 *ay8910)
{
}

/**
 * GdevAY8910::new()
 *
 * @return the GdevAY8910 instance
 */
GdevAY8910 *gdev_ay8910_new(void)
{
  return(g_object_new(GDEV_TYPE_AY8910, NULL));
}
