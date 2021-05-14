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
    void* user_data;
    void (*reserved0)(void* user_data);
    void (*reserved1)(void* user_data);
    void (*reserved2)(void* user_data);
    void (*reserved3)(void* user_data);
    void (*reserved4)(void* user_data);
    void (*reserved5)(void* user_data);
    void (*reserved6)(void* user_data);
    void (*reserved7)(void* user_data);
};

extern XcpcFrontend* xcpc_frontend_init(XcpcFrontend* frontend);
extern XcpcFrontend* xcpc_frontend_fini(XcpcFrontend* frontend);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_FRONTEND_H__ */
