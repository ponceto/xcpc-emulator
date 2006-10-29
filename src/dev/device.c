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

G_DEFINE_TYPE(GdevDevice, gdev_device, G_TYPE_OBJECT)

/**
 * GdevDevice::class_init()
 *
 * @param device_class specifies the GdevDevice class
 */
static void gdev_device_class_init(GdevDeviceClass *device_class)
{
  device_class->reset = NULL;
  device_class->clock = NULL;
}

/**
 * GdevDevice::init()
 *
 * @param device specifies the GdevDevice instance
 */
static void gdev_device_init(GdevDevice *device)
{
}

/**
 * GdevDevice::new()
 *
 * @return the GdevDevice instance
 */
GdevDevice *gdev_device_new(void)
{
  return(g_object_new(GDEV_TYPE_DEVICE, NULL));
}

/**
 * GdevDevice::reset()
 *
 * @param device specifies the GdevDevice instance
 */
void gdev_device_reset(GdevDevice *device)
{
  if(GDEV_IS_DEVICE(device) != FALSE) {
    GdevDeviceClass *device_class = GDEV_DEVICE_GET_CLASS(device);
    if(device_class->reset != NULL) {
      (*device_class->reset)(device);
    }
  }
}

/**
 * GdevDevice::clock()
 *
 * @param device specifies the GdevDevice instance
 */
void gdev_device_clock(GdevDevice *device)
{
  if(GDEV_IS_DEVICE(device) != FALSE) {
    GdevDeviceClass *device_class = GDEV_DEVICE_GET_CLASS(device);
    if(device_class->clock != NULL) {
      (*device_class->clock)(device);
    }
  }
}
