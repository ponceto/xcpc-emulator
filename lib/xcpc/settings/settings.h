/*
 * settings.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_SETTINGS_H__
#define __XCPC_SETTINGS_H__

#include <xcpc/settings/settings-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcSettings* xcpc_settings_alloc     (void);
extern XcpcSettings* xcpc_settings_free      (XcpcSettings* settings);
extern XcpcSettings* xcpc_settings_construct (XcpcSettings* settings);
extern XcpcSettings* xcpc_settings_destruct  (XcpcSettings* settings);
extern XcpcSettings* xcpc_settings_new       (void);
extern XcpcSettings* xcpc_settings_delete    (XcpcSettings* settings);
extern XcpcSettings* xcpc_settings_parse     (XcpcSettings* settings, int* argc, char*** argv);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SETTINGS_H__ */
