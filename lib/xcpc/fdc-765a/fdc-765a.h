/*
 * fdc-765a.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_FDC_765A_H__
#define __XCPC_FDC_765A_H__

#include <xcpc/fdc-765a/fdc-765a-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcFdc765a* xcpc_fdc_765a_alloc     (void);
extern XcpcFdc765a* xcpc_fdc_765a_free      (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_construct (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_destruct  (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_new       (void);
extern XcpcFdc765a* xcpc_fdc_765a_delete    (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_reset     (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_clock     (XcpcFdc765a* fdc_765a);
extern XcpcFdc765a* xcpc_fdc_765a_attach    (XcpcFdc765a* fdc_765a, int drive);
extern XcpcFdc765a* xcpc_fdc_765a_detach    (XcpcFdc765a* fdc_765a, int drive);
extern XcpcFdc765a* xcpc_fdc_765a_insert    (XcpcFdc765a* fdc_765a, const char* filename, int drive);
extern XcpcFdc765a* xcpc_fdc_765a_rstat     (XcpcFdc765a* fdc_765a, uint8_t* data);
extern XcpcFdc765a* xcpc_fdc_765a_wstat     (XcpcFdc765a* fdc_765a, uint8_t* data);
extern XcpcFdc765a* xcpc_fdc_765a_rdata     (XcpcFdc765a* fdc_765a, uint8_t* data);
extern XcpcFdc765a* xcpc_fdc_765a_wdata     (XcpcFdc765a* fdc_765a, uint8_t* data);
extern XcpcFdc765a* xcpc_fdc_765a_motor     (XcpcFdc765a* fdc_765a, uint8_t motor);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_FDC_765A_H__ */
