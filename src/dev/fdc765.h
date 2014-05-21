/*
 * fdc765.h - Copyright (c) 2001-2014 Olivier Poncet
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
#ifndef __GDEV_FDC765_H__
#define __GDEV_FDC765_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GDEV_TYPE_FDC765            (gdev_fdc765_get_type())
#define GDEV_FDC765(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_FDC765, GdevFDC765))
#define GDEV_FDC765_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_FDC765, GdevFDC765Class))
#define GDEV_IS_FDC765(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_FDC765))
#define GDEV_IS_FDC765_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_FDC765))
#define GDEV_FDC765_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_FDC765, GdevFDC765Class))

typedef struct _GdevUPD765      GdevFDCPXY;
typedef struct _GdevFDC765      GdevFDC765;
typedef struct _GdevFDC765Class GdevFDC765Class;

struct _GdevFDC765 {
  GObject parent_instance;
  GdevFDCPXY *upd765;
  FDC_765    *impl;
  guint8 unit_id;
  guint8 head_id;
  struct {
    gint   cmd;       /* current command           */
    guint8 msr;       /* main status register      */
    guint8 st0;       /* status register: ST0      */
    guint8 st1;       /* status register: ST1      */
    guint8 st2;       /* status register: ST2      */
    guint8 st3;       /* status register: ST3      */
    guint8 srt;       /* Step Rate Time            */
    guint8 hlt;       /* Head Load Time            */
    guint8 hut;       /* Head Unload Time          */
    guint8 ndm;       /* Non-DMA Mode              */
  } reg;
  struct {
    guint8 buf[16];   /* command buffer            */
    gint   len;       /* command buffer length     */
    gint   pos;       /* command buffer position   */
  } cmd;
  struct {
    guint8 buf[8192]; /* execution buffer          */
    gint   len;       /* execution buffer length   */
    gint   pos;       /* execution buffer position */
  } exe;
  struct {
    guint8 buf[16];   /* result buffer             */
    gint   len;       /* result buffer length      */
    gint   pos;       /* result buffer position    */
  } res;
  struct {
    guint state;      /* interrupt state           */
    guint count;      /* interrupt countdown       */
  } isr;
};

struct _GdevFDC765Class {
  GObjectClass parent_class;
  void (*reset)(GdevFDC765 *fdc765);
  void (*rstat)(GdevFDC765 *fdc765, guint8 *busptr);
  void (*wstat)(GdevFDC765 *fdc765, guint8 *busptr);
  void (*rdata)(GdevFDC765 *fdc765, guint8 *busptr);
  void (*wdata)(GdevFDC765 *fdc765, guint8 *busptr);
};

extern GType       gdev_fdc765_get_type (void);
extern GdevFDC765 *gdev_fdc765_new      (void);

G_END_DECLS

#endif /* __GDEV_FDC765_H__ */
