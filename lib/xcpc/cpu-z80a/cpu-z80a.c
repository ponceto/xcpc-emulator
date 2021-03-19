/*
 * cpu-z80a.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "cpu-z80a-priv.h"

static void xcpc_cpu_z80a_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcCpuZ80a::%s()"
          , function );
}

XcpcCpuZ80a* xcpc_cpu_z80a_alloc(void)
{
    xcpc_cpu_z80a_trace("alloc");

    return xcpc_new(XcpcCpuZ80a);
}

XcpcCpuZ80a* xcpc_cpu_z80a_free(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("free");

    return xcpc_delete(XcpcCpuZ80a, self);
}

XcpcCpuZ80a* xcpc_cpu_z80a_construct(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcCpuZ80aIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcCpuZ80aState));
    }
    return xcpc_cpu_z80a_reset(self);
}

XcpcCpuZ80a* xcpc_cpu_z80a_destruct(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("destruct");

    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_new(void)
{
    xcpc_cpu_z80a_trace("new");

    return xcpc_cpu_z80a_construct(xcpc_cpu_z80a_alloc());
}

XcpcCpuZ80a* xcpc_cpu_z80a_delete(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("delete");

    return xcpc_cpu_z80a_free(xcpc_cpu_z80a_destruct(self));
}

XcpcCpuZ80a* xcpc_cpu_z80a_reset(XcpcCpuZ80a* self)
{
    xcpc_cpu_z80a_trace("reset");

    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_clock(XcpcCpuZ80a* self)
{
    return self;
}
