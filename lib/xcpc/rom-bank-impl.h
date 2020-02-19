/*
 * rom-bank-impl.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_ROM_BANK_IMPL_H__
#define __XCPC_ROM_BANK_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_ROM_BANK_SIZE 16384

typedef enum   _XcpcRomBankStatus XcpcRomBankStatus;
typedef struct _XcpcRomBank       XcpcRomBank;

enum _XcpcRomBankStatus
{
    XCPC_ROM_BANK_STATUS_FAILURE = -1,
    XCPC_ROM_BANK_STATUS_SUCCESS =  0,
};

struct _XcpcRomBank
{
    uint8_t data[XCPC_ROM_BANK_SIZE];
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_ROM_BANK_IMPL_H__ */
