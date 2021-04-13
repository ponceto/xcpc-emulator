/*
 * vga-core.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_VGA_CORE_H__
#define __XCPC_VGA_CORE_H__

#include <xcpc/vga-core/vga-core-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcVgaCore* xcpc_vga_core_alloc     (void);
extern XcpcVgaCore* xcpc_vga_core_free      (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_construct (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_destruct  (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_new       (void);
extern XcpcVgaCore* xcpc_vga_core_delete    (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_set_iface (XcpcVgaCore* vga_core, const XcpcVgaCoreIface* vga_core_iface);
extern XcpcVgaCore* xcpc_vga_core_debug     (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_reset     (XcpcVgaCore* vga_core);
extern XcpcVgaCore* xcpc_vga_core_clock     (XcpcVgaCore* vga_core);

extern uint8_t      xcpc_vga_core_illegal   (XcpcVgaCore* vga_core, uint8_t data_bus);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VGA_CORE_H__ */
