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

XcpcPsg8910* xcpc_psg_8910_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcPsg8910" );
    }
    return xcpc_new(XcpcPsg8910);
}

XcpcPsg8910* xcpc_psg_8910_free(XcpcPsg8910* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcPsg8910" );
    }
    return xcpc_delete(XcpcPsg8910, self);
}

XcpcPsg8910* xcpc_psg_8910_construct(XcpcPsg8910* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcPsg8910" );
    }
    if(self != NULL) {
        (void) memset(self, 0, sizeof(XcpcPsg8910));
    }
    return xcpc_psg_8910_reset(self);
}

XcpcPsg8910* xcpc_psg_8910_destruct(XcpcPsg8910* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcPsg8910" );
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcPsg8910" );
    }
    return xcpc_psg_8910_construct(xcpc_psg_8910_alloc());
}

XcpcPsg8910* xcpc_psg_8910_delete(XcpcPsg8910* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcPsg8910" );
    }
    return xcpc_psg_8910_free(xcpc_psg_8910_destruct(self));
}

XcpcPsg8910* xcpc_psg_8910_reset(XcpcPsg8910* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcPsg8910" );
    }
    if(self != NULL) {
        self->regs.named.address_register      = DEFAULT_VALUE_OF_ADDRESS_REGISTER;
        self->regs.named.channel_a_fine_tune   = DEFAULT_VALUE_OF_CHANNEL_A_FINE_TUNE;
        self->regs.named.channel_a_coarse_tune = DEFAULT_VALUE_OF_CHANNEL_A_COARSE_TUNE;
        self->regs.named.channel_b_fine_tune   = DEFAULT_VALUE_OF_CHANNEL_B_FINE_TUNE;
        self->regs.named.channel_b_coarse_tune = DEFAULT_VALUE_OF_CHANNEL_B_COARSE_TUNE;
        self->regs.named.channel_c_fine_tune   = DEFAULT_VALUE_OF_CHANNEL_C_FINE_TUNE;
        self->regs.named.channel_c_coarse_tune = DEFAULT_VALUE_OF_CHANNEL_C_COARSE_TUNE;
        self->regs.named.noise_period          = DEFAULT_VALUE_OF_NOISE_PERIOD;
        self->regs.named.mixer_and_io_control  = DEFAULT_VALUE_OF_MIXER_AND_IO_CONTROL;
        self->regs.named.channel_a_volume      = DEFAULT_VALUE_OF_CHANNEL_A_VOLUME;
        self->regs.named.channel_b_volume      = DEFAULT_VALUE_OF_CHANNEL_B_VOLUME;
        self->regs.named.channel_c_volume      = DEFAULT_VALUE_OF_CHANNEL_C_VOLUME;
        self->regs.named.envelope_fine_tune    = DEFAULT_VALUE_OF_ENVELOPE_FINE_TUNE;
        self->regs.named.envelope_coarse_tune  = DEFAULT_VALUE_OF_ENVELOPE_COARSE_TUNE;
        self->regs.named.envelope_shape        = DEFAULT_VALUE_OF_ENVELOPE_SHAPE;
        self->regs.named.io_port_a             = DEFAULT_VALUE_OF_IO_PORT_A;
        self->regs.named.io_port_b             = DEFAULT_VALUE_OF_IO_PORT_B;
    }
    return self;
}

XcpcPsg8910* xcpc_psg_8910_clock(XcpcPsg8910* self)
{
    return self;
}

uint8_t xcpc_psg_8910_rg(XcpcPsg8910* self, uint8_t data_bus)
{
    if(IS_READABLE_ADDRESS_REGISTER) {
        data_bus = self->regs.named.address_register;
        data_bus &= MASK_OF_ADDRESS_REGISTER;
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_rs(XcpcPsg8910* self, uint8_t data_bus)
{
    if(IS_WRITABLE_ADDRESS_REGISTER) {
        data_bus &= MASK_OF_ADDRESS_REGISTER;
        self->regs.named.address_register = data_bus;
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_rd(XcpcPsg8910* self, uint8_t data_bus)
{
    switch(self->regs.named.address_register) {
        case INDEX_OF_CHANNEL_A_FINE_TUNE:
            if(IS_READABLE_CHANNEL_A_FINE_TUNE) {
                data_bus = self->regs.named.channel_a_fine_tune;
                data_bus &= MASK_OF_CHANNEL_A_FINE_TUNE;
            }
            break;
        case INDEX_OF_CHANNEL_A_COARSE_TUNE:
            if(IS_READABLE_CHANNEL_A_COARSE_TUNE) {
                data_bus = self->regs.named.channel_a_coarse_tune;
                data_bus &= MASK_OF_CHANNEL_A_COARSE_TUNE;
            }
            break;
        case INDEX_OF_CHANNEL_B_FINE_TUNE:
            if(IS_READABLE_CHANNEL_B_FINE_TUNE) {
                data_bus = self->regs.named.channel_b_fine_tune;
                data_bus &= MASK_OF_CHANNEL_B_FINE_TUNE;
            }
            break;
        case INDEX_OF_CHANNEL_B_COARSE_TUNE:
            if(IS_READABLE_CHANNEL_B_COARSE_TUNE) {
                data_bus = self->regs.named.channel_b_coarse_tune;
                data_bus &= MASK_OF_CHANNEL_B_COARSE_TUNE;
            }
            break;
        case INDEX_OF_CHANNEL_C_FINE_TUNE:
            if(IS_READABLE_CHANNEL_C_FINE_TUNE) {
                data_bus = self->regs.named.channel_c_fine_tune;
                data_bus &= MASK_OF_CHANNEL_C_FINE_TUNE;
            }
            break;
        case INDEX_OF_CHANNEL_C_COARSE_TUNE:
            if(IS_READABLE_CHANNEL_C_COARSE_TUNE) {
                data_bus = self->regs.named.channel_c_coarse_tune;
                data_bus &= MASK_OF_CHANNEL_C_COARSE_TUNE;
            }
            break;
        case INDEX_OF_NOISE_PERIOD:
            if(IS_READABLE_NOISE_PERIOD) {
                data_bus = self->regs.named.noise_period;
                data_bus &= MASK_OF_NOISE_PERIOD;
            }
            break;
        case INDEX_OF_MIXER_AND_IO_CONTROL:
            if(IS_READABLE_MIXER_AND_IO_CONTROL) {
                data_bus = self->regs.named.mixer_and_io_control;
                data_bus &= MASK_OF_MIXER_AND_IO_CONTROL;
            }
            break;
        case INDEX_OF_CHANNEL_A_VOLUME:
            if(IS_READABLE_CHANNEL_A_VOLUME) {
                data_bus = self->regs.named.channel_a_volume;
                data_bus &= MASK_OF_CHANNEL_A_VOLUME;
            }
            break;
        case INDEX_OF_CHANNEL_B_VOLUME:
            if(IS_READABLE_CHANNEL_B_VOLUME) {
                data_bus = self->regs.named.channel_b_volume;
                data_bus &= MASK_OF_CHANNEL_B_VOLUME;
            }
            break;
        case INDEX_OF_CHANNEL_C_VOLUME:
            if(IS_READABLE_CHANNEL_C_VOLUME) {
                data_bus = self->regs.named.channel_c_volume;
                data_bus &= MASK_OF_CHANNEL_C_VOLUME;
            }
            break;
        case INDEX_OF_ENVELOPE_FINE_TUNE:
            if(IS_READABLE_ENVELOPE_FINE_TUNE) {
                data_bus = self->regs.named.envelope_fine_tune;
                data_bus &= MASK_OF_ENVELOPE_FINE_TUNE;
            }
            break;
        case INDEX_OF_ENVELOPE_COARSE_TUNE:
            if(IS_READABLE_ENVELOPE_COARSE_TUNE) {
                data_bus = self->regs.named.envelope_coarse_tune;
                data_bus &= MASK_OF_ENVELOPE_COARSE_TUNE;
            }
            break;
        case INDEX_OF_ENVELOPE_SHAPE:
            if(IS_READABLE_ENVELOPE_SHAPE) {
                data_bus = self->regs.named.envelope_shape;
                data_bus &= MASK_OF_ENVELOPE_SHAPE;
            }
            break;
        case INDEX_OF_IO_PORT_A:
            if(IS_READABLE_IO_PORT_A) {
                data_bus = self->regs.named.io_port_a;
                data_bus &= MASK_OF_IO_PORT_A;
            }
            break;
        case INDEX_OF_IO_PORT_B:
            if(IS_READABLE_IO_PORT_B) {
                data_bus = self->regs.named.io_port_b;
                data_bus &= MASK_OF_IO_PORT_B;
            }
            break;
        default:
            break;
    }
    return data_bus;
}

uint8_t xcpc_psg_8910_wr(XcpcPsg8910* self, uint8_t data_bus)
{
    switch(self->regs.named.address_register) {
        case INDEX_OF_CHANNEL_A_FINE_TUNE:
            if(IS_WRITABLE_CHANNEL_A_FINE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_A_FINE_TUNE;
                self->regs.named.channel_a_fine_tune = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_A_COARSE_TUNE:
            if(IS_WRITABLE_CHANNEL_A_COARSE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_A_COARSE_TUNE;
                self->regs.named.channel_a_coarse_tune = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_B_FINE_TUNE:
            if(IS_WRITABLE_CHANNEL_B_FINE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_B_FINE_TUNE;
                self->regs.named.channel_b_fine_tune = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_B_COARSE_TUNE:
            if(IS_WRITABLE_CHANNEL_B_COARSE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_B_COARSE_TUNE;
                self->regs.named.channel_b_coarse_tune = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_C_FINE_TUNE:
            if(IS_WRITABLE_CHANNEL_C_FINE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_C_FINE_TUNE;
                self->regs.named.channel_c_fine_tune = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_C_COARSE_TUNE:
            if(IS_WRITABLE_CHANNEL_C_COARSE_TUNE) {
                data_bus &= MASK_OF_CHANNEL_C_COARSE_TUNE;
                self->regs.named.channel_c_coarse_tune = data_bus;
            }
            break;
        case INDEX_OF_NOISE_PERIOD:
            if(IS_WRITABLE_NOISE_PERIOD) {
                data_bus &= MASK_OF_NOISE_PERIOD;
                self->regs.named.noise_period = data_bus;
            }
            break;
        case INDEX_OF_MIXER_AND_IO_CONTROL:
            if(IS_WRITABLE_MIXER_AND_IO_CONTROL) {
                data_bus &= MASK_OF_MIXER_AND_IO_CONTROL;
                self->regs.named.mixer_and_io_control = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_A_VOLUME:
            if(IS_WRITABLE_CHANNEL_A_VOLUME) {
                data_bus &= MASK_OF_CHANNEL_A_VOLUME;
                self->regs.named.channel_a_volume = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_B_VOLUME:
            if(IS_WRITABLE_CHANNEL_B_VOLUME) {
                data_bus &= MASK_OF_CHANNEL_B_VOLUME;
                self->regs.named.channel_b_volume = data_bus;
            }
            break;
        case INDEX_OF_CHANNEL_C_VOLUME:
            if(IS_WRITABLE_CHANNEL_C_VOLUME) {
                data_bus &= MASK_OF_CHANNEL_C_VOLUME;
                self->regs.named.channel_c_volume = data_bus;
            }
            break;
        case INDEX_OF_ENVELOPE_FINE_TUNE:
            if(IS_WRITABLE_ENVELOPE_FINE_TUNE) {
                data_bus &= MASK_OF_ENVELOPE_FINE_TUNE;
                self->regs.named.envelope_fine_tune = data_bus;
            }
            break;
        case INDEX_OF_ENVELOPE_COARSE_TUNE:
            if(IS_WRITABLE_ENVELOPE_COARSE_TUNE) {
                data_bus &= MASK_OF_ENVELOPE_COARSE_TUNE;
                self->regs.named.envelope_coarse_tune = data_bus;
            }
            break;
        case INDEX_OF_ENVELOPE_SHAPE:
            if(IS_WRITABLE_ENVELOPE_SHAPE) {
                data_bus &= MASK_OF_ENVELOPE_SHAPE;
                self->regs.named.envelope_shape = data_bus;
            }
            break;
        case INDEX_OF_IO_PORT_A:
            if(IS_WRITABLE_IO_PORT_A) {
                data_bus &= MASK_OF_IO_PORT_A;
                self->regs.named.io_port_a = data_bus;
            }
            break;
        case INDEX_OF_IO_PORT_B:
            if(IS_WRITABLE_IO_PORT_B) {
                data_bus &= MASK_OF_IO_PORT_B;
                self->regs.named.io_port_b = data_bus;
            }
            break;
        default:
            break;
    }
    return data_bus;
}
