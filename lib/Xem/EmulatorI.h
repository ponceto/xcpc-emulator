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

#define XEM_EMULATOR_DATA(data) ((XemEmulatorData)(data))
#define XEM_EMULATOR_PROC(proc) ((XemEmulatorProc)(proc))

typedef XtPointer XemEmulatorData;

typedef unsigned long (*XemEmulatorProc)(XtPointer data, XEvent* event, void* extra);

typedef struct _XemX11      XemX11;
typedef struct _XemEvents   XemEvents;
typedef struct _XemMachine  XemMachine;
typedef struct _XemKeyboard XemKeyboard;
typedef struct _XemJoystick XemJoystick;

struct _XemX11
{
    Display* display;
    Window   window;
};

struct _XemEvents
{
    XEvent       last_rcv_event;
    XEvent       last_key_event;
    XEvent       list[256];
    unsigned int head;
    unsigned int tail;
};

struct _XemMachine
{
    XemEmulatorData instance;
    XemEmulatorProc create_proc;
    XemEmulatorProc destroy_proc;
    XemEmulatorProc realize_proc;
    XemEmulatorProc resize_proc;
    XemEmulatorProc expose_proc;
    XemEmulatorProc input_proc;
    XemEmulatorProc clock_proc;
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
    int            js_buttons;
    unsigned short js_mapping[1024];
};

extern XemX11*      XemX11Construct            (Widget widget, XemX11* x11);
extern XemX11*      XemX11Destruct             (Widget widget, XemX11* x11);
extern XemX11*      XemX11Realize              (Widget widget, XemX11* x11);
extern XemX11*      XemX11Unrealize            (Widget widget, XemX11* x11);

extern XemEvents*   XemEventsConstruct         (Widget widget, XemEvents* events);
extern XemEvents*   XemEventsDestruct          (Widget widget, XemEvents* events);
extern XemEvents*   XemEventsThrottle          (Widget widget, XemEvents* events, XEvent* event);
extern XemEvents*   XemEventsProcess           (Widget widget, XemEvents* events);
extern XEvent*      XemEventsCopyOrFill        (Widget widget, XemEvents* events, XEvent* event);

extern XemMachine*  XemMachineConstruct        (Widget widget, XemMachine* machine);
extern XemMachine*  XemMachineDestruct         (Widget widget, XemMachine* machine);
extern XemMachine*  XemMachineSanitize         (Widget widget, XemMachine* machine);

extern XemKeyboard* XemKeyboardConstruct       (Widget widget, XemKeyboard* keyboard, int id);
extern XemKeyboard* XemKeyboardDestruct        (Widget widget, XemKeyboard* keyboard);
static Boolean      XemKeyboardPreprocessEvent (Widget widget, XemKeyboard* keyboard, XEvent* event);

extern XemJoystick* XemJoystickConstruct       (Widget widget, XemJoystick* joystick, const char* device, int id);
extern XemJoystick* XemJoystickDestruct        (Widget widget, XemJoystick* joystick);
extern XemJoystick* XemJoystickLookupByFd      (Widget widget, int fd);
extern void         XemJoystickHandler         (Widget widget, int* source, XtInputId* input_id);
extern XemJoystick* XemJoystickDump            (Widget widget, XemJoystick* joystick, unsigned char button);

#ifdef __cplusplus
}
#endif

#endif /* __XemEmulatorI_h__ */
