/*
 * vga-core-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_VGA_CORE_IMPL_H__
#define __XCPC_VGA_CORE_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcVgaCoreIface XcpcVgaCoreIface;
typedef struct _XcpcVgaCoreState XcpcVgaCoreState;
typedef struct _XcpcVgaCore      XcpcVgaCore;

#ifndef XCPC_VGA_CORE_IFACE
#define XCPC_VGA_CORE_IFACE(instance, field) instance->iface.field
#endif

#ifndef XCPC_VGA_CORE_STATE
#define XCPC_VGA_CORE_STATE(instance, field) instance->state.field
#endif

struct _XcpcVgaCoreIface
{
    void* user_data;
};

struct _XcpcVgaCoreState
{
    uint8_t mode0[256];
    uint8_t mode1[256];
    uint8_t mode2[256];
    uint8_t pen;
    uint8_t ink[17];
    uint8_t rmr;
    uint8_t counter;
    uint8_t delayed;
};

struct _XcpcVgaCore
{
    XcpcVgaCoreIface iface;
    XcpcVgaCoreState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VGA_CORE_IMPL_H__ */
