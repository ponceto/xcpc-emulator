/*
 * keyboard-priv.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_KEYBOARD_PRIV_H__
#define __XCPC_KEYBOARD_PRIV_H__

#include <xcpc/keyboard.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SET_KEY(row, bit) do { line = row; data = bit; mods = ((mods & ~0x03) | 0x00); } while(0)
#define SFT_KEY(row, bit) do { line = row; data = bit; mods = ((mods & ~0x03) | 0x01); } while(0)
#define CTL_KEY(row, bit) do { line = row; data = bit; mods = ((mods & ~0x03) | 0x02); } while(0)
#define EXT_KEY(row, bit) do { line = row; data = bit; mods = ((mods & ~0x03) | 0x03); } while(0)

#define MOD_SHIFT     0x01
#define MOD_CONTROL   0x02
#define MOD_SHIFT_L   0x04
#define MOD_CONTROL_L 0x08
#define MOD_SHIFT_R   0x10
#define MOD_CONTROL_R 0x20

#define ROW0 0x00
#define ROW1 0x01
#define ROW2 0x02
#define ROW3 0x03
#define ROW4 0x04
#define ROW5 0x05
#define ROW6 0x06
#define ROW7 0x07
#define ROW8 0x08
#define ROW9 0x09
#define ROWF 0x0f

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_KEYBOARD_PRIV_H__ */
