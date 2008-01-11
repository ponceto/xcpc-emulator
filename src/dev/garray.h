/*
 * garray.h - Copyright (c) 2001, 2006, 2007, 2008 Olivier Poncet
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
#ifndef __GDEV_GARRAY_H__
#define __GDEV_GARRAY_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_GARRAY            (gdev_garray_get_type())
#define GDEV_GARRAY(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_GARRAY, GdevGArray))
#define GDEV_GARRAY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_GARRAY, GdevGArrayClass))
#define GDEV_IS_GARRAY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_GARRAY))
#define GDEV_IS_GARRAY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_GARRAY))
#define GDEV_GARRAY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_GARRAY, GdevGArrayClass))

typedef struct _GdevGArray      GdevGArray;
typedef struct _GdevGArrayClass GdevGArrayClass;

struct _GdevGArray {
  GdevDevice device;
  guint8 mode0[256];
  guint8 mode1[256];
  guint8 mode2[256];
  guint8 pen;
  guint8 ink[17];
  guint8 rom_cfg;
  guint8 ram_cfg;
  guint8 counter;
  guint8 gen_irq;
  guint8 delayed;
};

struct _GdevGArrayClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_garray_get_type (void);
extern GdevGArray *gdev_garray_new      (void);

G_END_DECLS

#endif /* __GDEV_GARRAY_H__ */
