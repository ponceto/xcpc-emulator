/*
 * libxcpc-priv.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __XCPC_LIBXCPC_PRIV_H__
#define __XCPC_LIBXCPC_PRIV_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_DEFAULT_INPUT_STREAM stdin
#define XCPC_DEFAULT_PRINT_STREAM stdout
#define XCPC_DEFAULT_ERROR_STREAM stderr
#define XCPC_DEFAULT_JOYSTICK0 "/dev/input/js0"
#define XCPC_DEFAULT_JOYSTICK1 "/dev/input/js1"

typedef struct _XcpcLibrary           XcpcLibrary;
typedef struct _XcpcCompanyNameEntry  XcpcCompanyNameEntry;
typedef struct _XcpcMachineTypeEntry  XcpcMachineTypeEntry;
typedef struct _XcpcMonitorTypeEntry  XcpcMonitorTypeEntry;
typedef struct _XcpcRefreshRateEntry  XcpcRefreshRateEntry;
typedef struct _XcpcKeyboardTypeEntry XcpcKeyboardTypeEntry;
typedef struct _XcpcMemorySizeEntry   XcpcMemorySizeEntry;
typedef struct _XcpcColorEntry        XcpcColorEntry;

struct _XcpcLibrary
{
    int         initialized;
    int         major_version;
    int         minor_version;
    int         micro_version;
    int         loglevel;
    FILE*       input_stream;
    FILE*       print_stream;
    FILE*       error_stream;
    char*       bindir;
    char*       libdir;
    char*       datdir;
    char*       docdir;
    char*       resdir;
    char*       romdir;
    char*       dskdir;
    char*       snadir;
    const char* joystick0;
    const char* joystick1;
};

struct _XcpcCompanyNameEntry
{
    const char*     label;
    XcpcCompanyName value;
};

struct _XcpcMachineTypeEntry
{
    const char*     label;
    XcpcMachineType value;
};

struct _XcpcMonitorTypeEntry
{
    const char*     label;
    XcpcMonitorType value;
};

struct _XcpcRefreshRateEntry
{
    const char*     label;
    XcpcRefreshRate value;
};

struct _XcpcKeyboardTypeEntry
{
    const char*      label;
    XcpcKeyboardType value;
};

struct _XcpcMemorySizeEntry
{
    const char*    label;
    XcpcMemorySize value;
};

struct _XcpcColorEntry
{
    const char*    label;
    XcpcColor      color;
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short luminance;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_LIBXCPC_PRIV_H__ */
