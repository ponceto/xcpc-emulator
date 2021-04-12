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

static const char* get_register_name(unsigned int address_register)
{
    static const char* const register_name[] = {
        "address_register",
        "horizontal_total",
        "horizontal_displayed",
        "horizontal_sync_position",
        "sync_width",
        "vertical_total",
        "vertical_total_adjust",
        "vertical_displayed",
        "vertical_sync_position",
        "interlace_mode_and_skew",
        "maximum_scanline_address",
        "cursor_start",
        "cursor_end",
        "start_address_high",
        "start_address_low",
        "cursor_high",
        "cursor_low",
        "light_pen_high",
        "light_pen_low",
    };

    if(++address_register < countof(register_name)) {
        return register_name[address_register];
    }
    return "invalid-register";
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
        self->count.hcc          = 0;
        self->count.vcc          = 0;
        self->count.slc          = 0;
        self->count.vac          = 0;
        self->count.hsc          = 0;
        self->count.vsc          = 0;
        self->count.hsync_signal = 0;
        self->count.vsync_signal = 0;
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
        xcpc_log_debug(format, get_register_name(VDC_ADDRESS_REGISTER        ), self->state.regs.named.address_register        );
        xcpc_log_debug(format, get_register_name(VDC_HORIZONTAL_TOTAL        ), self->state.regs.named.horizontal_total        );
        xcpc_log_debug(format, get_register_name(VDC_HORIZONTAL_DISPLAYED    ), self->state.regs.named.horizontal_displayed    );
        xcpc_log_debug(format, get_register_name(VDC_HORIZONTAL_SYNC_POSITION), self->state.regs.named.horizontal_sync_position);
        xcpc_log_debug(format, get_register_name(VDC_SYNC_WIDTH              ), self->state.regs.named.sync_width              );
        xcpc_log_debug(format, get_register_name(VDC_VERTICAL_TOTAL          ), self->state.regs.named.vertical_total          );
        xcpc_log_debug(format, get_register_name(VDC_VERTICAL_TOTAL_ADJUST   ), self->state.regs.named.vertical_total_adjust   );
        xcpc_log_debug(format, get_register_name(VDC_VERTICAL_DISPLAYED      ), self->state.regs.named.vertical_displayed      );
        xcpc_log_debug(format, get_register_name(VDC_VERTICAL_SYNC_POSITION  ), self->state.regs.named.vertical_sync_position  );
        xcpc_log_debug(format, get_register_name(VDC_INTERLACE_MODE_AND_SKEW ), self->state.regs.named.interlace_mode_and_skew );
        xcpc_log_debug(format, get_register_name(VDC_MAXIMUM_SCANLINE_ADDRESS), self->state.regs.named.maximum_scanline_address);
        xcpc_log_debug(format, get_register_name(VDC_CURSOR_START            ), self->state.regs.named.cursor_start            );
        xcpc_log_debug(format, get_register_name(VDC_CURSOR_END              ), self->state.regs.named.cursor_end              );
        xcpc_log_debug(format, get_register_name(VDC_START_ADDRESS_HIGH      ), self->state.regs.named.start_address_high      );
        xcpc_log_debug(format, get_register_name(VDC_START_ADDRESS_LOW       ), self->state.regs.named.start_address_low       );
        xcpc_log_debug(format, get_register_name(VDC_CURSOR_HIGH             ), self->state.regs.named.cursor_high             );
        xcpc_log_debug(format, get_register_name(VDC_CURSOR_LOW              ), self->state.regs.named.cursor_low              );
        xcpc_log_debug(format, get_register_name(VDC_LIGHT_PEN_HIGH          ), self->state.regs.named.light_pen_high          );
        xcpc_log_debug(format, get_register_name(VDC_LIGHT_PEN_LOW           ), self->state.regs.named.light_pen_low           );
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
    uint8_t const horizontal_sync_signal   = (self->count.hsync_signal != 0                      );
    uint8_t const vertical_total           = (self->state.regs.named.vertical_total           + 1);
    uint8_t const vertical_displayed       = (self->state.regs.named.vertical_displayed       + 0);
    uint8_t const vertical_sync_position   = (self->state.regs.named.vertical_sync_position   + 0);
    uint8_t const vertical_sync_width      = (((self->state.regs.named.sync_width >> 4) & 0x0f)  );
    uint8_t const vertical_sync_signal     = (self->count.vsync_signal != 0                      );
    uint8_t const scanline_total           = (self->state.regs.named.maximum_scanline_address + 1);
    uint8_t       process_hcc              = 1;
    uint8_t       process_vcc              = 0;
    uint8_t       process_slc              = 0;
    uint8_t       process_frame            = 0;

    if(process_hcc != 0) {
        if(++self->count.hcc == horizontal_total) {
            self->count.hcc = 0;
            process_slc = 1;
        }
        if(horizontal_sync_signal != 0) {
            self->count.hsc = ((self->count.hsc + 1) & 0x0f);
            if(self->count.hsc == horizontal_sync_width) {
                self->count.hsc = 0;
                self->count.hsync_signal  = 0;
            }
        }
        else {
            if(self->count.hcc == horizontal_sync_position) {
                self->count.hsc = 0;
                self->count.hsync_signal  = 1;
            }
        }
    }
    if(process_slc != 0) {
        if(++self->count.slc == scanline_total) {
            self->count.slc = 0;
            process_vcc = 1;
        }
        if(vertical_sync_signal != 0) {
            self->count.vsc = ((self->count.vsc + 1) & 0x0f);
            if(self->count.vsc == vertical_sync_width) {
                self->count.vsc = 0;
                self->count.vsync_signal  = 0;
            }
        }
        else {
            if(self->count.vcc == vertical_sync_position) {
                self->count.vsc = 0;
                self->count.vsync_signal  = 1;
            }
        }
    }
    if(process_vcc != 0) {
        if(++self->count.vcc == vertical_total) {
            self->count.vcc = 0;
            process_frame = 1;
        }
    }
    /* hsync handler */ {
        if(self->count.hsync_signal != horizontal_sync_signal) {
            (void) (*self->iface.hsync)(self, self->count.hsync_signal);
        }
    }
    /* vsync handler */ {
        if(self->count.vsync_signal != vertical_sync_signal) {
            (void) (*self->iface.vsync)(self, self->count.vsync_signal);
        }
    }
    /* frame handler */ {
        if(process_frame != 0) {
            (void) (*self->iface.frame)(self);
        }
    }
    return self;
}

uint8_t xcpc_vdc_6845_illegal(XcpcVdc6845* self, uint8_t data_bus)
{
    return data_bus;
}

uint8_t xcpc_vdc_6845_rd_addr(XcpcVdc6845* self, uint8_t data_bus)
{
    uint8_t const is_readable   = (self->setup.caps_of.addr & REG_READABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_readable != 0) {
        data_bus = (*register_addr &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_wr_addr(XcpcVdc6845* self, uint8_t data_bus)
{
    uint8_t const is_writable   = (self->setup.caps_of.addr & REG_WRITABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_writable != 0) {
        *register_addr = (data_bus &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_vdc_6845_rd_data(XcpcVdc6845* self, uint8_t data_bus)
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

uint8_t xcpc_vdc_6845_wr_data(XcpcVdc6845* self, uint8_t data_bus)
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
