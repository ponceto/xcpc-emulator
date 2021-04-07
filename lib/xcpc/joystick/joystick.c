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

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcJoystick::%s()", function);
}

XcpcJoystick* xcpc_joystick_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcJoystick);
}

XcpcJoystick* xcpc_joystick_free(XcpcJoystick* self)
{
    log_trace("free");

    return xcpc_delete(XcpcJoystick, self);
}

XcpcJoystick* xcpc_joystick_construct(XcpcJoystick* self)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcJoystickIface));
        (void) memset(&self->setup, 0, sizeof(XcpcJoystickSetup));
        (void) memset(&self->state, 0, sizeof(XcpcJoystickState));
    }
    /* initialize iface */ {
        (void) xcpc_joystick_set_iface(self, NULL);
    }
    /* reset */ {
        (void) xcpc_joystick_reset(self);
    }
    return self;
}

XcpcJoystick* xcpc_joystick_destruct(XcpcJoystick* self)
{
    log_trace("destruct");

    return self;
}

XcpcJoystick* xcpc_joystick_new(void)
{
    log_trace("new");

    return xcpc_joystick_construct(xcpc_joystick_alloc());
}

XcpcJoystick* xcpc_joystick_delete(XcpcJoystick* self)
{
    log_trace("delete");

    return xcpc_joystick_free(xcpc_joystick_destruct(self));
}

XcpcJoystick* xcpc_joystick_set_iface(XcpcJoystick* self, const XcpcJoystickIface* iface)
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

XcpcJoystick* xcpc_joystick_reset(XcpcJoystick* self)
{
    log_trace("reset");

    return self;
}

XcpcJoystick* xcpc_joystick_clock(XcpcJoystick* self)
{
    return self;
}
