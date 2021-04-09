/*
 * psg-8910-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_PSG_8910_PRIV_H__
#define __XCPC_PSG_8910_PRIV_H__

#include <xcpc/psg-8910/psg-8910.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NOT_READABLE 0x00
#define REG_READABLE 0x01
#define NOT_WRITABLE 0x00
#define REG_WRITABLE 0x02

enum Psg8910Registers
{
    PSG_ADDRESS_REGISTER      = -1,
    PSG_CHANNEL_A_FINE_TUNE   =  0,
    PSG_CHANNEL_A_COARSE_TUNE =  1,
    PSG_CHANNEL_B_FINE_TUNE   =  2,
    PSG_CHANNEL_B_COARSE_TUNE =  3,
    PSG_CHANNEL_C_FINE_TUNE   =  4,
    PSG_CHANNEL_C_COARSE_TUNE =  5,
    PSG_NOISE_PERIOD          =  6,
    PSG_MIXER_AND_IO_CONTROL  =  7,
    PSG_CHANNEL_A_VOLUME      =  8,
    PSG_CHANNEL_B_VOLUME      =  9,
    PSG_CHANNEL_C_VOLUME      = 10,
    PSG_ENVELOPE_FINE_TUNE    = 11,
    PSG_ENVELOPE_COARSE_TUNE  = 12,
    PSG_ENVELOPE_SHAPE        = 13,
    PSG_IO_PORT_A             = 14,
    PSG_IO_PORT_B             = 15,
    PSG_REGISTER_COUNT        = 16,
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PSG_8910_PRIV_H__ */
