/*
 * ay8910.h - Copyright (c) 2001-2014 Olivier Poncet
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
#ifndef __GDEV_AY8910_H__
#define __GDEV_AY8910_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_AY8910            (gdev_ay8910_get_type())
#define GDEV_AY8910(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_AY8910, GdevAY8910))
#define GDEV_AY8910_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_AY8910, GdevAY8910Class))
#define GDEV_IS_AY8910(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_AY8910))
#define GDEV_IS_AY8910_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_AY8910))
#define GDEV_AY8910_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_AY8910, GdevAY8910Class))

typedef struct _GdevAY8910      GdevAY8910;
typedef struct _GdevAY8910Class GdevAY8910Class;

struct _GdevAY8910 {
  GdevDevice device;
  guint8 addr_reg;
  guint8 reg_file[16];
};

struct _GdevAY8910Class {
  GdevDeviceClass parent_class;
};

extern GType       gdev_ay8910_get_type (void);
extern GdevAY8910 *gdev_ay8910_new      (void);

G_END_DECLS

#endif /* __GDEV_AY8910_H__ */
