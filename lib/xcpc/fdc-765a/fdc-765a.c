/*
 * fdc-765a.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "fdc-765a-priv.h"

void xcpc_fdc_765a_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcFdc765a::%s()"
          , function );
}

XcpcFdc765a* xcpc_fdc_765a_alloc(void)
{
    xcpc_fdc_765a_trace("alloc");

    return xcpc_new(XcpcFdc765a);
}

XcpcFdc765a* xcpc_fdc_765a_free(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("free");

    return xcpc_delete(XcpcFdc765a, self);
}

XcpcFdc765a* xcpc_fdc_765a_construct(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("construct");

    if(self != NULL) {
        (void) memset(self, 0, sizeof(XcpcFdc765a));
    }
    return xcpc_fdc_765a_reset(self);
}

XcpcFdc765a* xcpc_fdc_765a_destruct(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("destruct");

    return self;
}

XcpcFdc765a* xcpc_fdc_765a_new(void)
{
    xcpc_fdc_765a_trace("new");

    return xcpc_fdc_765a_construct(xcpc_fdc_765a_alloc());
}

XcpcFdc765a* xcpc_fdc_765a_delete(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("delete");

    return xcpc_fdc_765a_free(xcpc_fdc_765a_destruct(self));
}

XcpcFdc765a* xcpc_fdc_765a_reset(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("reset");

    return self;
}

XcpcFdc765a* xcpc_fdc_765a_clock(XcpcFdc765a* self)
{
    return self;
}
