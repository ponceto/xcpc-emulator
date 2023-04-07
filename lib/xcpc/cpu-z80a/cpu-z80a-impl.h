/*
 * cpu-z80a-impl.h - Copyright (c) 2001-2023 - Olivier Poncet
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

#define XCPC_CPU_Z80A_MREQ_FUNC(func) ((XcpcCpuZ80aMreqFunc)(func))
#define XCPC_CPU_Z80A_IORQ_FUNC(func) ((XcpcCpuZ80aIorqFunc)(func))

typedef struct _XcpcCpuZ80aIface XcpcCpuZ80aIface;
typedef struct _XcpcCpuZ80aSetup XcpcCpuZ80aSetup;
typedef struct _XcpcCpuZ80aState XcpcCpuZ80aState;
typedef struct _XcpcCpuZ80a      XcpcCpuZ80a;

typedef uint8_t (*XcpcCpuZ80aMreqFunc)(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data, void* user_data);
typedef uint8_t (*XcpcCpuZ80aIorqFunc)(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data, void* user_data);

struct _XcpcCpuZ80aIface
{
    void* user_data;
    XcpcCpuZ80aMreqFunc mreq_m1;
    XcpcCpuZ80aMreqFunc mreq_rd;
    XcpcCpuZ80aMreqFunc mreq_wr;
    XcpcCpuZ80aIorqFunc iorq_m1;
    XcpcCpuZ80aIorqFunc iorq_rd;
    XcpcCpuZ80aIorqFunc iorq_wr;
};

struct _XcpcCpuZ80aSetup
{
    int reserved;
};

struct _XcpcCpuZ80aState
{
    struct /* registers */
    {
        XcpcRegister WZ; /* WZ & WZ'            */
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
    XcpcCpuZ80aSetup setup;
    XcpcCpuZ80aState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_IMPL_H__ */
