/*
 * driver.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __GEMU_DRIVER_H__
#define __GEMU_DRIVER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GEMU_TYPE_DRIVER            (gemu_driver_get_type())
#define GEMU_DRIVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GEMU_TYPE_DRIVER, GemuDriver))
#define GEMU_DRIVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GEMU_TYPE_DRIVER, GemuDriverClass))
#define GEMU_IS_DRIVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GEMU_TYPE_DRIVER))
#define GEMU_IS_DRIVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GEMU_TYPE_DRIVER))
#define GEMU_DRIVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GEMU_TYPE_DRIVER, GemuDriverClass))

typedef struct _GemuDriver      GemuDriver;
typedef struct _GemuDriverClass GemuDriverClass;

struct _GemuDriver {
  GObject parent_instance;
};

struct _GemuDriverClass {
  GObjectClass parent_class;
};

extern GType       gemu_driver_get_type (void);
extern GemuDriver *gemu_driver_new      (void);

G_END_DECLS

#endif /* __GEMU_DRIVER_H__ */
