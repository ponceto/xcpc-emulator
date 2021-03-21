/*
 * amstrad-cpc.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __AMSTRAD_CPC_H__
#define __AMSTRAD_CPC_H__

#include <glib.h>
#include <sys/time.h>
#include <xcpc/blitter/blitter.h>
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

typedef struct _AMSTRAD_CPC_SETTINGS AMSTRAD_CPC_SETTINGS;
typedef struct _AMSTRAD_CPC_EMULATOR AMSTRAD_CPC_EMULATOR;

typedef void (*PaintProc)(AMSTRAD_CPC_EMULATOR* self);
typedef void (*KeybdProc)(AMSTRAD_CPC_EMULATOR* self, XEvent* xevent);
typedef void (*MouseProc)(AMSTRAD_CPC_EMULATOR* self, XEvent* xevent);

struct _AMSTRAD_CPC_SETTINGS
{
    gboolean turbo;
    gboolean no_xshm;
    gboolean show_fps;
    gchar*   computer_model;
    gchar*   monitor_model;
    gchar*   keyboard_layout;
    gchar*   refresh_rate;
    gchar*   manufacturer;
    gchar*   snapshot;
    gchar*   system_rom;
    gchar*   expansion[256];
};

struct _AMSTRAD_CPC_EMULATOR
{
    AMSTRAD_CPC_SETTINGS* settings;
    XcpcBlitter*  blitter;
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
    struct _memory {
        struct {
            unsigned char* bank[4];
        } rd;
        struct {
            unsigned char* bank[4];
        } wr;
        struct {
            unsigned char config;
        } ram;
        struct {
            unsigned char config;
        } rom;
    } memory;
    struct _scanline {
        unsigned int  mode;
        unsigned long ink[17];
    } scanline[312];
    int cur_scanline;
    struct _paint {
        PaintProc proc;
    } paint;
    struct _keybd {
        KeybdProc proc;
    } keybd;
    struct _mouse {
        MouseProc proc;
    } mouse;
    struct _timer {
        struct timeval deadline;
        struct timeval profiler;
    } timer;
    struct _frame {
        unsigned int rate;
        unsigned int time;
        unsigned int count;
        unsigned int drawn;
    } frame;
    int  ramsize;
    int  computer_model;
    int  monitor_model;
    int  keyboard_layout;
    int  refresh_rate;
    int  manufacturer;
    int  cpu_period;
    char stats[256];
};

extern AMSTRAD_CPC_EMULATOR amstrad_cpc;

extern int           amstrad_cpc_parse         (int* argc, char*** argv);
extern void          amstrad_cpc_start         (AMSTRAD_CPC_EMULATOR* amstrad_cpc);
extern void          amstrad_cpc_close         (AMSTRAD_CPC_EMULATOR* amstrad_cpc);
extern void          amstrad_cpc_reset         (AMSTRAD_CPC_EMULATOR* amstrad_cpc);

extern void          amstrad_cpc_load_snapshot (AMSTRAD_CPC_EMULATOR* amstrad_cpc, const char* filename);
extern void          amstrad_cpc_save_snapshot (AMSTRAD_CPC_EMULATOR* amstrad_cpc, const char* filename);

extern void          amstrad_cpc_insert_drive0 (AMSTRAD_CPC_EMULATOR* amstrad_cpc, const char* filename);
extern void          amstrad_cpc_remove_drive0 (AMSTRAD_CPC_EMULATOR* amstrad_cpc);
extern void          amstrad_cpc_insert_drive1 (AMSTRAD_CPC_EMULATOR* amstrad_cpc, const char* filename);
extern void          amstrad_cpc_remove_drive1 (AMSTRAD_CPC_EMULATOR* amstrad_cpc);

extern unsigned long amstrad_cpc_create_proc   (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_destroy_proc  (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_realize_proc  (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_resize_proc   (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_redraw_proc   (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_timer_proc    (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);
extern unsigned long amstrad_cpc_input_proc    (Widget widget, AMSTRAD_CPC_EMULATOR* amstrad_cpc, XEvent* event);

#ifdef __cplusplus
}
#endif

#endif /* __AMSTRAD_CPC_H__ */
