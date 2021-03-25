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

#ifndef XCPC_SETTINGS_GET
#define XCPC_SETTINGS_GET(self, what) self->state.what
#endif

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
    char* model;
    char* monitor;
    char* keyboard;
    char* refresh;
    char* manufacturer;
    char* sysrom;
    char* rom000;
    char* rom001;
    char* rom002;
    char* rom003;
    char* rom004;
    char* rom005;
    char* rom006;
    char* rom007;
    char* rom008;
    char* rom009;
    char* rom010;
    char* rom011;
    char* rom012;
    char* rom013;
    char* rom014;
    char* rom015;
    int   help;
    int   version;
    int   loglevel;
    int   turbo;
    int   xshm;
    int   fps;
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
