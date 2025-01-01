/*
 * gtkemulator.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __GTK_EMULATOR_H__
#define __GTK_EMULATOR_H__

#include <gtk3ui/gtkemulatortypes.h>

G_BEGIN_DECLS

#define GTK_TYPE_EMULATOR            (gtk_emulator_get_type ())
#define GTK_EMULATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_EMULATOR, GtkEmulator))
#define GTK_EMULATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_EMULATOR, GtkEmulatorClass))
#define GTK_IS_EMULATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_EMULATOR))
#define GTK_IS_EMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_EMULATOR))
#define GTK_EMULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_EMULATOR, GtkEmulatorClass))

typedef struct _GtkEmulator      GtkEmulator;
typedef struct _GtkEmulatorClass GtkEmulatorClass;

struct _GtkEmulator
{
    GtkWidget   widget;
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

struct _GtkEmulatorClass
{
    GtkWidgetClass parent_class;
    void (*sig_hotkey)(GtkEmulator*, KeySym keysym);
};

extern GType      gtk_emulator_get_type     (void) G_GNUC_CONST;
extern GtkWidget* gtk_emulator_new          (void);
extern void       gtk_emulator_set_backend  (GtkWidget* widget, const GemBackend* backend);
extern void       gtk_emulator_set_joystick (GtkWidget* widget, int id, const char* device);

G_END_DECLS

#endif /* __GTK_EMULATOR_H__ */
