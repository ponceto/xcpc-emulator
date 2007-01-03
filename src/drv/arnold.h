/*
 * arnold.h - Copyright (c) 2001, 2006, 2007 Olivier Poncet
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
#ifndef __GDRV_ARNOLD_H__
#define __GDRV_ARNOLD_H__

#include <drv/driver.h>
#include <dev/z80cpu.h>
#include <dev/garray.h>
#include <dev/cpcmem.h>
#include <dev/cpckbd.h>
#include <dev/mc6845.h>
#include <dev/ay8910.h>
#include <dev/upd765.h>
#include <dev/i8255.h>

G_BEGIN_DECLS

#define GDRV_TYPE_ARNOLD            (gdrv_arnold_get_type())
#define GDRV_ARNOLD(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDRV_TYPE_ARNOLD, GdrvArnold))
#define GDRV_ARNOLD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDRV_TYPE_ARNOLD, GdrvArnoldClass))
#define GDRV_IS_ARNOLD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDRV_TYPE_ARNOLD))
#define GDRV_IS_ARNOLD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDRV_TYPE_ARNOLD))
#define GDRV_ARNOLD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDRV_TYPE_ARNOLD, GdrvArnoldClass))

typedef struct _GdrvArnold        GdrvArnold;
typedef struct _GdrvArnoldClass   GdrvArnoldClass;
typedef struct _GdrvArnoldPrivate GdrvArnoldPrivate;

struct _GdrvArnold {
  GdrvDriver driver;
  GdrvArnoldPrivate *priv;
  GdevZ80CPU *z80cpu;
  GdevGArray *garray;
  GdevCPCKBD *cpckbd;
  GdevMC6845 *mc6845;
  GdevAY8910 *ay8910;
  GdevUPD765 *upd765;
  GdevI8255  *i8255;
};

struct _GdrvArnoldClass {
  GdrvDriverClass parent_class;
};

extern GType       gdrv_arnold_get_type (void);
extern GdrvArnold *gdrv_arnold_new      (void);

G_END_DECLS

#endif /* __GDRV_ARNOLD_H__ */
