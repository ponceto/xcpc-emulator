/*
 * settings-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_SETTINGS_IMPL_H__
#define __XCPC_SETTINGS_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcSettingsIface XcpcSettingsIface;
typedef struct _XcpcSettingsState XcpcSettingsState;
typedef struct _XcpcSettings      XcpcSettings;

struct _XcpcSettingsIface
{
    void* user_data;
};

struct _XcpcSettingsState
{
    char* program;
    char* drive0;
    char* drive1;
    char* snapshot;
};

struct _XcpcSettings
{
    XcpcSettingsIface iface;
    XcpcSettingsState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SETTINGS_IMPL_H__ */
