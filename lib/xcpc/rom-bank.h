/*
 * rom-bank.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_ROM_BANK_H__
#define __XCPC_ROM_BANK_H__

#include <xcpc/rom-bank-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcRomBank*      xcpc_rom_bank_alloc     (void);
extern XcpcRomBank*      xcpc_rom_bank_free      (XcpcRomBank* rom_bank);
extern XcpcRomBank*      xcpc_rom_bank_construct (XcpcRomBank* rom_bank);
extern XcpcRomBank*      xcpc_rom_bank_destruct  (XcpcRomBank* rom_bank);
extern XcpcRomBank*      xcpc_rom_bank_new       (void);
extern XcpcRomBank*      xcpc_rom_bank_delete    (XcpcRomBank* rom_bank);
extern XcpcRomBank*      xcpc_rom_bank_reset     (XcpcRomBank* rom_bank);
extern XcpcRomBank*      xcpc_rom_bank_clear     (XcpcRomBank* rom_bank);
extern XcpcRomBankStatus xcpc_rom_bank_load      (XcpcRomBank* rom_bank, const char* filename, size_t offset);
extern XcpcRomBankStatus xcpc_rom_bank_copy      (XcpcRomBank* rom_bank, const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_ROM_BANK_H__ */
