/*
 * cpu-z80a-defs.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_CPU_Z80A_DEFS_H__
#define __XCPC_CPU_Z80A_DEFS_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

union XcpcCpuZ80aRegister
{
    u_int32_t q;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
    struct {
        u_int16_t h;
        u_int16_t l;
    } w;
    struct {
        u_int8_t  x;
        u_int8_t  y;
        u_int8_t  h;
        u_int8_t  l;
    } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
    struct {
        u_int16_t l;
        u_int16_t h;
    } w;
    struct {
        u_int8_t  l;
        u_int8_t  h;
        u_int8_t  y;
        u_int8_t  x;
    } b;
#endif
};

#if 0
typedef int8_t   s_int08_t;
typedef uint8_t  u_int08_t;
typedef int16_t  s_int16_t;
typedef uint16_t u_int16_t;
typedef int32_t  s_int32_t;
typedef uint32_t u_int32_t;
typedef int64_t  s_int64_t;
typedef uint64_t u_int64_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_DEFS_H__ */
