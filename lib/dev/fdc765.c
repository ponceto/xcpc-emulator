/*
 * fdc765.c - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fdc765.h"
#include "fdd765.h"

G_DEFINE_TYPE(GdevFDC765, gdev_fdc765, G_TYPE_OBJECT)

/**
 * GdevFDC765::class_init()
 *
 * @param fdc765_class specifies the GdevFDC765 class
 */
static void gdev_fdc765_class_init(GdevFDC765Class *fdc765_class)
{
  g_type_class_add_private(fdc765_class, sizeof(FDC_765));
}

/**
 * GdevFDC765::init()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_init(GdevFDC765 *fdc765)
{
  fdc765->impl   = G_TYPE_INSTANCE_GET_PRIVATE(fdc765, GDEV_TYPE_FDC765, FDC_765);
  fdc_init_impl(fdc765->impl);
  gdev_fdc765_reset(fdc765);
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

/**
 * GdevFDC765::reset()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
void gdev_fdc765_reset(GdevFDC765 *fdc765)
{
    /* TODO */
}

/**
 * GdevFDC765::rstat()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param busptr specifies the data bus pointer
 */
void gdev_fdc765_rstat(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    *busptr = fdc_rd_stat(fdc765->impl);
  }
}

/**
 * GdevFDC765::wstat()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param busptr specifies the data bus pointer
 */
void gdev_fdc765_wstat(GdevFDC765 *fdc765, guint8 *busptr)
{
    /* TODO */
}

/**
 * GdevFDC765::rdata()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param busptr specifies the data bus pointer
 */
void gdev_fdc765_rdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    *busptr = fdc_rd_data(fdc765->impl);
  }
}

/**
 * GdevFDC765::wdata()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param busptr specifies the data bus pointer
 */
void gdev_fdc765_wdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    fdc_wr_data(fdc765->impl, *busptr);
  }
}

/**
 * GdevFDC765::motor()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param state specifies the motor state
 */
void gdev_fdc765_motor(GdevFDC765 *fdc765, guint8 state)
{
  if(fdc765->impl != NULL) {
    fdc_set_motor(fdc765->impl, state);
  }
}

/**
 * GdevFDC765::motor()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 * @param drive specifies the drive id
 */
void gdev_fdc765_attach(GdevFDC765 *fdc765, GdevFDDPTR fdd765, guint8 drive)
{
  if((fdc765->impl != NULL) && (fdd765->impl != NULL)) {
    fdc765->impl->fdc_drive[drive] = fdd765->impl;
  }
}
