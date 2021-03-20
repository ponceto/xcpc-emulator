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

#define _HLT  0x80 /* CPU is HALTed          */
#define _NMI  0x40 /* Pending NMI            */
#define _INT  0x20 /* Pending INT            */
#define _XYZ  0x10 /* Not Used               */
#define _IM2  0x08 /* Interrupt Mode #2      */
#define _IM1  0x04 /* Interrupt Mode #1      */
#define _IFF2 0x02 /* Interrupt Flip-Flop #2 */
#define _IFF1 0x01 /* Interrupt Flip-Flop #1 */

union XcpcCpuZ80aRegister
{
    uint32_t q;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
    struct {
        uint16_t h;
        uint16_t l;
    } w;
    struct {
        uint8_t  x;
        uint8_t  y;
        uint8_t  h;
        uint8_t  l;
    } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
    struct {
        uint16_t l;
        uint16_t h;
    } w;
    struct {
        uint8_t  l;
        uint8_t  h;
        uint8_t  y;
        uint8_t  x;
    } b;
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_DEFS_H__ */
