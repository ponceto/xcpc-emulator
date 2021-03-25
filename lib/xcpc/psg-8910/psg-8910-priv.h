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

#define INDEX_OF_ADDRESS_REGISTER              -1
#define INDEX_OF_CHANNEL_A_FINE_TUNE            0
#define INDEX_OF_CHANNEL_A_COARSE_TUNE          1
#define INDEX_OF_CHANNEL_B_FINE_TUNE            2
#define INDEX_OF_CHANNEL_B_COARSE_TUNE          3
#define INDEX_OF_CHANNEL_C_FINE_TUNE            4
#define INDEX_OF_CHANNEL_C_COARSE_TUNE          5
#define INDEX_OF_NOISE_PERIOD                   6
#define INDEX_OF_MIXER_AND_IO_CONTROL           7
#define INDEX_OF_CHANNEL_A_VOLUME               8
#define INDEX_OF_CHANNEL_B_VOLUME               9
#define INDEX_OF_CHANNEL_C_VOLUME              10
#define INDEX_OF_ENVELOPE_FINE_TUNE            11
#define INDEX_OF_ENVELOPE_COARSE_TUNE          12
#define INDEX_OF_ENVELOPE_SHAPE                13
#define INDEX_OF_IO_PORT_A                     14
#define INDEX_OF_IO_PORT_B                     15

#define MASK_OF_ADDRESS_REGISTER               0xff
#define MASK_OF_CHANNEL_A_FINE_TUNE            0xff
#define MASK_OF_CHANNEL_A_COARSE_TUNE          0x0f
#define MASK_OF_CHANNEL_B_FINE_TUNE            0xff
#define MASK_OF_CHANNEL_B_COARSE_TUNE          0x0f
#define MASK_OF_CHANNEL_C_FINE_TUNE            0xff
#define MASK_OF_CHANNEL_C_COARSE_TUNE          0x0f
#define MASK_OF_NOISE_PERIOD                   0x1f
#define MASK_OF_MIXER_AND_IO_CONTROL           0xff
#define MASK_OF_CHANNEL_A_VOLUME               0x1f
#define MASK_OF_CHANNEL_B_VOLUME               0x1f
#define MASK_OF_CHANNEL_C_VOLUME               0x1f
#define MASK_OF_ENVELOPE_FINE_TUNE             0xff
#define MASK_OF_ENVELOPE_COARSE_TUNE           0xff
#define MASK_OF_ENVELOPE_SHAPE                 0x0f
#define MASK_OF_IO_PORT_A                      0xff
#define MASK_OF_IO_PORT_B                      0xff

#define IS_READABLE_ADDRESS_REGISTER           0x1
#define IS_READABLE_CHANNEL_A_FINE_TUNE        0x1
#define IS_READABLE_CHANNEL_A_COARSE_TUNE      0x1
#define IS_READABLE_CHANNEL_B_FINE_TUNE        0x1
#define IS_READABLE_CHANNEL_B_COARSE_TUNE      0x1
#define IS_READABLE_CHANNEL_C_FINE_TUNE        0x1
#define IS_READABLE_CHANNEL_C_COARSE_TUNE      0x1
#define IS_READABLE_NOISE_PERIOD               0x1
#define IS_READABLE_MIXER_AND_IO_CONTROL       0x1
#define IS_READABLE_CHANNEL_A_VOLUME           0x1
#define IS_READABLE_CHANNEL_B_VOLUME           0x1
#define IS_READABLE_CHANNEL_C_VOLUME           0x1
#define IS_READABLE_ENVELOPE_FINE_TUNE         0x1
#define IS_READABLE_ENVELOPE_COARSE_TUNE       0x1
#define IS_READABLE_ENVELOPE_SHAPE             0x1
#define IS_READABLE_IO_PORT_A                  0x1
#define IS_READABLE_IO_PORT_B                  0x1

#define IS_WRITABLE_ADDRESS_REGISTER           0x1
#define IS_WRITABLE_CHANNEL_A_FINE_TUNE        0x1
#define IS_WRITABLE_CHANNEL_A_COARSE_TUNE      0x1
#define IS_WRITABLE_CHANNEL_B_FINE_TUNE        0x1
#define IS_WRITABLE_CHANNEL_B_COARSE_TUNE      0x1
#define IS_WRITABLE_CHANNEL_C_FINE_TUNE        0x1
#define IS_WRITABLE_CHANNEL_C_COARSE_TUNE      0x1
#define IS_WRITABLE_NOISE_PERIOD               0x1
#define IS_WRITABLE_MIXER_AND_IO_CONTROL       0x1
#define IS_WRITABLE_CHANNEL_A_VOLUME           0x1
#define IS_WRITABLE_CHANNEL_B_VOLUME           0x1
#define IS_WRITABLE_CHANNEL_C_VOLUME           0x1
#define IS_WRITABLE_ENVELOPE_FINE_TUNE         0x1
#define IS_WRITABLE_ENVELOPE_COARSE_TUNE       0x1
#define IS_WRITABLE_ENVELOPE_SHAPE             0x1
#define IS_WRITABLE_IO_PORT_A                  0x1
#define IS_WRITABLE_IO_PORT_B                  0x1

#define DEFAULT_VALUE_OF_ADDRESS_REGISTER      0x00
#define DEFAULT_VALUE_OF_CHANNEL_A_FINE_TUNE   0x00
#define DEFAULT_VALUE_OF_CHANNEL_A_COARSE_TUNE 0x00
#define DEFAULT_VALUE_OF_CHANNEL_B_FINE_TUNE   0x00
#define DEFAULT_VALUE_OF_CHANNEL_B_COARSE_TUNE 0x00
#define DEFAULT_VALUE_OF_CHANNEL_C_FINE_TUNE   0x00
#define DEFAULT_VALUE_OF_CHANNEL_C_COARSE_TUNE 0x00
#define DEFAULT_VALUE_OF_NOISE_PERIOD          0x00
#define DEFAULT_VALUE_OF_MIXER_AND_IO_CONTROL  0x00
#define DEFAULT_VALUE_OF_CHANNEL_A_VOLUME      0x00
#define DEFAULT_VALUE_OF_CHANNEL_B_VOLUME      0x00
#define DEFAULT_VALUE_OF_CHANNEL_C_VOLUME      0x00
#define DEFAULT_VALUE_OF_ENVELOPE_FINE_TUNE    0x00
#define DEFAULT_VALUE_OF_ENVELOPE_COARSE_TUNE  0x00
#define DEFAULT_VALUE_OF_ENVELOPE_SHAPE        0x00
#define DEFAULT_VALUE_OF_IO_PORT_A             0x00
#define DEFAULT_VALUE_OF_IO_PORT_B             0x00

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PSG_8910_PRIV_H__ */
