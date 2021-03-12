/*
 * blitter.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_BLITTER_H__
#define __XCPC_BLITTER_H__

#include <xcpc/blitter-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcBlitter* xcpc_blitter_alloc       (void);
extern XcpcBlitter* xcpc_blitter_free        (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_construct   (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_destruct    (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_new         (void);
extern XcpcBlitter* xcpc_blitter_delete      (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_reset       (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_realize     (XcpcBlitter* blitter, XcpcMonitorModel monitor_model, Display* display, Window window, Bool try_xshm);
extern XcpcBlitter* xcpc_blitter_unrealize   (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_is_realized (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_put_image   (XcpcBlitter* blitter);
extern XcpcBlitter* xcpc_blitter_resize      (XcpcBlitter* blitter, XEvent* event);
extern XcpcBlitter* xcpc_blitter_expose      (XcpcBlitter* blitter, XEvent* event);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BLITTER_H__ */
