/*
 * mc6845.h - Copyright (c) 2001, 2006, 2007, 2008, 2009 Olivier Poncet
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
#ifndef __GDEV_MC6845_H__
#define __GDEV_MC6845_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_MC6845            (gdev_mc6845_get_type())
#define GDEV_MC6845(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_MC6845, GdevMC6845))
#define GDEV_MC6845_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_MC6845, GdevMC6845Class))
#define GDEV_IS_MC6845(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_MC6845))
#define GDEV_IS_MC6845_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_MC6845))
#define GDEV_MC6845_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_MC6845, GdevMC6845Class))

typedef struct _GdevMC6845      GdevMC6845;
typedef struct _GdevMC6845Class GdevMC6845Class;

struct _GdevMC6845 {
  GdevDevice device;
  guint8 addr_reg;
  guint8 reg_file[18];
  guint8 h_ctr;
  guint8 r_ctr;
  guint8 v_ctr;
  guint  h_syn, h_syn_ctr;
  guint  v_syn, v_syn_ctr;
  /* User functions */
  void (*hsync)(GdevMC6845 *mc6845);
  void (*vsync)(GdevMC6845 *mc6845);
};

struct _GdevMC6845Class {
  GdevDeviceClass parent_class;
};

extern GType       gdev_mc6845_get_type (void);
extern GdevMC6845 *gdev_mc6845_new      (void);

G_END_DECLS

#endif /* __GDEV_MC6845_H__ */
