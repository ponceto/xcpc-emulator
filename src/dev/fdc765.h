/*
 * fdc765.h - Copyright (c) 2001, 2006 Olivier Poncet
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

typedef struct _GdevFDC765      GdevFDC765;
typedef struct _GdevFDC765Class GdevFDC765Class;

struct _GdevFDC765 {
  GObject parent_instance;
  gpointer *impl;
};

struct _GdevFDC765Class {
  GObjectClass parent_class;
};

extern GType       gdev_fdc765_get_type (void);
extern GdevFDC765 *gdev_fdc765_new      (void);

G_END_DECLS

#endif /* __GDEV_FDC765_H__ */
