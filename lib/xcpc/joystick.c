/*
 * joystick.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "joystick-priv.h"

XcpcJoystick* xcpc_joystick_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcJoystick" );
    }
    return xcpc_new(XcpcJoystick);
}

XcpcJoystick* xcpc_joystick_free(XcpcJoystick* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcJoystick" );
    }
    return xcpc_delete(XcpcJoystick, self);
}

XcpcJoystick* xcpc_joystick_construct(XcpcJoystick* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcJoystick" );
    }
    return xcpc_joystick_reset(self);
}

XcpcJoystick* xcpc_joystick_destruct(XcpcJoystick* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcJoystick" );
    }
    return self;
}

XcpcJoystick* xcpc_joystick_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcJoystick" );
    }
    return xcpc_joystick_construct(xcpc_joystick_alloc());
}

XcpcJoystick* xcpc_joystick_delete(XcpcJoystick* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcJoystick" );
    }
    return xcpc_joystick_free(xcpc_joystick_destruct(self));
}

XcpcJoystick* xcpc_joystick_reset(XcpcJoystick* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcJoystick" );
    }
    if(self != NULL) {
        self->reserved = NULL;
    }
    return self;
}
