/*
 * options.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_OPTIONS_H__
#define __XCPC_OPTIONS_H__

#include <xcpc/options/options-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcOptions* xcpc_options_alloc     (void);
extern XcpcOptions* xcpc_options_free      (XcpcOptions* options);
extern XcpcOptions* xcpc_options_construct (XcpcOptions* options, int* argc, char*** argv);
extern XcpcOptions* xcpc_options_destruct  (XcpcOptions* options);
extern XcpcOptions* xcpc_options_new       (int* argc, char*** argv);
extern XcpcOptions* xcpc_options_delete    (XcpcOptions* options);
extern XcpcOptions* xcpc_options_set_iface (XcpcOptions* options, const XcpcOptionsIface* options_iface);
extern XcpcOptions* xcpc_options_parse     (XcpcOptions* options);
extern int          xcpc_options_quit      (XcpcOptions* options);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_OPTIONS_H__ */
