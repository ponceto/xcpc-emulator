/*
 * i8255.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "i8255.h"

static void gdev_i8255_debug(GdevI8255 *i8255);
static void gdev_i8255_reset(GdevI8255 *i8255);
static void gdev_i8255_clock(GdevI8255 *i8255);

G_DEFINE_TYPE(GdevI8255, gdev_i8255, GDEV_TYPE_DEVICE)

/**
 * GdevI8255::class_init()
 *
 * @param i8255_class specifies the GdevI8255 class
 */
static void gdev_i8255_class_init(GdevI8255Class *i8255_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(i8255_class);

  device_class->debug = (GdevDeviceProc) gdev_i8255_debug;
  device_class->reset = (GdevDeviceProc) gdev_i8255_reset;
  device_class->clock = (GdevDeviceProc) gdev_i8255_clock;
}

/**
 * GdevI8255::init()
 *
 * @param i8255 specifies the GdevI8255 instance
 */
static void gdev_i8255_init(GdevI8255 *i8255)
{
  gdev_i8255_reset(i8255);
}

/**
 * GdevI8255::debug()
 *
 * @param i8255 specifies the GdevI8255 instance
 */
static void gdev_i8255_debug(GdevI8255 *i8255)
{
}

/**
 * GdevI8255::reset()
 *
 * @param i8255 specifies the GdevI8255 instance
 */
static void gdev_i8255_reset(GdevI8255 *i8255)
{
}

/**
 * GdevI8255::clock()
 *
 * @param i8255 specifies the GdevI8255 instance
 */
static void gdev_i8255_clock(GdevI8255 *i8255)
{
}

/**
 * GdevI8255::new()
 *
 * @return the GdevI8255 instance
 */
GdevI8255 *gdev_i8255_new(void)
{
  return(g_object_new(GDEV_TYPE_I8255, NULL));
}
