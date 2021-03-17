/*
 * fdd765.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __GDEV_FDD765_H__
#define __GDEV_FDD765_H__

#include <glib-object.h>
#include <dev/lib765.h>

G_BEGIN_DECLS

#define GDEV_TYPE_FDD765            (gdev_fdd765_get_type())
#define GDEV_FDD765(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_FDD765, GdevFDD765))
#define GDEV_FDD765_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_FDD765, GdevFDD765Class))
#define GDEV_IS_FDD765(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_FDD765))
#define GDEV_IS_FDD765_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_FDD765))
#define GDEV_FDD765_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_FDD765, GdevFDD765Class))

typedef struct _GdevFDD765      GdevFDD765;
typedef struct _GdevFDD765Class GdevFDD765Class;
typedef struct _GdevFDC765     *GdevFDCPTR;

struct _GdevFDD765 {
  GObject parent_instance;
  FDD_765 *impl;
};

struct _GdevFDD765Class {
  GObjectClass parent_class;
};

extern GType       gdev_fdd765_get_type (void);
extern GdevFDD765 *gdev_fdd765_new      (void);
extern void        gdev_fdd765_reset    (GdevFDD765 *fdd765);
extern void        gdev_fdd765_insert   (GdevFDD765 *fdd765, const gchar *filename);


G_END_DECLS

#endif /* __GDEV_FDD765_H__ */
