/*
 * vdc-6845.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vdc-6845-priv.h"

static void xcpc_vdc_6845_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcVdc6845::%s()"
          , function );
}

XcpcVdc6845* xcpc_vdc_6845_alloc(void)
{
    xcpc_vdc_6845_trace("alloc");

    return xcpc_new(XcpcVdc6845);
}

XcpcVdc6845* xcpc_vdc_6845_free(XcpcVdc6845* self)
{
    xcpc_vdc_6845_trace("free");

    return xcpc_delete(XcpcVdc6845, self);
}

XcpcVdc6845* xcpc_vdc_6845_construct(XcpcVdc6845* self)
{
    xcpc_vdc_6845_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcVdc6845Iface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcVdc6845State));
    }
    return xcpc_vdc_6845_reset(self);
}

XcpcVdc6845* xcpc_vdc_6845_destruct(XcpcVdc6845* self)
{
    xcpc_vdc_6845_trace("destruct");

    return self;
}

XcpcVdc6845* xcpc_vdc_6845_new(void)
{
    xcpc_vdc_6845_trace("new");

    return xcpc_vdc_6845_construct(xcpc_vdc_6845_alloc());
}

XcpcVdc6845* xcpc_vdc_6845_delete(XcpcVdc6845* self)
{
    xcpc_vdc_6845_trace("delete");

    return xcpc_vdc_6845_free(xcpc_vdc_6845_destruct(self));
}

XcpcVdc6845* xcpc_vdc_6845_reset(XcpcVdc6845* self)
{
    xcpc_vdc_6845_trace("reset");

    /* reset registers */ {
        self->state.regs.named.address_register         = DEFAULT_VALUE_OF_ADDRESS_REGISTER;
        self->state.regs.named.horizontal_total         = DEFAULT_VALUE_OF_HORIZONTAL_TOTAL;
        self->state.regs.named.horizontal_displayed     = DEFAULT_VALUE_OF_HORIZONTAL_DISPLAYED;
        self->state.regs.named.horizontal_sync_position = DEFAULT_VALUE_OF_HORIZONTAL_SYNC_POSITION;
        self->state.regs.named.sync_width               = DEFAULT_VALUE_OF_SYNC_WIDTH;
        self->state.regs.named.vertical_total           = DEFAULT_VALUE_OF_VERTICAL_TOTAL;
        self->state.regs.named.vertical_total_adjust    = DEFAULT_VALUE_OF_VERTICAL_TOTAL_ADJUST;
        self->state.regs.named.vertical_displayed       = DEFAULT_VALUE_OF_VERTICAL_DISPLAYED;
        self->state.regs.named.vertical_sync_position   = DEFAULT_VALUE_OF_VERTICAL_SYNC_POSITION;
        self->state.regs.named.interlace_mode_and_skew  = DEFAULT_VALUE_OF_INTERLACE_MODE_AND_SKEW;
        self->state.regs.named.maximum_scanline_address = DEFAULT_VALUE_OF_MAXIMUM_SCANLINE_ADDRESS;
        self->state.regs.named.cursor_start             = DEFAULT_VALUE_OF_CURSOR_START;
        self->state.regs.named.cursor_end               = DEFAULT_VALUE_OF_CURSOR_END;
        self->state.regs.named.start_address_high       = DEFAULT_VALUE_OF_START_ADDRESS_HIGH;
        self->state.regs.named.start_address_low        = DEFAULT_VALUE_OF_START_ADDRESS_LOW;
        self->state.regs.named.cursor_high              = DEFAULT_VALUE_OF_CURSOR_HIGH;
        self->state.regs.named.cursor_low               = DEFAULT_VALUE_OF_CURSOR_LOW;
        self->state.regs.named.light_pen_high           = DEFAULT_VALUE_OF_LIGHT_PEN_HIGH;
        self->state.regs.named.light_pen_low            = DEFAULT_VALUE_OF_LIGHT_PEN_LOW;
    }
    /* reset counters */ {
        self->state.ctrs.named.horizontal_counter = 0;
        self->state.ctrs.named.vertical_counter   = 0;
        self->state.ctrs.named.scanline_counter   = 0;
        self->state.ctrs.named.hsync_counter      = 0;
        self->state.ctrs.named.vsync_counter      = 0;
        self->state.ctrs.named.hsync_signal       = 0;
        self->state.ctrs.named.vsync_signal       = 0;
    }
    return self;
}

XcpcVdc6845* xcpc_vdc_6845_clock(XcpcVdc6845* self)
{
    uint8_t old_hsync_signal = self->state.ctrs.named.hsync_signal;
    uint8_t old_vsync_signal = self->state.ctrs.named.vsync_signal;

    if(self->state.ctrs.named.hsync_counter > 0) {
        if(--self->state.ctrs.named.hsync_counter == 0) {
            self->state.ctrs.named.hsync_signal = 0;
        }
    }
    if(++self->state.ctrs.named.horizontal_counter == (self->state.regs.named.horizontal_total + 1)) {
        self->state.ctrs.named.horizontal_counter = 0;
        if(self->state.ctrs.named.vsync_counter > 0) {
            if(--self->state.ctrs.named.vsync_counter == 0) {
                self->state.ctrs.named.vsync_signal = 0;
            }
        }
        if(++self->state.ctrs.named.scanline_counter == (self->state.regs.named.maximum_scanline_address + 1)) {
            self->state.ctrs.named.scanline_counter = 0;
            if(++self->state.ctrs.named.vertical_counter == (self->state.regs.named.vertical_total + 1)) {
                self->state.ctrs.named.vertical_counter = 0;
            }
        }
    }
    if((self->state.ctrs.named.hsync_signal == 0) && (self->state.ctrs.named.horizontal_counter == self->state.regs.named.horizontal_sync_position)) {
        self->state.ctrs.named.hsync_signal = 1;
        if((self->state.ctrs.named.hsync_counter = (self->state.regs.named.sync_width >> 0) & 0x0f) == 0) {
            self->state.ctrs.named.hsync_counter = 16;
        }
    }
    if((self->state.ctrs.named.vsync_signal == 0) && (self->state.ctrs.named.vertical_counter == self->state.regs.named.vertical_sync_position)) {
        self->state.ctrs.named.vsync_signal = 1;
        if((self->state.ctrs.named.vsync_counter = (self->state.regs.named.sync_width >> 4) & 0x0f) == 0) {
            self->state.ctrs.named.vsync_counter = 16;
        }
    }
    if((self->state.ctrs.named.vsync_signal != old_vsync_signal) && (self->iface.vsync_callback != NULL)) {
        (*self->iface.vsync_callback)(self, self->state.ctrs.named.vsync_signal, self->iface.user_data);
    }
    if((self->state.ctrs.named.hsync_signal != old_hsync_signal) && (self->iface.hsync_callback != NULL)) {
        (*self->iface.hsync_callback)(self, self->state.ctrs.named.hsync_signal, self->iface.user_data);
    }
    return self;
}

uint8_t xcpc_vdc_6845_rg(XcpcVdc6845* self, uint8_t data_bus)
{
    if(IS_READABLE_ADDRESS_REGISTER) {
        data_bus = self->state.regs.named.address_register;
        data_bus &= MASK_OF_ADDRESS_REGISTER;
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_rs(XcpcVdc6845* self, uint8_t data_bus)
{
    if(IS_WRITABLE_ADDRESS_REGISTER) {
        data_bus &= MASK_OF_ADDRESS_REGISTER;
        self->state.regs.named.address_register = data_bus;
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_rd(XcpcVdc6845* self, uint8_t data_bus)
{
    switch(self->state.regs.named.address_register) {
        case INDEX_OF_HORIZONTAL_TOTAL:
            if(IS_READABLE_HORIZONTAL_TOTAL) {
                data_bus = self->state.regs.named.horizontal_total;
                data_bus &= MASK_OF_HORIZONTAL_TOTAL;
            }
            break;
        case INDEX_OF_HORIZONTAL_DISPLAYED:
            if(IS_READABLE_HORIZONTAL_DISPLAYED) {
                data_bus = self->state.regs.named.horizontal_displayed;
                data_bus &= MASK_OF_HORIZONTAL_DISPLAYED;
            }
            break;
        case INDEX_OF_HORIZONTAL_SYNC_POSITION:
            if(IS_READABLE_HORIZONTAL_SYNC_POSITION) {
                data_bus = self->state.regs.named.horizontal_sync_position;
                data_bus &= MASK_OF_HORIZONTAL_SYNC_POSITION;
            }
            break;
        case INDEX_OF_SYNC_WIDTH:
            if(IS_READABLE_SYNC_WIDTH) {
                data_bus = self->state.regs.named.sync_width;
                data_bus &= MASK_OF_SYNC_WIDTH;
            }
            break;
        case INDEX_OF_VERTICAL_TOTAL:
            if(IS_READABLE_VERTICAL_TOTAL) {
                data_bus = self->state.regs.named.vertical_total;
                data_bus &= MASK_OF_VERTICAL_TOTAL;
            }
            break;
        case INDEX_OF_VERTICAL_TOTAL_ADJUST:
            if(IS_READABLE_VERTICAL_TOTAL_ADJUST) {
                data_bus = self->state.regs.named.vertical_total_adjust;
                data_bus &= MASK_OF_VERTICAL_TOTAL_ADJUST;
            }
            break;
        case INDEX_OF_VERTICAL_DISPLAYED:
            if(IS_READABLE_VERTICAL_DISPLAYED) {
                data_bus = self->state.regs.named.vertical_displayed;
                data_bus &= MASK_OF_VERTICAL_DISPLAYED;
            }
            break;
        case INDEX_OF_VERTICAL_SYNC_POSITION:
            if(IS_READABLE_VERTICAL_SYNC_POSITION) {
                data_bus = self->state.regs.named.vertical_sync_position;
                data_bus &= MASK_OF_VERTICAL_SYNC_POSITION;
            }
            break;
        case INDEX_OF_INTERLACE_MODE_AND_SKEW:
            if(IS_READABLE_INTERLACE_MODE_AND_SKEW) {
                data_bus = self->state.regs.named.interlace_mode_and_skew;
                data_bus &= MASK_OF_INTERLACE_MODE_AND_SKEW;
            }
            break;
        case INDEX_OF_MAXIMUM_SCANLINE_ADDRESS:
            if(IS_READABLE_MAXIMUM_SCANLINE_ADDRESS) {
                data_bus = self->state.regs.named.maximum_scanline_address;
                data_bus &= MASK_OF_MAXIMUM_SCANLINE_ADDRESS;
            }
            break;
        case INDEX_OF_CURSOR_START:
            if(IS_READABLE_CURSOR_START) {
                data_bus = self->state.regs.named.cursor_start;
                data_bus &= MASK_OF_CURSOR_START;
            }
            break;
        case INDEX_OF_CURSOR_END:
            if(IS_READABLE_CURSOR_END) {
                data_bus = self->state.regs.named.cursor_end;
                data_bus &= MASK_OF_CURSOR_END;
            }
            break;
        case INDEX_OF_START_ADDRESS_HIGH:
            if(IS_READABLE_START_ADDRESS_HIGH) {
                data_bus = self->state.regs.named.start_address_high;
                data_bus &= MASK_OF_START_ADDRESS_HIGH;
            }
            break;
        case INDEX_OF_START_ADDRESS_LOW:
            if(IS_READABLE_START_ADDRESS_LOW) {
                data_bus = self->state.regs.named.start_address_low;
                data_bus &= MASK_OF_START_ADDRESS_LOW;
            }
            break;
        case INDEX_OF_CURSOR_HIGH:
            if(IS_READABLE_CURSOR_HIGH) {
                data_bus = self->state.regs.named.cursor_high;
                data_bus &= MASK_OF_CURSOR_HIGH;
            }
            break;
        case INDEX_OF_CURSOR_LOW:
            if(IS_READABLE_CURSOR_LOW) {
                data_bus = self->state.regs.named.cursor_low;
                data_bus &= MASK_OF_CURSOR_LOW;
            }
            break;
        case INDEX_OF_LIGHT_PEN_HIGH:
            if(IS_READABLE_LIGHT_PEN_HIGH) {
                data_bus = self->state.regs.named.light_pen_high;
                data_bus &= MASK_OF_LIGHT_PEN_HIGH;
            }
            break;
        case INDEX_OF_LIGHT_PEN_LOW:
            if(IS_READABLE_LIGHT_PEN_LOW) {
                data_bus = self->state.regs.named.light_pen_low;
                data_bus &= MASK_OF_LIGHT_PEN_LOW;
            }
            break;
        default:
            break;
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_wr(XcpcVdc6845* self, uint8_t data_bus)
{
    switch(self->state.regs.named.address_register) {
        case INDEX_OF_HORIZONTAL_TOTAL:
            if(IS_WRITABLE_HORIZONTAL_TOTAL) {
                data_bus &= MASK_OF_HORIZONTAL_TOTAL;
                self->state.regs.named.horizontal_total = data_bus;
            }
            break;
        case INDEX_OF_HORIZONTAL_DISPLAYED:
            if(IS_WRITABLE_HORIZONTAL_DISPLAYED) {
                data_bus &= MASK_OF_HORIZONTAL_DISPLAYED;
                self->state.regs.named.horizontal_displayed = data_bus;
            }
            break;
        case INDEX_OF_HORIZONTAL_SYNC_POSITION:
            if(IS_WRITABLE_HORIZONTAL_SYNC_POSITION) {
                data_bus &= MASK_OF_HORIZONTAL_SYNC_POSITION;
                self->state.regs.named.horizontal_sync_position = data_bus;
            }
            break;
        case INDEX_OF_SYNC_WIDTH:
            if(IS_WRITABLE_SYNC_WIDTH) {
                data_bus &= MASK_OF_SYNC_WIDTH;
                self->state.regs.named.sync_width = data_bus;
            }
            break;
        case INDEX_OF_VERTICAL_TOTAL:
            if(IS_WRITABLE_VERTICAL_TOTAL) {
                data_bus &= MASK_OF_VERTICAL_TOTAL;
                self->state.regs.named.vertical_total = data_bus;
            }
            break;
        case INDEX_OF_VERTICAL_TOTAL_ADJUST:
            if(IS_WRITABLE_VERTICAL_TOTAL_ADJUST) {
                data_bus &= MASK_OF_VERTICAL_TOTAL_ADJUST;
                self->state.regs.named.vertical_total_adjust = data_bus;
            }
            break;
        case INDEX_OF_VERTICAL_DISPLAYED:
            if(IS_WRITABLE_VERTICAL_DISPLAYED) {
                data_bus &= MASK_OF_VERTICAL_DISPLAYED;
                self->state.regs.named.vertical_displayed = data_bus;
            }
            break;
        case INDEX_OF_VERTICAL_SYNC_POSITION:
            if(IS_WRITABLE_VERTICAL_SYNC_POSITION) {
                data_bus &= MASK_OF_VERTICAL_SYNC_POSITION;
                self->state.regs.named.vertical_sync_position = data_bus;
            }
            break;
        case INDEX_OF_INTERLACE_MODE_AND_SKEW:
            if(IS_WRITABLE_INTERLACE_MODE_AND_SKEW) {
                data_bus &= MASK_OF_INTERLACE_MODE_AND_SKEW;
                self->state.regs.named.interlace_mode_and_skew = data_bus;
            }
            break;
        case INDEX_OF_MAXIMUM_SCANLINE_ADDRESS:
            if(IS_WRITABLE_MAXIMUM_SCANLINE_ADDRESS) {
                data_bus &= MASK_OF_MAXIMUM_SCANLINE_ADDRESS;
                self->state.regs.named.maximum_scanline_address = data_bus;
            }
            break;
        case INDEX_OF_CURSOR_START:
            if(IS_WRITABLE_CURSOR_START) {
                data_bus &= MASK_OF_CURSOR_START;
                self->state.regs.named.cursor_start = data_bus;
            }
            break;
        case INDEX_OF_CURSOR_END:
            if(IS_WRITABLE_CURSOR_END) {
                data_bus &= MASK_OF_CURSOR_END;
                self->state.regs.named.cursor_end = data_bus;
            }
            break;
        case INDEX_OF_START_ADDRESS_HIGH:
            if(IS_WRITABLE_START_ADDRESS_HIGH) {
                data_bus &= MASK_OF_START_ADDRESS_HIGH;
                self->state.regs.named.start_address_high = data_bus;
            }
            break;
        case INDEX_OF_START_ADDRESS_LOW:
            if(IS_WRITABLE_START_ADDRESS_LOW) {
                data_bus &= MASK_OF_START_ADDRESS_LOW;
                self->state.regs.named.start_address_low = data_bus;
            }
            break;
        case INDEX_OF_CURSOR_HIGH:
            if(IS_WRITABLE_CURSOR_HIGH) {
                data_bus &= MASK_OF_CURSOR_HIGH;
                self->state.regs.named.cursor_high = data_bus;
            }
            break;
        case INDEX_OF_CURSOR_LOW:
            if(IS_WRITABLE_CURSOR_LOW) {
                data_bus &= MASK_OF_CURSOR_LOW;
                self->state.regs.named.cursor_low = data_bus;
            }
            break;
        case INDEX_OF_LIGHT_PEN_HIGH:
            if(IS_WRITABLE_LIGHT_PEN_HIGH) {
                data_bus &= MASK_OF_LIGHT_PEN_HIGH;
                self->state.regs.named.light_pen_high = data_bus;
            }
            break;
        case INDEX_OF_LIGHT_PEN_LOW:
            if(IS_WRITABLE_LIGHT_PEN_LOW) {
                data_bus &= MASK_OF_LIGHT_PEN_LOW;
                self->state.regs.named.light_pen_low = data_bus;
            }
            break;
        default:
            break;
    }
    return data_bus;
}
