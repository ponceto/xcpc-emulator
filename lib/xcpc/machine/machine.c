/*
 * machine.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "machine-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcMachine::%s()", function);
}

static void default_error_handler(XcpcMachine* self)
{
    log_trace("default_error_handler");
}

static void create_devices(XcpcMachine* self)
{
    log_trace("create_devices");

    /* monitor */ {
        if(self->state.monitor == NULL) {
            self->state.monitor = xcpc_monitor_new();
        }
    }
    /* keyboard */ {
        if(self->state.keyboard == NULL) {
            self->state.keyboard = xcpc_keyboard_new();
        }
    }
    /* joystick */ {
        if(self->state.joystick == NULL) {
            self->state.joystick = xcpc_joystick_new();
        }
    }
    /* cpu_z80a */ {
        if(self->state.cpu_z80a == NULL) {
            self->state.cpu_z80a = xcpc_cpu_z80a_new();
        }
    }
    /* vga_core */ {
        if(self->state.vga_core == NULL) {
            self->state.vga_core = xcpc_vga_core_new();
        }
    }
    /* vdc_6845 */ {
        if(self->state.vdc_6845 == NULL) {
            self->state.vdc_6845 = xcpc_vdc_6845_new();
        }
    }
    /* ppi_8255 */ {
        if(self->state.ppi_8255 == NULL) {
            self->state.ppi_8255 = xcpc_ppi_8255_new();
        }
    }
    /* psg_8910 */ {
        if(self->state.psg_8910 == NULL) {
            self->state.psg_8910 = xcpc_psg_8910_new();
        }
    }
    /* fdc_765a */ {
        if(self->state.fdc_765a == NULL) {
            self->state.fdc_765a = xcpc_fdc_765a_new();
        }
    }
    /* ram_bank */ {
        int ram_index = 0;
        int ram_count = countof(self->state.ram_bank);
        do {
            if(self->state.ram_bank[ram_index] == NULL) {
                self->state.ram_bank[ram_index] = xcpc_ram_bank_new();
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        int rom_index = 0;
        int rom_count = countof(self->state.rom_bank);
        do {
            if(self->state.rom_bank[rom_index] == NULL) {
                self->state.rom_bank[rom_index] = xcpc_rom_bank_new();
            }
        } while(++rom_index < rom_count);
    }
    /* exp_bank */ {
        int exp_index = 0;
        int exp_count = countof(self->state.exp_bank);
        do {
            if(self->state.exp_bank[exp_index] == NULL) {
                self->state.exp_bank[exp_index] = xcpc_rom_bank_new();
            }
        } while(++exp_index < exp_count);
    }
}

static void delete_devices(XcpcMachine* self)
{
    log_trace("delete_devices");

    /* monitor */ {
        if(self->state.monitor != NULL) {
            self->state.monitor = xcpc_monitor_delete(self->state.monitor);
        }
    }
    /* keyboard */ {
        if(self->state.keyboard != NULL) {
            self->state.keyboard = xcpc_keyboard_delete(self->state.keyboard);
        }
    }
    /* joystick */ {
        if(self->state.joystick != NULL) {
            self->state.joystick = xcpc_joystick_delete(self->state.joystick);
        }
    }
    /* cpu_z80a */ {
        if(self->state.cpu_z80a != NULL) {
            self->state.cpu_z80a = xcpc_cpu_z80a_delete(self->state.cpu_z80a);
        }
    }
    /* vga_core */ {
        if(self->state.vga_core != NULL) {
            self->state.vga_core = xcpc_vga_core_delete(self->state.vga_core);
        }
    }
    /* vdc_6845 */ {
        if(self->state.vdc_6845 != NULL) {
            self->state.vdc_6845 = xcpc_vdc_6845_delete(self->state.vdc_6845);
        }
    }
    /* ppi_8255 */ {
        if(self->state.ppi_8255 != NULL) {
            self->state.ppi_8255 = xcpc_ppi_8255_delete(self->state.ppi_8255);
        }
    }
    /* psg_8910 */ {
        if(self->state.psg_8910 != NULL) {
            self->state.psg_8910 = xcpc_psg_8910_delete(self->state.psg_8910);
        }
    }
    /* fdc_765a */ {
        if(self->state.fdc_765a != NULL) {
            self->state.fdc_765a = xcpc_fdc_765a_delete(self->state.fdc_765a);
        }
    }
    /* ram_bank */ {
        int ram_index = 0;
        int ram_count = countof(self->state.ram_bank);
        do {
            if(self->state.ram_bank[ram_index] != NULL) {
                self->state.ram_bank[ram_index] = xcpc_ram_bank_delete(self->state.ram_bank[ram_index]);
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        int rom_index = 0;
        int rom_count = countof(self->state.rom_bank);
        do {
            if(self->state.rom_bank[rom_index] != NULL) {
                self->state.rom_bank[rom_index] = xcpc_rom_bank_delete(self->state.rom_bank[rom_index]);
            }
        } while(++rom_index < rom_count);
    }
    /* exp_bank */ {
        int exp_index = 0;
        int exp_count = countof(self->state.exp_bank);
        do {
            if(self->state.exp_bank[exp_index] != NULL) {
                self->state.exp_bank[exp_index] = xcpc_rom_bank_delete(self->state.exp_bank[exp_index]);
            }
        } while(++exp_index < exp_count);
    }
}

static void reset_devices(XcpcMachine* self)
{
    log_trace("reset_devices");

    /* monitor */ {
        if(self->state.monitor != NULL) {
            (void) xcpc_monitor_reset(self->state.monitor);
        }
    }
    /* keyboard */ {
        if(self->state.keyboard != NULL) {
            (void) xcpc_keyboard_reset(self->state.keyboard);
        }
    }
    /* joystick */ {
        if(self->state.joystick != NULL) {
            (void) xcpc_joystick_reset(self->state.joystick);
        }
    }
    /* cpu_z80a */ {
        if(self->state.cpu_z80a != NULL) {
            (void) xcpc_cpu_z80a_reset(self->state.cpu_z80a);
        }
    }
    /* vga_core */ {
        if(self->state.vga_core != NULL) {
            (void) xcpc_vga_core_reset(self->state.vga_core);
        }
    }
    /* vdc_6845 */ {
        if(self->state.vdc_6845 != NULL) {
            (void) xcpc_vdc_6845_reset(self->state.vdc_6845);
        }
    }
    /* ppi_8255 */ {
        if(self->state.ppi_8255 != NULL) {
            (void) xcpc_ppi_8255_reset(self->state.ppi_8255);
        }
    }
    /* psg_8910 */ {
        if(self->state.psg_8910 != NULL) {
            (void) xcpc_psg_8910_reset(self->state.psg_8910);
        }
    }
    /* fdc_765a */ {
        if(self->state.fdc_765a != NULL) {
            (void) xcpc_fdc_765a_reset(self->state.fdc_765a);
        }
    }
    /* ram_bank */ {
        int ram_index = 0;
        int ram_count = countof(self->state.ram_bank);
        do {
            if(self->state.ram_bank[ram_index] != NULL) {
                (void) xcpc_ram_bank_reset(self->state.ram_bank[ram_index]);
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        int rom_index = 0;
        int rom_count = countof(self->state.rom_bank);
        do {
            if(self->state.rom_bank[rom_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->state.rom_bank[rom_index]);
            }
        } while(++rom_index < rom_count);
    }
    /* exp_bank */ {
        int exp_index = 0;
        int exp_count = countof(self->state.exp_bank);
        do {
            if(self->state.exp_bank[exp_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->state.exp_bank[exp_index]);
            }
        } while(++exp_index < exp_count);
    }
}

XcpcMachine* xcpc_machine_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcMachine);
}

XcpcMachine* xcpc_machine_free(XcpcMachine* self)
{
    log_trace("free");

    return xcpc_delete(XcpcMachine, self);
}

XcpcMachine* xcpc_machine_construct(XcpcMachine* self)
{
    log_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMachineIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcMachineState));
    }
    /* initialize iface */ {
        (void) xcpc_machine_set_iface(self, NULL);
    }
    /* create devices */ {
        create_devices(self);
    }
    /* reset */ {
        (void) xcpc_machine_reset(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_destruct(XcpcMachine* self)
{
    log_trace("destruct");

    /* delete devices */ {
        delete_devices(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_new(void)
{
    log_trace("new");

    return xcpc_machine_construct(xcpc_machine_alloc());
}

XcpcMachine* xcpc_machine_delete(XcpcMachine* self)
{
    log_trace("delete");

    return xcpc_machine_free(xcpc_machine_destruct(self));
}

XcpcMachine* xcpc_machine_set_iface(XcpcMachine* self, const XcpcMachineIface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data      = self;
        self->iface.drive0_error   = &default_error_handler;
        self->iface.drive1_error   = &default_error_handler;
        self->iface.snapshot_error = &default_error_handler;
    }
    return self;
}

XcpcMachine* xcpc_machine_reset(XcpcMachine* self)
{
    log_trace("reset");

    /* reset devices */ {
        reset_devices(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_clock(XcpcMachine* self)
{
    return self;
}
