/*
 * psg-8910.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_PSG_8910_H__
#define __XCPC_PSG_8910_H__

#include <xcpc/psg-8910-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcPsg8910* xcpc_psg_8910_alloc     (void);
extern XcpcPsg8910* xcpc_psg_8910_free      (XcpcPsg8910* psg_8910);
extern XcpcPsg8910* xcpc_psg_8910_construct (XcpcPsg8910* psg_8910);
extern XcpcPsg8910* xcpc_psg_8910_destruct  (XcpcPsg8910* psg_8910);
extern XcpcPsg8910* xcpc_psg_8910_new       (void);
extern XcpcPsg8910* xcpc_psg_8910_delete    (XcpcPsg8910* psg_8910);
extern XcpcPsg8910* xcpc_psg_8910_reset     (XcpcPsg8910* psg_8910);

extern uint8_t      xcpc_psg_8910_rg        (XcpcPsg8910* psg_8910, uint8_t address_register);
extern uint8_t      xcpc_psg_8910_rs        (XcpcPsg8910* psg_8910, uint8_t address_register);
extern uint8_t      xcpc_psg_8910_rd        (XcpcPsg8910* psg_8910, uint8_t register_data);
extern uint8_t      xcpc_psg_8910_wr        (XcpcPsg8910* psg_8910, uint8_t register_data);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PSG_8910_H__ */
