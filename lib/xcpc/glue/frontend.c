/*
 * frontend.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "frontend.h"

static void default_proc(void* instance)
{
}

static XcpcFrontend* reset_frontend(XcpcFrontend* frontend)
{
    if(frontend != NULL) {
        frontend->instance  = NULL;
        frontend->reserved0 = &default_proc;
        frontend->reserved1 = &default_proc;
        frontend->reserved2 = &default_proc;
        frontend->reserved3 = &default_proc;
        frontend->reserved4 = &default_proc;
        frontend->reserved5 = &default_proc;
        frontend->reserved6 = &default_proc;
        frontend->reserved7 = &default_proc;
    }
    return frontend;
}

XcpcFrontend* xcpc_frontend_init(XcpcFrontend* frontend)
{
    return reset_frontend(frontend);
}

XcpcFrontend* xcpc_frontend_fini(XcpcFrontend* frontend)
{
    return reset_frontend(frontend);
}
