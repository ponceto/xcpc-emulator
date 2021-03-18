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

static void xcpc_vga_core_init_mode0_lut(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->state.mode0);

    for(index = 0; index < count; ++index) {
        self->state.mode0[index] = ((index & 0x80) >> 7) | ((index & 0x08) >> 2)
                                 | ((index & 0x20) >> 3) | ((index & 0x02) << 2)
                                 | ((index & 0x40) >> 2) | ((index & 0x04) << 3)
                                 | ((index & 0x10) << 2) | ((index & 0x01) << 7)
                                 ;
    }
}

static void xcpc_vga_core_init_mode1_lut(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->state.mode1);

    for(index = 0; index < count; ++index) {
        self->state.mode1[index] = ((index & 0x80) >> 7) | ((index & 0x08) >> 2)
                                 | ((index & 0x40) >> 4) | ((index & 0x04) << 1)
                                 | ((index & 0x20) >> 1) | ((index & 0x02) << 4)
                                 | ((index & 0x10) << 2) | ((index & 0x01) << 7)
                                 ;
    }
}

static void xcpc_vga_core_init_mode2_lut(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->state.mode2);

    for(index = 0; index < count; ++index) {
        self->state.mode2[index] = ((index & 0x80) >> 7) | ((index & 0x40) >> 5)
                                 | ((index & 0x20) >> 3) | ((index & 0x10) >> 1)
                                 | ((index & 0x08) << 1) | ((index & 0x04) << 3)
                                 | ((index & 0x02) << 5) | ((index & 0x01) << 7)
                                 ;
    }
}

static void xcpc_vga_core_init_pen(XcpcVgaCore* self)
{
    self->state.pen = 0x00;
}

static void xcpc_vga_core_init_ink(XcpcVgaCore* self)
{
    unsigned int index = 0;
    unsigned int count = countof(self->state.ink);

    for(index = 0; index < count; index++) {
        self->state.ink[index] = 0x00;
    }
}

static void xcpc_vga_core_init_rmr(XcpcVgaCore* self)
{
    self->state.rmr = 0x00;
}

static void xcpc_vga_core_init_ctr(XcpcVgaCore* self)
{
    self->state.counter = 0x00;
    self->state.delayed = 0x00;
}

void xcpc_vga_core_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcVgaCore::%s()"
          , function );
}

XcpcVgaCore* xcpc_vga_core_alloc(void)
{
    xcpc_vga_core_trace("alloc");

    return xcpc_new(XcpcVgaCore);
}

XcpcVgaCore* xcpc_vga_core_free(XcpcVgaCore* self)
{
    xcpc_vga_core_trace("free");

    return xcpc_delete(XcpcVgaCore, self);
}

XcpcVgaCore* xcpc_vga_core_construct(XcpcVgaCore* self)
{
    xcpc_vga_core_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcVgaCoreIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcVgaCoreState));
    }
    return xcpc_vga_core_reset(self);
}

XcpcVgaCore* xcpc_vga_core_destruct(XcpcVgaCore* self)
{
    xcpc_vga_core_trace("destruct");

    return self;
}

XcpcVgaCore* xcpc_vga_core_new(void)
{
    xcpc_vga_core_trace("new");

    return xcpc_vga_core_construct(xcpc_vga_core_alloc());
}

XcpcVgaCore* xcpc_vga_core_delete(XcpcVgaCore* self)
{
    xcpc_vga_core_trace("delete");

    return xcpc_vga_core_free(xcpc_vga_core_destruct(self));
}

XcpcVgaCore* xcpc_vga_core_reset(XcpcVgaCore* self)
{
    xcpc_vga_core_trace("reset");

    /* reset state */ {
        xcpc_vga_core_init_mode0_lut(self);
        xcpc_vga_core_init_mode1_lut(self);
        xcpc_vga_core_init_mode2_lut(self);
        xcpc_vga_core_init_pen(self);
        xcpc_vga_core_init_ink(self);
        xcpc_vga_core_init_rmr(self);
        xcpc_vga_core_init_ctr(self);
    }
    return self;
}

XcpcVgaCore* xcpc_vga_core_clock(XcpcVgaCore* self)
{
    return self;
}
