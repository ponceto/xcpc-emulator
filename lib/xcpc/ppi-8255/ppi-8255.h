/*
 * ppi-8255.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_PPI_8255_H__
#define __XCPC_PPI_8255_H__

#include <xcpc/ppi-8255/ppi-8255-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcPpi8255* xcpc_ppi_8255_alloc     (void);
extern XcpcPpi8255* xcpc_ppi_8255_free      (XcpcPpi8255* ppi_8255);
extern XcpcPpi8255* xcpc_ppi_8255_construct (XcpcPpi8255* ppi_8255, const XcpcPpi8255Iface* ppi_8255_iface);
extern XcpcPpi8255* xcpc_ppi_8255_destruct  (XcpcPpi8255* ppi_8255);
extern XcpcPpi8255* xcpc_ppi_8255_new       (const XcpcPpi8255Iface* ppi_8255_iface);
extern XcpcPpi8255* xcpc_ppi_8255_delete    (XcpcPpi8255* ppi_8255);
extern XcpcPpi8255* xcpc_ppi_8255_reset     (XcpcPpi8255* ppi_8255);
extern XcpcPpi8255* xcpc_ppi_8255_clock     (XcpcPpi8255* ppi_8255);

extern uint8_t      xcpc_ppi_8255_illegal   (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_rd_port_a (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_wr_port_a (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_rd_port_b (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_wr_port_b (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_rd_port_c (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_wr_port_c (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_rd_ctrl_p (XcpcPpi8255* ppi_8255, uint8_t data_bus);
extern uint8_t      xcpc_ppi_8255_wr_ctrl_p (XcpcPpi8255* ppi_8255, uint8_t data_bus);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PPI_8255_H__ */
