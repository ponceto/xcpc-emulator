/*
 * arnold.c - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "arnold.h"

struct _GdrvArnoldPrivate {
  struct timeval timer1;
  struct timeval timer2;
  GTimer *gtimer;
  int num_frames;
  int drw_frames;
};

#define GDRV_ARNOLD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GDRV_TYPE_ARNOLD, GdrvArnoldPrivate))

static void gdrv_arnold_reset   (GdrvDriver *driver);
static void gdrv_arnold_clock   (GdrvDriver *driver);
static void gdrv_arnold_event   (GdrvDriver *driver, XEvent *xevent);
static void gdrv_arnold_dispose (GObject    *object);
static void gdrv_arnold_finalize(GObject    *object);

G_DEFINE_TYPE(GdrvArnold, gdrv_arnold, GDRV_TYPE_DRIVER)

/**
 * GdrvArnold::class_init()
 *
 * @param arnold_class specifies the GdrvArnold class
 */
static void gdrv_arnold_class_init(GdrvArnoldClass *arnold_class)
{
  GdrvDriverClass *driver_class = (GdrvDriverClass *) arnold_class;
  GObjectClass    *object_class = (GObjectClass    *) arnold_class;

  driver_class->reset    = gdrv_arnold_reset;
  driver_class->clock    = gdrv_arnold_clock;
  driver_class->event    = gdrv_arnold_event;
  object_class->dispose  = gdrv_arnold_dispose;
  object_class->finalize = gdrv_arnold_finalize;

  g_type_class_add_private(arnold_class, sizeof(GdrvArnoldPrivate));
}

/**
 * GdrvArnold::init()
 *
 * @param arnold specifies the GdrvArnold instance
 */
static void gdrv_arnold_init(GdrvArnold *arnold)
{
  arnold->priv = GDRV_ARNOLD_GET_PRIVATE(arnold);
  (void) gettimeofday(&arnold->priv->timer1, NULL);
  (void) gettimeofday(&arnold->priv->timer2, NULL);
  arnold->priv->gtimer = g_timer_new();
  arnold->priv->num_frames = 0;
  arnold->priv->drw_frames = 0;
  arnold->z80cpu = gdev_z80cpu_new();
  arnold->garray = gdev_garray_new();
  arnold->cpckbd = gdev_cpckbd_new();
  arnold->mc6845 = gdev_mc6845_new();
  arnold->ay8910 = gdev_ay8910_new();
  arnold->upd765 = gdev_upd765_new();
  arnold->i8255  = gdev_i8255_new();
}

/**
 * GdrvArnold::reset()
 *
 * @param driver specifies the GdrvDriver instance
 */
static void gdrv_arnold_reset(GdrvDriver *driver)
{
  GdrvArnold *arnold = GDRV_ARNOLD(driver);

  if(arnold->z80cpu != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->z80cpu));
  }
  if(arnold->garray != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->garray));
  }
  if(arnold->cpckbd != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->cpckbd));
  }
  if(arnold->mc6845 != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->mc6845));
  }
  if(arnold->ay8910 != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->ay8910));
  }
  if(arnold->upd765 != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->upd765));
  }
  if(arnold->i8255 != NULL) {
    gdev_device_reset(GDEV_DEVICE(arnold->i8255));
  }
}

/**
 * GdrvArnold::clock()
 *
 * @param driver specifies the GdrvDriver instance
 */
static void gdrv_arnold_clock(GdrvDriver *driver)
{
}

/**
 * GdrvArnold::event()
 *
 * @param driver specifies the GdrvDriver instance
 * @param xevent specifies the XEvent structure
 */
static void gdrv_arnold_event(GdrvDriver *driver, XEvent *xevent)
{
  switch(xevent->type) {
    case KeyPress:
      break;
    case KeyRelease:
      break;
    case ButtonPress:
      break;
    case ButtonRelease:
      break;
    case MotionNotify:
      break;
    case Expose:
      if(driver->window == None) {
        XWindowAttributes xwinattr;
        if(XGetWindowAttributes(xevent->xexpose.display, xevent->xexpose.window, &xwinattr) != 0) {
          driver->ximage = NULL;
          driver->screen = xwinattr.screen;
          driver->visual = xwinattr.visual;
          driver->window = xevent->xexpose.window;
          driver->colmap = xwinattr.colormap;
          driver->depth  = xwinattr.depth;
        }
      }
      break;
    default:
      break;
  }
}

/**
 * GdrvArnold::dispose()
 *
 * @param object specifies the GObject instance
 */
static void gdrv_arnold_dispose(GObject *object)
{
  GdrvArnold *arnold = GDRV_ARNOLD(object);

  if(arnold->z80cpu != NULL) {
    GObject *z80cpu = G_OBJECT(arnold->z80cpu);
    arnold->z80cpu = NULL; g_object_unref(z80cpu);
  }
  if(arnold->garray != NULL) {
    GObject *garray = G_OBJECT(arnold->garray);
    arnold->garray = NULL; g_object_unref(garray);
  }
  if(arnold->cpckbd != NULL) {
    GObject *cpckbd = G_OBJECT(arnold->cpckbd);
    arnold->cpckbd = NULL; g_object_unref(cpckbd);
  }
  if(arnold->mc6845 != NULL) {
    GObject *mc6845 = G_OBJECT(arnold->mc6845);
    arnold->mc6845 = NULL; g_object_unref(mc6845);
  }
  if(arnold->ay8910 != NULL) {
    GObject *ay8910 = G_OBJECT(arnold->ay8910);
    arnold->ay8910 = NULL; g_object_unref(ay8910);
  }
  if(arnold->upd765 != NULL) {
    GObject *upd765 = G_OBJECT(arnold->upd765);
    arnold->upd765 = NULL; g_object_unref(upd765);
  }
  if(arnold->i8255 != NULL) {
    GObject *i8255 = G_OBJECT(arnold->i8255);
    arnold->i8255 = NULL; g_object_unref(i8255);
  }
  if(G_OBJECT_CLASS(gdrv_arnold_parent_class)->dispose != NULL) {
    (*G_OBJECT_CLASS(gdrv_arnold_parent_class)->dispose)(object);
  }
}

/**
 * GdrvArnold::finalize()
 *
 * @param object specifies the GObject instance
 */
static void gdrv_arnold_finalize(GObject *object)
{
  GdrvArnold *arnold = GDRV_ARNOLD(object);

  if(arnold->priv->gtimer != NULL) {
    g_timer_destroy(arnold->priv->gtimer);
    arnold->priv->gtimer = (GTimer *) NULL;
  }
  if(G_OBJECT_CLASS(gdrv_arnold_parent_class)->finalize != NULL) {
    (*G_OBJECT_CLASS(gdrv_arnold_parent_class)->finalize)(object);
  }
}

/**
 * GdrvArnold::new()
 *
 * @return the GdrvArnold instance
 */
GdrvArnold *gdrv_arnold_new(void)
{
  return(g_object_new(GDRV_TYPE_ARNOLD, NULL));
}
