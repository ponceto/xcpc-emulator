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

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcVdc6845::%s()", function);
}

static uint8_t default_frame_handler(XcpcVdc6845* self)
{
    log_trace("default_frame_handler");

    return 0x00;
}

static uint8_t default_hsync_handler(XcpcVdc6845* self, int hsync)
{
    log_trace("default_hsync_handler");

    return 0x00;
}

static uint8_t default_vsync_handler(XcpcVdc6845* self, int vsync)
{
    log_trace("default_vsync_handler");

    return 0x00;
}

static void reset_setup(XcpcVdc6845* self)
{
    /* reset caps_of */ {
        self->setup.caps_of.addr       = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x00] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x01] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x02] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x03] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x04] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x05] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x06] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x07] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x08] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x09] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0a] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0b] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0c] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0d] = (NOT_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0e] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0f] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x10] = (REG_READABLE | NOT_WRITABLE);
        self->setup.caps_of.data[0x11] = (REG_READABLE | NOT_WRITABLE);
    }
    /* reset mask_of */ {
        self->setup.mask_of.addr       = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x00] = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x01] = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x02] = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x03] = 0x0f; /* ----xxxx */
        self->setup.mask_of.data[0x04] = 0x7f; /* -xxxxxxx */
        self->setup.mask_of.data[0x05] = 0x1f; /* ---xxxxx */
        self->setup.mask_of.data[0x06] = 0x7f; /* -xxxxxxx */
        self->setup.mask_of.data[0x07] = 0x7f; /* -xxxxxxx */
        self->setup.mask_of.data[0x08] = 0x03; /* ------xx */
        self->setup.mask_of.data[0x09] = 0x1f; /* ---xxxxx */
        self->setup.mask_of.data[0x0a] = 0x7f; /* -xxxxxxx */
        self->setup.mask_of.data[0x0b] = 0x1f; /* ---xxxxx */
        self->setup.mask_of.data[0x0c] = 0x3f; /* --xxxxxx */
        self->setup.mask_of.data[0x0d] = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x0e] = 0x3f; /* --xxxxxx */
        self->setup.mask_of.data[0x0f] = 0xff; /* xxxxxxxx */
        self->setup.mask_of.data[0x10] = 0x3f; /* --xxxxxx */
        self->setup.mask_of.data[0x11] = 0xff; /* xxxxxxxx */
    }
}

static void reset_state(XcpcVdc6845* self)
{
    /* reset internal registers */ {
        self->state.regs.named.address_register         = 0x00; /*       0 */
        self->state.regs.named.horizontal_total         = 0x3f; /*      63 */
        self->state.regs.named.horizontal_displayed     = 0x28; /*      40 */
        self->state.regs.named.horizontal_sync_position = 0x2e; /*      46 */
        self->state.regs.named.sync_width               = 0x0e; /* 16 + 14 */
        self->state.regs.named.vertical_total           = 0x26; /*      38 */
        self->state.regs.named.vertical_total_adjust    = 0x00; /*       0 */
        self->state.regs.named.vertical_displayed       = 0x19; /*      25 */
        self->state.regs.named.vertical_sync_position   = 0x1e; /*      30 */
        self->state.regs.named.interlace_mode_and_skew  = 0x00; /*       0 */
        self->state.regs.named.maximum_scanline_address = 0x07; /*       7 */
        self->state.regs.named.cursor_start             = 0x00; /*       0 */
        self->state.regs.named.cursor_end               = 0x00; /*       0 */
        self->state.regs.named.start_address_high       = 0x30; /*    0x30 */
        self->state.regs.named.start_address_low        = 0x00; /*    0x00 */
        self->state.regs.named.cursor_high              = 0x00; /*    0x00 */
        self->state.regs.named.cursor_low               = 0x00; /*    0x00 */
        self->state.regs.named.light_pen_high           = 0x00; /*    0x00 */
        self->state.regs.named.light_pen_low            = 0x00; /*    0x00 */
    }
}

static void reset_count(XcpcVdc6845* self)
{
    /* reset internal counters */ {
        self->count.horizontal_counter = 0;
        self->count.vertical_counter   = 0;
        self->count.scanline_counter   = 0;
        self->count.hsync_counter      = 0;
        self->count.hsync_active       = 0;
        self->count.vsync_counter      = 0;
        self->count.vsync_active       = 0;
    }
}

XcpcVdc6845* xcpc_vdc_6845_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcVdc6845);
}

XcpcVdc6845* xcpc_vdc_6845_free(XcpcVdc6845* self)
{
    log_trace("free");

    return xcpc_delete(XcpcVdc6845, self);
}

XcpcVdc6845* xcpc_vdc_6845_construct(XcpcVdc6845* self)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcVdc6845Iface));
        (void) memset(&self->setup, 0, sizeof(XcpcVdc6845Setup));
        (void) memset(&self->state, 0, sizeof(XcpcVdc6845State));
        (void) memset(&self->count, 0, sizeof(XcpcVdc6845Count));
    }
    /* initialize iface */ {
        (void) xcpc_vdc_6845_set_iface(self, NULL);
    }
    /* reset */ {
        (void) xcpc_vdc_6845_reset(self);
    }
    return self;
}

XcpcVdc6845* xcpc_vdc_6845_destruct(XcpcVdc6845* self)
{
    log_trace("destruct");

    return self;
}

XcpcVdc6845* xcpc_vdc_6845_new(void)
{
    log_trace("new");

    return xcpc_vdc_6845_construct(xcpc_vdc_6845_alloc());
}

XcpcVdc6845* xcpc_vdc_6845_delete(XcpcVdc6845* self)
{
    log_trace("delete");

    return xcpc_vdc_6845_free(xcpc_vdc_6845_destruct(self));
}

XcpcVdc6845* xcpc_vdc_6845_set_iface(XcpcVdc6845* self, const XcpcVdc6845Iface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
        self->iface.frame     = &default_frame_handler;
        self->iface.hsync     = &default_hsync_handler;
        self->iface.vsync     = &default_vsync_handler;
    }
    return self;
}

XcpcVdc6845* xcpc_vdc_6845_debug(XcpcVdc6845* self)
{
    const char* format = "  - %-24s : 0x%02x";

    /* debug state */ {
        xcpc_log_debug("vdc_6845:");
        xcpc_log_debug(format, "address_register"        , self->state.regs.named.address_register        );
        xcpc_log_debug(format, "horizontal_total"        , self->state.regs.named.horizontal_total        );
        xcpc_log_debug(format, "horizontal_displayed"    , self->state.regs.named.horizontal_displayed    );
        xcpc_log_debug(format, "horizontal_sync_position", self->state.regs.named.horizontal_sync_position);
        xcpc_log_debug(format, "sync_width"              , self->state.regs.named.sync_width              );
        xcpc_log_debug(format, "vertical_total"          , self->state.regs.named.vertical_total          );
        xcpc_log_debug(format, "vertical_total_adjust"   , self->state.regs.named.vertical_total_adjust   );
        xcpc_log_debug(format, "vertical_displayed"      , self->state.regs.named.vertical_displayed      );
        xcpc_log_debug(format, "vertical_sync_position"  , self->state.regs.named.vertical_sync_position  );
        xcpc_log_debug(format, "interlace_mode_and_skew" , self->state.regs.named.interlace_mode_and_skew );
        xcpc_log_debug(format, "maximum_scanline_address", self->state.regs.named.maximum_scanline_address);
        xcpc_log_debug(format, "cursor_start"            , self->state.regs.named.cursor_start            );
        xcpc_log_debug(format, "cursor_end"              , self->state.regs.named.cursor_end              );
        xcpc_log_debug(format, "start_address_high"      , self->state.regs.named.start_address_high      );
        xcpc_log_debug(format, "start_address_low"       , self->state.regs.named.start_address_low       );
        xcpc_log_debug(format, "cursor_high"             , self->state.regs.named.cursor_high             );
        xcpc_log_debug(format, "cursor_low"              , self->state.regs.named.cursor_low              );
        xcpc_log_debug(format, "light_pen_high"          , self->state.regs.named.light_pen_high          );
        xcpc_log_debug(format, "light_pen_low"           , self->state.regs.named.light_pen_low           );
    }
    return self;
}

XcpcVdc6845* xcpc_vdc_6845_reset(XcpcVdc6845* self)
{
    log_trace("reset");

    /* reset */ {
        reset_setup(self);
        reset_state(self);
        reset_count(self);
    }
    return self;
}

XcpcVdc6845* xcpc_vdc_6845_clock(XcpcVdc6845* self)
{
    uint8_t const horizontal_total         = (self->state.regs.named.horizontal_total         + 1);
    uint8_t const horizontal_displayed     = (self->state.regs.named.horizontal_displayed     + 0);
    uint8_t const horizontal_sync_position = (self->state.regs.named.horizontal_sync_position + 0);
    uint8_t const horizontal_sync_width    = (((self->state.regs.named.sync_width >> 0) & 0x0f)  );
    uint8_t const horizontal_sync_active   = (self->count.hsync_active != 0                      );
    uint8_t const vertical_total           = (self->state.regs.named.vertical_total           + 1);
    uint8_t const vertical_displayed       = (self->state.regs.named.vertical_displayed       + 0);
    uint8_t const vertical_sync_position   = (self->state.regs.named.vertical_sync_position   + 0);
    uint8_t const vertical_sync_width      = (((self->state.regs.named.sync_width >> 4) & 0x0f)  );
    uint8_t const vertical_sync_active     = (self->count.vsync_active != 0                      );
    uint8_t const scanline_total           = (self->state.regs.named.maximum_scanline_address + 1);
    uint8_t       process_horizontal       = 1;
    uint8_t       process_scanline         = 0;
    uint8_t       process_vertical         = 0;
    uint8_t       process_frame            = 0;

    if(process_horizontal != 0) {
        if(++self->count.horizontal_counter == horizontal_total) {
            self->count.horizontal_counter = 0;
            process_scanline = 1;
        }
        if(horizontal_sync_active != 0) {
            self->count.hsync_counter = ((self->count.hsync_counter + 1) & 0x0f);
            if(self->count.hsync_counter == horizontal_sync_width) {
                self->count.hsync_counter = 0;
                self->count.hsync_active  = 0;
            }
        }
        else {
            if(self->count.horizontal_counter == horizontal_sync_position) {
                self->count.hsync_counter = 0;
                self->count.hsync_active  = 1;
            }
        }
    }
    if(process_scanline != 0) {
        if(++self->count.scanline_counter == scanline_total) {
            self->count.scanline_counter = 0;
            process_vertical = 1;
        }
        if(vertical_sync_active != 0) {
            self->count.vsync_counter = ((self->count.vsync_counter + 1) & 0x0f);
            if(self->count.vsync_counter == vertical_sync_width) {
                self->count.vsync_counter = 0;
                self->count.vsync_active  = 0;
            }
        }
        else {
            if(self->count.vertical_counter == vertical_sync_position) {
                self->count.vsync_counter = 0;
                self->count.vsync_active  = 1;
            }
        }
    }
    if(process_vertical != 0) {
        if(++self->count.vertical_counter == vertical_total) {
            self->count.vertical_counter = 0;
            process_frame = 1;
        }
    }
    /* hsync handler */ {
        if(self->count.hsync_active != horizontal_sync_active) {
            (void) (*self->iface.hsync)(self, self->count.hsync_active);
        }
    }
    /* vsync handler */ {
        if(self->count.vsync_active != vertical_sync_active) {
            (void) (*self->iface.vsync)(self, self->count.vsync_active);
        }
    }
    /* frame handler */ {
        if(process_frame != 0) {
            (void) (*self->iface.frame)(self);
        }
    }
    return self;
}

uint8_t xcpc_vdc_6845_rg(XcpcVdc6845* self, uint8_t data_bus)
{
    uint8_t const is_readable   = (self->setup.caps_of.addr & REG_READABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_readable != 0) {
        data_bus = (*register_addr &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_rs(XcpcVdc6845* self, uint8_t data_bus)
{
    uint8_t const is_writable   = (self->setup.caps_of.addr & REG_WRITABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_writable != 0) {
        *register_addr = (data_bus &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_rd(XcpcVdc6845* self, uint8_t data_bus)
{
    const uint8_t address_register = self->state.regs.named.address_register;

    if(address_register < VDC_REGISTER_COUNT) {
        uint8_t const is_readable   = (self->setup.caps_of.data[address_register] & REG_READABLE);
        uint8_t const register_mask = (self->setup.mask_of.data[address_register]);
        uint8_t*      register_addr = (&self->state.regs.array.data[address_register]);
        if(is_readable != 0) {
            data_bus = (*register_addr &= register_mask);
        }
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_wr(XcpcVdc6845* self, uint8_t data_bus)
{
    const uint8_t address_register = self->state.regs.named.address_register;

    if(address_register < VDC_REGISTER_COUNT) {
        uint8_t const is_writable   = (self->setup.caps_of.data[address_register] & REG_WRITABLE);
        uint8_t const register_mask = (self->setup.mask_of.data[address_register]);
        uint8_t*      register_addr = (&self->state.regs.array.data[address_register]);
        if(is_writable != 0) {
            *register_addr = (data_bus &= register_mask);
        }
    }
    return data_bus;
}
