/*
 * ram-bank.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_RAM_BANK_H__
#define __XCPC_RAM_BANK_H__

#include <xcpc/ram-bank/ram-bank-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcRamBank*      xcpc_ram_bank_alloc     (void);
extern XcpcRamBank*      xcpc_ram_bank_free      (XcpcRamBank* ram_bank);
extern XcpcRamBank*      xcpc_ram_bank_construct (XcpcRamBank* ram_bank, const XcpcRamBankIface* ram_bank_iface);
extern XcpcRamBank*      xcpc_ram_bank_destruct  (XcpcRamBank* ram_bank);
extern XcpcRamBank*      xcpc_ram_bank_new       (const XcpcRamBankIface* ram_bank_iface);
extern XcpcRamBank*      xcpc_ram_bank_delete    (XcpcRamBank* ram_bank);
extern XcpcRamBank*      xcpc_ram_bank_reset     (XcpcRamBank* ram_bank);
extern XcpcRamBankStatus xcpc_ram_bank_load      (XcpcRamBank* ram_bank, const char* filename, size_t offset);
extern XcpcRamBankStatus xcpc_ram_bank_copy      (XcpcRamBank* ram_bank, const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_RAM_BANK_H__ */
