/*
 * amstrad_cpc.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <Xem/Emulator.h>
#include "common.h"
#include "xcpc.h"
#include "cpu_z80.h"
#include "crtc_6845.h"
#include "ay_3_8910.h"
#include "ppi_8255.h"
#include "fdc_765.h"
#include "amstrad_cpc.h"

static AMSTRAD_CPC_CFG cfg = {
  AMSTRAD_CPC_6128,       /* version           */
  AMSTRAD_CPC_QWERTY,     /* keyboard          */
  AMSTRAD_CPC_CTM644,     /* monitor           */
  AMSTRAD_CPC_4MHZ,       /* clock             */
  AMSTRAD_CPC_PRESENT,    /* expansion         */
  AMSTRAD_CPC_50HZ,       /* framerate         */
  AMSTRAD_CPC_AMSTRAD,    /* manufacturer      */
  768,                    /* width             */
  576,                    /* height            */
  128,                    /* ramsize           */
  ROMSDIR "/cpc6128.rom", /* system rom        */
  NULL,                   /* expansion rom #1  */
  NULL,                   /* expansion rom #2  */
  NULL,                   /* expansion rom #3  */
  NULL,                   /* expansion rom #4  */
  NULL,                   /* expansion rom #5  */
  NULL,                   /* expansion rom #6  */
  ROMSDIR "/amsdos.rom",  /* expansion rom #7  */
};

AMSTRAD_CPC amstrad_cpc;

CPU_Z80 cpu_z80;
CRTC_6845 crtc_6845;
AY_3_8910 ay_3_8910;
PPI_8255 ppi_8255;
FDC_765 fdc_765;

#define SHFT_L_MASK 0x01
#define SHFT_R_MASK 0x02
#define CTRL_L_MASK 0x04
#define CTRL_R_MASK 0x08

static byte *_bank[4];
static byte *_read[4];
static byte *_write[4];

static void amstrad_cpc_rom_select(void)
{
  _read[0] = (!(amstrad_cpc.gate_array.rom_cfg & 0x04) ? amstrad_cpc.memory.lower_rom : _bank[0]);
  if(!(amstrad_cpc.gate_array.rom_cfg & 0x08)) {
    _read[3] = (amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion] == NULL ? amstrad_cpc.memory.upper_rom[0] : amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion]);
  }
  else {
    _read[3] = _bank[3];
  }
}

static void amstrad_cpc_ram_select(void)
{
  switch(amstrad_cpc.gate_array.ram_cfg) {
    case 0x00:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x01:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x02:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x10000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x14000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x18000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x03:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x04:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x10000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x05:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x14000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x06:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x18000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x07:
                 _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _read[1] = _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x1C000;
      _read[2] = _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
                 _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    default:
      (void) fprintf(stderr, "RAM-SELECT: Bad Configuration (%02x) !!\n", amstrad_cpc.gate_array.ram_cfg);
      (void) fflush(stderr);
      break;
  }
  _read[0] = (!(amstrad_cpc.gate_array.rom_cfg & 0x04) ? amstrad_cpc.memory.lower_rom : _bank[0]);
  if(!(amstrad_cpc.gate_array.rom_cfg & 0x08)) {
    _read[3] = (amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion] == NULL ? amstrad_cpc.memory.upper_rom[0] : amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion]);
  }
  else {
    _read[3] = _bank[3];
  }
}

static Screen *_screen = NULL;
static Window  _window = None;
static XImage *_ximage = NULL;
static struct timeval timer1;
static struct timeval timer2;

static unsigned short _palette[32][3] = {
  { 0x7F7F, 0x7F7F, 0x7F7F }, /* White                        */
  { 0x7F7F, 0x7F7F, 0x7F7F }, /* White (not official)         */
  { 0x0000, 0xFFFF, 0x7F7F }, /* Sea Green                    */
  { 0xFFFF, 0xFFFF, 0x7F7F }, /* Pastel Yellow                */
  { 0x0000, 0x0000, 0x7F7F }, /* Blue                         */
  { 0xFFFF, 0x0000, 0x7F7F }, /* Purple                       */
  { 0x0000, 0x7F7F, 0x7F7F }, /* Cyan                         */
  { 0xFFFF, 0x7F7F, 0x7F7F }, /* Pink                         */
  { 0xFFFF, 0x0000, 0x7F7F }, /* Purple (not official)        */
  { 0xFFFF, 0xFFFF, 0x7F7F }, /* Pastel Yellow (not official) */
  { 0xFFFF, 0xFFFF, 0x0000 }, /* Bright Yellow                */
  { 0xFFFF, 0xFFFF, 0xFFFF }, /* Bright White                 */
  { 0xFFFF, 0x0000, 0x0000 }, /* Bright Red                   */
  { 0xFFFF, 0x0000, 0xFFFF }, /* Bright Magenta               */
  { 0xFFFF, 0x7F7F, 0x0000 }, /* Orange                       */
  { 0xFFFF, 0x7F7F, 0xFFFF }, /* Pastel Magenta               */
  { 0x0000, 0x0000, 0x7F7F }, /* Blue (not official)          */
  { 0x0000, 0xFFFF, 0x7F7F }, /* Sea Green (not official)     */
  { 0x0000, 0xFFFF, 0x0000 }, /* Bright Green                 */
  { 0x0000, 0xFFFF, 0xFFFF }, /* Bright Cyan                  */
  { 0x0000, 0x0000, 0x0000 }, /* Black                        */
  { 0x0000, 0x0000, 0xFFFF }, /* Bright Blue                  */
  { 0x0000, 0x7F7F, 0x0000 }, /* Green                        */
  { 0x0000, 0x7F7F, 0xFFFF }, /* Sky Blue                     */
  { 0x7F7F, 0x0000, 0x7F7F }, /* Magenta                      */
  { 0x7F7F, 0xFFFF, 0x7F7F }, /* Pastel Green                 */
  { 0x7F7F, 0xFFFF, 0x0000 }, /* Lime                         */
  { 0x7F7F, 0xFFFF, 0xFFFF }, /* Pastel Cyan                  */
  { 0x7F7F, 0x0000, 0x0000 }, /* Red                          */
  { 0x7F7F, 0x0000, 0xFFFF }, /* Mauve                        */
  { 0x7F7F, 0x7F7F, 0x0000 }, /* Yellow                       */
  { 0x7F7F, 0x7F7F, 0xFFFF }  /* Pastel Blue                  */
};

static unsigned long _col[32] = {
  0L, /* color 0  */
  0L, /* color 1  */
  0L, /* color 2  */
  0L, /* color 3  */
  0L, /* color 4  */
  0L, /* color 5  */
  0L, /* color 6  */
  0L, /* color 7  */
  0L, /* color 8  */
  0L, /* color 9  */
  0L, /* color 10 */
  0L, /* color 11 */
  0L, /* color 12 */
  0L, /* color 13 */
  0L, /* color 14 */
  0L, /* color 15 */
  0L, /* color 16 */
  0L, /* color 17 */
  0L, /* color 18 */
  0L, /* color 19 */
  0L, /* color 20 */
  0L, /* color 21 */
  0L, /* color 22 */
  0L, /* color 23 */
  0L, /* color 24 */
  0L, /* color 25 */
  0L, /* color 26 */
  0L, /* color 27 */
  0L, /* color 28 */
  0L, /* color 29 */
  0L, /* color 30 */
  0L  /* color 31 */
};

static void amstrad_cpc_init_palette(void)
{
int ix;
XColor xcolor;

  xcolor.flags = DoRed | DoGreen | DoBlue;
  xcolor.pad = 0x00;
  for(ix = 0; ix < 32; ix++) {
    if(cfg.monitor == AMSTRAD_CPC_GT65) {
      xcolor.red = 0x0000;
      xcolor.green = (_palette[ix][0] + _palette[ix][1] + _palette[ix][2]) / 3;
      xcolor.blue = 0x0000;
    }
    else {
      xcolor.red = _palette[ix][0];
      xcolor.green = _palette[ix][1];
      xcolor.blue = _palette[ix][2];
    }
    if(XAllocColor(DisplayOfScreen(_screen), DefaultColormapOfScreen(_screen), &xcolor) == False) {
      fprintf(stderr, "cannot allocate color ... %04x/%04x/%04x\n", xcolor.red, xcolor.green, xcolor.blue);
    }
    _col[ix] = xcolor.pixel;
  }
}

static byte _mode0[256] = {
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F
};

static byte _mode1[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

static byte _mode2[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

static void amstrad_cpc_redraw_0(void)
{
}

static void amstrad_cpc_redraw_8(void)
{
  byte *src = _write[crtc_6845.reg_file[12] >> 4];
  word base = 0x0000;
  word offset = ((crtc_6845.reg_file[12] << 8) | crtc_6845.reg_file[13]) << 1;
  unsigned int hd = crtc_6845.reg_file[1] << 1;
  unsigned int hp = (cfg.width - (hd << 3)) >> 1;
  unsigned int mr = crtc_6845.reg_file[9] + 1;
  unsigned int mask = (mr << 11) - 1;
  unsigned int vd = crtc_6845.reg_file[6];
  unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
  unsigned int cx, cy, cxx, cyy;
  unsigned int color, border;
  unsigned char *dst = (unsigned char *) _ximage->data;
  unsigned char *nxt = (unsigned char *) _ximage->data;
  int scanline = 20;
  byte value;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  for(cy = 0; cy < vd; cy++) {
    for(cyy = 0; cyy < mr; cyy++) {
      nxt += cfg.width;
      border = amstrad_cpc.scanline[scanline].ink[16];
      switch(amstrad_cpc.scanline[scanline].mode) {
        case 0x00:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x01:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x02:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
      }
      dst = nxt;
      scanline++;
      base = (base + 2048) & mask;
    }
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  if(_window != None) {
    XPutImage(DisplayOfScreen(_screen), _window, DefaultGCOfScreen(_screen), _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
    XFlush(DisplayOfScreen(_screen));
  }
}

static void amstrad_cpc_redraw_16(void)
{
  byte *src = _write[crtc_6845.reg_file[12] >> 4];
  word base = 0x0000;
  word offset = ((crtc_6845.reg_file[12] << 8) | crtc_6845.reg_file[13]) << 1;
  unsigned int hd = crtc_6845.reg_file[1] << 1;
  unsigned int hp = (cfg.width - (hd << 3)) >> 1;
  unsigned int mr = crtc_6845.reg_file[9] + 1;
  unsigned int mask = (mr << 11) - 1;
  unsigned int vd = crtc_6845.reg_file[6];
  unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
  unsigned int cx, cy, cxx, cyy;
  unsigned int color, border;
  unsigned short *dst = (unsigned short *) _ximage->data;
  unsigned short *nxt = (unsigned short *) _ximage->data;
  int scanline = 20;
  byte value;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  for(cy = 0; cy < vd; cy++) {
    for(cyy = 0; cyy < mr; cyy++) {
      nxt += cfg.width;
      border = amstrad_cpc.scanline[scanline].ink[16];
      switch(amstrad_cpc.scanline[scanline].mode) {
        case 0x00:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x01:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x02:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
      }
      dst = nxt;
      scanline++;
      base = (base + 2048) & mask;
    }
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  if(_window != None) {
    XPutImage(DisplayOfScreen(_screen), _window, DefaultGCOfScreen(_screen), _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
    XFlush(DisplayOfScreen(_screen));
  }
}

static void amstrad_cpc_redraw_32(void)
{
  byte *src = _write[crtc_6845.reg_file[12] >> 4];
  word base = 0x0000;
  word offset = ((crtc_6845.reg_file[12] << 8) | crtc_6845.reg_file[13]) << 1;
  unsigned int hd = crtc_6845.reg_file[1] << 1;
  unsigned int hp = (cfg.width - (hd << 3)) >> 1;
  unsigned int mr = crtc_6845.reg_file[9] + 1;
  unsigned int mask = (mr << 11) - 1;
  unsigned int vd = crtc_6845.reg_file[6];
  unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
  unsigned int cx, cy, cxx, cyy;
  unsigned int color, border;
  unsigned int *dst = (unsigned int *) _ximage->data;
  unsigned int *nxt = (unsigned int *) _ximage->data;
  int scanline = 20;
  byte value;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  for(cy = 0; cy < vd; cy++) {
    for(cyy = 0; cyy < mr; cyy++) {
      nxt += cfg.width;
      border = amstrad_cpc.scanline[scanline].ink[16];
      switch(amstrad_cpc.scanline[scanline].mode) {
        case 0x00:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x01:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
        case 0x02:
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = amstrad_cpc.scanline[scanline].ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          break;
      }
      dst = nxt;
      scanline++;
      base = (base + 2048) & mask;
    }
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    border = amstrad_cpc.scanline[scanline].ink[16];
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
    scanline++;
  }
  if(_window != None) {
    XPutImage(DisplayOfScreen(_screen), _window, DefaultGCOfScreen(_screen), _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
    XFlush(DisplayOfScreen(_screen));
  }
}

static void (*amstrad_cpc_redraw)(void) = amstrad_cpc_redraw_0;

static void amstrad_cpc_decode_qwerty(Widget widget, XEvent *xevent)
{
  char buffer[8];
  KeySym keysym;
  byte line = 0x09;
  byte mask = 0x40;
  byte mods = 0x00;

  if((amstrad_cpc.keyboard.mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((amstrad_cpc.keyboard.mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, buffer, 8, &keysym, NULL);
  if((keysym >> 8) == 0x00) {
    switch(keysym) {
      case XK_space:
        line = 0x05; mask = 0x80;
        break;
      case XK_exclam:
        mods |= SHFT_L_MASK;
      case XK_1:
        line = 0x08; mask = 0x01;
        break;
      case XK_asciitilde:
        mods |= CTRL_L_MASK;
      case XK_quotedbl:
        mods |= SHFT_L_MASK;
      case XK_2:
        line = 0x08; mask = 0x02;
        break;
      case XK_numbersign:
        mods |= SHFT_L_MASK;
      case XK_3:
        line = 0x07; mask = 0x02;
        break;
      case XK_dollar:
        mods |= SHFT_L_MASK;
      case XK_4:
        line = 0x07; mask = 0x01;
        break;
      case XK_percent:
        mods |= SHFT_L_MASK;
      case XK_5:
        line = 0x06; mask = 0x02;
        break;
      case XK_ampersand:
        mods |= SHFT_L_MASK;
      case XK_6:
        line = 0x06; mask = 0x01;
        break;
      case XK_apostrophe:
        mods |= SHFT_L_MASK;
      case XK_7:
        line = 0x05; mask = 0x02;
        break;
      case XK_parenleft:
        mods |= SHFT_L_MASK;
      case XK_8:
        line = 0x05; mask = 0x01;
        break;
      case XK_parenright:
        mods |= SHFT_L_MASK;
      case XK_9:
        line = 0x04; mask = 0x02;
        break;
      case XK_underscore:
        mods |= SHFT_L_MASK;
      case XK_0:
        line = 0x04; mask = 0x01;
        break;
      case XK_equal:
        mods |= SHFT_L_MASK;
      case XK_minus:
        line = 0x03; mask = 0x02;
        break;
      case XK_sterling:
        mods |= SHFT_L_MASK;
      case XK_asciicircum:
        line = 0x03; mask = 0x01;
        break;
      case XK_Q:
        mods |= SHFT_L_MASK;
      case XK_q:
        line = 0x08; mask = 0x08;
        break;
      case XK_W:
        mods |= SHFT_L_MASK;
      case XK_w:
        line = 0x07; mask = 0x08;
        break;
      case XK_E:
        mods |= SHFT_L_MASK;
      case XK_e:
        line = 0x07; mask = 0x04;
        break;
      case XK_R:
        mods |= SHFT_L_MASK;
      case XK_r:
        line = 0x06; mask = 0x04;
        break;
      case XK_T:
        mods |= SHFT_L_MASK;
      case XK_t:
        line = 0x06; mask = 0x08;
        break;
      case XK_Y:
        mods |= SHFT_L_MASK;
      case XK_y:
        line = 0x05; mask = 0x08;
        break;
      case XK_U:
        mods |= SHFT_L_MASK;
      case XK_u:
        line = 0x05; mask = 0x04;
        break;
      case XK_I:
        mods |= SHFT_L_MASK;
      case XK_i:
        line = 0x04; mask = 0x08;
        break;
      case XK_O:
        mods |= SHFT_L_MASK;
      case XK_o:
        line = 0x04; mask = 0x04;
        break;
      case XK_P:
        mods |= SHFT_L_MASK;
      case XK_p:
        line = 0x03; mask = 0x08;
        break;
      case XK_bar:
        mods |= SHFT_L_MASK;
      case XK_at:
        line = 0x03; mask = 0x04;
        break;
      case XK_braceleft:
        mods |= SHFT_L_MASK;
      case XK_bracketleft:
        line = 0x02; mask = 0x02;
        break;
      case XK_A:
        mods |= SHFT_L_MASK;
      case XK_a:
        line = 0x08; mask = 0x20;
        break;
      case XK_S:
        mods |= SHFT_L_MASK;
      case XK_s:
        line = 0x07; mask = 0x10;
        break;
      case XK_D:
        mods |= SHFT_L_MASK;
      case XK_d:
        line = 0x07; mask = 0x20;
        break;
      case XK_F:
        mods |= SHFT_L_MASK;
      case XK_f:
        line = 0x06; mask = 0x20;
        break;
      case XK_G:
        mods |= SHFT_L_MASK;
      case XK_g:
        line = 0x06; mask = 0x10;
        break;
      case XK_H:
        mods |= SHFT_L_MASK;
      case XK_h:
        line = 0x05; mask = 0x10;
        break;
      case XK_J:
        mods |= SHFT_L_MASK;
      case XK_j:
        line = 0x05; mask = 0x20;
        break;
      case XK_K:
        mods |= SHFT_L_MASK;
      case XK_k:
        line = 0x04; mask = 0x20;
        break;
      case XK_L:
        mods |= SHFT_L_MASK;
      case XK_l:
        line = 0x04; mask = 0x10;
        break;
      case XK_asterisk:
        mods |= SHFT_L_MASK;
      case XK_colon:
        line = 0x03; mask = 0x20;
        break;
      case XK_plus:
        mods |= SHFT_L_MASK;
      case XK_semicolon:
        line = 0x03; mask = 0x10;
        break;
      case XK_braceright:
        mods |= SHFT_L_MASK;
      case XK_bracketright:
        line = 0x02; mask = 0x08;
        break;
      case XK_Z:
        mods |= SHFT_L_MASK;
      case XK_z:
        line = 0x08; mask = 0x80;
        break;
      case XK_X:
        mods |= SHFT_L_MASK;
      case XK_x:
        line = 0x07; mask = 0x80;
        break;
      case XK_C:
        mods |= SHFT_L_MASK;
      case XK_c:
        line = 0x07; mask = 0x40;
        break;
      case XK_V:
        mods |= SHFT_L_MASK;
      case XK_v:
        line = 0x06; mask = 0x80;
        break;
      case XK_B:
        mods |= SHFT_L_MASK;
      case XK_b:
        line = 0x06; mask = 0x40;
        break;
      case XK_N:
        mods |= SHFT_L_MASK;
      case XK_n:
        line = 0x05; mask = 0x40;
        break;
      case XK_M:
        mods |= SHFT_L_MASK;
      case XK_m:
        line = 0x04; mask = 0x40;
        break;
      case XK_less:
        mods |= SHFT_L_MASK;
      case XK_comma:
        line = 0x04; mask = 0x80;
        break;
      case XK_greater:
        mods |= SHFT_L_MASK;
      case XK_period:
        line = 0x03; mask = 0x80;
        break;
      case XK_question:
        mods |= SHFT_L_MASK;
      case XK_slash:
        line = 0x03; mask = 0x40;
        break;
      case XK_grave:
        mods |= SHFT_L_MASK;
      case XK_backslash:
        line = 0x02; mask = 0x40;
        break;
    }
  }
  else if((keysym >> 8) == 0xff) {
    switch(keysym) {
      case XK_BackSpace:
        line = 0x09; mask = 0x80;
        break;
      case XK_Tab:
        line = 0x08; mask = 0x10;
        break;
      case XK_Return:
        line = 0x02; mask = 0x04;
        break;
      case XK_Escape:
        line = 0x08; mask = 0x04;
        break;
      case XK_Delete:
        line = 0x02; mask = 0x01;
        break;
      case XK_Shift_L:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  SHFT_L_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  SHFT_R_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  CTRL_L_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  CTRL_R_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~CTRL_R_MASK;
        }
        break;
      case XK_Alt_L:
        line = 0x01; mask = 0x02;
        break;
      case XK_Caps_Lock:
        line = 0x08; mask = 0x40;
        break;
      case XK_Up:
        line = 0x00; mask = 0x01;
        break;
      case XK_Down:
        line = 0x00; mask = 0x04;
        break;
      case XK_Left:
        line = 0x01; mask = 0x01;
        break;
      case XK_Right:
        line = 0x00; mask = 0x02;
        break;
      case XK_KP_Up:
        line = 0x09; mask = 0x01;
        break;
      case XK_KP_Down:
        line = 0x09; mask = 0x02;
        break;
      case XK_KP_Left:
        line = 0x09; mask = 0x04;
        break;
      case XK_KP_Right:
        line = 0x09; mask = 0x08;
        break;
      case XK_KP_Insert:
        line = 0x09; mask = 0x10;
        break;
      case XK_KP_Delete:
        line = 0x09; mask = 0x20;
        break;
      case XK_KP_Enter:
        line = 0x00; mask = 0x40;
        break;
      case XK_KP_Decimal:
        line = 0x00; mask = 0x80;
        break;
      case XK_KP_0:
        line = 0x01; mask = 0x80;
        break;
      case XK_KP_1:
        line = 0x01; mask = 0x20;
        break;
      case XK_KP_2:
        line = 0x01; mask = 0x40;
        break;
      case XK_KP_3:
        line = 0x00; mask = 0x20;
        break;
      case XK_KP_4:
        line = 0x02; mask = 0x10;
        break;
      case XK_KP_5:
        line = 0x01; mask = 0x10;
        break;
      case XK_KP_6:
        line = 0x00; mask = 0x10;
        break;
      case XK_KP_7:
        line = 0x01; mask = 0x04;
        break;
      case XK_KP_8:
        line = 0x01; mask = 0x08;
        break;
      case XK_KP_9:
        line = 0x00; mask = 0x08;
        break;
    }
  }
  if((amstrad_cpc.keyboard.mods & SHFT_L_MASK) != 0) {
    amstrad_cpc.keyboard.bits[0x02] &= ~0x20;
  }
  else {
    amstrad_cpc.keyboard.bits[0x02] |=  0x20;
  }
  if((amstrad_cpc.keyboard.mods & CTRL_L_MASK) != 0) {
    amstrad_cpc.keyboard.bits[0x02] &= ~0x80;
  }
  else {
    amstrad_cpc.keyboard.bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      amstrad_cpc.keyboard.bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      amstrad_cpc.keyboard.bits[0x02] &= ~0x80;
    }
    amstrad_cpc.keyboard.bits[line] &= ~mask;
  }
  else {
    amstrad_cpc.keyboard.bits[line] |=  mask;
  }
}

static void amstrad_cpc_decode_azerty(Widget widget, XEvent *xevent)
{
  char buffer[8];
  KeySym keysym;
  byte line = 0x09;
  byte mask = 0x40;
  byte mods = 0x00;

  if((amstrad_cpc.keyboard.mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((amstrad_cpc.keyboard.mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, buffer, 8, &keysym, NULL);
  if((keysym >> 8) == 0x00) {
    switch(keysym) {
      case XK_space:
        line = 0x05; mask = 0x80;
        break;
      case XK_1:
        mods |= SHFT_L_MASK;
      case XK_ampersand:
        line = 0x08; mask = 0x01;
        break;
      case XK_asciitilde:
        mods |= CTRL_L_MASK;
      case XK_2:
        mods |= SHFT_L_MASK;
      case XK_eacute:
        line = 0x08; mask = 0x02;
        break;
      case XK_3:
        mods |= SHFT_L_MASK;
      case XK_quotedbl:
        line = 0x07; mask = 0x02;
        break;
      case XK_4:
        mods |= SHFT_L_MASK;
      case XK_apostrophe:
        line = 0x07; mask = 0x01;
        break;
      case XK_5:
        mods |= SHFT_L_MASK;
      case XK_parenleft:
        line = 0x06; mask = 0x02;
        break;
      case XK_6:
        mods |= SHFT_L_MASK;
      case XK_bracketright:
        line = 0x06; mask = 0x01;
        break;
      case XK_7:
        mods |= SHFT_L_MASK;
      case XK_egrave:
        line = 0x05; mask = 0x02;
        break;
      case XK_8:
        mods |= SHFT_L_MASK;
      case XK_exclam:
        line = 0x05; mask = 0x01;
        break;
      case XK_9:
        mods |= SHFT_L_MASK;
      case XK_ccedilla:
        line = 0x04; mask = 0x02;
        break;
      case XK_0:
        mods |= SHFT_L_MASK;
      case XK_agrave:
        line = 0x04; mask = 0x01;
        break;
      case XK_bracketleft:
        mods |= SHFT_L_MASK;
      case XK_parenright:
        line = 0x03; mask = 0x02;
        break;
      case XK_underscore:
        mods |= SHFT_L_MASK;
      case XK_minus:
        line = 0x03; mask = 0x01;
        break;
      case XK_A:
        mods |= SHFT_L_MASK;
      case XK_a:
        line = 0x08; mask = 0x08;
        break;
      case XK_Z:
        mods |= SHFT_L_MASK;
      case XK_z:
        line = 0x07; mask = 0x08;
        break;
      case XK_E:
        mods |= SHFT_L_MASK;
      case XK_e:
        line = 0x07; mask = 0x04;
        break;
      case XK_R:
        mods |= SHFT_L_MASK;
      case XK_r:
        line = 0x06; mask = 0x04;
        break;
      case XK_T:
        mods |= SHFT_L_MASK;
      case XK_t:
        line = 0x06; mask = 0x08;
        break;
      case XK_Y:
        mods |= SHFT_L_MASK;
      case XK_y:
        line = 0x05; mask = 0x08;
        break;
      case XK_U:
        mods |= SHFT_L_MASK;
      case XK_u:
        line = 0x05; mask = 0x04;
        break;
      case XK_I:
        mods |= SHFT_L_MASK;
      case XK_i:
        line = 0x04; mask = 0x08;
        break;
      case XK_O:
        mods |= SHFT_L_MASK;
      case XK_o:
        line = 0x04; mask = 0x04;
        break;
      case XK_P:
        mods |= SHFT_L_MASK;
      case XK_p:
        line = 0x03; mask = 0x08;
        break;
      case XK_bar:
        mods |= SHFT_L_MASK;
      case XK_asciicircum:
        line = 0x03; mask = 0x04;
        break;
      case XK_less:
        mods |= SHFT_L_MASK;
      case XK_asterisk:
        line = 0x02; mask = 0x02;
        break;
      case XK_Q:
        mods |= SHFT_L_MASK;
      case XK_q:
        line = 0x08; mask = 0x20;
        break;
      case XK_S:
        mods |= SHFT_L_MASK;
      case XK_s:
        line = 0x07; mask = 0x10;
        break;
      case XK_D:
        mods |= SHFT_L_MASK;
      case XK_d:
        line = 0x07; mask = 0x20;
        break;
      case XK_F:
        mods |= SHFT_L_MASK;
      case XK_f:
        line = 0x06; mask = 0x20;
        break;
      case XK_G:
        mods |= SHFT_L_MASK;
      case XK_g:
        line = 0x06; mask = 0x10;
        break;
      case XK_H:
        mods |= SHFT_L_MASK;
      case XK_h:
        line = 0x05; mask = 0x10;
        break;
      case XK_J:
        mods |= SHFT_L_MASK;
      case XK_j:
        line = 0x05; mask = 0x20;
        break;
      case XK_K:
        mods |= SHFT_L_MASK;
      case XK_k:
        line = 0x04; mask = 0x20;
        break;
      case XK_L:
        mods |= SHFT_L_MASK;
      case XK_l:
        line = 0x04; mask = 0x10;
        break;
      case XK_M:
        mods |= SHFT_L_MASK;
      case XK_m:
        line = 0x03; mask = 0x20;
        break;
      case XK_percent:
        mods |= SHFT_L_MASK;
      case XK_ugrave:
        line = 0x03; mask = 0x10;
        break;
      case XK_greater:
        mods |= SHFT_L_MASK;
      case XK_numbersign:
        line = 0x02; mask = 0x08;
        break;
      case XK_W:
        mods |= SHFT_L_MASK;
      case XK_w:
        line = 0x08; mask = 0x80;
        break;
      case XK_X:
        mods |= SHFT_L_MASK;
      case XK_x:
        line = 0x07; mask = 0x80;
        break;
      case XK_C:
        mods |= SHFT_L_MASK;
      case XK_c:
        line = 0x07; mask = 0x40;
        break;
      case XK_V:
        mods |= SHFT_L_MASK;
      case XK_v:
        line = 0x06; mask = 0x80;
        break;
      case XK_B:
        mods |= SHFT_L_MASK;
      case XK_b:
        line = 0x06; mask = 0x40;
        break;
      case XK_N:
        mods |= SHFT_L_MASK;
      case XK_n:
        line = 0x05; mask = 0x40;
        break;
      case XK_question:
        mods |= SHFT_L_MASK;
      case XK_comma:
        line = 0x04; mask = 0x40;
        break;
      case XK_period:
        mods |= SHFT_L_MASK;
      case XK_semicolon:
        line = 0x04; mask = 0x80;
        break;
      case XK_slash:
        mods |= SHFT_L_MASK;
      case XK_colon:
        line = 0x03; mask = 0x80;
        break;
      case XK_plus:
        mods |= SHFT_L_MASK;
      case XK_equal:
        line = 0x03; mask = 0x40;
        break;
      case XK_at:
        mods |= SHFT_L_MASK;
      case XK_dollar:
        line = 0x02; mask = 0x40;
        break;
    }
  }
  else if((keysym >> 8) == 0xff) {
    switch(keysym) {
      case XK_BackSpace:
        line = 0x09; mask = 0x80;
        break;
      case XK_Tab:
        line = 0x08; mask = 0x10;
        break;
      case XK_Return:
        line = 0x02; mask = 0x04;
        break;
      case XK_Escape:
        line = 0x08; mask = 0x04;
        break;
      case XK_Delete:
        line = 0x02; mask = 0x01;
        break;
      case XK_Shift_L:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  SHFT_L_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  SHFT_R_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  CTRL_L_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          amstrad_cpc.keyboard.mods |=  CTRL_R_MASK;
        }
        else {
          amstrad_cpc.keyboard.mods &= ~CTRL_R_MASK;
        }
        break;
      case XK_Alt_L:
        line = 0x01; mask = 0x02;
        break;
      case XK_Caps_Lock:
        line = 0x08; mask = 0x40;
        break;
      case XK_Up:
        line = 0x00; mask = 0x01;
        break;
      case XK_Down:
        line = 0x00; mask = 0x04;
        break;
      case XK_Left:
        line = 0x01; mask = 0x01;
        break;
      case XK_Right:
        line = 0x00; mask = 0x02;
        break;
      case XK_KP_Up:
        line = 0x09; mask = 0x01;
        break;
      case XK_KP_Down:
        line = 0x09; mask = 0x02;
        break;
      case XK_KP_Left:
        line = 0x09; mask = 0x04;
        break;
      case XK_KP_Right:
        line = 0x09; mask = 0x08;
        break;
      case XK_KP_Insert:
        line = 0x09; mask = 0x10;
        break;
      case XK_KP_Delete:
        line = 0x09; mask = 0x20;
        break;
      case XK_KP_Enter:
        line = 0x00; mask = 0x40;
        break;
      case XK_KP_Decimal:
        line = 0x00; mask = 0x80;
        break;
      case XK_KP_0:
        line = 0x01; mask = 0x80;
        break;
      case XK_KP_1:
        line = 0x01; mask = 0x20;
        break;
      case XK_KP_2:
        line = 0x01; mask = 0x40;
        break;
      case XK_KP_3:
        line = 0x00; mask = 0x20;
        break;
      case XK_KP_4:
        line = 0x02; mask = 0x10;
        break;
      case XK_KP_5:
        line = 0x01; mask = 0x10;
        break;
      case XK_KP_6:
        line = 0x00; mask = 0x10;
        break;
      case XK_KP_7:
        line = 0x01; mask = 0x04;
        break;
      case XK_KP_8:
        line = 0x01; mask = 0x08;
        break;
      case XK_KP_9:
        line = 0x00; mask = 0x08;
        break;
    }
  }
  if((amstrad_cpc.keyboard.mods & SHFT_L_MASK) != 0) {
    amstrad_cpc.keyboard.bits[0x02] &= ~0x20;
  }
  else {
    amstrad_cpc.keyboard.bits[0x02] |=  0x20;
  }
  if((amstrad_cpc.keyboard.mods & CTRL_L_MASK) != 0) {
    amstrad_cpc.keyboard.bits[0x02] &= ~0x80;
  }
  else {
    amstrad_cpc.keyboard.bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      amstrad_cpc.keyboard.bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      amstrad_cpc.keyboard.bits[0x02] &= ~0x80;
    }
    amstrad_cpc.keyboard.bits[line] &= ~mask;
  }
  else {
    amstrad_cpc.keyboard.bits[line] |=  mask;
  }
}

void amstrad_cpc_reset(void)
{
int ix;

  amstrad_cpc.memory.expansion = 0x00;
  amstrad_cpc.keyboard.mods = 0x00;
  amstrad_cpc.keyboard.line = 0x00;
  for(ix = 0; ix < 16; ix++) {
    amstrad_cpc.keyboard.bits[ix] = 0xFF;
  }
  amstrad_cpc.gate_array.pen = 0x00;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = 0x00;
  }
  amstrad_cpc.gate_array.rom_cfg = 0x00;
  amstrad_cpc.gate_array.ram_cfg = 0x00;
  amstrad_cpc.gate_array.counter = 0x00;
  amstrad_cpc.gate_array.set_irq = 0x00;
  amstrad_cpc_ram_select();
  amstrad_cpc.beam.x = 0;
  amstrad_cpc.beam.y = 0;
  cpu_z80_reset(&cpu_z80);
  crtc_6845_reset(&crtc_6845);
  ay_3_8910_reset(&ay_3_8910);
  ppi_8255_reset(&ppi_8255);
  fdc_765_reset(&fdc_765);
  /* XXX */
  (void) gettimeofday(&timer1, NULL);
  (void) gettimeofday(&timer2, NULL);
}

int amstrad_cpc_parse(int argc, char *argv[])
{
  while(--argc) {
    argv++;
    if(!strcmp("-cpc464", *argv)) {
      cfg.version = AMSTRAD_CPC_464;
      cfg.keyboard = AMSTRAD_CPC_QWERTY;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_NOT_PRESENT;
      cfg.framerate = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 64;
      cfg.rom[0x00] = ROMSDIR "/cpc464.rom";
      cfg.rom[0x07] = NULL;
    }
    else if(!strcmp("-cpc664", *argv)) {
      cfg.version = AMSTRAD_CPC_664;
      cfg.keyboard = AMSTRAD_CPC_QWERTY;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.framerate = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 64;
      cfg.rom[0x00] = ROMSDIR "/cpc664.rom";
      cfg.rom[0x07] = ROMSDIR "/amsdos.rom";
    }
    else if(!strcmp("-cpc6128", *argv)) {
      cfg.version = AMSTRAD_CPC_6128;
      cfg.keyboard = AMSTRAD_CPC_QWERTY;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.framerate = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 128;
      cfg.rom[0x00] = ROMSDIR "/cpc6128.rom";
      cfg.rom[0x07] = ROMSDIR "/amsdos.rom";
    }
    else if(!strcmp("-4MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_4MHZ;
    }
    else if(!strcmp("-8MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_8MHZ;
    }
    else if(!strcmp("-12MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_12MHZ;
    }
    else if(!strcmp("-16MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_16MHZ;
    }
    else if(!strcmp("-50Hz", *argv)) {
      cfg.framerate = AMSTRAD_CPC_50HZ;
    }
    else if(!strcmp("-60Hz", *argv)) {
      cfg.framerate = AMSTRAD_CPC_60HZ;
    }
    else if(!strcmp("-isp", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_ISP;
    }
    else if(!strcmp("-triumph", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_TRIUMPH;
    }
    else if(!strcmp("-saisho", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SAISHO;
    }
    else if(!strcmp("-solavox", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SOLAVOX;
    }
    else if(!strcmp("-awa", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_AWA;
    }
    else if(!strcmp("-schneider", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SCHNEIDER;
    }
    else if(!strcmp("-orion", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_ORION;
    }
    else if(!strcmp("-amstrad", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
    }
    else if(!strcmp("-GT65", *argv)) {
      cfg.monitor = AMSTRAD_CPC_GT65;
    }
    else if(!strcmp("-CTM644", *argv)) {
      cfg.monitor = AMSTRAD_CPC_CTM644;
    }
    else if(!strcmp("-tiny", *argv)) {
      cfg.width = 656;
      cfg.height = 416;
    }
    else if(!strcmp("-small", *argv)) {
      cfg.width = 680;
      cfg.height = 456;
    }
    else if(!strcmp("-medium", *argv)) {
      cfg.width = 720;
      cfg.height = 512;
    }
    else if(!strcmp("-big", *argv)) {
      cfg.width = 760;
      cfg.height = 568;
    }
    else if(!strcmp("-huge", *argv)) {
      cfg.width = 800;
      cfg.height = 624;
    }
    else {
      return(EXIT_FAILURE);
    }
  }
  return(EXIT_SUCCESS);
}

void amstrad_cpc_load_snapshot(char *filename)
{
FILE *file;
byte buffer[256], *bufptr = buffer;
int ix;
int ramsize;

  if((file = fopen(filename, "r")) == NULL) {
    perror("amstrad_cpc");
    return;
  }
  fread(buffer, 1, 256, file);
  if(memcmp(bufptr, "MV - SNA", 8)) {
    fprintf(stderr, "not a valid snapshot file (bad signature)\n");
    fclose(file);
    return;
  } bufptr += 8;
  bufptr += 8; /* not used */
  bufptr++; /* snapshot version */
  cpu_z80.AF.B.l = *bufptr++;
  cpu_z80.AF.B.h = *bufptr++;
  cpu_z80.BC.B.l = *bufptr++;
  cpu_z80.BC.B.h = *bufptr++;
  cpu_z80.DE.B.l = *bufptr++;
  cpu_z80.DE.B.h = *bufptr++;
  cpu_z80.HL.B.l = *bufptr++;
  cpu_z80.HL.B.h = *bufptr++;
  cpu_z80.IR.B.l = *bufptr++;
  cpu_z80.IR.B.h = *bufptr++;
  cpu_z80.IFF = (*bufptr++ != 0 ? cpu_z80.IFF | IFF_1 : cpu_z80.IFF & (~IFF_2));
  cpu_z80.IFF = (*bufptr++ != 0 ? cpu_z80.IFF | IFF_1 : cpu_z80.IFF & (~IFF_2));
  cpu_z80.IX.B.l = *bufptr++;
  cpu_z80.IX.B.h = *bufptr++;
  cpu_z80.IY.B.l = *bufptr++;
  cpu_z80.IY.B.h = *bufptr++;
  cpu_z80.SP.B.l = *bufptr++;
  cpu_z80.SP.B.h = *bufptr++;
  cpu_z80.PC.B.l = *bufptr++;
  cpu_z80.PC.B.h = *bufptr++;
  switch(*bufptr++) {
    case 1:
      cpu_z80.IFF = (cpu_z80.IFF | IFF_IM1) & ~(IFF_IM2);
      break;
    case 2:
      cpu_z80.IFF = (cpu_z80.IFF | IFF_IM2) & ~(IFF_IM1);
      break;
    default:
      cpu_z80.IFF = (cpu_z80.IFF) & ~(IFF_IM1 | IFF_IM2);
      break;
  }
  cpu_z80.AF1.B.l = *bufptr++;
  cpu_z80.AF1.B.h = *bufptr++;
  cpu_z80.BC1.B.l = *bufptr++;
  cpu_z80.BC1.B.h = *bufptr++;
  cpu_z80.DE1.B.l = *bufptr++;
  cpu_z80.DE1.B.h = *bufptr++;
  cpu_z80.HL1.B.l = *bufptr++;
  cpu_z80.HL1.B.h = *bufptr++;
  amstrad_cpc.gate_array.pen = *bufptr++;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = *bufptr++;
  }
  amstrad_cpc.gate_array.rom_cfg = *bufptr++;
  amstrad_cpc.gate_array.ram_cfg = *bufptr++;
  amstrad_cpc_ram_select();
  crtc_6845.addr_reg = *bufptr++;
  for(ix = 0; ix < 18; ix++) {
    crtc_6845.reg_file[ix] = *bufptr++;
  }
  amstrad_cpc.memory.expansion = *bufptr++;
  ppi_8255.port_a = *bufptr++;
  ppi_8255.port_b = *bufptr++;
  ppi_8255.port_c = *bufptr++;
  ppi_8255.control = *bufptr++;
  ay_3_8910.current = *bufptr++;
  for(ix = 0; ix < 16; ix++) {
    ay_3_8910.registers[ix] = *bufptr++;
  }
  ramsize = *bufptr++;
  ramsize |= *bufptr++ << 8;
  if(ramsize > cfg.ramsize) {
    fprintf(stderr, "snapshot file too large (%d Kb)\n", ramsize);
    amstrad_cpc_reset();
    fclose(file);
    return;
  }
  fread(amstrad_cpc.memory.ram, 1, ramsize * 1024, file);
  fclose(file);
}

void amstrad_cpc_save_snapshot(char *filename)
{
FILE *file;
byte buffer[256], *bufptr = buffer;
int ix;
int ramsize;

  if((file = fopen(filename, "w")) == NULL) {
    perror("amstrad_cpc");
    return;
  }
  memcpy(bufptr, "MV - SNA", 8); bufptr += 8;
  memset(bufptr, 0, 8); bufptr += 8; /* not used */
  *bufptr++ = 1; /* snapshot version */
  *bufptr++ = cpu_z80.AF.B.l;
  *bufptr++ = cpu_z80.AF.B.h;
  *bufptr++ = cpu_z80.BC.B.l;
  *bufptr++ = cpu_z80.BC.B.h;
  *bufptr++ = cpu_z80.DE.B.l;
  *bufptr++ = cpu_z80.DE.B.h;
  *bufptr++ = cpu_z80.HL.B.l;
  *bufptr++ = cpu_z80.HL.B.h;
  *bufptr++ = cpu_z80.IR.B.l;
  *bufptr++ = cpu_z80.IR.B.h;
  *bufptr++ = (cpu_z80.IFF & IFF_1 ? 0x01 : 0x00);
  *bufptr++ = (cpu_z80.IFF & IFF_2 ? 0x01 : 0x00);
  *bufptr++ = cpu_z80.IX.B.l;
  *bufptr++ = cpu_z80.IX.B.h;
  *bufptr++ = cpu_z80.IY.B.l;
  *bufptr++ = cpu_z80.IY.B.h;
  *bufptr++ = cpu_z80.SP.B.l;
  *bufptr++ = cpu_z80.SP.B.h;
  *bufptr++ = cpu_z80.PC.B.l;
  *bufptr++ = cpu_z80.PC.B.h;
  switch(cpu_z80.IFF & (IFF_IM1 | IFF_IM2)) {
    case IFF_IM1:
      *bufptr++ = 0x01;
      break;
    case IFF_IM2:
      *bufptr++ = 0x02;
      break;
    default:
      *bufptr++ = 0x00;
      break;
  }
  *bufptr++ = cpu_z80.AF1.B.l;
  *bufptr++ = cpu_z80.AF1.B.h;
  *bufptr++ = cpu_z80.BC1.B.l;
  *bufptr++ = cpu_z80.BC1.B.h;
  *bufptr++ = cpu_z80.DE1.B.l;
  *bufptr++ = cpu_z80.DE1.B.h;
  *bufptr++ = cpu_z80.HL1.B.l;
  *bufptr++ = cpu_z80.HL1.B.h;
  *bufptr++ = amstrad_cpc.gate_array.pen;
  for(ix = 0; ix < 17; ix++) {
    *bufptr++ = amstrad_cpc.gate_array.ink[ix];
  }
  *bufptr++ = amstrad_cpc.gate_array.rom_cfg;
  *bufptr++ = amstrad_cpc.gate_array.ram_cfg;
  *bufptr++ = crtc_6845.addr_reg;
  for(ix = 0; ix < 18; ix++) {
    *bufptr++ = crtc_6845.reg_file[ix];
  }
  *bufptr++ = amstrad_cpc.memory.expansion;
  *bufptr++ = ppi_8255.port_a;
  *bufptr++ = ppi_8255.port_b;
  *bufptr++ = ppi_8255.port_c;
  *bufptr++ = ppi_8255.control;
  *bufptr++ = ay_3_8910.current;
  for(ix = 0; ix < 16; ix++) {
    *bufptr++ = ay_3_8910.registers[ix];
  }
  *bufptr++ = cfg.ramsize & 0xFF;
  *bufptr++ = (cfg.ramsize >> 8) & 0xFF;
  memset(bufptr, 0, 147);
  bufptr += 147;
  fwrite(buffer, 1, 256, file);
  fwrite(amstrad_cpc.memory.ram, 1, cfg.ramsize * 1024, file);
  fclose(file);
}

byte cpu_z80_mm_rd(CPU_Z80 *cpu_z80, word address)
{
  return(_read[address >> 14][address & 0x3FFF]);
}

void cpu_z80_mm_wr(CPU_Z80 *cpu_z80, word address, byte value)
{
  _write[address >> 14][address & 0x3FFF] = value;
}

byte cpu_z80_io_rd(CPU_Z80 *cpu_z80, word port)
{
  byte data = 0x00;

  /* Gate-Array   [0-------xxxxxxxx] [0x7fxx] */
  if((port & 0x8000) == 0) {
    (void) fprintf(stderr, "IO_RD[0x%04x]: Gate-Array   [---- Illegal ----]\n", port);
    (void) fflush(stderr);
  }
  /* CRTC-6845    [-0------xxxxxxxx] [0xbfxx] */
  if((port & 0x4000) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: CRTC-6845    [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
      case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: CRTC-6845    [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
      case 2:  /* [-0----10xxxxxxxx] [0xbexx] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: CRTC-6845    [- Not Supported -]\n", port);
        (void) fflush(stderr);
        break;
      case 3:  /* [-0----11xxxxxxxx] [0xbfxx] */
        data = crtc_6845.reg_file[crtc_6845.addr_reg];
        break;
    }
  }
  /* ROM Select   [--0-----xxxxxxxx] [0xdfxx] */
  if((port & 0x2000) == 0) {
    (void) fprintf(stderr, "IO_RD[0x%04x]: ROM Select   [---- Illegal ----]\n", port);
    (void) fflush(stderr);
  }
  /* Printer Port [---0----xxxxxxxx] [0xefxx] */
  if((port & 0x1000) == 0) {
    (void) fprintf(stderr, "IO_RD[0x%04x]: Printer Port [---- Illegal ----]\n", port);
    (void) fflush(stderr);
  }
  /* PPI-8255     [----0---xxxxxxxx] [0xf7xx] */
  if((port & 0x0800) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
        ppi_8255.port_a = amstrad_cpc.keyboard.bits[amstrad_cpc.keyboard.line];
        data = ppi_8255.port_a;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        ppi_8255.port_b = ((0                & 0x01) << 7)
                        | ((1                & 0x01) << 6)
                        | ((cfg.expansion    & 0x01) << 5)
                        | ((cfg.framerate    & 0x01) << 4)
                        | ((cfg.manufacturer & 0x07) << 1)
                        | ((crtc_6845.vsync  & 0x01) << 0);
        data = ppi_8255.port_b;
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        data = ppi_8255.port_c;
        break;
      case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: PPI-8255     [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
    }
  }
  /* FDC-765      [-----0--0xxxxxxx] [0xfb7f] */
  if((port & 0x0480) == 0) {
    switch(((port >> 7) & 2) | (port & 1)) {
      case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: FDC-765      [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
      case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
        (void) fprintf(stderr, "IO_RD[0x%04x]: FDC-765      [------ N/A ------]\n", port);
        (void) fflush(stderr);
        break;
      case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
        data = fdc_765.status;
        (void) fprintf(stderr, "IO_RD[0x%04x]: FDC-765      [--- RD_STATUS ---]\n", port);
        (void) fflush(stderr);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        data = fdc_765.data;
        (void) fprintf(stderr, "IO_RD[0x%04x]: FDC-765      [---- RD_DATA ----]\n", port);
        (void) fflush(stderr);
        break;
    }
  }
  return(data);
}

void cpu_z80_io_wr(CPU_Z80 *cpu_z80, word port, byte data)
{
  /* Gate-Array   [0-------xxxxxxxx] [0x7fxx] */
  if((port & 0x8000) == 0) {
    switch((data >> 6) & 3) {
      case 0: /* Select pen */
        amstrad_cpc.gate_array.pen = (data & 0x10 ? 0x10 : data & 0x0f);
        break;
      case 1: /* Select color */
        amstrad_cpc.gate_array.ink[amstrad_cpc.gate_array.pen] = data & 0x1f;
        break;
      case 2: /* Interrupt control, ROM configuration and screen mode */
        if((data & 0x10) != 0) {
          amstrad_cpc.gate_array.counter = 0;
          amstrad_cpc.gate_array.set_irq = 0;
        }
        amstrad_cpc.gate_array.rom_cfg = data & 0x1f;
        amstrad_cpc_rom_select();
        break;
      case 3: /* RAM memory management */
        amstrad_cpc.gate_array.ram_cfg = data & 0x3f;
        amstrad_cpc_ram_select();
        break;
    }
  }
  /* CRTC-6845    [-0------xxxxxxxx] [0xbfxx] */
  if((port & 0x4000) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
        crtc_6845.addr_reg = data;
        break;
      case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
        crtc_6845.reg_file[crtc_6845.addr_reg] = data;
        break;
      case 2:  /* [-0----10xxxxxxxx] [0xbexx] */
        (void) fprintf(stderr, "IO_WR[0x%04x]: CRTC-6845    [- Not Supported -]\n", port);
        (void) fflush(stderr);
        break;
      case 3:  /* [-0----11xxxxxxxx] [0xbfxx] */
        (void) fprintf(stderr, "IO_WR[0x%04x]: CRTC-6845    [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
    }
  }
  /* ROM Select   [--0-----xxxxxxxx] [0xdfxx] */
  if((port & 0x2000) == 0) {
    amstrad_cpc.memory.expansion = data;
    amstrad_cpc_rom_select();
  }
  /* Printer Port [---0----xxxxxxxx] [0xefxx] */
  if((port & 0x1000) == 0) {
  }
  /* PPI-8255     [----0---xxxxxxxx] [0xf7xx] */
  if((port & 0x0800) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
        ppi_8255.port_a = data;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        /*ppi_8255.port_b = data;*/
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        ppi_8255.port_c = data;
        amstrad_cpc.keyboard.line = data & 0x0F;
        break;
      case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
        ppi_8255.control = data;
        break;
    }
  }
  /* FDC-765      [-----0--0xxxxxxx] [0xfb7f] */
  if((port & 0x0480) == 0) {
    switch(((port >> 7) & 2) | ((port >> 0) & 1)) {
      case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
        fdc_765.motors = data & 0x01;
        (void) fprintf(stderr, "IO_WR[0x%04x]: FDC-765      [--- MOTOR_CTL ---]\n", port);
        (void) fflush(stderr);
        break;
      case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
        (void) fprintf(stderr, "IO_WR[0x%04x]: FDC-765      [------ N/A ------]\n", port);
        (void) fflush(stderr);
        break;
      case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
        (void) fprintf(stderr, "IO_WR[0x%04x]: FDC-765      [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        (void) fprintf(stderr, "IO_WR[0x%04x]: FDC-765      [---- WR_DATA ----]\n", port);
        (void) fflush(stderr);
        break;
    }
  }
}

void amstrad_cpc_start_handler(Widget widget, XtPointer data)
{
  int ix;
  FILE *file;
  int clock;
  Arg arglist[8];
  Cardinal argcount;

  argcount = 0;
  XtSetArg(arglist[argcount], XtNwidth,  cfg.width ); argcount++;
  XtSetArg(arglist[argcount], XtNheight, cfg.height); argcount++;
  XtSetValues(widget, arglist, argcount);

  _screen = XtScreen(widget);
  _window = XtWindow(widget);
  if((_ximage = XCreateImage(DisplayOfScreen(_screen), DefaultVisualOfScreen(_screen), DefaultDepthOfScreen(_screen), ZPixmap, 0, NULL, cfg.width, cfg.height, 8, 0)) == NULL) {
    perror("xmcpc");
    exit(-1);
  }
  _ximage->data = (char *) XtMalloc(_ximage->bytes_per_line * _ximage->height);
  (void) memset(_ximage->data, 0, _ximage->bytes_per_line * _ximage->height);
  switch(_ximage->depth) {
    case 8:
      amstrad_cpc_redraw = amstrad_cpc_redraw_8;
      break;
    case 15:
    case 16:
      amstrad_cpc_redraw = amstrad_cpc_redraw_16;
      break;
    case 24:
    case 32:
      amstrad_cpc_redraw = amstrad_cpc_redraw_32;
      break;
    default:
      amstrad_cpc_redraw = amstrad_cpc_redraw_0;
      break;
  }
  amstrad_cpc_init_palette();
  if((amstrad_cpc.memory.ram = (byte *) malloc(cfg.ramsize * 1024)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  if((file = fopen(cfg.rom[0], "r")) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  if((amstrad_cpc.memory.lower_rom = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.lower_rom, 1, 16384, file);
  if((amstrad_cpc.memory.upper_rom[0] = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.upper_rom[0], 1, 16384, file);
  fclose(file);
  for(ix = 1; ix < 256; ix++) {
    amstrad_cpc.memory.upper_rom[ix] = NULL;
    if((ix < 8) && (cfg.rom[ix] != NULL)) {
      if((file = fopen(cfg.rom[ix], "r")) == NULL) {
        perror("amstrad_cpc"); continue;
      }
      if((amstrad_cpc.memory.upper_rom[ix] = (byte *) malloc(16384)) == NULL) {
        perror("amstrad_cpc"); exit(-1);
      }
      fread(amstrad_cpc.memory.upper_rom[ix], 1, 16384, file);
      fclose(file);
    }
  }
  switch(cfg.clock) {
    default:
    case AMSTRAD_CPC_4MHZ:
      clock = 4000000;
      break;
    case AMSTRAD_CPC_8MHZ:
      clock = 8000000;
      break;
    case AMSTRAD_CPC_12MHZ:
      clock = 12000000;
      break;
    case AMSTRAD_CPC_16MHZ:
      clock = 16000000;
      break;
  }
  switch(cfg.framerate) {
    default:
    case AMSTRAD_CPC_50HZ:
      cpu_z80.IPeriod = (clock * 64) / 1000000;
      break;
    case AMSTRAD_CPC_60HZ:
      cpu_z80.IPeriod = (clock * 53) / 1000000;
      break;
  }
  cpu_z80_init(&cpu_z80);
  crtc_6845_init(&crtc_6845);
  ay_3_8910_init(&ay_3_8910);
  ppi_8255_init(&ppi_8255);
  fdc_765_init(&fdc_765);
  amstrad_cpc_reset();
  (void) gettimeofday(&timer1, NULL);
  (void) gettimeofday(&timer2, NULL);
}

void amstrad_cpc_clock_handler(Widget widget, XtPointer data)
{
  long delay, ix;
  int scanline = 0;
  int vsync_length;
  int vsyncpos_min;
  int vsyncpos_max;

  vsync_length = (crtc_6845.reg_file[3] >> 4) & 0x0f;
  if(vsync_length == 0) {
    vsync_length = 16;
  }
  vsyncpos_min = crtc_6845.reg_file[7] * (crtc_6845.reg_file[9] + 1);
  vsyncpos_max = vsyncpos_min + vsync_length - 1;
  do {
    amstrad_cpc.scanline[amstrad_cpc.beam.y].mode = amstrad_cpc.gate_array.rom_cfg & 0x03;
    for(ix = 0; ix < 17; ix++) {
      amstrad_cpc.scanline[amstrad_cpc.beam.y].ink[ix] = _col[amstrad_cpc.gate_array.ink[ix]];
    }
    if(amstrad_cpc.gate_array.set_irq != 0) {
      if((cpu_z80.IFF & IFF_1) != 0) {
        cpu_z80_intr(&cpu_z80, INT_RST38);
        amstrad_cpc.gate_array.counter &= 31;
        amstrad_cpc.gate_array.set_irq  = 0;
      }
    }
    cpu_z80_clock(&cpu_z80);
    if((scanline >= vsyncpos_min) && (scanline <= vsyncpos_max)) {
      if(crtc_6845.vsync == 0) {
        /* rising edge of V-SYNC */
      }
      if((scanline - vsyncpos_min) == 2) {
        if((amstrad_cpc.gate_array.counter & 32) != 0) {
          amstrad_cpc.gate_array.counter = 0;
        }
        else {
          amstrad_cpc.gate_array.counter = 0;
          amstrad_cpc.gate_array.set_irq = 1;
        }
      }
      crtc_6845.vsync = 1; /* set V-SYNC */
    }
    else {
      if(crtc_6845.vsync != 0) {
        /* falling edge of V-SYNC */
        amstrad_cpc.beam.y = 0;
      }
      crtc_6845.vsync = 0; /* reset V-SYNC */
    }
    if(++amstrad_cpc.gate_array.counter >= 52) {
      amstrad_cpc.gate_array.counter = 0;
      amstrad_cpc.gate_array.set_irq = 1;
    }
    if(++amstrad_cpc.beam.y > 311) {
      amstrad_cpc.beam.y = 311;
    }
  } while(++scanline < 312);
  (void) gettimeofday(&timer2, NULL);
  delay = ((long) (timer2.tv_sec  -  timer1.tv_sec) * 1000)
        + ((long) (timer2.tv_usec - timer1.tv_usec) / 1000);
  if(delay >= 1000) {
    *(&timer1) = *(&timer2); delay = 0;
  }
  switch(cfg.framerate) {
    case AMSTRAD_CPC_50HZ:
      if((delay >= 0) && (delay <= 20)) {
        amstrad_cpc_redraw();
      }
      if((timer1.tv_usec += 20000) >= 1000000) {
        timer1.tv_usec -= 1000000; timer1.tv_sec++;
      }
      break;
    case AMSTRAD_CPC_60HZ:
      if((delay >= 0) && (delay <= 16)) {
        amstrad_cpc_redraw();
      }
      if((timer1.tv_usec += 16667) >= 1000000) {
        timer1.tv_usec -= 1000000; timer1.tv_sec++;
      }
      break;
  }
  delay = ((long) (timer1.tv_sec  -  timer2.tv_sec) * 1000)
        + ((long) (timer1.tv_usec - timer2.tv_usec) / 1000);
  if(delay > 0) {
    *((unsigned long *) data) = (unsigned long) (delay - 1);
  }
  else {
    *((unsigned long *) data) = (unsigned long) (1);
  }
}

void amstrad_cpc_close_handler(Widget widget, XtPointer data)
{
  int ix;

  if(_ximage != NULL) {
    if(_ximage->data != NULL) {
      XtFree((char *) _ximage->data);
      _ximage->data = NULL;
    }
    XDestroyImage(_ximage);
    _ximage = NULL;
  }
  _window = None;
  _screen = NULL;
  amstrad_cpc_redraw = amstrad_cpc_redraw_0;
  free(amstrad_cpc.memory.lower_rom);
  amstrad_cpc.memory.lower_rom = NULL;
  free(amstrad_cpc.memory.ram);
  amstrad_cpc.memory.ram = NULL;
  for(ix = 0; ix < 256; ix++) {
    if(amstrad_cpc.memory.upper_rom[ix] != NULL) {
      free(amstrad_cpc.memory.upper_rom[ix]);
      amstrad_cpc.memory.upper_rom[ix] = NULL;
    }
  }
  cpu_z80_exit(&cpu_z80);
  crtc_6845_exit(&crtc_6845);
  ay_3_8910_exit(&ay_3_8910);
  ppi_8255_exit(&ppi_8255);
  fdc_765_exit(&fdc_765);
}

void amstrad_cpc_keybd_handler(Widget widget, XEvent *xevent)
{
  switch(cfg.keyboard) {
    case AMSTRAD_CPC_QWERTY:
      amstrad_cpc_decode_qwerty(widget, xevent);
      break;
    case AMSTRAD_CPC_AZERTY:
      amstrad_cpc_decode_azerty(widget, xevent);
      break;
  }
}

void amstrad_cpc_mouse_handler(Widget widget, XEvent *xevent)
{
}

void amstrad_cpc_paint_handler(Widget widget, XEvent *xevent)
{
  _screen = XtScreen(widget);
  _window = XtWindow(widget);
  (void) XPutImage(xevent->xexpose.display,
                   xevent->xexpose.window,
                   DefaultGCOfScreen(_screen), _ximage,
                   xevent->xexpose.x,     xevent->xexpose.y,
                   xevent->xexpose.x,     xevent->xexpose.y,
                   xevent->xexpose.width, xevent->xexpose.height);
  (void) XFlush(DisplayOfScreen(_screen));
}
