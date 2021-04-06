/*
 * vdc-6845.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_VDC_6845_H__
#define __XCPC_VDC_6845_H__

#include <xcpc/vdc-6845/vdc-6845-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcVdc6845* xcpc_vdc_6845_alloc     (void);
extern XcpcVdc6845* xcpc_vdc_6845_free      (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_construct (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_destruct  (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_new       (void);
extern XcpcVdc6845* xcpc_vdc_6845_delete    (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_reset     (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_set_iface (XcpcVdc6845* vdc_6845, const XcpcVdc6845Iface* vdc_6845_iface);
extern XcpcVdc6845* xcpc_vdc_6845_clock     (XcpcVdc6845* vdc_6845);
extern XcpcVdc6845* xcpc_vdc_6845_debug     (XcpcVdc6845* vdc_6845);

extern uint8_t      xcpc_vdc_6845_rg        (XcpcVdc6845* vdc_6845, uint8_t data_bus);
extern uint8_t      xcpc_vdc_6845_rs        (XcpcVdc6845* vdc_6845, uint8_t data_bus);
extern uint8_t      xcpc_vdc_6845_rd        (XcpcVdc6845* vdc_6845, uint8_t data_bus);
extern uint8_t      xcpc_vdc_6845_wr        (XcpcVdc6845* vdc_6845, uint8_t data_bus);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VDC_6845_H__ */
