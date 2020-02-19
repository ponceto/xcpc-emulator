/*
 * ppi-8255.c - Copyright (c) 2001-2020 - Olivier Poncet
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

XcpcPpi8255* xcpc_ppi_8255_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcPpi8255" );
    }
    return xcpc_new(XcpcPpi8255);
}

XcpcPpi8255* xcpc_ppi_8255_free(XcpcPpi8255* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcPpi8255" );
    }
    return xcpc_delete(XcpcPpi8255, self);
}

XcpcPpi8255* xcpc_ppi_8255_construct(XcpcPpi8255* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcPpi8255" );
    }
    return xcpc_ppi_8255_reset(self);
}

XcpcPpi8255* xcpc_ppi_8255_destruct(XcpcPpi8255* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcPpi8255" );
    }
    return self;
}

XcpcPpi8255* xcpc_ppi_8255_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcPpi8255" );
    }
    return xcpc_ppi_8255_construct(xcpc_ppi_8255_alloc());
}

XcpcPpi8255* xcpc_ppi_8255_delete(XcpcPpi8255* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcPpi8255" );
    }
    return xcpc_ppi_8255_free(xcpc_ppi_8255_destruct(self));
}

XcpcPpi8255* xcpc_ppi_8255_reset(XcpcPpi8255* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcPpi8255" );
    }
    if(self != NULL) {
        self->control = 0x00;
        self->port_a  = 0x00;
        self->port_b  = 0x00;
        self->port_c  = 0x00;
    }
    return self;
}
