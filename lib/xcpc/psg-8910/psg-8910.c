/*
 * psg-8910.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "psg-8910-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcPsg8910::%s()", function);
}

static uint8_t default_rd_handler(XcpcPsg8910* self, uint8_t data)
{
    log_trace("default_rd_handler");

    return data;
}

static uint8_t default_wr_handler(XcpcPsg8910* self, uint8_t data)
{
    log_trace("default_wr_handler");

    return data;
}

static void reset_setup(XcpcPsg8910* self)
{
    /* reset caps_of */ {
        self->setup.caps_of.addr       = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x00] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x01] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x02] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x03] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x04] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x05] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x06] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x07] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x08] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x09] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0a] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0b] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0c] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0d] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0e] = (REG_READABLE | REG_WRITABLE);
        self->setup.caps_of.data[0x0f] = (REG_READABLE | REG_WRITABLE);
    }
    /* reset mask_of */ {
        self->setup.mask_of.addr       = 0xff;
        self->setup.mask_of.data[0x00] = 0xff;
        self->setup.mask_of.data[0x01] = 0x0f;
        self->setup.mask_of.data[0x02] = 0xff;
        self->setup.mask_of.data[0x03] = 0x0f;
        self->setup.mask_of.data[0x04] = 0xff;
        self->setup.mask_of.data[0x05] = 0x0f;
        self->setup.mask_of.data[0x06] = 0x1f;
        self->setup.mask_of.data[0x07] = 0xff;
        self->setup.mask_of.data[0x08] = 0x1f;
        self->setup.mask_of.data[0x09] = 0x1f;
        self->setup.mask_of.data[0x0a] = 0x1f;
        self->setup.mask_of.data[0x0b] = 0xff;
        self->setup.mask_of.data[0x0c] = 0xff;
        self->setup.mask_of.data[0x0d] = 0x0f;
        self->setup.mask_of.data[0x0e] = 0xff;
        self->setup.mask_of.data[0x0f] = 0xff;
    }
}

static void reset_state(XcpcPsg8910* self)
{
    /* reset internal registers */ {
        self->state.regs.named.address_register      = 0x00;
        self->state.regs.named.channel_a_fine_tune   = 0x00;
        self->state.regs.named.channel_a_coarse_tune = 0x00;
        self->state.regs.named.channel_b_fine_tune   = 0x00;
        self->state.regs.named.channel_b_coarse_tune = 0x00;
        self->state.regs.named.channel_c_fine_tune   = 0x00;
        self->state.regs.named.channel_c_coarse_tune = 0x00;
        self->state.regs.named.noise_period          = 0x00;
        self->state.regs.named.mixer_and_io_control  = 0x00;
        self->state.regs.named.channel_a_volume      = 0x00;
        self->state.regs.named.channel_b_volume      = 0x00;
        self->state.regs.named.channel_c_volume      = 0x00;
        self->state.regs.named.envelope_fine_tune    = 0x00;
        self->state.regs.named.envelope_coarse_tune  = 0x00;
        self->state.regs.named.envelope_shape        = 0x00;
        self->state.regs.named.io_port_a             = 0xff;
        self->state.regs.named.io_port_b             = 0xff;
    }
}

XcpcPsg8910* xcpc_psg_8910_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcPsg8910);
}

XcpcPsg8910* xcpc_psg_8910_free(XcpcPsg8910* self)
{
    log_trace("free");

    return xcpc_delete(XcpcPsg8910, self);
}

XcpcPsg8910* xcpc_psg_8910_construct(XcpcPsg8910* self)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcPsg8910Iface));
        (void) memset(&self->setup, 0, sizeof(XcpcPsg8910Setup));
        (void) memset(&self->state, 0, sizeof(XcpcPsg8910State));
    }
    /* initialize iface */ {
        (void) xcpc_psg_8910_set_iface(self, NULL);
    }
    /* reset */ {
        (void) xcpc_psg_8910_reset(self);
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_destruct(XcpcPsg8910* self)
{
    log_trace("destruct");

    return self;
}

XcpcPsg8910* xcpc_psg_8910_new(void)
{
    log_trace("new");

    return xcpc_psg_8910_construct(xcpc_psg_8910_alloc());
}

XcpcPsg8910* xcpc_psg_8910_delete(XcpcPsg8910* self)
{
    log_trace("delete");

    return xcpc_psg_8910_free(xcpc_psg_8910_destruct(self));
}

XcpcPsg8910* xcpc_psg_8910_set_iface(XcpcPsg8910* self, const XcpcPsg8910Iface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
        self->iface.rd_port_a = &default_rd_handler;
        self->iface.wr_port_a = &default_wr_handler;
        self->iface.rd_port_b = &default_rd_handler;
        self->iface.wr_port_b = &default_wr_handler;
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_debug(XcpcPsg8910* self)
{
    const char* format = "  - %-24s : 0x%02x";

    /* debug state */ {
        xcpc_log_debug("psg_8910:");
        xcpc_log_debug(format, "address_register"     , self->state.regs.named.address_register     );
        xcpc_log_debug(format, "channel_a_fine_tune"  , self->state.regs.named.channel_a_fine_tune  );
        xcpc_log_debug(format, "channel_a_coarse_tune", self->state.regs.named.channel_a_coarse_tune);
        xcpc_log_debug(format, "channel_b_fine_tune"  , self->state.regs.named.channel_b_fine_tune  );
        xcpc_log_debug(format, "channel_b_coarse_tune", self->state.regs.named.channel_b_coarse_tune);
        xcpc_log_debug(format, "channel_c_fine_tune"  , self->state.regs.named.channel_c_fine_tune  );
        xcpc_log_debug(format, "channel_c_coarse_tune", self->state.regs.named.channel_c_coarse_tune);
        xcpc_log_debug(format, "noise_period"         , self->state.regs.named.noise_period         );
        xcpc_log_debug(format, "mixer_and_io_control" , self->state.regs.named.mixer_and_io_control );
        xcpc_log_debug(format, "channel_a_volume"     , self->state.regs.named.channel_a_volume     );
        xcpc_log_debug(format, "channel_b_volume"     , self->state.regs.named.channel_b_volume     );
        xcpc_log_debug(format, "channel_c_volume"     , self->state.regs.named.channel_c_volume     );
        xcpc_log_debug(format, "envelope_fine_tune"   , self->state.regs.named.envelope_fine_tune   );
        xcpc_log_debug(format, "envelope_coarse_tune" , self->state.regs.named.envelope_coarse_tune );
        xcpc_log_debug(format, "envelope_shape"       , self->state.regs.named.envelope_shape       );
        xcpc_log_debug(format, "io_port_a"            , self->state.regs.named.io_port_a            );
        xcpc_log_debug(format, "io_port_b"            , self->state.regs.named.io_port_b            );
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_reset(XcpcPsg8910* self)
{
    log_trace("reset");

    /* reset */ {
        reset_setup(self);
        reset_state(self);
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_clock(XcpcPsg8910* self)
{
    return self;
}

uint8_t xcpc_psg_8910_no_func(XcpcPsg8910* self, uint8_t data_bus)
{
    return data_bus;
}

uint8_t xcpc_psg_8910_rd_addr(XcpcPsg8910* self, uint8_t data_bus)
{
    uint8_t const is_readable   = (self->setup.caps_of.addr & REG_READABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_readable != 0) {
        data_bus = (*register_addr &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_wr_addr(XcpcPsg8910* self, uint8_t data_bus)
{
    uint8_t const is_writable   = (self->setup.caps_of.addr & REG_WRITABLE);
    uint8_t const register_mask = (self->setup.mask_of.addr);
    uint8_t*      register_addr = (&self->state.regs.named.address_register);

    if(is_writable != 0) {
        *register_addr = (data_bus &= register_mask);
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_rd_data(XcpcPsg8910* self, uint8_t data_bus)
{
    const uint8_t address_register = self->state.regs.named.address_register;

    if(address_register < PSG_REGISTER_COUNT) {
        uint8_t const is_readable   = (self->setup.caps_of.data[address_register] & REG_READABLE);
        uint8_t const register_mask = (self->setup.mask_of.data[address_register]);
        uint8_t*      register_addr = (&self->state.regs.array.data[address_register]);
        if(is_readable != 0) {
            if(address_register == PSG_IO_PORT_A) {
                *register_addr = (*self->iface.rd_port_a)(self, data_bus);
            }
            if(address_register == PSG_IO_PORT_B) {
                *register_addr = (*self->iface.rd_port_b)(self, data_bus);
            }
            data_bus = (*register_addr &= register_mask);
        }
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_wr_data(XcpcPsg8910* self, uint8_t data_bus)
{
    const uint8_t address_register = self->state.regs.named.address_register;

    if(address_register < PSG_REGISTER_COUNT) {
        uint8_t const is_writable   = (self->setup.caps_of.data[address_register] & REG_WRITABLE);
        uint8_t const register_mask = (self->setup.mask_of.data[address_register]);
        uint8_t*      register_addr = (&self->state.regs.array.data[address_register]);
        if(is_writable != 0) {
            if(address_register == PSG_IO_PORT_A) {
                *register_addr = (*self->iface.wr_port_a)(self, data_bus);
            }
            if(address_register == PSG_IO_PORT_B) {
                *register_addr = (*self->iface.wr_port_b)(self, data_bus);
            }
            *register_addr = (data_bus &= register_mask);
        }
    }
    return data_bus;
}
