/*
 * ram-bank-impl.h - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifndef __XCPC_RAM_BANK_IMPL_H__
#define __XCPC_RAM_BANK_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum   _XcpcRamBankStatus XcpcRamBankStatus;
typedef struct _XcpcRamBankIface  XcpcRamBankIface;
typedef struct _XcpcRamBankSetup  XcpcRamBankSetup;
typedef struct _XcpcRamBankState  XcpcRamBankState;
typedef struct _XcpcRamBank       XcpcRamBank;

enum _XcpcRamBankStatus
{
    XCPC_RAM_BANK_STATUS_FAILURE = -1,
    XCPC_RAM_BANK_STATUS_SUCCESS =  0,
};

struct _XcpcRamBankIface
{
    void* user_data;
};

struct _XcpcRamBankSetup
{
    int reserved;
};

struct _XcpcRamBankState
{
    uint8_t data[16384];
};

struct _XcpcRamBank
{
    XcpcRamBankIface iface;
    XcpcRamBankSetup setup;
    XcpcRamBankState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_RAM_BANK_IMPL_H__ */
