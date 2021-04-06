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
#define XCPC_MACHINE_SETUP(instance) (&(instance)->setup)
#define XCPC_MACHINE_STATE(instance) (&(instance)->state)

typedef struct _XcpcMachineIface XcpcMachineIface;
typedef struct _XcpcMachineSetup XcpcMachineSetup;
typedef struct _XcpcMachineState XcpcMachineState;
typedef struct _XcpcMachineBoard XcpcMachineBoard;
typedef struct _XcpcMachinePager XcpcMachinePager;
typedef struct _XcpcMachineFrame XcpcMachineFrame;
typedef struct _XcpcMachineStats XcpcMachineStats;
typedef struct _XcpcMachineTimer XcpcMachineTimer;
typedef struct _XcpcMachineFuncs XcpcMachineFuncs;
typedef struct _XcpcMachine      XcpcMachine;

typedef void (*XcpcPaintFunc)(XcpcMachine* machine);
typedef void (*XcpcKeybdFunc)(XcpcMachine* machine, XEvent* xevent);
typedef void (*XcpcMouseFunc)(XcpcMachine* machine, XEvent* xevent);

struct _XcpcMachineIface
{
    void* user_data;
};

struct _XcpcMachineSetup
{
    XcpcOptions*     options;
    XcpcCompanyName  company_name;
    XcpcMachineType  machine_type;
    XcpcMonitorType  monitor_type;
    XcpcRefreshRate  refresh_rate;
    XcpcKeyboardType keyboard_type;
    XcpcMemorySize   memory_size;
    int              turbo;
    int              xshm;
    int              fps;
};

struct _XcpcMachineState
{
    uint8_t hsync;
    uint8_t vsync;
    uint8_t refresh;
    uint8_t company;
    uint8_t expansion;
    uint8_t parallel;
    uint8_t cassette;
};

struct _XcpcMachineBoard
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

struct _XcpcMachineFrame
{
    XcpcScanline scanline_array[576];
    unsigned int scanline_count;
    unsigned int beam_x;
    unsigned int beam_y;
    unsigned int rate;
    unsigned int duration;
    unsigned int cpu_ticks;
};

struct _XcpcMachineStats
{
    unsigned int count;
    unsigned int drawn;
    char         buffer[256];
};

struct _XcpcMachineTimer
{
    struct timeval deadline;
    struct timeval profiler;
};

struct _XcpcMachineFuncs
{
    XcpcPaintFunc paint_func;
    XcpcKeybdFunc keybd_func;
    XcpcMouseFunc mouse_func;
};

struct _XcpcMachine
{
    XcpcMachineIface iface;
    XcpcMachineSetup setup;
    XcpcMachineState state;
    XcpcMachineBoard board;
    XcpcMachinePager pager;
    XcpcMachineFrame frame;
    XcpcMachineStats stats;
    XcpcMachineTimer timer;
    XcpcMachineFuncs funcs;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MACHINE_IMPL_H__ */
