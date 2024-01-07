/*
 * fdc-765a-priv.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_FDC_765A_PRIV_H__
#define __XCPC_FDC_765A_PRIV_H__

#include <libdsk/libdsk.h>
#include <lib765/765.h>
#include <xcpc/fdc-765a/fdc-765a.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcFdcImpl* xcpc_fdc_impl_new       (void);
extern XcpcFdcImpl* xcpc_fdc_impl_delete    (XcpcFdcImpl* fdc_impl);
extern XcpcFdcImpl* xcpc_fdc_impl_reset     (XcpcFdcImpl* fdc_impl);
extern XcpcFdcImpl* xcpc_fdc_impl_clock     (XcpcFdcImpl* fdc_impl);
extern XcpcFdcImpl* xcpc_fdc_impl_set_drive (XcpcFdcImpl* fdc_impl, XcpcFddImpl* fdd_impl, int drive);
extern XcpcFdcImpl* xcpc_fdc_impl_set_motor (XcpcFdcImpl* fdc_impl, uint8_t motor);
extern XcpcFdcImpl* xcpc_fdc_impl_rd_stat   (XcpcFdcImpl* fdc_impl, uint8_t* data);
extern XcpcFdcImpl* xcpc_fdc_impl_wr_stat   (XcpcFdcImpl* fdc_impl, uint8_t* data);
extern XcpcFdcImpl* xcpc_fdc_impl_rd_data   (XcpcFdcImpl* fdc_impl, uint8_t* data);
extern XcpcFdcImpl* xcpc_fdc_impl_wr_data   (XcpcFdcImpl* fdc_impl, uint8_t* data);

extern XcpcFddImpl* xcpc_fdd_impl_new       (void);
extern XcpcFddImpl* xcpc_fdd_impl_delete    (XcpcFddImpl* fdd_impl);
extern XcpcFddImpl* xcpc_fdd_impl_reset     (XcpcFddImpl* fdd_impl);
extern XcpcFddImpl* xcpc_fdd_impl_insert    (XcpcFddImpl* fdd_impl, const char* filename);
extern XcpcFddImpl* xcpc_fdd_impl_remove    (XcpcFddImpl* fdd_impl);
extern XcpcFddImpl* xcpc_fdd_impl_filename  (XcpcFddImpl* fdd_impl, const char**);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_FDC_765A_PRIV_H__ */
