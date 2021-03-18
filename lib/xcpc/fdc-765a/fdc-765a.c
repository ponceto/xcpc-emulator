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

static XcpcFdcImpl* fdc_impl_new(void)
{
    XcpcFdcImpl* fdc_impl = xcpc_new(XcpcFdcImpl);

    /* clear */ {
        (void) memset(fdc_impl, 0, sizeof(XcpcFdcImpl));
    }
    /* initialize */ {
        fdc_init_impl(fdc_impl);
    }
    return fdc_impl;
}

static XcpcFdcImpl* fdc_impl_delete(XcpcFdcImpl* fdc_impl)
{
    return xcpc_delete(XcpcFdcImpl, fdc_impl);
}

static XcpcFddImpl* fdd_impl_new(void)
{
    XcpcFddImpl* fdd_impl = xcpc_new(XcpcFddImpl);

    /* clear */ {
        (void) memset(fdd_impl, 0, sizeof(XcpcFddImpl));
    }
    /* initialize */ {
        fdd_init_impl(fdd_impl);
    }
    return fdd_impl;
}

static XcpcFddImpl* fdd_impl_delete(XcpcFddImpl* fdd_impl)
{
    return xcpc_delete(XcpcFddImpl, fdd_impl);
}

static XcpcFddImpl* fdd_impl_insert(XcpcFddImpl* fdd_impl, const char* filename)
{
    if((fdd_impl->fd_vtable != NULL)
    && (fdd_impl->fd_vtable->fdv_eject != NULL)) {
        (*fdd_impl->fd_vtable->fdv_eject)(fdd_impl);
    }
    if((filename != NULL) && (*filename != '\0')) {
        (void) snprintf(fdd_impl->fdl_filename, sizeof(fdd_impl->fdl_filename), "%s", filename);
    }
    return fdd_impl;
}

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

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcFdc765aIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcFdc765aState));
    }
    /* initialize state */ {
        self->state.fdc_impl = fdc_impl_new();
        self->state.fd0_impl = fdd_impl_new();
        self->state.fd1_impl = fdd_impl_new();
        self->state.fd2_impl = fdd_impl_new();
        self->state.fd3_impl = fdd_impl_new();
    }
    return xcpc_fdc_765a_reset(self);
}

XcpcFdc765a* xcpc_fdc_765a_destruct(XcpcFdc765a* self)
{
    xcpc_fdc_765a_trace("destruct");

    /* finalize state */ {
        self->state.fd3_impl = fdd_impl_delete(self->state.fd3_impl);
        self->state.fd2_impl = fdd_impl_delete(self->state.fd2_impl);
        self->state.fd1_impl = fdd_impl_delete(self->state.fd1_impl);
        self->state.fd0_impl = fdd_impl_delete(self->state.fd0_impl);
        self->state.fdc_impl = fdc_impl_delete(self->state.fdc_impl);
    }
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

    /* reset state */ {
        if(self->state.fdc_impl != NULL) {
            fdc_reset_impl(self->state.fdc_impl);
        }
        if(self->state.fd0_impl != NULL) {
            fdd_reset_impl(self->state.fd0_impl);
        }
        if(self->state.fd1_impl != NULL) {
            fdd_reset_impl(self->state.fd1_impl);
        }
        if(self->state.fd2_impl != NULL) {
            fdd_reset_impl(self->state.fd2_impl);
        }
        if(self->state.fd3_impl != NULL) {
            fdd_reset_impl(self->state.fd3_impl);
        }
    }
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_clock(XcpcFdc765a* self)
{
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_attach(XcpcFdc765a* self, int drive)
{
    if(self->state.fdc_impl != NULL) {
        switch(drive) {
            case 0:
                self->state.fdc_impl->fdc_drive[0] = self->state.fd0_impl;
                break;
            case 1:
                self->state.fdc_impl->fdc_drive[1] = self->state.fd1_impl;
                break;
            case 2:
                self->state.fdc_impl->fdc_drive[2] = self->state.fd2_impl;
                break;
            case 3:
                self->state.fdc_impl->fdc_drive[3] = self->state.fd3_impl;
                break;
            default:
                break;
        }
    }
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_detach(XcpcFdc765a* self, int drive)
{
    if(self->state.fdc_impl != NULL) {
        switch(drive) {
            case 0:
                self->state.fdc_impl->fdc_drive[0] = NULL;
                break;
            case 1:
                self->state.fdc_impl->fdc_drive[1] = NULL;
                break;
            case 2:
                self->state.fdc_impl->fdc_drive[2] = NULL;
                break;
            case 3:
                self->state.fdc_impl->fdc_drive[3] = NULL;
                break;
            default:
                break;
        }
    }
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_insert(XcpcFdc765a* self, const char* filename, int drive)
{
    switch(drive) {
        case 0:
            if(self->state.fd0_impl != NULL) {
                fdd_impl_insert(self->state.fd0_impl, filename);
            }
            break;
        case 1:
            if(self->state.fd1_impl != NULL) {
                fdd_impl_insert(self->state.fd1_impl, filename);
            }
            break;
        case 2:
            if(self->state.fd2_impl != NULL) {
                fdd_impl_insert(self->state.fd2_impl, filename);
            }
            break;
        case 3:
            if(self->state.fd3_impl != NULL) {
                fdd_impl_insert(self->state.fd3_impl, filename);
            }
            break;
        default:
            break;
    }
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_rstat(XcpcFdc765a* self, uint8_t* data)
{
#if 1
    if(self->state.fdc_impl != NULL) {
        *data = fdc_rd_stat(self->state.fdc_impl);
    }
#endif
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_wstat(XcpcFdc765a* self, uint8_t* data)
{
#if 0
    if(self->state.fdc_impl != NULL) {
        fdc_wr_stat(self->state.fdc_impl, *data);
    }
#endif
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_rdata(XcpcFdc765a* self, uint8_t* data)
{
#if 1
    if(self->state.fdc_impl != NULL) {
        *data = fdc_rd_data(self->state.fdc_impl);
    }
#endif
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_wdata(XcpcFdc765a* self, uint8_t* data)
{
#if 1
    if(self->state.fdc_impl != NULL) {
        fdc_wr_data(self->state.fdc_impl, *data);
    }
#endif
    return self;
}

XcpcFdc765a* xcpc_fdc_765a_motor(XcpcFdc765a* self, uint8_t motor)
{
#if 1
    if(self->state.fdc_impl != NULL) {
        fdc_set_motor(self->state.fdc_impl, motor);
    }
#endif
    return self;
}
