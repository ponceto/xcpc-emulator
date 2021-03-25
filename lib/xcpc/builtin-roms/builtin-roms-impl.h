/*
 * builtin-roms-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_BUILTIN_ROMS_IMPL_H__
#define __XCPC_BUILTIN_ROMS_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcSystemRom XcpcSystemRom;
typedef struct _XcpcAmsdosRom XcpcAmsdosRom;

struct _XcpcSystemRom
{
    uint8_t lower_rom[16384];
    uint8_t upper_rom[16384];
};

struct _XcpcAmsdosRom
{
    uint8_t rom[16384];
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BUILTIN_ROMS_IMPL_H__ */
