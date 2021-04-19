/*
 * EmulatorI.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XemEmulatorI_h__
#define __XemEmulatorI_h__

#include <Xem/Emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef XtPointer XemEmulatorData;

typedef unsigned long (*XemEmulatorProc)(XtPointer data, XEvent* event);

typedef struct _XemMachine         XemMachine;
typedef struct _XemKeyboard        XemKeyboard;
typedef struct _XemJoystick        XemJoystick;
typedef struct _XemThrottledEvents XemThrottledEvents;

struct _XemMachine
{
    XemEmulatorData instance;
    XemEmulatorProc create_proc;
    XemEmulatorProc destroy_proc;
    XemEmulatorProc realize_proc;
    XemEmulatorProc resize_proc;
    XemEmulatorProc expose_proc;
    XemEmulatorProc input_proc;
    XemEmulatorProc timer_proc;
};

struct _XemKeyboard
{
    Boolean js_enabled;
    int     js_id;
    int     js_axis_x;
    int     js_axis_y;
    int     js_button0;
    int     js_button1;
};

struct _XemJoystick
{
    String         device;
    String         identifier;
    int            fd;
    XtInputId      input_id;
    int            js_id;
    int            js_axis_x;
    int            js_axis_y;
    int            js_button0;
    int            js_button1;
    int            js_btn_select;
    int            js_btn_start;
    unsigned short js_mapping[1024];
};

struct _XemThrottledEvents
{
    XEvent       list[256];
    unsigned int head;
    unsigned int tail;
};

extern void         XemKeyboardConstruct       (Widget widget, XemKeyboard* keyboard, int id);
extern void         XemKeyboardDestruct        (Widget widget, XemKeyboard* keyboard);
static Boolean      XemKeyboardPreprocessEvent (Widget widget, XemKeyboard* keyboard, XEvent* event);
extern void         XemJoystickConstruct       (Widget widget, XemJoystick* joystick, const char* device, int id);
extern void         XemJoystickDestruct        (Widget widget, XemJoystick* joystick);
extern XemJoystick* XemJoystickLookupByFd      (Widget widget, int fd);
extern void         XemJoystickHandler         (Widget widget, int* source, XtInputId* input_id);

#ifdef __cplusplus
}
#endif

#endif /* __XemEmulatorI_h__ */
