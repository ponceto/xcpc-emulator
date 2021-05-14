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

typedef struct _XcpcBackend XcpcBackend;

typedef unsigned long (*XcpcBackendFunc)(void* user_data, XEvent* event, void* extra);

struct _XcpcBackend
{
    void* user_data;
    unsigned long (*idle_func)    (void* user_data, XEvent* event, void* extra);
    unsigned long (*create_func)  (void* user_data, XEvent* event, void* extra);
    unsigned long (*destroy_func) (void* user_data, XEvent* event, void* extra);
    unsigned long (*realize_func) (void* user_data, XEvent* event, void* extra);
    unsigned long (*resize_func)  (void* user_data, XEvent* event, void* extra);
    unsigned long (*expose_func)  (void* user_data, XEvent* event, void* extra);
    unsigned long (*input_func)   (void* user_data, XEvent* event, void* extra);
    unsigned long (*clock_func)   (void* user_data, XEvent* event, void* extra);
};

extern XcpcBackend* xcpc_backend_init(XcpcBackend* backend);
extern XcpcBackend* xcpc_backend_fini(XcpcBackend* backend);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BACKEND_H__ */
