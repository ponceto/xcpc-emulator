/*
 * gtkemulatorogl.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK_EMULATOR_OGL_H__
#define __GTK_EMULATOR_OGL_H__

#include <gtk3ui/gtkemulatortypes.h>

G_BEGIN_DECLS

#define GTK_TYPE_EMULATOR_OGL            (gtk_emulator_ogl_get_type ())
#define GTK_EMULATOR_OGL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_EMULATOR_OGL, GtkEmulatorOGL))
#define GTK_EMULATOR_OGL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_EMULATOR_OGL, GtkEmulatorOGLClass))
#define GTK_IS_EMULATOR_OGL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_EMULATOR_OGL))
#define GTK_IS_EMULATOR_OGL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_EMULATOR_OGL))
#define GTK_EMULATOR_OGL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_EMULATOR_OGL, GtkEmulatorOGLClass))

typedef struct _GtkEmulatorOGL      GtkEmulatorOGL;
typedef struct _GtkEmulatorOGLClass GtkEmulatorOGLClass;

struct _GtkEmulatorOGL
{
    GtkGLArea   parent;
    GemVideo    video;
    GemEvents   events;
    GemKeyboard keyboard;
    GemJoystick joystick0;
    GemJoystick joystick1;
    GemBackend  backend;
    guint       minimum_width;
    guint       minimum_height;
    guint       natural_width;
    guint       natural_height;
    guint       timer;
};

struct _GtkEmulatorOGLClass
{
    GtkGLAreaClass parent_class;
    void (*sig_hotkey)(GtkEmulatorOGL*, KeySym keysym);
};

extern GType      gtk_emulator_ogl_get_type               (void) G_GNUC_CONST;
extern GtkWidget* gtk_emulator_ogl_new                    (void);
extern void       gtk_emulator_ogl_shutdown               (GtkWidget* widget);
extern void       gtk_emulator_ogl_set_backend            (GtkWidget* widget, const GemBackend* backend);
extern void       gtk_emulator_ogl_set_joystick           (GtkWidget* widget, int id, const char* device);
extern gboolean   gtk_emulator_ogl_get_joystick_emulation (GtkWidget* widget);
extern void       gtk_emulator_ogl_set_joystick_emulation (GtkWidget* widget, gboolean enabled);

G_END_DECLS

#endif /* __GTK_EMULATOR_OGL_H__ */
