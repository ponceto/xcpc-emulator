/*
 * vdc-6845-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_VDC_6845_IMPL_H__
#define __XCPC_VDC_6845_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcVdc6845Iface XcpcVdc6845Iface;
typedef struct _XcpcVdc6845State XcpcVdc6845State;
typedef struct _XcpcVdc6845      XcpcVdc6845;

struct _XcpcVdc6845Iface
{
    void* user_data;
    void (*hsync_callback)(XcpcVdc6845*, int hsync);
    void (*vsync_callback)(XcpcVdc6845*, int vsync);
};

struct _XcpcVdc6845State
{
    union
    {
        struct
        {
            uint8_t addr;
            uint8_t data[18];
        } array;
        struct
        {
            uint8_t address_register;
            uint8_t horizontal_total;
            uint8_t horizontal_displayed;
            uint8_t horizontal_sync_position;
            uint8_t sync_width;
            uint8_t vertical_total;
            uint8_t vertical_total_adjust;
            uint8_t vertical_displayed;
            uint8_t vertical_sync_position;
            uint8_t interlace_mode_and_skew;
            uint8_t maximum_scanline_address;
            uint8_t cursor_start;
            uint8_t cursor_end;
            uint8_t start_address_high;
            uint8_t start_address_low;
            uint8_t cursor_high;
            uint8_t cursor_low;
            uint8_t light_pen_high;
            uint8_t light_pen_low;
        } named;
    } regs;
    union
    {
        struct
        {
            uint8_t horizontal_counter;
            uint8_t vertical_counter;
            uint8_t scanline_counter;
            uint8_t hsync_counter;
            uint8_t vsync_counter;
            uint8_t hsync_signal;
            uint8_t vsync_signal;
        } named;
    } ctrs;
};

struct _XcpcVdc6845
{
    XcpcVdc6845Iface iface;
    XcpcVdc6845State state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_VDC_6845_IMPL_H__ */
