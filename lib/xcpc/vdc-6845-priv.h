/*
 * vdc-6845-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_VDC_6845_PRIV_H__
#define __XCPC_VDC_6845_PRIV_H__

#include <xcpc/vdc-6845.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INDEX_OF_ADDRESS_REGISTER                 -1
#define INDEX_OF_HORIZONTAL_TOTAL                  0
#define INDEX_OF_HORIZONTAL_DISPLAYED              1
#define INDEX_OF_HORIZONTAL_SYNC_POSITION          2
#define INDEX_OF_SYNC_WIDTH                        3
#define INDEX_OF_VERTICAL_TOTAL                    4
#define INDEX_OF_VERTICAL_TOTAL_ADJUST             5
#define INDEX_OF_VERTICAL_DISPLAYED                6
#define INDEX_OF_VERTICAL_SYNC_POSITION            7
#define INDEX_OF_INTERLACE_MODE_AND_SKEW           8
#define INDEX_OF_MAXIMUM_SCANLINE_ADDRESS          9
#define INDEX_OF_CURSOR_START                     10
#define INDEX_OF_CURSOR_END                       11
#define INDEX_OF_START_ADDRESS_HIGH               12
#define INDEX_OF_START_ADDRESS_LOW                13
#define INDEX_OF_CURSOR_HIGH                      14
#define INDEX_OF_CURSOR_LOW                       15
#define INDEX_OF_LIGHT_PEN_HIGH                   16
#define INDEX_OF_LIGHT_PEN_LOW                    17

#define MASK_OF_ADDRESS_REGISTER                  0xff /* 0b11111111 */
#define MASK_OF_HORIZONTAL_TOTAL                  0xff /* 0b11111111 */
#define MASK_OF_HORIZONTAL_DISPLAYED              0xff /* 0b11111111 */
#define MASK_OF_HORIZONTAL_SYNC_POSITION          0xff /* 0b11111111 */
#define MASK_OF_SYNC_WIDTH                        0x0f /* 0b00001111 */
#define MASK_OF_VERTICAL_TOTAL                    0x7f /* 0b01111111 */
#define MASK_OF_VERTICAL_TOTAL_ADJUST             0x1f /* 0b00011111 */
#define MASK_OF_VERTICAL_DISPLAYED                0x7f /* 0b01111111 */
#define MASK_OF_VERTICAL_SYNC_POSITION            0x7f /* 0b01111111 */
#define MASK_OF_INTERLACE_MODE_AND_SKEW           0x03 /* 0b00000011 */
#define MASK_OF_MAXIMUM_SCANLINE_ADDRESS          0x1f /* 0b00011111 */
#define MASK_OF_CURSOR_START                      0x7f /* 0b01111111 */
#define MASK_OF_CURSOR_END                        0x1f /* 0b00011111 */
#define MASK_OF_START_ADDRESS_HIGH                0x3f /* 0b00111111 */
#define MASK_OF_START_ADDRESS_LOW                 0xff /* 0b11111111 */
#define MASK_OF_CURSOR_HIGH                       0x3f /* 0b00111111 */
#define MASK_OF_CURSOR_LOW                        0xff /* 0b11111111 */
#define MASK_OF_LIGHT_PEN_HIGH                    0x3f /* 0b00111111 */
#define MASK_OF_LIGHT_PEN_LOW                     0xff /* 0b11111111 */

#define IS_READABLE_ADDRESS_REGISTER              0x0
#define IS_READABLE_HORIZONTAL_TOTAL              0x0
#define IS_READABLE_HORIZONTAL_DISPLAYED          0x0
#define IS_READABLE_HORIZONTAL_SYNC_POSITION      0x0
#define IS_READABLE_SYNC_WIDTH                    0x0
#define IS_READABLE_VERTICAL_TOTAL                0x0
#define IS_READABLE_VERTICAL_TOTAL_ADJUST         0x0
#define IS_READABLE_VERTICAL_DISPLAYED            0x0
#define IS_READABLE_VERTICAL_SYNC_POSITION        0x0
#define IS_READABLE_INTERLACE_MODE_AND_SKEW       0x0
#define IS_READABLE_MAXIMUM_SCANLINE_ADDRESS      0x0
#define IS_READABLE_CURSOR_START                  0x0
#define IS_READABLE_CURSOR_END                    0x0
#define IS_READABLE_START_ADDRESS_HIGH            0x0
#define IS_READABLE_START_ADDRESS_LOW             0x0
#define IS_READABLE_CURSOR_HIGH                   0x1
#define IS_READABLE_CURSOR_LOW                    0x1
#define IS_READABLE_LIGHT_PEN_HIGH                0x1
#define IS_READABLE_LIGHT_PEN_LOW                 0x1

#define IS_WRITABLE_ADDRESS_REGISTER              0x1
#define IS_WRITABLE_HORIZONTAL_TOTAL              0x1
#define IS_WRITABLE_HORIZONTAL_DISPLAYED          0x1
#define IS_WRITABLE_HORIZONTAL_SYNC_POSITION      0x1
#define IS_WRITABLE_SYNC_WIDTH                    0x1
#define IS_WRITABLE_VERTICAL_TOTAL                0x1
#define IS_WRITABLE_VERTICAL_TOTAL_ADJUST         0x1
#define IS_WRITABLE_VERTICAL_DISPLAYED            0x1
#define IS_WRITABLE_VERTICAL_SYNC_POSITION        0x1
#define IS_WRITABLE_INTERLACE_MODE_AND_SKEW       0x1
#define IS_WRITABLE_MAXIMUM_SCANLINE_ADDRESS      0x1
#define IS_WRITABLE_CURSOR_START                  0x1
#define IS_WRITABLE_CURSOR_END                    0x1
#define IS_WRITABLE_START_ADDRESS_HIGH            0x1
#define IS_WRITABLE_START_ADDRESS_LOW             0x1
#define IS_WRITABLE_CURSOR_HIGH                   0x1
#define IS_WRITABLE_CURSOR_LOW                    0x1
#define IS_WRITABLE_LIGHT_PEN_HIGH                0x0
#define IS_WRITABLE_LIGHT_PEN_LOW                 0x0

#define DEFAULT_VALUE_OF_ADDRESS_REGISTER         0x00
#define DEFAULT_VALUE_OF_HORIZONTAL_TOTAL         0x65
#define DEFAULT_VALUE_OF_HORIZONTAL_DISPLAYED     0x50
#define DEFAULT_VALUE_OF_HORIZONTAL_SYNC_POSITION 0x56
#define DEFAULT_VALUE_OF_SYNC_WIDTH               0x09
#define DEFAULT_VALUE_OF_VERTICAL_TOTAL           0x18
#define DEFAULT_VALUE_OF_VERTICAL_TOTAL_ADJUST    0x0a
#define DEFAULT_VALUE_OF_VERTICAL_DISPLAYED       0x18
#define DEFAULT_VALUE_OF_VERTICAL_SYNC_POSITION   0x18
#define DEFAULT_VALUE_OF_INTERLACE_MODE_AND_SKEW  0x00
#define DEFAULT_VALUE_OF_MAXIMUM_SCANLINE_ADDRESS 0x0b
#define DEFAULT_VALUE_OF_CURSOR_START             0x00
#define DEFAULT_VALUE_OF_CURSOR_END               0x0b
#define DEFAULT_VALUE_OF_START_ADDRESS_HIGH       0x00
#define DEFAULT_VALUE_OF_START_ADDRESS_LOW        0x80
#define DEFAULT_VALUE_OF_CURSOR_HIGH              0x00
#define DEFAULT_VALUE_OF_CURSOR_LOW               0x80
#define DEFAULT_VALUE_OF_LIGHT_PEN_HIGH           0x00
#define DEFAULT_VALUE_OF_LIGHT_PEN_LOW            0x00

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VDC_6845_PRIV_H__ */
