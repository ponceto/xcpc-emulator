/*
 * mc6845.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "mc6845.h"

static void gdev_mc6845_reset(GdevMC6845 *mc6845);
static void gdev_mc6845_clock(GdevMC6845 *mc6845);

G_DEFINE_TYPE(GdevMC6845, gdev_mc6845, GDEV_TYPE_DEVICE)

/**
 * GdevMC6845::class_init()
 *
 * @param mc6845_class specifies the GdevMC6845 class
 */
static void gdev_mc6845_class_init(GdevMC6845Class *mc6845_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(mc6845_class);

  device_class->reset = (GdevDeviceProc) gdev_mc6845_reset;
  device_class->clock = (GdevDeviceProc) gdev_mc6845_clock;
}

/**
 * GdevMC6845::init()
 *
 * @param mc6845 specifies the GdevMC6845 instance
 */
static void gdev_mc6845_init(GdevMC6845 *mc6845)
{
  gdev_mc6845_reset(mc6845);
}

/**
 * GdevMC6845::reset()
 *
 * @param mc6845 specifies the GdevMC6845 instance
 */
static void gdev_mc6845_reset(GdevMC6845 *mc6845)
{
  mc6845->addr_reg     = 0x00; /* Address Register      [---xxxxx] */
  mc6845->reg_file[ 0] = 0x65; /* Horiz. Total          [hhhhhhhh] */
  mc6845->reg_file[ 1] = 0x50; /* Horiz. Displayed      [hhhhhhhh] */
  mc6845->reg_file[ 2] = 0x56; /* Horiz. Sync Position  [hhhhhhhh] */
  mc6845->reg_file[ 3] = 0x09; /* H // V Sync Width     [vvvvhhhh] */
  mc6845->reg_file[ 4] = 0x18; /* Verti. Total          [-vvvvvvv] */
  mc6845->reg_file[ 5] = 0x0a; /* Verti. Total Adjust   [---vvvvv] */
  mc6845->reg_file[ 6] = 0x18; /* Verti. Displayed      [-vvvvvvv] */
  mc6845->reg_file[ 7] = 0x18; /* Verti. Sync Position  [-vvvvvvv] */
  mc6845->reg_file[ 8] = 0x00; /* Interlace Mode        [------xx] */
  mc6845->reg_file[ 9] = 0x0b; /* Max Scan Line Address [---xxxxx] */
  mc6845->reg_file[10] = 0x00; /* Cursor Start          [-xxxxxxx] */
  mc6845->reg_file[11] = 0x0b; /* Cursor End            [---xxxxx] */
  mc6845->reg_file[12] = 0x00; /* Start Address (MSB)   [--xxxxxx] */
  mc6845->reg_file[13] = 0x80; /* Start Address (LSB)   [xxxxxxxx] */
  mc6845->reg_file[14] = 0x00; /* Cursor (MSB)          [--xxxxxx] */
  mc6845->reg_file[15] = 0x80; /* Cursor (LSB)          [xxxxxxxx] */
  mc6845->reg_file[16] = 0x00; /* Light Pen (MSB)       [--xxxxxx] */
  mc6845->reg_file[17] = 0x00; /* Light Pen (LSB)       [xxxxxxxx] */
  mc6845->hsync = 0;
  mc6845->vsync = 0;
}

/**
 * GdevMC6845::clock()
 *
 * @param mc6845 specifies the GdevMC6845 instance
 */
static void gdev_mc6845_clock(GdevMC6845 *mc6845)
{
}

/**
 * GdevMC6845::new()
 *
 * @return the GdevMC6845 instance
 */
GdevMC6845 *gdev_mc6845_new(void)
{
  return(g_object_new(GDEV_TYPE_MC6845, NULL));
}
