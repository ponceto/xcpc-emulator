/*
 * rom-bank.c - Copyright (c) 2001-2021 - Olivier Poncet
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

#ifndef CHECK_INSTANCE
#define CHECK_INSTANCE(pointer) (pointer != NULL ? XCPC_ROM_BANK_STATUS_SUCCESS : XCPC_ROM_BANK_STATUS_FAILURE)
#endif

XcpcRomBank* xcpc_rom_bank_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcRomBank" );
    }
    return xcpc_new(XcpcRomBank);
}

XcpcRomBank* xcpc_rom_bank_free(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcRomBank" );
    }
    return xcpc_delete(XcpcRomBank, self);
}

XcpcRomBank* xcpc_rom_bank_construct(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcRomBank" );
    }
    return xcpc_rom_bank_clear(self);
}

XcpcRomBank* xcpc_rom_bank_destruct(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcRomBank" );
    }
    return self;
}

XcpcRomBank* xcpc_rom_bank_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcRomBank" );
    }
    return xcpc_rom_bank_construct(xcpc_rom_bank_alloc());
}

XcpcRomBank* xcpc_rom_bank_delete(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcRomBank" );
    }
    return xcpc_rom_bank_free(xcpc_rom_bank_destruct(self));
}

XcpcRomBank* xcpc_rom_bank_reset(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcRomBank" );
    }
    if(self != NULL) {
        /* XXX */
    }
    return self;
}

XcpcRomBank* xcpc_rom_bank_clear(XcpcRomBank* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::clear()"
              , "XcpcRomBank" );
    }
    if(self != NULL) {
        (void) memset(self->data, 0, sizeof(self->data));
    }
    return self;
}

XcpcRomBankStatus xcpc_rom_bank_load(XcpcRomBank* self, const char* filename, size_t offset)
{
    XcpcRomBankStatus status = CHECK_INSTANCE(self);
    FILE*             file   = NULL;

    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::load()"
              , "XcpcRomBank" );
    }
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
            void*  rom_data = &self->data;
            size_t rom_size = sizeof(self->data);
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
    XcpcRomBankStatus status = CHECK_INSTANCE(self);

    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::copy()"
              , "XcpcRomBank" );
    }
    /* check data and size */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            if((data == NULL) || (size > sizeof(self->data))) {
                status = XCPC_ROM_BANK_STATUS_FAILURE;
            }
        }
    }
    /* copy data */ {
        if(status == XCPC_ROM_BANK_STATUS_SUCCESS) {
            (void) memcpy(self->data, data, size);
        }
    }
    return status;
}
