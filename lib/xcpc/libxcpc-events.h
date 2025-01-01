/*
 * events.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_LIBXCPC_EVENTS_H__
#define __XCPC_LIBXCPC_EVENTS_H__

#ifdef __cplusplus
extern "C" {
#endif

enum _XcpcEventType
{
    XCPC_INVALID_EVENT  = -1,
    XCPC_RESERVED_EVENT =  0,
    XCPC_CREATE_WINDOW  =  1,
    XCPC_DELETE_WINDOW  =  2,
    XCPC_RESIZE_WINDOW  =  3,
    XCPC_EXPOSE_WINDOW  =  4,
    XCPC_KEY_PRESS      =  5,
    XCPC_KEY_RELEASE    =  6,
    XCPC_BUTTON_PRESS   =  7,
    XCPC_BUTTON_RELEASE =  8,
    XCPC_MOTION_NOTIFY  =  9,
};

enum _XcpcModifierType
{
    XCPC_SHIFT_MASK       = (1 << 0),
    XCPC_LOCK_MASK        = (1 << 1),
    XCPC_CONTROL_MASK     = (1 << 2),
    XCPC_MOD1_MASK        = (1 << 3),
    XCPC_MOD2_MASK        = (1 << 4),
    XCPC_MOD3_MASK        = (1 << 5),
    XCPC_MOD4_MASK        = (1 << 6),
    XCPC_MOD5_MASK        = (1 << 7),
    XCPC_BUTTON1_MASK     = (1 << 8),
    XCPC_BUTTON2_MASK     = (1 << 9),
    XCPC_BUTTON3_MASK     = (1 << 10),
    XCPC_BUTTON4_MASK     = (1 << 11),
    XCPC_BUTTON5_MASK     = (1 << 12),
    XCPC_RESERVED_13_MASK = (1 << 13),
    XCPC_RESERVED_14_MASK = (1 << 14),
    XCPC_RESERVED_15_MASK = (1 << 15),
    XCPC_JOYSTICK0_MASK   = (1 << 16),
    XCPC_JOYSTICK1_MASK   = (1 << 17),
    XCPC_RESERVED_18_MASK = (1 << 18),
    XCPC_RESERVED_19_MASK = (1 << 19),
    XCPC_RESERVED_20_MASK = (1 << 20),
    XCPC_RESERVED_21_MASK = (1 << 21),
    XCPC_RESERVED_22_MASK = (1 << 22),
    XCPC_RESERVED_23_MASK = (1 << 23),
};

struct _XcpcAnyEvent
{
    XEvent* x11_event;
};

struct _XcpcCreateWindowEvent
{
    XEvent* x11_event;
};

struct _XcpcDeleteWindowEvent
{
    XEvent* x11_event;
};

struct _XcpcResizeWindowEvent
{
    XEvent* x11_event;
};

struct _XcpcExposeWindowEvent
{
    XEvent* x11_event;
};

struct _XcpcKeyPressEvent
{
    XEvent* x11_event;
};

struct _XcpcKeyReleaseEvent
{
    XEvent* x11_event;
};

struct _XcpcButtonPressEvent
{
    XEvent* x11_event;
};

struct _XcpcButtonReleaseEvent
{
    XEvent* x11_event;
};

struct _XcpcMotionNotifyEvent
{
    XEvent* x11_event;
};

typedef enum   _XcpcEventType          XcpcEventType;
typedef enum   _XcpcModifierType       XcpcModifierType;
typedef struct _XcpcAnyEvent           XcpcAnyEvent;
typedef struct _XcpcCreateWindowEvent  XcpcCreateWindowEvent;
typedef struct _XcpcDeleteWindowEvent  XcpcDeleteWindowEvent;
typedef struct _XcpcResizeWindowEvent  XcpcResizeWindowEvent;
typedef struct _XcpcExposeWindowEvent  XcpcExposeWindowEvent;
typedef struct _XcpcKeyPressEvent      XcpcKeyPressEvent;
typedef struct _XcpcKeyReleaseEvent    XcpcKeyReleaseEvent;
typedef struct _XcpcButtonPressEvent   XcpcButtonPressEvent;
typedef struct _XcpcButtonReleaseEvent XcpcButtonReleaseEvent;
typedef struct _XcpcMotionNotifyEvent  XcpcMotionNotifyEvent;
typedef struct _XcpcEvent              XcpcEvent;

struct _XcpcEvent
{
    union {
        XcpcAnyEvent           any;
        XcpcCreateWindowEvent  create_window;
        XcpcDeleteWindowEvent  delete_window;
        XcpcResizeWindowEvent  resize_window;
        XcpcExposeWindowEvent  expose_window;
        XcpcKeyPressEvent      key_press;
        XcpcKeyReleaseEvent    key_release;
        XcpcButtonPressEvent   button_press;
        XcpcButtonReleaseEvent button_release;
        XcpcMotionNotifyEvent  motion_notify;
    } u;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_LIBXCPC_EVENTS_H__ */
