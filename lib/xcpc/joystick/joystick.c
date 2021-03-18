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

void xcpc_joystick_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcJoystick::%s()"
          , function );
}

XcpcJoystick* xcpc_joystick_alloc(void)
{
    xcpc_joystick_trace("alloc");

    return xcpc_new(XcpcJoystick);
}

XcpcJoystick* xcpc_joystick_free(XcpcJoystick* self)
{
    xcpc_joystick_trace("free");

    return xcpc_delete(XcpcJoystick, self);
}

XcpcJoystick* xcpc_joystick_construct(XcpcJoystick* self)
{
    xcpc_joystick_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcJoystickIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcJoystickState));
    }
    return xcpc_joystick_reset(self);
}

XcpcJoystick* xcpc_joystick_destruct(XcpcJoystick* self)
{
    xcpc_joystick_trace("destruct");

    return self;
}

XcpcJoystick* xcpc_joystick_new(void)
{
    xcpc_joystick_trace("new");

    return xcpc_joystick_construct(xcpc_joystick_alloc());
}

XcpcJoystick* xcpc_joystick_delete(XcpcJoystick* self)
{
    xcpc_joystick_trace("delete");

    return xcpc_joystick_free(xcpc_joystick_destruct(self));
}

XcpcJoystick* xcpc_joystick_reset(XcpcJoystick* self)
{
    xcpc_joystick_trace("reset");

    return self;
}

XcpcJoystick* xcpc_joystick_clock(XcpcJoystick* self)
{
    return self;
}
