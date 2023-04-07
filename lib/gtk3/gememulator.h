/*
 * gememulator.h - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifndef __GEM_EMULATOR_H__
#define __GEM_EMULATOR_H__

#include <gtk3/gememulator-impl.h>

G_BEGIN_DECLS

#define GEM_TYPE_EMULATOR            (gem_emulator_get_type ())
#define GEM_EMULATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEM_TYPE_EMULATOR, GemEmulator))
#define GEM_EMULATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GEM_TYPE_EMULATOR, GemEmulatorClass))
#define GEM_IS_EMULATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEM_TYPE_EMULATOR))
#define GEM_IS_EMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GEM_TYPE_EMULATOR))
#define GEM_EMULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GEM_TYPE_EMULATOR, GemEmulatorClass))

typedef struct _GemEmulator      GemEmulator;
typedef struct _GemEmulatorClass GemEmulatorClass;

struct _GemEmulator
{
    GtkWidget   widget;
    GemVideo    video;
    GemAudio    audio;
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

struct _GemEmulatorClass
{
    GtkWidgetClass parent_class;
    void (*sig_hotkey)(GemEmulator*, KeySym keysym);
};

extern GType      gem_emulator_get_type     (void) G_GNUC_CONST;
extern GtkWidget* gem_emulator_new          (void);
extern void       gem_emulator_set_backend  (GtkWidget* widget, const GemBackend* backend);
extern void       gem_emulator_set_joystick (GtkWidget* widget, int id, const char* device);

G_END_DECLS

#endif /* __GEM_EMULATOR_H__ */
