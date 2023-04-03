/*
 * rom-bank.c - Copyright (c) 2001-2023 - Olivier Poncet
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
#include "rom-bank-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcRomBank::%s()", function);
}

XcpcRomBank* xcpc_rom_bank_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcRomBank);
}

XcpcRomBank* xcpc_rom_bank_free(XcpcRomBank* self)
{
    log_trace("free");

    return xcpc_delete(XcpcRomBank, self);
}

XcpcRomBank* xcpc_rom_bank_construct(XcpcRomBank* self, const XcpcRomBankIface* iface)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcRomBankIface));
        (void) memset(&self->setup, 0, sizeof(XcpcRomBankSetup));
        (void) memset(&self->state, 0, sizeof(XcpcRomBankState));
    }
    /* initialize iface */ {
        if(iface != NULL) {
            *(&self->iface) = *(iface);
        }
        else {
            self->iface.user_data = self;
        }
    }
    /* reset */ {
        (void) xcpc_rom_bank_reset(self);
    }
    return self;
}

XcpcRomBank* xcpc_rom_bank_destruct(XcpcRomBank* self)
{
    log_trace("destruct");

    return self;
}

XcpcRomBank* xcpc_rom_bank_new(const XcpcRomBankIface* iface)
{
    log_trace("new");

    return xcpc_rom_bank_construct(xcpc_rom_bank_alloc(), iface);
}

XcpcRomBank* xcpc_rom_bank_delete(XcpcRomBank* self)
{
    log_trace("delete");

    return xcpc_rom_bank_free(xcpc_rom_bank_destruct(self));
}

XcpcRomBank* xcpc_rom_bank_reset(XcpcRomBank* self)
{
    log_trace("reset");

    /* reset state */ {
        unsigned int index = 0;
        unsigned int count = countof(self->state.data);
        do {
            self->state.data[index] |= 0;
        } while(++index < count);
    }
    return self;
}

XcpcRomBankStatus xcpc_rom_bank_load(XcpcRomBank* self, const char* filename, size_t offset)
{
    XcpcRomBankStatus status = XCPC_ROM_BANK_STATUS_SUCCESS;
    FILE*             file   = NULL;

    log_trace("load");
    /* check filename */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            if((filename == NULL) || (*filename == '\0')) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* open file */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            file = fopen(filename, "r");
            if(file == NULL) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* seek file */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            if(fseek(file, offset, SEEK_SET) != 0) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* load data */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            void*  rom_data = &self->state.data;
            size_t rom_size = sizeof(self->state.data);
            size_t byte_count = fread(rom_data, 1, rom_size, file);
            if(byte_count != rom_size) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* close file */ {
        if(file != NULL) {
            file = ((void) fclose(file), NULL);
        }
    }
    return status;
}

XcpcRomBankStatus xcpc_rom_bank_copy(XcpcRomBank* self, const uint8_t* data, size_t size)
{
    XcpcRomBankStatus status = XCPC_ROM_BANK_STATUS_SUCCESS;

    log_trace("copy");
    /* check data and size */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            if((data == NULL) || (size > sizeof(self->state.data))) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* copy data */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            (void) memcpy(self->state.data, data, size);
        }
    }
    return status;
}
