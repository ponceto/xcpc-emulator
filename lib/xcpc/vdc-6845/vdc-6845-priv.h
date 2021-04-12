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

#include <xcpc/vdc-6845/vdc-6845.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NOT_READABLE 0x00
#define REG_READABLE 0x01
#define NOT_WRITABLE 0x00
#define REG_WRITABLE 0x02

enum Vdc6845RegisterIndex
{
    VDC_ADDRESS_REGISTER         = -1,
    VDC_HORIZONTAL_TOTAL         =  0,
    VDC_HORIZONTAL_DISPLAYED     =  1,
    VDC_HORIZONTAL_SYNC_POSITION =  2,
    VDC_SYNC_WIDTH               =  3,
    VDC_VERTICAL_TOTAL           =  4,
    VDC_VERTICAL_TOTAL_ADJUST    =  5,
    VDC_VERTICAL_DISPLAYED       =  6,
    VDC_VERTICAL_SYNC_POSITION   =  7,
    VDC_INTERLACE_MODE_AND_SKEW  =  8,
    VDC_MAXIMUM_SCANLINE_ADDRESS =  9,
    VDC_CURSOR_START             = 10,
    VDC_CURSOR_END               = 11,
    VDC_START_ADDRESS_HIGH       = 12,
    VDC_START_ADDRESS_LOW        = 13,
    VDC_CURSOR_HIGH              = 14,
    VDC_CURSOR_LOW               = 15,
    VDC_LIGHT_PEN_HIGH           = 16,
    VDC_LIGHT_PEN_LOW            = 17,
    VDC_REGISTER_COUNT           = 18,
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VDC_6845_PRIV_H__ */
