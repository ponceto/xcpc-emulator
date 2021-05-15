/*
 * backend.h - Copyright (c) 2001-2021 - Olivier Poncet
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

typedef struct _XcpcBackend               XcpcBackend;
typedef struct _XcpcBackendParam          XcpcBackendParam;
typedef struct _XcpcBackendIdleEvent      XcpcBackendIdleEvent;
typedef struct _XcpcBackendAttachEvent    XcpcBackendAttachEvent;
typedef struct _XcpcBackendDetachEvent    XcpcBackendDetachEvent;
typedef struct _XcpcBackendRealizeEvent   XcpcBackendRealizeEvent;
typedef struct _XcpcBackendUnrealizeEvent XcpcBackendUnrealizeEvent;
typedef struct _XcpcBackendResizeEvent    XcpcBackendResizeEvent;
typedef struct _XcpcBackendExposeEvent    XcpcBackendExposeEvent;
typedef struct _XcpcBackendInputEvent     XcpcBackendInputEvent;
typedef struct _XcpcBackendClockEvent     XcpcBackendClockEvent;

typedef unsigned long (*XcpcBackendFunc)(void* instance, XcpcBackendParam* data);

struct _XcpcBackendIdleEvent
{
    int padding;
};

struct _XcpcBackendAttachEvent
{
    int padding;
};

struct _XcpcBackendDetachEvent
{
    int padding;
};

struct _XcpcBackendRealizeEvent
{
    int padding;
};

struct _XcpcBackendUnrealizeEvent
{
    int padding;
};

struct _XcpcBackendResizeEvent
{
    int padding;
};

struct _XcpcBackendExposeEvent
{
    int padding;
};

struct _XcpcBackendInputEvent
{
    int padding;
};

struct _XcpcBackendClockEvent
{
    int padding;
};

struct _XcpcBackendParam
{
    XEvent* event;
    union {
        XcpcBackendIdleEvent      idle;
        XcpcBackendAttachEvent    attach;
        XcpcBackendDetachEvent    detach;
        XcpcBackendRealizeEvent   realize;
        XcpcBackendUnrealizeEvent unrealize;
        XcpcBackendResizeEvent    resize;
        XcpcBackendExposeEvent    expose;
        XcpcBackendInputEvent     input;
        XcpcBackendClockEvent     clock;
    } u;
};

struct _XcpcBackend
{
    void* instance;
    unsigned long (*idle_func)      (void* instance, XcpcBackendParam* data);
    unsigned long (*attach_func)    (void* instance, XcpcBackendParam* data);
    unsigned long (*detach_func)    (void* instance, XcpcBackendParam* data);
    unsigned long (*realize_func)   (void* instance, XcpcBackendParam* data);
    unsigned long (*unrealize_func) (void* instance, XcpcBackendParam* data);
    unsigned long (*resize_func)    (void* instance, XcpcBackendParam* data);
    unsigned long (*expose_func)    (void* instance, XcpcBackendParam* data);
    unsigned long (*input_func)     (void* instance, XcpcBackendParam* data);
    unsigned long (*clock_func)     (void* instance, XcpcBackendParam* data);
};

extern XcpcBackend* xcpc_backend_init(XcpcBackend* backend);
extern XcpcBackend* xcpc_backend_fini(XcpcBackend* backend);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BACKEND_H__ */
