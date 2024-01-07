/*
 * backend.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_BACKEND_H__
#define __XCPC_BACKEND_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcBackend                  XcpcBackend;
typedef struct _XcpcBackendClosure           XcpcBackendClosure;
typedef struct _XcpcBackendAttachEvent       XcpcBackendAttachEvent;
typedef struct _XcpcBackendDetachEvent       XcpcBackendDetachEvent;
typedef struct _XcpcBackendIdleEvent         XcpcBackendIdleEvent;
typedef struct _XcpcBackendClockEvent        XcpcBackendClockEvent;
typedef struct _XcpcBackendCreateWindowEvent XcpcBackendCreateWindowEvent;
typedef struct _XcpcBackendDeleteWindowEvent XcpcBackendDeleteWindowEvent;
typedef struct _XcpcBackendResizeWindowEvent XcpcBackendResizeWindowEvent;
typedef struct _XcpcBackendExposeWindowEvent XcpcBackendExposeWindowEvent;
typedef struct _XcpcBackendKeyEvent          XcpcBackendKeyPressEvent;
typedef struct _XcpcBackendKeyEvent          XcpcBackendKeyReleaseEvent;
typedef struct _XcpcBackendButtonEvent       XcpcBackendButtonPressEvent;
typedef struct _XcpcBackendButtonEvent       XcpcBackendButtonReleaseEvent;
typedef struct _XcpcBackendMotionEvent       XcpcBackendMotionNotifyEvent;
typedef struct _XcpcBackendAudioEvent        XcpcBackendAudioEvent;

typedef unsigned long (*XcpcBackendFunc)(void* instance, XcpcBackendClosure* closure);

struct _XcpcBackendAttachEvent
{
    int padding;
};

struct _XcpcBackendDetachEvent
{
    int padding;
};

struct _XcpcBackendIdleEvent
{
    int padding;
};

struct _XcpcBackendClockEvent
{
    int padding;
};

struct _XcpcBackendCreateWindowEvent
{
    int padding;
};

struct _XcpcBackendDeleteWindowEvent
{
    int padding;
};

struct _XcpcBackendResizeWindowEvent
{
    int padding;
};

struct _XcpcBackendExposeWindowEvent
{
    int padding;
};

struct _XcpcBackendKeyEvent
{
    int padding;
};

struct _XcpcBackendButtonEvent
{
    int padding;
};

struct _XcpcBackendMotionEvent
{
    int padding;
};

struct _XcpcBackendAudioEvent
{
    int padding;
};

struct _XcpcBackendClosure
{
    XEvent* event;
    union {
        XcpcBackendAttachEvent        attach;
        XcpcBackendDetachEvent        detach;
        XcpcBackendIdleEvent          idle;
        XcpcBackendClockEvent         clock;
        XcpcBackendCreateWindowEvent  create_window;
        XcpcBackendDeleteWindowEvent  delete_window;
        XcpcBackendResizeWindowEvent  resize_window;
        XcpcBackendExposeWindowEvent  expose_window;
        XcpcBackendKeyPressEvent      key_press;
        XcpcBackendKeyReleaseEvent    key_release;
        XcpcBackendButtonPressEvent   button_press;
        XcpcBackendButtonReleaseEvent button_release;
        XcpcBackendMotionNotifyEvent  motion_notify;
        XcpcBackendAudioEvent         audio;
    } u;
};

struct _XcpcBackend
{
    void* instance;
    unsigned long (*attach_func)         (void* instance, XcpcBackendClosure* closure);
    unsigned long (*detach_func)         (void* instance, XcpcBackendClosure* closure);
    unsigned long (*idle_func)           (void* instance, XcpcBackendClosure* closure);
    unsigned long (*clock_func)          (void* instance, XcpcBackendClosure* closure);
    unsigned long (*create_window_func)  (void* instance, XcpcBackendClosure* closure);
    unsigned long (*delete_window_func)  (void* instance, XcpcBackendClosure* closure);
    unsigned long (*resize_window_func)  (void* instance, XcpcBackendClosure* closure);
    unsigned long (*expose_window_func)  (void* instance, XcpcBackendClosure* closure);
    unsigned long (*key_press_func)      (void* instance, XcpcBackendClosure* closure);
    unsigned long (*key_release_func)    (void* instance, XcpcBackendClosure* closure);
    unsigned long (*button_press_func)   (void* instance, XcpcBackendClosure* closure);
    unsigned long (*button_release_func) (void* instance, XcpcBackendClosure* closure);
    unsigned long (*motion_notify_func)  (void* instance, XcpcBackendClosure* closure);
    unsigned long (*audio_func)          (void* instance, XcpcBackendClosure* closure);
};

extern XcpcBackend* xcpc_backend_init(XcpcBackend* backend);
extern XcpcBackend* xcpc_backend_fini(XcpcBackend* backend);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BACKEND_H__ */
