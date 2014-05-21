/*
 * i8255.h - Copyright (c) 2001-2014 Olivier Poncet
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
#ifndef __GDEV_I8255_H__
#define __GDEV_I8255_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_I8255            (gdev_i8255_get_type())
#define GDEV_I8255(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_I8255, GdevI8255))
#define GDEV_I8255_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_I8255, GdevI8255Class))
#define GDEV_IS_I8255(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_I8255))
#define GDEV_IS_I8255_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_I8255))
#define GDEV_I8255_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_I8255, GdevI8255Class))

typedef struct _GdevI8255      GdevI8255;
typedef struct _GdevI8255Class GdevI8255Class;

struct _GdevI8255 {
  GdevDevice device;
  guint8 control;
  guint8 port_a;
  guint8 port_b;
  guint8 port_c;
};

struct _GdevI8255Class {
  GdevDeviceClass parent_class;
};

extern GType      gdev_i8255_get_type (void);
extern GdevI8255 *gdev_i8255_new      (void);

G_END_DECLS

#endif /* __GDEV_I8255_H__ */
