/*
 * EmulatorP.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XemEmulatorP_h__
#define __XemEmulatorP_h__

#include <X11/CoreP.h>
#include <Xem/Emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef XtPointer XemEmulatorData;
typedef unsigned long (*XemEmulatorProc)(XtPointer data, XEvent* event);

typedef struct _XemMachine
{
    XemEmulatorData instance;
    XemEmulatorProc create_proc;
    XemEmulatorProc destroy_proc;
    XemEmulatorProc realize_proc;
    XemEmulatorProc resize_proc;
    XemEmulatorProc expose_proc;
    XemEmulatorProc input_proc;
    XemEmulatorProc timer_proc;
} XemMachine;

typedef struct _XemKeyboard
{
    Boolean js_enabled;
    int     js_id;
    int     js_axis_x;
    int     js_axis_y;
    int     js_button0;
    int     js_button1;
} XemKeyboard;

typedef struct _XemJoystick
{
    String    device;
    int       fd;
    XtInputId input_id;
    int       js_id;
    int       js_axis_x;
    int       js_axis_y;
    int       js_button0;
    int       js_button1;
} XemJoystick;

typedef struct _XemEmulatorClassPart
{
    XtPointer extension;
} XemEmulatorClassPart;

typedef struct _XemEmulatorClassRec
{
    CoreClassPart        core_class;
    XemEmulatorClassPart emulator_class;
} XemEmulatorClassRec;

externalref XemEmulatorClassRec xemEmulatorClassRec;

typedef struct _XemEmulatorPart
{
    XemMachine     machine;
    XemKeyboard    keyboard;
    XemJoystick    joystick0;
    XemJoystick    joystick1;
    XtCallbackList hotkey_callback;
    XtIntervalId   timer;
    XEvent         event;
    XEvent         throttled_list[64];
    unsigned int   throttled_head;
    unsigned int   throttled_tail;
} XemEmulatorPart;

typedef struct _XemEmulatorRec
{
    CorePart        core;
    XemEmulatorPart emulator;
} XemEmulatorRec;

#ifdef __cplusplus
}
#endif

#endif /* __XemEmulatorP_h__ */
