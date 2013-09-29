/*
 * upd765.h - Copyright (c) 2001-2013 Olivier Poncet
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
#ifndef __GDEV_UPD765_H__
#define __GDEV_UPD765_H__

#include <dev/device.h>
#include <dev/lib765.h>
#include <dev/fdc765.h>
#include <dev/fdd765.h>

G_BEGIN_DECLS

#define GDEV_TYPE_UPD765            (gdev_upd765_get_type())
#define GDEV_UPD765(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_UPD765, GdevUPD765))
#define GDEV_UPD765_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_UPD765, GdevUPD765Class))
#define GDEV_IS_UPD765(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_UPD765))
#define GDEV_IS_UPD765_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_UPD765))
#define GDEV_UPD765_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_UPD765, GdevUPD765Class))

typedef struct _GdevUPD765      GdevUPD765;
typedef struct _GdevUPD765Class GdevUPD765Class;

struct _GdevUPD765 {
  GdevDevice device;
  GdevFDC765 *fdc;
  GdevFDD765 *fdd[4];
};

struct _GdevUPD765Class {
  GdevDeviceClass parent_class;
};

extern GType       gdev_upd765_get_type (void);
extern GdevUPD765 *gdev_upd765_new      (void);
extern void        gdev_upd765_set_fdc  (GdevUPD765 *upd765, GdevFDC765 *fdc765);
extern void        gdev_upd765_set_fdd  (GdevUPD765 *upd765, GdevFDD765 *fdd765, guint8 drive);
extern void        gdev_upd765_set_motor(GdevUPD765 *upd765, guint8 data);

G_END_DECLS

#endif /* __GDEV_UPD765_H__ */
