/*
 * cpu-z80a.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_CPU_Z80A_H__
#define __XCPC_CPU_Z80A_H__

#include <xcpc/cpu-z80a/cpu-z80a-defs.h>
#include <xcpc/cpu-z80a/cpu-z80a-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcCpuZ80a* xcpc_cpu_z80a_alloc     (void);
extern XcpcCpuZ80a* xcpc_cpu_z80a_free      (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_construct (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_destruct  (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_new       (void);
extern XcpcCpuZ80a* xcpc_cpu_z80a_delete    (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_reset     (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_clock     (XcpcCpuZ80a* cpu_z80a);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_H__ */
