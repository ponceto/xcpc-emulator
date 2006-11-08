/*
 * fdd765.c - Copyright (c) 2001, 2006 Olivier Poncet
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

static void gdev_fdd765_reset(GdevFDD765 *fdd765);

G_DEFINE_TYPE(GdevFDD765, gdev_fdd765, G_TYPE_OBJECT)

/**
 * GdevFDD765::class_init()
 *
 * @param fdd765_class specifies the GdevFDD765 class
 */
static void gdev_fdd765_class_init(GdevFDD765Class *fdd765_class)
{
  fdd765_class->reset = gdev_fdd765_reset;
}

/**
 * GdevFDD765::init()
 *
 * @param fdd765 specifies the GdevFDD765 instance
 */
static void gdev_fdd765_init(GdevFDD765 *fdd765)
{
  fdd765->upd765 = NULL;
  fdd765->impl   = (gpointer) fd_newldsk();
  gdev_fdd765_reset(fdd765);
}

/**
 * GdevFDD765::reset()
 *
 * @param fdd765 specifies the GdevFDD765 instance
 */
static void gdev_fdd765_reset(GdevFDD765 *fdd765)
{
}

/**
 * GdevFDD765::new()
 *
 * @return the GdevFDD765 instance
 */
GdevFDD765 *gdev_fdd765_new(void)
{
  return(g_object_new(GDEV_TYPE_FDD765, NULL));
}

/**
 * GdevFDD765::insert()
 *
 * @param fdd765 specifies the GdevFDD765 instance
 * @param dsk_fn specifies the disk image filename
 */
void gdev_fdd765_insert(GdevFDD765 *fdd765, gchar *dsk_fn)
{
  if(dsk_fn != NULL) {
    fdl_setfilename((FDRV_PTR) fdd765->impl, dsk_fn);
  }
  else {
    fd_eject((FDRV_PTR) fdd765->impl);
  }
}
