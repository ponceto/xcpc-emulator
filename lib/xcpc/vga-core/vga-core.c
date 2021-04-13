/*
 * vga-core.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "vga-core-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcVgaCore::%s()", function);
}

static void build_mode0_lookup_table(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->setup.mode0);

    do {
        self->setup.mode0[index] = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                                 | ((index & BIT5) >> 3) | ((index & BIT1) << 2)
                                 | ((index & BIT6) >> 2) | ((index & BIT2) << 3)
                                 | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                                 ;
    } while(++index < count);
}

static void build_mode1_lookup_table(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->setup.mode1);

    do {
        self->setup.mode1[index] = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                                 | ((index & BIT6) >> 4) | ((index & BIT2) << 1)
                                 | ((index & BIT5) >> 1) | ((index & BIT1) << 4)
                                 | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                                 ;
    } while(++index < count);
}

static void build_mode2_lookup_table(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->setup.mode2);

    do {
        self->setup.mode2[index] = ((index & BIT7) >> 7) | ((index & BIT6) >> 5)
                                 | ((index & BIT5) >> 3) | ((index & BIT4) >> 1)
                                 | ((index & BIT3) << 1) | ((index & BIT2) << 3)
                                 | ((index & BIT1) << 5) | ((index & BIT0) << 7)
                                 ;
    } while(++index < count);
}

static void build_mode3_lookup_table(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->setup.mode3);

    do {
        self->setup.mode3[index] = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                                 | ((index & BIT6) >> 4) | ((index & BIT2) << 1)
                                 | ((index & BIT5) >> 1) | ((index & BIT1) << 4)
                                 | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                                 ;
    } while(++index < count);
}

static void reset_pen(XcpcVgaCore* self)
{
    self->state.pen = 0x00;
}

static void reset_ink(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->state.ink);

    do {
        self->state.ink[index] = 0x00;
    } while(++index < count);
}

static void reset_rmr(XcpcVgaCore* self)
{
    self->state.rmr = 0x00;
}

static void reset_ctr(XcpcVgaCore* self)
{
    self->state.counter = 0x00;
    self->state.delayed = 0x00;
}

XcpcVgaCore* xcpc_vga_core_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcVgaCore);
}

XcpcVgaCore* xcpc_vga_core_free(XcpcVgaCore* self)
{
    log_trace("free");

    return xcpc_delete(XcpcVgaCore, self);
}

XcpcVgaCore* xcpc_vga_core_construct(XcpcVgaCore* self)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcVgaCoreIface));
        (void) memset(&self->setup, 0, sizeof(XcpcVgaCoreSetup));
        (void) memset(&self->state, 0, sizeof(XcpcVgaCoreState));
    }
    /* initialize iface */ {
        (void) xcpc_vga_core_set_iface(self, NULL);
    }
    /* initialize setup */ {
        build_mode0_lookup_table(self);
        build_mode1_lookup_table(self);
        build_mode2_lookup_table(self);
        build_mode3_lookup_table(self);
    }
    /* reset */ {
        (void) xcpc_vga_core_reset(self);
    }
    return self;
}

XcpcVgaCore* xcpc_vga_core_destruct(XcpcVgaCore* self)
{
    log_trace("destruct");

    return self;
}

XcpcVgaCore* xcpc_vga_core_new(void)
{
    log_trace("new");

    return xcpc_vga_core_construct(xcpc_vga_core_alloc());
}

XcpcVgaCore* xcpc_vga_core_delete(XcpcVgaCore* self)
{
    log_trace("delete");

    return xcpc_vga_core_free(xcpc_vga_core_destruct(self));
}

XcpcVgaCore* xcpc_vga_core_set_iface(XcpcVgaCore* self, const XcpcVgaCoreIface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
    }
    return self;
}

XcpcVgaCore* xcpc_vga_core_debug(XcpcVgaCore* self)
{
    const char* format = "  - %-24s : 0x%02x";

    /* debug state */ {
        xcpc_log_debug("vga_core:");
        xcpc_log_debug(format, "pen"   , self->state.pen      );
        xcpc_log_debug(format, "ink 00", self->state.ink[0x00]);
        xcpc_log_debug(format, "ink 01", self->state.ink[0x01]);
        xcpc_log_debug(format, "ink 02", self->state.ink[0x02]);
        xcpc_log_debug(format, "ink 03", self->state.ink[0x03]);
        xcpc_log_debug(format, "ink 04", self->state.ink[0x04]);
        xcpc_log_debug(format, "ink 05", self->state.ink[0x05]);
        xcpc_log_debug(format, "ink 06", self->state.ink[0x06]);
        xcpc_log_debug(format, "ink 07", self->state.ink[0x07]);
        xcpc_log_debug(format, "ink 08", self->state.ink[0x08]);
        xcpc_log_debug(format, "ink 09", self->state.ink[0x09]);
        xcpc_log_debug(format, "ink 10", self->state.ink[0x0a]);
        xcpc_log_debug(format, "ink 11", self->state.ink[0x0b]);
        xcpc_log_debug(format, "ink 12", self->state.ink[0x0c]);
        xcpc_log_debug(format, "ink 13", self->state.ink[0x0d]);
        xcpc_log_debug(format, "ink 14", self->state.ink[0x0e]);
        xcpc_log_debug(format, "ink 15", self->state.ink[0x0f]);
        xcpc_log_debug(format, "ink 16", self->state.ink[0x10]);
        xcpc_log_debug(format, "rmr"   , self->state.rmr      );
    }
    return self;
}

XcpcVgaCore* xcpc_vga_core_reset(XcpcVgaCore* self)
{
    log_trace("reset");

    /* reset state */ {
        reset_pen(self);
        reset_ink(self);
        reset_rmr(self);
        reset_ctr(self);
    }
    return self;
}

XcpcVgaCore* xcpc_vga_core_clock(XcpcVgaCore* self)
{
    return self;
}

uint8_t xcpc_vga_core_illegal(XcpcVgaCore* self, uint8_t data_bus)
{
    return data_bus;
}
