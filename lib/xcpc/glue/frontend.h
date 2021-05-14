/*
 * frontend.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_FRONTEND_H__
#define __XCPC_FRONTEND_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcFrontend XcpcFrontend;

struct _XcpcFrontend
{
    void* instance;
    void (*reserved0)(void* instance);
    void (*reserved1)(void* instance);
    void (*reserved2)(void* instance);
    void (*reserved3)(void* instance);
    void (*reserved4)(void* instance);
    void (*reserved5)(void* instance);
    void (*reserved6)(void* instance);
    void (*reserved7)(void* instance);
};

extern XcpcFrontend* xcpc_frontend_init(XcpcFrontend* frontend);
extern XcpcFrontend* xcpc_frontend_fini(XcpcFrontend* frontend);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_FRONTEND_H__ */
