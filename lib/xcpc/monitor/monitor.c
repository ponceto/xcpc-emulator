/*
 * monitor.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "monitor-priv.h"

void xcpc_monitor_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcMonitor::%s()"
          , function );
}

XcpcMonitor* xcpc_monitor_alloc(void)
{
    xcpc_monitor_trace("alloc");

    return xcpc_new(XcpcMonitor);
}

XcpcMonitor* xcpc_monitor_free(XcpcMonitor* self)
{
    xcpc_monitor_trace("free");

    return xcpc_delete(XcpcMonitor, self);
}

XcpcMonitor* xcpc_monitor_construct(XcpcMonitor* self)
{
    xcpc_monitor_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMonitorIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcMonitorState));
    }
    return xcpc_monitor_reset(self);
}

XcpcMonitor* xcpc_monitor_destruct(XcpcMonitor* self)
{
    xcpc_monitor_trace("destruct");

    return self;
}

XcpcMonitor* xcpc_monitor_new(void)
{
    xcpc_monitor_trace("new");

    return xcpc_monitor_construct(xcpc_monitor_alloc());
}

XcpcMonitor* xcpc_monitor_delete(XcpcMonitor* self)
{
    xcpc_monitor_trace("delete");

    return xcpc_monitor_free(xcpc_monitor_destruct(self));
}

XcpcMonitor* xcpc_monitor_reset(XcpcMonitor* self)
{
    xcpc_monitor_trace("reset");

    return self;
}

XcpcMonitor* xcpc_monitor_clock(XcpcMonitor* self)
{
    return self;
}
