/*
 * monitor.c - Copyright (c) 2001-2020 - Olivier Poncet
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

XcpcMonitor* xcpc_monitor_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcMonitor" );
    }
    return xcpc_new(XcpcMonitor);
}

XcpcMonitor* xcpc_monitor_free(XcpcMonitor* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcMonitor" );
    }
    return xcpc_delete(XcpcMonitor, self);
}

XcpcMonitor* xcpc_monitor_construct(XcpcMonitor* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcMonitor" );
    }
    return xcpc_monitor_reset(self);
}

XcpcMonitor* xcpc_monitor_destruct(XcpcMonitor* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcMonitor" );
    }
    return self;
}

XcpcMonitor* xcpc_monitor_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcMonitor" );
    }
    return xcpc_monitor_construct(xcpc_monitor_alloc());
}

XcpcMonitor* xcpc_monitor_delete(XcpcMonitor* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcMonitor" );
    }
    return xcpc_monitor_free(xcpc_monitor_destruct(self));
}

XcpcMonitor* xcpc_monitor_reset(XcpcMonitor* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcMonitor" );
    }
    if(self != NULL) {
        self->reserved = NULL;
    }
    return self;
}
