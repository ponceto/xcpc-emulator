/*
 * amstrad_cpc.h - Copyright (c) 2001, 2006 Olivier Poncet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __AMSTRAD_CPC_H__
#define __AMSTRAD_CPC_H__

#include <drv/driver.h>
#include <dev/z80cpu.h>
#include <dev/garray.h>
#include <dev/cpcmem.h>
#include <dev/mc6845.h>
#include <dev/ay8910.h>
#include <dev/fdd765.h>
#include <dev/upd765.h>
#include <dev/i8255.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AMSTRAD_CPC AMSTRAD_CPC;

struct _AMSTRAD_CPC {
  GdevZ80CPU *z80cpu;
  GdevGArray *garray;
  GdevMC6845 *mc6845;
  GdevAY8910 *ay8910;
  GdevUPD765 *upd765;
  GdevI8255  *i8255;
  guint8 *rd_bank[4];
  guint8 *wr_bank[4];
  struct {
    guint8     *total_ram;
    GdevCPCMEM *lower_rom;
    GdevCPCMEM *upper_rom;
    GdevCPCMEM *expan_rom[256];
    guint8      expansion;
  } memory;
  struct {
    guint8 mods;
    guint8 line;
    guint8 bits[16];
  } keyboard;
  struct _scanline {
    unsigned int mode;
    unsigned long ink[17];
  } scanline[312];
  struct {
    int x, y;
  } beam;
  Screen *screen;
  Window  window;
  XImage *ximage;
  Bool    useshm;
  struct timeval timer1;
  struct timeval timer2;
  unsigned long palette[32];
  void (*keybd_hnd)(AMSTRAD_CPC *self, XEvent *xevent);
  void (*mouse_hnd)(AMSTRAD_CPC *self, XEvent *xevent);
  void (*paint_hnd)(AMSTRAD_CPC *self, XtPointer data);
  int ramsize;
  int refresh;
  int firmname;
  int cpu_period;
};

extern AMSTRAD_CPC amstrad_cpc;

extern void amstrad_cpc_reset(void);
extern int  amstrad_cpc_parse(int *argc, char ***argv);
extern void amstrad_cpc_load_snapshot(char *filename);
extern void amstrad_cpc_save_snapshot(char *filename);

extern void amstrad_cpc_start_handler(Widget widget, XtPointer data);
extern void amstrad_cpc_clock_handler(Widget widget, XtPointer data);
extern void amstrad_cpc_close_handler(Widget widget, XtPointer data);
extern void amstrad_cpc_keybd_handler(Widget widget, XEvent *xevent);
extern void amstrad_cpc_mouse_handler(Widget widget, XEvent *xevent);
extern void amstrad_cpc_paint_handler(Widget widget, XEvent *xevent);

#ifdef __cplusplus
}
#endif

#endif
