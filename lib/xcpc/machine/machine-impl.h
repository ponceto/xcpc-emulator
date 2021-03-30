/*
 * machine-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MACHINE_IMPL_H__
#define __XCPC_MACHINE_IMPL_H__

#include <xcpc/libxcpc.h>
#include <xcpc/options/options.h>
#include <xcpc/monitor/monitor.h>
#include <xcpc/keyboard/keyboard.h>
#include <xcpc/joystick/joystick.h>
#include <xcpc/cpu-z80a/cpu-z80a.h>
#include <xcpc/vga-core/vga-core.h>
#include <xcpc/vdc-6845/vdc-6845.h>
#include <xcpc/ppi-8255/ppi-8255.h>
#include <xcpc/psg-8910/psg-8910.h>
#include <xcpc/fdc-765a/fdc-765a.h>
#include <xcpc/ram-bank/ram-bank.h>
#include <xcpc/rom-bank/rom-bank.h>
#include <xcpc/snapshot/snapshot.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_MACHINE_IFACE(instance) (&(instance)->iface)
#define XCPC_MACHINE_STATE(instance) (&(instance)->state)

typedef struct _XcpcMachineIface XcpcMachineIface;
typedef struct _XcpcMachineSetup XcpcMachineSetup;
typedef struct _XcpcMachineState XcpcMachineState;
typedef struct _XcpcMachinePager XcpcMachinePager;
typedef struct _XcpcMachine      XcpcMachine;

struct _XcpcMachineIface
{
    void* user_data;
};

struct _XcpcMachineSetup
{
    XcpcComputerModel  computer_model;
    XcpcMonitorModel   monitor_model;
    XcpcRefreshRate    refresh_rate;
    XcpcKeyboardLayout keyboard_layout;
    XcpcManufacturer   manufacturer;
    XcpcRamSize        ramsize;
};

struct _XcpcMachineState
{
    XcpcMonitor*  monitor;
    XcpcKeyboard* keyboard;
    XcpcJoystick* joystick;
    XcpcCpuZ80a*  cpu_z80a;
    XcpcVgaCore*  vga_core;
    XcpcVdc6845*  vdc_6845;
    XcpcPpi8255*  ppi_8255;
    XcpcPsg8910*  psg_8910;
    XcpcFdc765a*  fdc_765a;
    XcpcRamBank*  ram_bank[8];
    XcpcRomBank*  rom_bank[2];
    XcpcRomBank*  exp_bank[256];
};

struct _XcpcMachinePager
{
    struct
    {
        uint8_t* rd[4];
        uint8_t* wr[4];
    } bank;
    struct
    {
        uint8_t ram;
        uint8_t rom;
    } conf;
};

struct _XcpcMachine
{
    XcpcMachineIface iface;
    XcpcMachineSetup setup;
    XcpcMachineState state;
    XcpcMachinePager pager;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MACHINE_IMPL_H__ */
