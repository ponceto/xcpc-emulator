/*
 * ppi-8255.c - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "ppi-8255-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcPpi8255::%s()", function);
}

static uint8_t default_rd_handler(XcpcPpi8255* self, uint8_t data, void* user_data)
{
    log_trace("default_rd_handler");

    return data;
}

static uint8_t default_wr_handler(XcpcPpi8255* self, uint8_t data, void* user_data)
{
    log_trace("default_wr_handler");

    return data;
}

XcpcPpi8255* xcpc_ppi_8255_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcPpi8255);
}

XcpcPpi8255* xcpc_ppi_8255_free(XcpcPpi8255* self)
{
    log_trace("free");

    return xcpc_delete(XcpcPpi8255, self);
}

XcpcPpi8255* xcpc_ppi_8255_construct(XcpcPpi8255* self, const XcpcPpi8255Iface* iface)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcPpi8255Iface));
        (void) memset(&self->setup, 0, sizeof(XcpcPpi8255Setup));
        (void) memset(&self->state, 0, sizeof(XcpcPpi8255State));
        (void) memset(&self->ports, 0, sizeof(XcpcPpi8255Ports));
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
            self->iface.rd_port_c = NULL;
            self->iface.wr_port_c = NULL;
        }
    }
    /* adjust iface */ {
        if(self->iface.rd_port_a == NULL) { self->iface.rd_port_a = &default_rd_handler; }
        if(self->iface.wr_port_a == NULL) { self->iface.wr_port_a = &default_wr_handler; }
        if(self->iface.rd_port_b == NULL) { self->iface.rd_port_b = &default_rd_handler; }
        if(self->iface.wr_port_b == NULL) { self->iface.wr_port_b = &default_wr_handler; }
        if(self->iface.rd_port_c == NULL) { self->iface.rd_port_c = &default_rd_handler; }
        if(self->iface.wr_port_c == NULL) { self->iface.wr_port_c = &default_wr_handler; }
    }
    /* reset */ {
        (void) xcpc_ppi_8255_reset(self);
    }
    return self;
}

XcpcPpi8255* xcpc_ppi_8255_destruct(XcpcPpi8255* self)
{
    log_trace("destruct");

    return self;
}

XcpcPpi8255* xcpc_ppi_8255_new(const XcpcPpi8255Iface* iface)
{
    log_trace("new");

    return xcpc_ppi_8255_construct(xcpc_ppi_8255_alloc(), iface);
}

XcpcPpi8255* xcpc_ppi_8255_delete(XcpcPpi8255* self)
{
    log_trace("delete");

    return xcpc_ppi_8255_free(xcpc_ppi_8255_destruct(self));
}

XcpcPpi8255* xcpc_ppi_8255_reset(XcpcPpi8255* self)
{
    log_trace("reset");

    /* reset state */ {
        self->state.port_a = DEFAULT_VALUE_OF_PORT_A;
        self->state.port_b = DEFAULT_VALUE_OF_PORT_B;
        self->state.port_c = DEFAULT_VALUE_OF_PORT_C;
        self->state.ctrl_p = DEFAULT_VALUE_OF_CTRL_P;
    }
    /* reset ports */ {
        self->ports.ga = 0x00;
        self->ports.gb = 0x00;
        self->ports.pa = 0x00;
        self->ports.pb = 0x00;
        self->ports.pc = 0x00;
    }
    return self;
}

XcpcPpi8255* xcpc_ppi_8255_clock(XcpcPpi8255* self)
{
    return self;
}

uint8_t xcpc_ppi_8255_illegal(XcpcPpi8255* self, uint8_t data_bus)
{
    return data_bus;
}

uint8_t xcpc_ppi_8255_rd_port_a(XcpcPpi8255* self, uint8_t data_bus)
{
    if(self->ports.pa != 0) {
        self->state.port_a = (*self->iface.rd_port_a)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_a;
}

uint8_t xcpc_ppi_8255_wr_port_a(XcpcPpi8255* self, uint8_t data_bus)
{
    self->state.port_a = data_bus;

    if(self->ports.pa == 0) {
        data_bus = (*self->iface.wr_port_a)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_a;
}

uint8_t xcpc_ppi_8255_rd_port_b(XcpcPpi8255* self, uint8_t data_bus)
{
    if(self->ports.pb != 0) {
        self->state.port_b = (*self->iface.rd_port_b)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_b;
}

uint8_t xcpc_ppi_8255_wr_port_b(XcpcPpi8255* self, uint8_t data_bus)
{
    self->state.port_b = data_bus;

    if(self->ports.pb == 0) {
        data_bus = (*self->iface.wr_port_b)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_b;
}

uint8_t xcpc_ppi_8255_rd_port_c(XcpcPpi8255* self, uint8_t data_bus)
{
    if(self->ports.pc != 0) {
        self->state.port_c = (*self->iface.rd_port_c)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_c;
}

uint8_t xcpc_ppi_8255_wr_port_c(XcpcPpi8255* self, uint8_t data_bus)
{
    self->state.port_c = data_bus;

    if(self->ports.pc == 0) {
        data_bus = (*self->iface.wr_port_c)(self, data_bus, self->iface.user_data);
    }
    return self->state.port_c;
}

uint8_t xcpc_ppi_8255_rd_ctrl_p(XcpcPpi8255* self, uint8_t data_bus)
{
    data_bus = self->state.ctrl_p;

    return data_bus;
}

uint8_t xcpc_ppi_8255_wr_ctrl_p(XcpcPpi8255* self, uint8_t data_bus)
{
    if(((self->state.ctrl_p = data_bus) & 0x80) != 0) {
        /* I/O mode */ {
            const uint8_t ga = ((self->state.ctrl_p >> 5) & 0x03);
            const uint8_t gb = ((self->state.ctrl_p >> 2) & 0x01);
            const uint8_t pa = ((self->state.ctrl_p >> 4) & 0x01);
            const uint8_t pb = ((self->state.ctrl_p >> 1) & 0x01);
            const uint8_t pc = ((self->state.ctrl_p >> 2) & 0x02)
                             | ((self->state.ctrl_p >> 0) & 0x01);
            /* process group a */ {
                if(ga != self->ports.ga) {
                    if((self->ports.ga = ga) != 0) {
                        xcpc_log_debug("ppi-8255: mode %d is not supported for group a", ga);
                    }
                }
            }
            /* process group b */ {
                if(gb != self->ports.gb) {
                    if((self->ports.gb = gb) != 0) {
                        xcpc_log_debug("ppi-8255: mode %d is not supported for group b", gb);
                    }
                }
            }
            /* process port a */ {
                if(pa != self->ports.pa) {
                    if((self->ports.pa = pa) != 0) {
                        data_bus = (*self->iface.rd_port_a)(self, self->state.port_a, self->iface.user_data);
                        self->state.port_a = data_bus;
                    }
                    else {
                        data_bus = (*self->iface.wr_port_a)(self, self->state.port_a, self->iface.user_data);
                    }
                }
            }
            /* process port b */ {
                if(pb != self->ports.pb) {
                    if((self->ports.pb = pb) != 0) {
                        data_bus = (*self->iface.rd_port_b)(self, self->state.port_b, self->iface.user_data);
                        self->state.port_b = data_bus;
                    }
                    else {
                        data_bus = (*self->iface.wr_port_b)(self, self->state.port_b, self->iface.user_data);
                    }
                }
            }
            /* process port c */ {
                if(pc != self->ports.pc) {
                    if((self->ports.pc = pc) != 0) {
                        data_bus = (*self->iface.rd_port_c)(self, self->state.port_c, self->iface.user_data);
                        self->state.port_c = data_bus;
                    }
                    else {
                        data_bus = (*self->iface.wr_port_c)(self, self->state.port_c, self->iface.user_data);
                    }
                }
            }
        }
    }
    else {
        /* BSR mode */ {
            const uint8_t bit = ((self->state.ctrl_p >> 1) & 0x07);
            const uint8_t val = ((self->state.ctrl_p >> 0) & 0x01);
            /* process port c */ {
                if(self->ports.pc == 0) {
                    self->state.port_c = ((self->state.port_c & ~(0x1 << bit)) | (val << bit));
                    data_bus = (*self->iface.wr_port_c)(self, self->state.port_c, self->iface.user_data);
                }
            }
        }
    }
    return data_bus;
}
