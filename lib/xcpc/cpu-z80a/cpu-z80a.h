/*
 * cpu-z80a.h - Copyright (c) 2001-2024 - Olivier Poncet
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

#include <xcpc/cpu-z80a/cpu-z80a-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcCpuZ80a* xcpc_cpu_z80a_alloc     (void);
extern XcpcCpuZ80a* xcpc_cpu_z80a_free      (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_construct (XcpcCpuZ80a* cpu_z80a, const XcpcCpuZ80aIface* cpu_z80a_iface);
extern XcpcCpuZ80a* xcpc_cpu_z80a_destruct  (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_new       (const XcpcCpuZ80aIface* cpu_z80a_iface);
extern XcpcCpuZ80a* xcpc_cpu_z80a_delete    (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_reset     (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_clock     (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_pulse_nmi (XcpcCpuZ80a* cpu_z80a);
extern XcpcCpuZ80a* xcpc_cpu_z80a_pulse_int (XcpcCpuZ80a* cpu_z80a);

extern uint8_t      xcpc_cpu_z80a_get_af_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_af_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_af_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_af_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_bc_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_bc_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_bc_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_bc_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_de_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_de_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_de_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_de_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_hl_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_hl_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_hl_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_hl_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_ix_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_ix_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_ix_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_ix_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_iy_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_iy_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_iy_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_iy_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_sp_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_sp_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_sp_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_sp_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_pc_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_pc_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_pc_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_pc_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_ir_h  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_ir_h  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_ir_l  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_ir_l  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_af_x  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_af_x  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_af_y  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_af_y  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_bc_x  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_bc_x  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_bc_y  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_bc_y  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_de_x  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_de_x  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_de_y  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_de_y  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_hl_x  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_hl_x  (XcpcCpuZ80a* cpu_z80a, uint8_t data);
extern uint8_t      xcpc_cpu_z80a_get_hl_y  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_hl_y  (XcpcCpuZ80a* cpu_z80a, uint8_t data);

extern uint8_t      xcpc_cpu_z80a_get_im    (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_im    (XcpcCpuZ80a* cpu_z80a, uint8_t im);

extern uint8_t      xcpc_cpu_z80a_get_iff1  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_iff1  (XcpcCpuZ80a* cpu_z80a, uint8_t iff1);

extern uint8_t      xcpc_cpu_z80a_get_iff2  (XcpcCpuZ80a* cpu_z80a);
extern uint8_t      xcpc_cpu_z80a_set_iff2  (XcpcCpuZ80a* cpu_z80a, uint8_t iff2);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_H__ */
