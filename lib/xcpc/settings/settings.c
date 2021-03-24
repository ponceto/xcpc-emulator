/*
 * settings.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "settings-priv.h"

static void xcpc_settings_trace(const char* function)
{
    xcpc_trace("XcpcSettings::%s()", function);
}

XcpcSettings* xcpc_settings_alloc(void)
{
    xcpc_settings_trace("alloc");

    return xcpc_new(XcpcSettings);
}

XcpcSettings* xcpc_settings_free(XcpcSettings* self)
{
    xcpc_settings_trace("free");

    return xcpc_delete(XcpcSettings, self);
}

XcpcSettings* xcpc_settings_construct(XcpcSettings* self)
{
    xcpc_settings_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcSettingsIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcSettingsState));
    }
    return self;
}

XcpcSettings* xcpc_settings_destruct(XcpcSettings* self)
{
    xcpc_settings_trace("destruct");

    return self;
}

XcpcSettings* xcpc_settings_new(void)
{
    xcpc_settings_trace("new");

    return xcpc_settings_construct(xcpc_settings_alloc());
}

XcpcSettings* xcpc_settings_delete(XcpcSettings* self)
{
    xcpc_settings_trace("delete");

    return xcpc_settings_free(xcpc_settings_destruct(self));
}
