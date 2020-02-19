/*
 * builtin-roms.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_BUILTIN_ROMS_H__
#define __XCPC_BUILTIN_ROMS_H__

#include <xcpc/builtin-roms-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t xcpc_cpc464_lower_rom[XCPC_BUILTIN_ROM_SIZE];
extern const uint8_t xcpc_cpc464_upper_rom[XCPC_BUILTIN_ROM_SIZE];

extern const uint8_t xcpc_cpc664_lower_rom[XCPC_BUILTIN_ROM_SIZE];
extern const uint8_t xcpc_cpc664_upper_rom[XCPC_BUILTIN_ROM_SIZE];

extern const uint8_t xcpc_cpc6128_lower_rom[XCPC_BUILTIN_ROM_SIZE];
extern const uint8_t xcpc_cpc6128_upper_rom[XCPC_BUILTIN_ROM_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BUILTIN_ROMS_H__ */
