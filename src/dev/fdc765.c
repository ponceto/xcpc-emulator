/*
 * fdc765.c - Copyright (c) 2001, 2006 Olivier Poncet
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

G_DEFINE_TYPE(GdevFDC765, gdev_fdc765, G_TYPE_OBJECT)

/**
 * GdevFDC765::class_init()
 *
 * @param fdc765_class specifies the GdevFDC765 class
 */
static void gdev_fdc765_class_init(GdevFDC765Class *fdc765_class)
{
}

/**
 * GdevFDC765::init()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_init(GdevFDC765 *fdc765)
{
  fdc765->impl = (gpointer) fdc_new();
}

/**
 * GdevFDC765::new()
 *
 * @return the GdevFDC765 instance
 */
GdevFDC765 *gdev_fdc765_new(void)
{
  return(g_object_new(GDEV_TYPE_FDC765, NULL));
}
