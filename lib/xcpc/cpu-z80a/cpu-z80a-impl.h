/*
 * cpu-z80a-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_CPU_Z80A_IMPL_H__
#define __XCPC_CPU_Z80A_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcCpuZ80aIface XcpcCpuZ80aIface;
typedef struct _XcpcCpuZ80aState XcpcCpuZ80aState;
typedef struct _XcpcCpuZ80a      XcpcCpuZ80a;

struct _XcpcCpuZ80aIface
{
    void* user_data;
    uint8_t (*mreq_m1)(XcpcCpuZ80a* cpu_z80a, uint16_t addr);
    uint8_t (*mreq_rd)(XcpcCpuZ80a* cpu_z80a, uint16_t addr);
    void    (*mreq_wr)(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data);
    uint8_t (*iorq_m1)(XcpcCpuZ80a* cpu_z80a, uint16_t addr);
    uint8_t (*iorq_rd)(XcpcCpuZ80a* cpu_z80a, uint16_t addr);
    void    (*iorq_wr)(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data);
};

struct _XcpcCpuZ80aState
{
    struct /* registers */
    {
        XcpcRegister AF; /* AF & AF'            */
        XcpcRegister BC; /* BC & BC'            */
        XcpcRegister DE; /* DE & DE'            */
        XcpcRegister HL; /* HL & HL'            */
        XcpcRegister IX; /* IX Index            */
        XcpcRegister IY; /* IY Index            */
        XcpcRegister SP; /* Stack Pointer       */
        XcpcRegister PC; /* Program Counter     */
        XcpcRegister IR; /* Interrupt & Refresh */
        XcpcRegister ST; /* IFF, IM & Control   */
    } regs;
    struct /* counters */
    {
        int32_t m_cycles;
        int32_t t_states;
        int32_t i_period;
    } ctrs;
};

struct _XcpcCpuZ80a
{
    XcpcCpuZ80aIface iface;
    XcpcCpuZ80aState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_IMPL_H__ */
