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

G_DEFINE_TYPE(GdevMC6845, gdev_mc6845, GDEV_TYPE_DEVICE)

/**
 * GdevMC6845::class_init()
 *
 * @param mc6845_class specifies the GdevMC6845 class
 */
static void gdev_mc6845_class_init(GdevMC6845Class *mc6845_class)
{
}

/**
 * GdevMC6845::init()
 *
 * @param mc6845 specifies the GdevMC6845 instance
 */
static void gdev_mc6845_init(GdevMC6845 *mc6845)
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
