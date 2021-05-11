/*
 * gememulator-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __GEM_EMULATOR_IMPL_H__
#define __GEM_EMULATOR_IMPL_H__

#include <gtk/gtkx.h>
#include <glib-unix.h>

G_BEGIN_DECLS

#define GEM_EMULATOR_DATA(data) ((GemEmulatorData)(data))
#define GEM_EMULATOR_PROC(proc) ((GemEmulatorProc)(proc))

typedef gpointer GemEmulatorData;

typedef unsigned long (*GemEmulatorProc)(GemEmulatorData data, XEvent* event, void* extra);

typedef struct _GemX11      GemX11;
typedef struct _GemEvents   GemEvents;
typedef struct _GemMachine  GemMachine;
typedef struct _GemKeyboard GemKeyboard;
typedef struct _GemJoystick GemJoystick;

struct _GemX11
{
    Display* display;
    Window   window;
};

struct _GemEvents
{
    XEvent       last_rcv_event;
    XEvent       last_key_event;
    XEvent       list[256];
    unsigned int head;
    unsigned int tail;
};

struct _GemMachine
{
    GemEmulatorData instance;
    GemEmulatorProc create_proc;
    GemEmulatorProc destroy_proc;
    GemEmulatorProc realize_proc;
    GemEmulatorProc resize_proc;
    GemEmulatorProc expose_proc;
    GemEmulatorProc input_proc;
    GemEmulatorProc clock_proc;
};

struct _GemKeyboard
{
    gboolean js_enabled;
    int      js_id;
    int      js_axis_x;
    int      js_axis_y;
    int      js_button0;
    int      js_button1;
};

struct _GemJoystick
{
    gchar*         device;
    gchar*         identifier;
    int            fd;
    guint          input_id;
    int            js_id;
    int            js_axis_x;
    int            js_axis_y;
    int            js_button0;
    int            js_button1;
    int            js_buttons;
    unsigned short js_mapping[1024];
};

extern GemX11*      gem_x11_construct         (GtkWidget* widget, GemX11* x11);
extern GemX11*      gem_x11_destruct          (GtkWidget* widget, GemX11* x11);
extern GemX11*      gem_x11_realize           (GtkWidget* widget, GemX11* x11);
extern GemX11*      gem_x11_unrealize         (GtkWidget* widget, GemX11* x11);

extern GemEvents*   gem_events_construct      (GtkWidget* widget, GemEvents* events);
extern GemEvents*   gem_events_destruct       (GtkWidget* widget, GemEvents* events);
extern GemEvents*   gem_events_throttle       (GtkWidget* widget, GemEvents* events, XEvent* event);
extern GemEvents*   gem_events_process        (GtkWidget* widget, GemEvents* events);
extern XEvent*      gem_events_copy_or_fill   (GtkWidget* widget, GemEvents* events, XEvent* event);

extern GemMachine*  gem_machine_construct     (GtkWidget* widget, GemMachine* machine);
extern GemMachine*  gem_machine_destruct      (GtkWidget* widget, GemMachine* machine);
extern GemMachine*  gem_machine_sanitize      (GtkWidget* widget, GemMachine* machine);

extern GemKeyboard* gem_keyboard_construct    (GtkWidget* widget, GemKeyboard* keyboard, int id);
extern GemKeyboard* gem_keyboard_destruct     (GtkWidget* widget, GemKeyboard* keyboard);
extern gboolean     gem_keyboard_preprocess   (GtkWidget* widget, GemKeyboard* keyboard, XEvent* event);

extern GemJoystick* gem_joystick_construct    (GtkWidget* widget, GemJoystick* joystick, const char* device, int id);
extern GemJoystick* gem_joystick_destruct     (GtkWidget* widget, GemJoystick* joystick);
extern GemJoystick* gem_joystick_lookup_by_fd (GtkWidget* widget, int fd);
extern gboolean     gem_joystick_handler      (gint fd, GIOCondition condition, GtkWidget* widget);

G_END_DECLS

#endif /* __GEM_EMULATOR_IMPL_H__ */
