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

static const uint16_t volume_table[16] = {
    /*  0 */ 0,
    /*  1 */ 836,
    /*  2 */ 1212,
    /*  3 */ 1773,
    /*  4 */ 2619,
    /*  5 */ 3875,
    /*  6 */ 5397,
    /*  7 */ 8823,
    /*  8 */ 10392,
    /*  9 */ 16706,
    /* 10 */ 23339,
    /* 11 */ 29292,
    /* 12 */ 36969,
    /* 13 */ 46421,
    /* 14 */ 55195,
    /* 15 */ 65535,
};

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcPsg8910::%s()", function);
}

static uint8_t port_a_rd_handler(XcpcPsg8910* self, uint8_t data, void* user_data)
{
    log_trace("port_a_rd_handler");

    return data;
}

static uint8_t port_a_wr_handler(XcpcPsg8910* self, uint8_t data, void* user_data)
{
    log_trace("port_a_wr_handler");

    return data;
}

static uint8_t port_b_rd_handler(XcpcPsg8910* self, uint8_t data, void* user_data)
{
    log_trace("port_b_rd_handler");

    return data;
}

static uint8_t port_b_wr_handler(XcpcPsg8910* self, uint8_t data, void* user_data)
{
    log_trace("port_b_wr_handler");

    return data;
}

static const char* get_register_name(unsigned int address_register)
{
    static const char* const register_name[] = {
        "address_register",
        "channel_a_fine_tune",
        "channel_a_coarse_tune",
        "channel_b_fine_tune",
        "channel_b_coarse_tune",
        "channel_c_fine_tune",
        "channel_c_coarse_tune",
        "noise_period",
        "mixer_and_io_control",
        "channel_a_volume",
        "channel_b_volume",
        "channel_c_volume",
        "envelope_fine_tune",
        "envelope_coarse_tune",
        "envelope_shape",
        "io_port_a",
        "io_port_b",
    };

    if(++address_register < countof(register_name)) {
        return register_name[address_register];
    }
    return "invalid-register";
}

static void reset_registers(XcpcPsg8910* self, XcpcPsg8910Registers* registers)
{
    registers->named.address_register      = 0x00;
    registers->named.channel_a_fine_tune   = 0x00;
    registers->named.channel_a_coarse_tune = 0x00;
    registers->named.channel_b_fine_tune   = 0x00;
    registers->named.channel_b_coarse_tune = 0x00;
    registers->named.channel_c_fine_tune   = 0x00;
    registers->named.channel_c_coarse_tune = 0x00;
    registers->named.noise_period          = 0x00;
    registers->named.mixer_and_io_control  = 0x00;
    registers->named.channel_a_volume      = 0x00;
    registers->named.channel_b_volume      = 0x00;
    registers->named.channel_c_volume      = 0x00;
    registers->named.envelope_fine_tune    = 0x00;
    registers->named.envelope_coarse_tune  = 0x00;
    registers->named.envelope_shape        = 0x00;
    registers->named.io_port_a             = 0xff;
    registers->named.io_port_b             = 0xff;
}

static void reset_channel(XcpcPsg8910* self, XcpcPsg8910Channel* channel)
{
    channel->buffer[0] = 0;
    channel->buf_rd    = 0;
    channel->buf_wr    = 0;
    channel->tone      = 0;
    channel->noise     = 0;
    channel->amplitude = 0;
    channel->envelope  = 0;
    channel->shape     = 0;
    channel->mixer     = 0;
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
    /* volume table */ {
        int level = 0;
        do {
            self->setup.volume.level[level] = volume_table[level];
        } while(++level < 16);
    }
}

static void reset_state(XcpcPsg8910* self)
{
    reset_registers(self, &self->state.regs);
    reset_channel(self, &self->state.channel_a);
    reset_channel(self, &self->state.channel_b);
    reset_channel(self, &self->state.channel_c);
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

XcpcPsg8910* xcpc_psg_8910_construct(XcpcPsg8910* self, const XcpcPsg8910Iface* iface)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcPsg8910Iface));
        (void) memset(&self->setup, 0, sizeof(XcpcPsg8910Setup));
        (void) memset(&self->state, 0, sizeof(XcpcPsg8910State));
    }
    /* initialize iface */ {
        if(iface != NULL) {
            *(&self->iface) = *(iface);
        }
        else {
            self->iface.user_data = NULL;
            self->iface.rd_port_a = NULL;
            self->iface.wr_port_a = NULL;
            self->iface.rd_port_b = NULL;
            self->iface.wr_port_b = NULL;
        }
    }
    /* adjust iface */ {
        if(self->iface.rd_port_a == NULL) { self->iface.rd_port_a = &port_a_rd_handler; }
        if(self->iface.wr_port_a == NULL) { self->iface.wr_port_a = &port_a_wr_handler; }
        if(self->iface.rd_port_b == NULL) { self->iface.rd_port_b = &port_b_rd_handler; }
        if(self->iface.wr_port_b == NULL) { self->iface.wr_port_b = &port_b_wr_handler; }
    }
    return xcpc_psg_8910_reset(self);
}

XcpcPsg8910* xcpc_psg_8910_destruct(XcpcPsg8910* self)
{
    log_trace("destruct");

    return self;
}

XcpcPsg8910* xcpc_psg_8910_new(const XcpcPsg8910Iface* iface)
{
    log_trace("new");

    return xcpc_psg_8910_construct(xcpc_psg_8910_alloc(), iface);
}

XcpcPsg8910* xcpc_psg_8910_delete(XcpcPsg8910* self)
{
    log_trace("delete");

    return xcpc_psg_8910_free(xcpc_psg_8910_destruct(self));
}

XcpcPsg8910* xcpc_psg_8910_debug(XcpcPsg8910* self)
{
    const char* format1 = "psg-8910:";
    const char* format2 = "  - %-24s : 0x%02x";

    /* debug state */ {
        xcpc_log_debug(format1);
        xcpc_log_debug(format2, get_register_name(PSG_ADDRESS_REGISTER     ), self->state.regs.named.address_register     );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_A_FINE_TUNE  ), self->state.regs.named.channel_a_fine_tune  );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_A_COARSE_TUNE), self->state.regs.named.channel_a_coarse_tune);
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_B_FINE_TUNE  ), self->state.regs.named.channel_b_fine_tune  );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_B_COARSE_TUNE), self->state.regs.named.channel_b_coarse_tune);
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_C_FINE_TUNE  ), self->state.regs.named.channel_c_fine_tune  );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_C_COARSE_TUNE), self->state.regs.named.channel_c_coarse_tune);
        xcpc_log_debug(format2, get_register_name(PSG_NOISE_PERIOD         ), self->state.regs.named.noise_period         );
        xcpc_log_debug(format2, get_register_name(PSG_MIXER_AND_IO_CONTROL ), self->state.regs.named.mixer_and_io_control );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_A_VOLUME     ), self->state.regs.named.channel_a_volume     );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_B_VOLUME     ), self->state.regs.named.channel_b_volume     );
        xcpc_log_debug(format2, get_register_name(PSG_CHANNEL_C_VOLUME     ), self->state.regs.named.channel_c_volume     );
        xcpc_log_debug(format2, get_register_name(PSG_ENVELOPE_FINE_TUNE   ), self->state.regs.named.envelope_fine_tune   );
        xcpc_log_debug(format2, get_register_name(PSG_ENVELOPE_COARSE_TUNE ), self->state.regs.named.envelope_coarse_tune );
        xcpc_log_debug(format2, get_register_name(PSG_ENVELOPE_SHAPE       ), self->state.regs.named.envelope_shape       );
        xcpc_log_debug(format2, get_register_name(PSG_IO_PORT_A            ), self->state.regs.named.io_port_a            );
        xcpc_log_debug(format2, get_register_name(PSG_IO_PORT_B            ), self->state.regs.named.io_port_b            );
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
#if 0
    /* channel a */ {
        self->state.channel_a.tone = (((uint16_t)(self->state.regs.named.channel_a_coarse_tune)) << 8)
                                   | (((uint16_t)(self->state.regs.named.channel_a_fine_tune  )) << 0)
                                   ;
    }
    /* channel b */ {
        self->state.channel_b.tone = (((uint16_t)(self->state.regs.named.channel_b_coarse_tune)) << 8)
                                   | (((uint16_t)(self->state.regs.named.channel_b_fine_tune  )) << 0)
                                   ;
    }
    /* channel c */ {
        self->state.channel_c.tone = (((uint16_t)(self->state.regs.named.channel_c_coarse_tune)) << 8)
                                   | (((uint16_t)(self->state.regs.named.channel_c_fine_tune  )) << 0)
                                   ;
    }
#endif
    return self;
}

uint8_t xcpc_psg_8910_illegal(XcpcPsg8910* self, uint8_t data_bus)
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
            if((address_register == PSG_IO_PORT_A) && ((self->state.regs.named.mixer_and_io_control & BIT6) == 0)) {
                *register_addr = (*self->iface.rd_port_a)(self, data_bus, self->iface.user_data);
            }
            if((address_register == PSG_IO_PORT_B) && ((self->state.regs.named.mixer_and_io_control & BIT7) == 0)) {
                *register_addr = (*self->iface.rd_port_b)(self, data_bus, self->iface.user_data);
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
            if((address_register == PSG_IO_PORT_A) && ((self->state.regs.named.mixer_and_io_control & BIT6) != 0)) {
                *register_addr = (*self->iface.wr_port_a)(self, data_bus, self->iface.user_data);
            }
            if((address_register == PSG_IO_PORT_B) && ((self->state.regs.named.mixer_and_io_control & BIT7) != 0)) {
                *register_addr = (*self->iface.wr_port_b)(self, data_bus, self->iface.user_data);
            }
            *register_addr = (data_bus &= register_mask);
        }
    }
    return data_bus;
}
