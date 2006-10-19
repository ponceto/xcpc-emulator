/*
 * z80cpu.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __GDEV_Z80CPU_H__
#define __GDEV_Z80CPU_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_Z80CPU            (gdev_z80cpu_get_type())
#define GDEV_Z80CPU(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_Z80CPU, GdevZ80CPU))
#define GDEV_Z80CPU_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_Z80CPU, GdevZ80CPUClass))
#define GDEV_IS_Z80CPU(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_Z80CPU))
#define GDEV_IS_Z80CPU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_Z80CPU))
#define GDEV_Z80CPU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_Z80CPU, GdevZ80CPUClass))

typedef struct _GdevZ80CPU      GdevZ80CPU;
typedef struct _GdevZ80CPUClass GdevZ80CPUClass;

struct _GdevZ80CPU {
  GdevDevice device;
};

struct _GdevZ80CPUClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_z80cpu_get_type (void);
extern GdevZ80CPU *gdev_z80cpu_new      (void);

G_END_DECLS

#endif /* __GDEV_Z80CPU_H__ */
