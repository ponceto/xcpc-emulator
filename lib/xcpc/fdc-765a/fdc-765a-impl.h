/*
 * fdc-765a-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_FDC_765A_IMPL_H__
#define __XCPC_FDC_765A_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fdc_765      XcpcFdcImpl;
typedef struct floppy_drive XcpcFddImpl;

typedef enum   _XcpcFdc765aDrive XcpcFdc765aDrive;
typedef struct _XcpcFdc765aIface XcpcFdc765aIface;
typedef struct _XcpcFdc765aSetup XcpcFdc765aSetup;
typedef struct _XcpcFdc765aState XcpcFdc765aState;
typedef struct _XcpcFdc765a      XcpcFdc765a;

enum _XcpcFdc765aDrive
{
    XCPC_FDC_765A_DRIVE0 = 0,
    XCPC_FDC_765A_DRIVE1 = 1,
    XCPC_FDC_765A_DRIVE2 = 2,
    XCPC_FDC_765A_DRIVE3 = 3
};

struct _XcpcFdc765aIface
{
    void* user_data;
};

struct _XcpcFdc765aSetup
{
    int reserved;
};

struct _XcpcFdc765aState
{
    XcpcFdcImpl* fdc_impl;
    XcpcFddImpl* fd0_impl;
    XcpcFddImpl* fd1_impl;
    XcpcFddImpl* fd2_impl;
    XcpcFddImpl* fd3_impl;
};

struct _XcpcFdc765a
{
    XcpcFdc765aIface iface;
    XcpcFdc765aSetup setup;
    XcpcFdc765aState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_FDC_765A_IMPL_H__ */
