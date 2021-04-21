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

G_BEGIN_DECLS

#define GEM_EMULATOR_DATA(data) ((GemEmulatorData)(data))
#define GEM_EMULATOR_PROC(proc) ((GemEmulatorProc)(proc))

typedef gpointer GemEmulatorData;

typedef unsigned long (*GemEmulatorProc)(GemEmulatorData data, XEvent* event);

typedef struct _GemX11             GemX11;
typedef struct _GemMachine         GemMachine;
typedef struct _GemKeyboard        GemKeyboard;
typedef struct _GemJoystick        GemJoystick;
typedef struct _GemThrottledEvents GemThrottledEvents;

struct _GemX11
{
    Display* display;
    Screen*  screen;
    Window   window;
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
    GemEmulatorProc timer_proc;
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
    gpointer       input_id;
    int            js_id;
    int            js_axis_x;
    int            js_axis_y;
    int            js_button0;
    int            js_button1;
    int            js_buttons;
    unsigned short js_mapping[1024];
};

struct _GemThrottledEvents
{
    XEvent       list[256];
    unsigned int head;
    unsigned int tail;
};

G_END_DECLS

#endif /* __GEM_EMULATOR_IMPL_H__ */
