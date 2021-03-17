/*
 * fdc765.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __GDEV_FDC765_H__
#define __GDEV_FDC765_H__

#include <glib-object.h>
#include <dev/lib765.h>

G_BEGIN_DECLS

#define GDEV_TYPE_FDC765            (gdev_fdc765_get_type())
#define GDEV_FDC765(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_FDC765, GdevFDC765))
#define GDEV_FDC765_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_FDC765, GdevFDC765Class))
#define GDEV_IS_FDC765(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_FDC765))
#define GDEV_IS_FDC765_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_FDC765))
#define GDEV_FDC765_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_FDC765, GdevFDC765Class))

typedef struct _GdevFDC765      GdevFDC765;
typedef struct _GdevFDC765Class GdevFDC765Class;
typedef struct _GdevFDD765     *GdevFDDPTR;

struct _GdevFDC765 {
  GObject parent_instance;
  FDC_765 *impl;
};

struct _GdevFDC765Class {
  GObjectClass parent_class;
};

extern GType       gdev_fdc765_get_type (void);
extern GdevFDC765 *gdev_fdc765_new      (void);
extern void        gdev_fdc765_reset    (GdevFDC765 *fdc765);
extern void        gdev_fdc765_rstat    (GdevFDC765 *fdc765, guint8 *busptr);
extern void        gdev_fdc765_wstat    (GdevFDC765 *fdc765, guint8 *busptr);
extern void        gdev_fdc765_rdata    (GdevFDC765 *fdc765, guint8 *busptr);
extern void        gdev_fdc765_wdata    (GdevFDC765 *fdc765, guint8 *busptr);
extern void        gdev_fdc765_motor    (GdevFDC765 *fdc765, guint8 state);
extern void        gdev_fdc765_attach   (GdevFDC765 *fdc765, GdevFDDPTR fdd765, guint8 drive);

G_END_DECLS

#endif /* __GDEV_FDC765_H__ */
