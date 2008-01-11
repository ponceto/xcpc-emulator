/*
 * driver.h - Copyright (c) 2001, 2006, 2007, 2008 Olivier Poncet
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
#ifndef __GDRV_DRIVER_H__
#define __GDRV_DRIVER_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GDRV_TYPE_DRIVER            (gdrv_driver_get_type())
#define GDRV_DRIVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDRV_TYPE_DRIVER, GdrvDriver))
#define GDRV_DRIVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDRV_TYPE_DRIVER, GdrvDriverClass))
#define GDRV_IS_DRIVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDRV_TYPE_DRIVER))
#define GDRV_IS_DRIVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDRV_TYPE_DRIVER))
#define GDRV_DRIVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDRV_TYPE_DRIVER, GdrvDriverClass))

typedef struct _GdrvDriver      GdrvDriver;
typedef struct _GdrvDriverClass GdrvDriverClass;

struct _GdrvDriver {
  GObject parent_instance;
  XImage  *ximage;
  Screen  *screen;
  Visual  *visual;
  Window   window;
  Colormap colmap;
  int      depth;
};

struct _GdrvDriverClass {
  GObjectClass parent_class;
  void (*reset)(GdrvDriver *driver);
  void (*clock)(GdrvDriver *driver);
  void (*event)(GdrvDriver *driver, XEvent *xevent);
};

extern GType       gdrv_driver_get_type (void);
extern GdrvDriver *gdrv_driver_new      (void);
extern void        gdrv_driver_reset    (GdrvDriver *driver);
extern void        gdrv_driver_clock    (GdrvDriver *driver);
extern void        gdrv_driver_event    (GdrvDriver *driver, XEvent *xevent);

G_END_DECLS

#endif /* __GDRV_DRIVER_H__ */
