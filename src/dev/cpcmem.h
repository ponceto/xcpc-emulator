/*
 * cpcmem.h - Copyright (c) 2001-2013 Olivier Poncet
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
#ifndef __GDEV_CPCMEM_H__
#define __GDEV_CPCMEM_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_CPCMEM            (gdev_cpcmem_get_type())
#define GDEV_CPCMEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_CPCMEM, GdevCPCMEM))
#define GDEV_CPCMEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_CPCMEM, GdevCPCMEMClass))
#define GDEV_IS_CPCMEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_CPCMEM))
#define GDEV_IS_CPCMEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_CPCMEM))
#define GDEV_CPCMEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_CPCMEM, GdevCPCMEMClass))

typedef struct _GdevCPCMEM      GdevCPCMEM;
typedef struct _GdevCPCMEMClass GdevCPCMEMClass;

struct _GdevCPCMEM {
  GdevDevice device;
  guint8 data[16384];
};

struct _GdevCPCMEMClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_cpcmem_get_type      (void);
extern GdevCPCMEM *gdev_cpcmem_new           (void);
extern GdevCPCMEM *gdev_cpcmem_new_from_file (gchar *filename, gulong offset);

G_END_DECLS

#endif /* __GDEV_CPCMEM_H__ */
