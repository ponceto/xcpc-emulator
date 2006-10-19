/*
 * device.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "device.h"

G_DEFINE_TYPE(GemuDevice, gemu_device, G_TYPE_OBJECT)

/**
 * GemuDevice::class_init()
 *
 * @param device_class specifies the GemuDevice class
 */
static void gemu_device_class_init(GemuDeviceClass *device_class)
{
}

/**
 * GemuDevice::init()
 *
 * @param device specifies the GemuDevice instance
 */
static void gemu_device_init(GemuDevice *device)
{
}

/**
 * GemuDevice::new()
 *
 * @return the GemuDevice instance
 */
GemuDevice *gemu_device_new(void)
{
  return(g_object_new(GEMU_TYPE_DEVICE, NULL));
}
