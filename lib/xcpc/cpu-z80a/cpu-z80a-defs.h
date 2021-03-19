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

#if 0
#define _SF   0x80 /* Sign                   */
#define _ZF   0x40 /* Zero                   */
#define _5F   0x20 /* Undocumented           */
#define _HF   0x10 /* HalfCarry / HalfBorrow */
#define _3F   0x08 /* Undocumented           */
#define _PF   0x04 /* Parity                 */
#define _OF   0x04 /* Overflow               */
#define _NF   0x02 /* Add / Sub              */
#define _CF   0x01 /* Carry / Borrow         */

#define _HLT  0x80 /* CPU is HALTed          */
#define _NMI  0x40 /* Pending NMI            */
#define _INT  0x20 /* Pending INT            */
#define _XYZ  0x10 /* Not Used               */
#define _IM2  0x08 /* Interrupt Mode #2      */
#define _IM1  0x04 /* Interrupt Mode #1      */
#define _IFF2 0x02 /* Interrupt Flip-Flop #2 */
#define _IFF1 0x01 /* Interrupt Flip-Flop #1 */

typedef int8_t   s_int08_t;
typedef uint8_t  u_int08_t;
typedef int16_t  s_int16_t;
typedef uint16_t u_int16_t;
typedef int32_t  s_int32_t;
typedef uint32_t u_int32_t;
typedef int64_t  s_int64_t;
typedef uint64_t u_int64_t;

union CpuZ80aRegister
{
  u_int32_t q;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
  struct { u_int16_t h, l; } w;
  struct { u_int08_t x, y, h, l; } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
  struct { u_int16_t l, h; } w;
  struct { u_int08_t l, h, y, x; } b;
#endif
};

struct CpuZ80aContext
{
    struct
    {
        union CpuZ80aRegister AF; /* AF & AF'            */
        union CpuZ80aRegister BC; /* BC & BC'            */
        union CpuZ80aRegister DE; /* DE & DE'            */
        union CpuZ80aRegister HL; /* HL & HL'            */
        union CpuZ80aRegister IX; /* IX Index            */
        union CpuZ80aRegister IY; /* IY Index            */
        union CpuZ80aRegister SP; /* Stack Pointer       */
        union CpuZ80aRegister PC; /* Program Counter     */
        union CpuZ80aRegister IR; /* Interrupt & Refresh */
        union CpuZ80aRegister IF; /* IFF, IM & Control   */
    } reg;
    u_int32_t m_cycles;
    u_int32_t t_states;
    s_int32_t ccounter;
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_DEFS_H__ */
