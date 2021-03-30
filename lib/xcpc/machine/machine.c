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

static void cpc_mem_select(XcpcMachine* self)
{
    if(self->setup.ramsize >= XCPC_RAMSIZE_128K) {
        switch(self->pager.conf.ram) {
            case 0x00:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[1]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
                }
                break;
            case 0x01:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[1]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[7]->state.data;
                }
                break;
            case 0x02:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[4]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[5]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[6]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[7]->state.data;
                }
                break;
            case 0x03:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[3]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[7]->state.data;
                }
                break;
            case 0x04:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[4]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
                }
                break;
            case 0x05:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[5]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
                }
                break;
            case 0x06:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[6]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
                }
                break;
            case 0x07:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[7]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
                }
                break;
            default:
                xcpc_log_alert("cpc_mem_select() : unsupported ram configuration (%02x)", self->pager.conf.ram);
                break;
        }
    }
    else {
        self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->state.ram_bank[0]->state.data;
        self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->state.ram_bank[1]->state.data;
        self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->state.ram_bank[2]->state.data;
        self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->state.ram_bank[3]->state.data;
    }
    if((self->state.vga_core->state.rmr & 0x04) == 0) {
        if(self->state.rom_bank[0] != NULL) {
            self->pager.bank.rd[0] = self->state.rom_bank[0]->state.data;
        }
    }
    if((self->state.vga_core->state.rmr & 0x08) == 0) {
        if(self->state.rom_bank[1] != NULL) {
            self->pager.bank.rd[3] = self->state.rom_bank[1]->state.data;
        }
        if(self->state.exp_bank[self->pager.conf.rom] != NULL) {
            self->pager.bank.rd[3] = self->state.exp_bank[self->pager.conf.rom]->state.data;
        }
    }
}

static uint8_t cpu_mreq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t addr)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));
    const uint8_t byte = (self->pager.bank.rd[addr >> 14][addr & 0x3fff]);

    return byte;
}

static uint8_t cpu_mreq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t addr)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));
    const uint8_t byte = (self->pager.bank.rd[addr >> 14][addr & 0x3fff]);

    return byte;
}

static uint8_t cpu_mreq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));
    const uint8_t byte = (self->pager.bank.wr[addr >> 14][addr & 0x3fff] = data);

    return byte;
}

static uint8_t cpu_iorq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));

    if(self != NULL) {
        log_trace("cpu_iorq_m1");
    }
    return 0x00;
}

static uint8_t cpu_iorq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));

    if(self != NULL) {
        log_trace("cpu_iorq_rd");
    }
    return 0x00;
}

static uint8_t cpu_iorq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(cpu_z80a->iface.user_data));

    if(self != NULL) {
        log_trace("cpu_iorq_wr");
    }
    return 0x00;
}

static uint8_t vdc_hsync(XcpcVdc6845* vdc_6845, int hsync)
{
    XcpcMachine* self = ((XcpcMachine*)(vdc_6845->iface.user_data));

    if(self != NULL) {
        log_trace("vdc_hsync");
    }
    return 0x00;
}

static uint8_t vdc_vsync(XcpcVdc6845* vdc_6845, int vsync)
{
    XcpcMachine* self = ((XcpcMachine*)(vdc_6845->iface.user_data));

    if(self != NULL) {
        log_trace("vdc_vsync");
    }
    return 0x00;
}

static uint8_t ppi_rd_port_a(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_rd_port_a");
    }
    return 0x00;
}

static uint8_t ppi_wr_port_a(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_wr_port_a");
    }
    return 0x00;
}

static uint8_t ppi_rd_port_b(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_rd_port_b");
    }
    return 0x00;
}

static uint8_t ppi_wr_port_b(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_wr_port_b");
    }
    return 0x00;
}

static uint8_t ppi_rd_port_c(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_rd_port_c");
    }
    return 0x00;
}

static uint8_t ppi_wr_port_c(XcpcPpi8255* ppi_8255, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(ppi_8255->iface.user_data));

    if(self != NULL) {
        log_trace("ppi_wr_port_c");
    }
    return 0x00;
}

static uint8_t psg_rd_port_a(XcpcPsg8910* psg_8910, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(psg_8910->iface.user_data));

    if(self != NULL) {
        log_trace("psg_rd_port_a");
    }
    return 0x00;
}

static uint8_t psg_wr_port_a(XcpcPsg8910* psg_8910, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(psg_8910->iface.user_data));

    if(self != NULL) {
        log_trace("psg_wr_port_a");
    }
    return 0x00;
}

static uint8_t psg_rd_port_b(XcpcPsg8910* psg_8910, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(psg_8910->iface.user_data));

    if(self != NULL) {
        log_trace("psg_rd_port_b");
    }
    return 0x00;
}

static uint8_t psg_wr_port_b(XcpcPsg8910* psg_8910, uint8_t data)
{
    XcpcMachine* self = ((XcpcMachine*)(psg_8910->iface.user_data));

    if(self != NULL) {
        log_trace("psg_wr_port_b");
    }
    return 0x00;
}

static void init_iface(XcpcMachine* self)
{
    (void) xcpc_machine_set_iface(self, NULL);
}

static void fini_iface(XcpcMachine* self)
{
}

static void init_setup(XcpcMachine* self)
{
    self->setup.computer_model  = XCPC_COMPUTER_MODEL_UNKNOWN;
    self->setup.monitor_model   = XCPC_MONITOR_MODEL_UNKNOWN;
    self->setup.refresh_rate    = XCPC_REFRESH_RATE_UNKNOWN;
    self->setup.keyboard_layout = XCPC_KEYBOARD_LAYOUT_UNKNOWN;
    self->setup.manufacturer    = XCPC_MANUFACTURER_UNKNOWN;
    self->setup.ramsize         = XCPC_RAMSIZE_UNKNOWN;
}

static void fini_setup(XcpcMachine* self)
{
}

static void init_state(XcpcMachine* self)
{
    /* monitor */ {
        const XcpcMonitorIface monitor_iface = {
            self /* user_data */
        };
        if(self->state.monitor == NULL) {
            self->state.monitor = xcpc_monitor_new();
            self->state.monitor = xcpc_monitor_set_iface(self->state.monitor, &monitor_iface);
        }
    }
    /* keyboard */ {
        const XcpcKeyboardIface keyboard_iface = {
            self /* user_data */
        };
        if(self->state.keyboard == NULL) {
            self->state.keyboard = xcpc_keyboard_new();
            self->state.keyboard = xcpc_keyboard_set_iface(self->state.keyboard, &keyboard_iface);
        }
    }
    /* joystick */ {
        const XcpcJoystickIface joystick_iface = {
            self /* user_data */
        };
        if(self->state.joystick == NULL) {
            self->state.joystick = xcpc_joystick_new();
            self->state.joystick = xcpc_joystick_set_iface(self->state.joystick, &joystick_iface);
        }
    }
    /* cpu_z80a */ {
        const XcpcCpuZ80aIface cpu_z80a_iface = {
            self,         /* user_data */
            &cpu_mreq_m1, /* mreq_m1   */
            &cpu_mreq_rd, /* mreq_rd   */
            &cpu_mreq_wr, /* mreq_wr   */
            &cpu_iorq_m1, /* iorq_m1   */
            &cpu_iorq_rd, /* iorq_rd   */
            &cpu_iorq_wr, /* iorq_wr   */
        };
        if(self->state.cpu_z80a == NULL) {
            self->state.cpu_z80a = xcpc_cpu_z80a_new();
            self->state.cpu_z80a = xcpc_cpu_z80a_set_iface(self->state.cpu_z80a, &cpu_z80a_iface);
        }
    }
    /* vga_core */ {
        const XcpcVgaCoreIface vga_core_iface = {
            self /* user_data */
        };
        if(self->state.vga_core == NULL) {
            self->state.vga_core = xcpc_vga_core_new();
            self->state.vga_core = xcpc_vga_core_set_iface(self->state.vga_core, &vga_core_iface);
        }
    }
    /* vdc_6845 */ {
        const XcpcVdc6845Iface vdc_6845_iface = {
            self,       /* user_data */
            &vdc_hsync, /* hsync     */
            &vdc_vsync, /* vsync     */
        };
        if(self->state.vdc_6845 == NULL) {
            self->state.vdc_6845 = xcpc_vdc_6845_new();
            self->state.vdc_6845 = xcpc_vdc_6845_set_iface(self->state.vdc_6845, &vdc_6845_iface);
        }
    }
    /* ppi_8255 */ {
        const XcpcPpi8255Iface ppi_8255_iface = {
            self,           /* user_data */
            &ppi_rd_port_a, /* rd_port_a */
            &ppi_wr_port_a, /* wr_port_a */
            &ppi_rd_port_b, /* rd_port_b */
            &ppi_wr_port_b, /* wr_port_b */
            &ppi_rd_port_c, /* rd_port_c */
            &ppi_wr_port_c, /* wr_port_c */
        };
        if(self->state.ppi_8255 == NULL) {
            self->state.ppi_8255 = xcpc_ppi_8255_new();
            self->state.ppi_8255 = xcpc_ppi_8255_set_iface(self->state.ppi_8255, &ppi_8255_iface);
        }
    }
    /* psg_8910 */ {
        const XcpcPsg8910Iface psg_8910_iface = {
            self,           /* user_data */
            &psg_rd_port_a, /* rd_port_a */
            &psg_wr_port_a, /* wr_port_a */
            &psg_rd_port_b, /* rd_port_b */
            &psg_wr_port_b, /* wr_port_b */
        };
        if(self->state.psg_8910 == NULL) {
            self->state.psg_8910 = xcpc_psg_8910_new();
            self->state.psg_8910 = xcpc_psg_8910_set_iface(self->state.psg_8910, &psg_8910_iface);
        }
    }
    /* fdc_765a */ {
        const XcpcFdc765aIface fdc_765a_iface = {
            self /* user_data */
        };
        if(self->state.fdc_765a == NULL) {
            self->state.fdc_765a = xcpc_fdc_765a_new();
            self->state.fdc_765a = xcpc_fdc_765a_set_iface(self->state.fdc_765a, &fdc_765a_iface);
            (void) xcpc_fdc_765a_attach(self->state.fdc_765a, XCPC_FDC_765A_DRIVE0);
            (void) xcpc_fdc_765a_attach(self->state.fdc_765a, XCPC_FDC_765A_DRIVE1);
        }
    }
    /* ram_bank */ {
        const XcpcRamBankIface ram_bank_iface = {
            self /* user_data */
        };
        int ram_index = 0;
        int ram_count = countof(self->state.ram_bank);
        do {
            if(self->state.ram_bank[ram_index] == NULL) {
                self->state.ram_bank[ram_index] = xcpc_ram_bank_new();
                self->state.ram_bank[ram_index] = xcpc_ram_bank_set_iface(self->state.ram_bank[ram_index], &ram_bank_iface);
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        const XcpcRomBankIface rom_bank_iface = {
            self /* user_data */
        };
        int rom_index = 0;
        int rom_count = countof(self->state.rom_bank);
        do {
            if(self->state.rom_bank[rom_index] == NULL) {
                self->state.rom_bank[rom_index] = xcpc_rom_bank_new();
                self->state.rom_bank[rom_index] = xcpc_rom_bank_set_iface(self->state.rom_bank[rom_index], &rom_bank_iface);
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

static void fini_state(XcpcMachine* self)
{
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

static void init_pager(XcpcMachine* self)
{
}

static void fini_pager(XcpcMachine* self)
{
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

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMachineIface));
        (void) memset(&self->setup, 0, sizeof(XcpcMachineSetup));
        (void) memset(&self->state, 0, sizeof(XcpcMachineState));
        (void) memset(&self->pager, 0, sizeof(XcpcMachinePager));
    }
    /* initialize */ {
        init_iface(self);
        init_setup(self);
        init_state(self);
        init_pager(self);
    }
    /* reset */ {
        (void) xcpc_machine_reset(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_destruct(XcpcMachine* self)
{
    log_trace("destruct");

    /* finalize */ {
        fini_pager(self);
        fini_state(self);
        fini_setup(self);
        fini_iface(self);
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
        self->iface.user_data = self;
    }
    return self;
}

XcpcMachine* xcpc_machine_reset(XcpcMachine* self)
{
    log_trace("reset");

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
    /* pager */ {
        self->pager.bank.rd[0] = self->pager.bank.wr[0] = NULL;
        self->pager.bank.rd[1] = self->pager.bank.wr[1] = NULL;
        self->pager.bank.rd[2] = self->pager.bank.wr[2] = NULL;
        self->pager.bank.rd[3] = self->pager.bank.wr[3] = NULL;
        self->pager.conf.ram = 0x00;
        self->pager.conf.rom = 0x00;
        cpc_mem_select(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_clock(XcpcMachine* self)
{
    return self;
}

XcpcMachine* xcpc_machine_insert_drive0(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_insert_drive0");

    if(self->state.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_insert(self->state.fdc_765a, XCPC_FDC_765A_DRIVE0, filename);
    }
    return self;
}

XcpcMachine* xcpc_machine_remove_drive0(XcpcMachine* self)
{
    log_trace("xcpc_machine_remove_drive0");

    if(self->state.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_remove(self->state.fdc_765a, XCPC_FDC_765A_DRIVE0);
    }
    return self;
}

XcpcMachine* xcpc_machine_insert_drive1(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_insert_drive1");

    if(self->state.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_insert(self->state.fdc_765a, XCPC_FDC_765A_DRIVE1, filename);
    }
    return self;
}

XcpcMachine* xcpc_machine_remove_drive1(XcpcMachine* self)
{
    log_trace("xcpc_machine_remove_drive1");

    if(self->state.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_remove(self->state.fdc_765a, XCPC_FDC_765A_DRIVE1);
    }
    return self;
}

XcpcMachine* xcpc_machine_load_snapshot(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_load_snapshot");

    return self;
}

XcpcMachine* xcpc_machine_save_snapshot(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_save_snapshot");

    return self;
}
