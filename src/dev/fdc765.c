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

static void gdev_fdc765_reset(GdevFDC765 *fdc765);
static void gdev_fdc765_rstat(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_wstat(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_rdata(GdevFDC765 *fdc765, guint8 *busptr);
static void gdev_fdc765_wdata(GdevFDC765 *fdc765, guint8 *busptr);

G_DEFINE_TYPE(GdevFDC765, gdev_fdc765, G_TYPE_OBJECT)

/**
 * GdevFDC765::class_init()
 *
 * @param fdc765_class specifies the GdevFDC765 class
 */
static void gdev_fdc765_class_init(GdevFDC765Class *fdc765_class)
{
  fdc765_class->reset = gdev_fdc765_reset;
  fdc765_class->rstat = gdev_fdc765_rstat;
  fdc765_class->wstat = gdev_fdc765_wstat;
  fdc765_class->rdata = gdev_fdc765_rdata;
  fdc765_class->wdata = gdev_fdc765_wdata;
}

/**
 * GdevFDC765::init()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_init(GdevFDC765 *fdc765)
{
  fdc765->upd765 = NULL;
  fdc765->impl   = (gpointer) fdc_new();
  gdev_fdc765_reset(fdc765);
}

/**
 * GdevFDC765::reset()
 *
 * @param fdc765 specifies the GdevFDC765 instance
 */
static void gdev_fdc765_reset(GdevFDC765 *fdc765)
{
  fdc765->reg.xyz   = 0;
  fdc765->reg.msr   = 0x80;
  fdc765->reg.st0   = 0x00;
  fdc765->reg.st1   = 0x00;
  fdc765->reg.st2   = 0x00;
  fdc765->reg.st3   = 0x00;
  fdc765->cmd.len   = 0;
  fdc765->cmd.pos   = 0;
  fdc765->exe.len   = 0;
  fdc765->exe.pos   = 0;
  fdc765->res.len   = 0;
  fdc765->res.pos   = 0;
  fdc765->isr.state = 0;
  fdc765->isr.count = 0;
}

/**
 * GdevFDC765::rstat()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_rstat(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    *busptr = fdc_read_ctrl((FDC_PTR) fdc765->impl);
  }
  else {
    *busptr = fdc765->reg.msr;
  }
}

/**
 * GdevFDC765::wstat()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_wstat(GdevFDC765 *fdc765, guint8 *busptr)
{
}

/**
 * GdevFDC765::rdata()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_rdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    *busptr = fdc_read_data((FDC_PTR) fdc765->impl);
  }
  else {
    switch(fdc765->reg.xyz) {
      case 0x02: /* EXE PHASE */
        if(fdc765->exe.pos < fdc765->exe.len) {
          *busptr = fdc765->exe.buf[fdc765->exe.pos];
          if(++fdc765->exe.pos >= fdc765->exe.len) {
            /* TODO: do something here */
            fdc765->exe.len = 0;
            fdc765->exe.pos = 0;
          }
        }
        break;
      case 0x03: /* RES PHASE */
        if(fdc765->res.pos < fdc765->res.len) {
          *busptr = fdc765->res.buf[fdc765->res.pos];
          if(++fdc765->res.pos >= fdc765->res.len) {
            /* TODO: do something here */
            fdc765->res.len = 0;
            fdc765->res.pos = 0;
          }
        }
        break;
      default:
        break;
    }
  }
}

/**
 * GdevFDC765::wdata()
 *
 * @param fdc765 specifies the GdevUPD765 instance
 * @param busptr specifies the data bus pointer
 */
static void gdev_fdc765_wdata(GdevFDC765 *fdc765, guint8 *busptr)
{
  if(fdc765->impl != NULL) {
    fdc_write_data((FDC_PTR) fdc765->impl, *busptr);
  }
  else {
    switch(fdc765->reg.xyz) {
      case 0x02: /* EXE PHASE */
        if(fdc765->exe.pos < fdc765->exe.len) {
          fdc765->exe.buf[fdc765->exe.pos] = *busptr;
          if(++fdc765->exe.pos >= fdc765->exe.len) {
            /* TODO: do something here */
            fdc765->exe.len = 0;
            fdc765->exe.pos = 0;
          }
        }
        break;
      case 0x01: /* CMD PHASE */
        if(fdc765->cmd.pos < fdc765->cmd.len) {
          fdc765->cmd.buf[fdc765->cmd.pos] = *busptr;
          if(++fdc765->cmd.pos >= fdc765->cmd.len) {
            /* TODO: do something here */
            fdc765->cmd.len = 0;
            fdc765->cmd.pos = 0;
          }
        }
        break;
      default:
        break;
    }
  }
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
