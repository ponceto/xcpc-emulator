/*
 * cpckbd.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __GDEV_CPCKBD_H__
#define __GDEV_CPCKBD_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_CPCKBD            (gdev_cpckbd_get_type())
#define GDEV_CPCKBD(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_CPCKBD, GdevCPCKBD))
#define GDEV_CPCKBD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_CPCKBD, GdevCPCKBDClass))
#define GDEV_IS_CPCKBD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_CPCKBD))
#define GDEV_IS_CPCKBD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_CPCKBD))
#define GDEV_CPCKBD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_CPCKBD, GdevCPCKBDClass))

typedef struct _GdevCPCKBD      GdevCPCKBD;
typedef struct _GdevCPCKBDClass GdevCPCKBDClass;

struct _GdevCPCKBD {
  GdevDevice device;
  guint8 mods;
  guint8 line;
  guint8 bits[16];
};

struct _GdevCPCKBDClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_cpckbd_get_type (void);
extern GdevCPCKBD *gdev_cpckbd_new      (void);
extern void        gdev_cpckbd_qwerty   (GdevCPCKBD *cpckbd, XEvent *xevent);
extern void        gdev_cpckbd_azerty   (GdevCPCKBD *cpckbd, XEvent *xevent);

G_END_DECLS

#endif /* __GDEV_CPCKBD_H__ */
