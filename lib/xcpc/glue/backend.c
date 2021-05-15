/*
 * backend.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "backend.h"

static unsigned long default_func(void* instance, XcpcBackendParam* data)
{
    return 100UL;
}

static XcpcBackend* reset_backend(XcpcBackend* backend)
{
    if(backend != NULL) {
        backend->instance       = NULL;
        backend->idle_func      = &default_func;
        backend->attach_func    = &default_func;
        backend->detach_func    = &default_func;
        backend->realize_func   = &default_func;
        backend->unrealize_func = &default_func;
        backend->resize_func    = &default_func;
        backend->expose_func    = &default_func;
        backend->input_func     = &default_func;
        backend->clock_func     = &default_func;
    }
    return backend;
}

XcpcBackend* xcpc_backend_init(XcpcBackend* backend)
{
    return reset_backend(backend);
}

XcpcBackend* xcpc_backend_fini(XcpcBackend* backend)
{
    return reset_backend(backend);
}
