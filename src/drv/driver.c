/*
 * driver.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "driver.h"

G_DEFINE_TYPE(GemuDriver, gemu_driver, G_TYPE_OBJECT)

/**
 * GemuDriver::class_init()
 *
 * @param driver_class specifies the GemuDriver class
 */
static void gemu_driver_class_init(GemuDriverClass *driver_class)
{
}

/**
 * GemuDriver::init()
 *
 * @param driver specifies the GemuDriver instance
 */
static void gemu_driver_init(GemuDriver *driver)
{
}

/**
 * GemuDriver::new()
 *
 * @return the GemuDriver instance
 */
GemuDriver *gemu_driver_new(void)
{
  return(g_object_new(GEMU_TYPE_DRIVER, NULL));
}
