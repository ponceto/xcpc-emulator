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
#include "XArea.h"
#include "common.h"
#include "xcpc.h"
#include "cpu_z80.h"
#include "crtc_6845.h"
#include "ppi_8255.h"
#include "ay_3_8910.h"
#include "fdc_765.h"
#include "amstrad_cpc.h"

static char *tbl_version[6] = {
  "Hybride",
  "CPC 464",
  "CPC 664",
  "CPC 6128",
  "CPC 464+",
  "CPC 6128+"
};

static char *tbl_monitor[2] = {
  "CTM 65",
  "CTM 644"
};

static char *tbl_clock[8] = {
  "1.65 MHz",
  "2 MHz",
  "3.3 MHz",
  "4 MHz",
  "6.6 MHz",
  "8 MHz",
  "9.9 MHz",
  "16 MHz"
};

static char *tbl_cassette[2] = {
  "No data",
  "Data"
};

static char *tbl_printer[2] = {
  "Ready",
  "Not ready"
};

static char *tbl_expansion[2] = {
  "Not present",
  "Present"
};

static char *tbl_refresh[2] = {
  "60 Hz",
  "50 Hz"
};

static char *tbl_manufacturer[8] = {
  "Isp",
  "Triumph",
  "Saisho",
  "Solavox",
  "Awa",
  "Schneider",
  "Orion",
  "Amstrad"
};

static char *tbl_vsync[2] = {
  "Not active",
  "Active"
};

static AMSTRAD_CPC_CFG cfg = {
  AMSTRAD_CPC_6128,       /* version           */
  AMSTRAD_CPC_CTM644,     /* monitor           */
  AMSTRAD_CPC_4MHZ,       /* clock             */
  AMSTRAD_CPC_DATA,       /* cassette          */
  AMSTRAD_CPC_READY,      /* printer           */
  AMSTRAD_CPC_PRESENT,    /* expansion         */
  AMSTRAD_CPC_50HZ,       /* refresh           */
  AMSTRAD_CPC_AMSTRAD,    /* manufacturer      */
  AMSTRAD_CPC_NOT_ACTIVE, /* vsync             */
  800,                    /* width             */
  624,                    /* height            */
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
  }
  _read[0] = (!(amstrad_cpc.gate_array.rom_cfg & 0x04) ? amstrad_cpc.memory.lower_rom : _bank[0]);
  if(!(amstrad_cpc.gate_array.rom_cfg & 0x08)) {
    _read[3] = (amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion] == NULL ? amstrad_cpc.memory.upper_rom[0] : amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion]);
  }
  else {
    _read[3] = _bank[3];
  }
}

static byte _key[256][4] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x09, 0x80, 0x00 }, /* bs  */
  { 0x01, 0x08, 0x10, 0x00 }, /* ht  */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x02, 0x04, 0x00 }, /* cr  */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x08, 0x04, 0x00 }, /* esc */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x05, 0x80, 0x00 }, /* sp  */
  { 0x01, 0x08, 0x01, 0x01 }, /* !   */
  { 0x01, 0x08, 0x02, 0x01 }, /* "   */
  { 0x01, 0x07, 0x02, 0x01 }, /* #   */
  { 0x01, 0x07, 0x01, 0x01 }, /* $   */
  { 0x01, 0x06, 0x02, 0x01 }, /* %   */
  { 0x01, 0x06, 0x01, 0x01 }, /* &   */
  { 0x01, 0x05, 0x02, 0x01 }, /* '   */
  { 0x01, 0x05, 0x01, 0x01 }, /* (   */
  { 0x01, 0x04, 0x02, 0x01 }, /* )   */
  { 0x01, 0x03, 0x20, 0x01 }, /* *   */
  { 0x01, 0x03, 0x10, 0x01 }, /* +   */
  { 0x01, 0x04, 0x80, 0x00 }, /* ,   */
  { 0x01, 0x03, 0x02, 0x00 }, /* -   */
  { 0x01, 0x03, 0x80, 0x00 }, /* .   */
  { 0x01, 0x03, 0x40, 0x00 }, /* /   */
  { 0x01, 0x04, 0x01, 0x00 }, /* 0   */
  { 0x01, 0x08, 0x01, 0x00 }, /* 1   */
  { 0x01, 0x08, 0x02, 0x00 }, /* 2   */
  { 0x01, 0x07, 0x02, 0x00 }, /* 3   */
  { 0x01, 0x07, 0x01, 0x00 }, /* 4   */
  { 0x01, 0x06, 0x02, 0x00 }, /* 5   */
  { 0x01, 0x06, 0x01, 0x00 }, /* 6   */
  { 0x01, 0x05, 0x02, 0x00 }, /* 7   */
  { 0x01, 0x05, 0x01, 0x00 }, /* 8   */
  { 0x01, 0x04, 0x02, 0x00 }, /* 9   */
  { 0x01, 0x03, 0x20, 0x00 }, /* :   */
  { 0x01, 0x03, 0x10, 0x00 }, /* ;   */
  { 0x01, 0x04, 0x80, 0x00 }, /* <   */
  { 0x01, 0x03, 0x02, 0x01 }, /* =   */
  { 0x01, 0x03, 0x80, 0x00 }, /* >   */
  { 0x01, 0x03, 0x40, 0x01 }, /* ?   */
  { 0x01, 0x03, 0x04, 0x00 }, /* @   */
  { 0x01, 0x08, 0x20, 0x01 }, /* A   */
  { 0x01, 0x06, 0x40, 0x01 }, /* B   */
  { 0x01, 0x07, 0x40, 0x01 }, /* C   */
  { 0x01, 0x07, 0x20, 0x01 }, /* D   */
  { 0x01, 0x07, 0x04, 0x01 }, /* E   */
  { 0x01, 0x06, 0x20, 0x01 }, /* F   */
  { 0x01, 0x06, 0x10, 0x01 }, /* G   */
  { 0x01, 0x05, 0x10, 0x01 }, /* H   */
  { 0x01, 0x04, 0x08, 0x01 }, /* I   */
  { 0x01, 0x05, 0x20, 0x01 }, /* J   */
  { 0x01, 0x04, 0x20, 0x01 }, /* K   */
  { 0x01, 0x04, 0x10, 0x01 }, /* L   */
  { 0x01, 0x04, 0x40, 0x01 }, /* M   */
  { 0x01, 0x05, 0x40, 0x01 }, /* N   */
  { 0x01, 0x04, 0x04, 0x01 }, /* O   */
  { 0x01, 0x03, 0x08, 0x01 }, /* P   */
  { 0x01, 0x08, 0x08, 0x01 }, /* Q   */
  { 0x01, 0x06, 0x04, 0x01 }, /* R   */
  { 0x01, 0x07, 0x10, 0x01 }, /* S   */
  { 0x01, 0x06, 0x08, 0x01 }, /* T   */
  { 0x01, 0x05, 0x04, 0x01 }, /* U   */
  { 0x01, 0x06, 0x80, 0x01 }, /* V   */
  { 0x01, 0x07, 0x08, 0x01 }, /* W   */
  { 0x01, 0x07, 0x80, 0x01 }, /* X   */
  { 0x01, 0x05, 0x08, 0x01 }, /* Y   */
  { 0x01, 0x08, 0x80, 0x01 }, /* Z   */
  { 0x01, 0x02, 0x02, 0x00 }, /* [   */
  { 0x01, 0x02, 0x40, 0x00 }, /* \   */
  { 0x01, 0x02, 0x08, 0x00 }, /* ]   */
  { 0x01, 0x03, 0x01, 0x00 }, /* ^   */
  { 0x01, 0x04, 0x01, 0x01 }, /* _   */
  { 0x01, 0x02, 0x40, 0x01 }, /* `   */
  { 0x01, 0x08, 0x20, 0x00 }, /* a   */
  { 0x01, 0x06, 0x40, 0x00 }, /* b   */
  { 0x01, 0x07, 0x40, 0x00 }, /* c   */
  { 0x01, 0x07, 0x20, 0x00 }, /* d   */
  { 0x01, 0x07, 0x04, 0x00 }, /* e   */
  { 0x01, 0x06, 0x20, 0x00 }, /* f   */
  { 0x01, 0x06, 0x10, 0x00 }, /* g   */
  { 0x01, 0x05, 0x10, 0x00 }, /* h   */
  { 0x01, 0x04, 0x08, 0x00 }, /* i   */
  { 0x01, 0x05, 0x20, 0x00 }, /* j   */
  { 0x01, 0x04, 0x20, 0x00 }, /* k   */
  { 0x01, 0x04, 0x10, 0x00 }, /* l   */
  { 0x01, 0x04, 0x40, 0x00 }, /* m   */
  { 0x01, 0x05, 0x40, 0x00 }, /* n   */
  { 0x01, 0x04, 0x04, 0x00 }, /* o   */
  { 0x01, 0x03, 0x08, 0x00 }, /* p   */
  { 0x01, 0x08, 0x08, 0x00 }, /* q   */
  { 0x01, 0x06, 0x04, 0x00 }, /* r   */
  { 0x01, 0x07, 0x10, 0x00 }, /* s   */
  { 0x01, 0x06, 0x08, 0x00 }, /* t   */
  { 0x01, 0x05, 0x04, 0x00 }, /* u   */
  { 0x01, 0x06, 0x80, 0x00 }, /* v   */
  { 0x01, 0x07, 0x08, 0x00 }, /* w   */
  { 0x01, 0x07, 0x80, 0x00 }, /* x   */
  { 0x01, 0x05, 0x08, 0x00 }, /* y   */
  { 0x01, 0x08, 0x80, 0x00 }, /* z   */
  { 0x01, 0x02, 0x02, 0x01 }, /* {   */
  { 0x01, 0x03, 0x04, 0x01 }, /* |   */
  { 0x01, 0x02, 0x08, 0x01 }, /* }   */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x02, 0x01, 0x00 }, /* del */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
};

static void amstrad_cpc_key_press_cbk(Widget widget, XtPointer data, XEvent *event)
{
char buffer[8], ascii;
KeySym keysym;
byte row, value, shift;

  XLookupString((XKeyEvent *) event, buffer, 8, &keysym, NULL);
  ascii = buffer[0];
  if(IsCursorKey(keysym)) {
    switch(keysym) {
      case XK_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsKeypadKey(keysym)) {
    switch(keysym) {
      case XK_KP_Enter:
        row = 0x00; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Decimal:
        row = 0x00; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_0:
        row = 0x01; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_1:
        row = 0x01; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_2:
        row = 0x01; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_3:
        row = 0x00; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_4:
        row = 0x02; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_5:
        row = 0x01; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_6:
        row = 0x00; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_7:
        row = 0x01; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_8:
        row = 0x01; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_9:
        row = 0x00; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsModifierKey(keysym)) {
    switch(keysym) {
      case XK_Shift_L:
        row = 0x02; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Control_L:
        row = 0x02; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Caps_Lock:
        row = 0x08; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Alt_L:
        row = 0x01; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else {
    if(!_key[ascii][0]) {
      return;
    }
    row = _key[ascii][1];
    value = _key[ascii][2];
    shift = _key[ascii][3];
  }
  amstrad_cpc.keyboard.line[row] &= ~value;
  if(shift) {
    amstrad_cpc.keyboard.line[0x02] &= ~0x20;
  }
}

static void amstrad_cpc_key_release_cbk(Widget widget, XtPointer data, XEvent *event)
{
char buffer[8], ascii;
KeySym keysym;
byte row, value, shift;

  XLookupString((XKeyEvent *) event, buffer, 8, &keysym, NULL);
  ascii = buffer[0];
  if(IsCursorKey(keysym)) {
    switch(keysym) {
      case XK_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsKeypadKey(keysym)) {
    switch(keysym) {
      case XK_KP_Enter:
        row = 0x00; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Decimal:
        row = 0x00; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_0:
        row = 0x01; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_1:
        row = 0x01; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_2:
        row = 0x01; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_3:
        row = 0x00; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_4:
        row = 0x02; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_5:
        row = 0x01; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_6:
        row = 0x00; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_7:
        row = 0x01; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_8:
        row = 0x01; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_9:
        row = 0x00; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsModifierKey(keysym)) {
    switch(keysym) {
      case XK_Shift_L:
        row = 0x02; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Control_L:
        row = 0x02; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Caps_Lock:
        row = 0x08; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Alt_L:
        row = 0x01; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else {
    if(!_key[ascii][0]) {
      return;
    }
    row = _key[ascii][1];
    value = _key[ascii][2];
    shift = _key[ascii][3];
  }
  amstrad_cpc.keyboard.line[row] |= value;
  amstrad_cpc.keyboard.line[0x02] |= 0x20;
}

static Screen *_screen = NULL;
static Display *_display = NULL;
static Window _window = None;
static GC _gc = None;
static XImage *_ximage = NULL;

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

static unsigned long _ink[17] = {
  0L, /* ink 0  */
  0L, /* ink 1  */
  0L, /* ink 2  */
  0L, /* ink 3  */
  0L, /* ink 4  */
  0L, /* ink 5  */
  0L, /* ink 6  */
  0L, /* ink 7  */
  0L, /* ink 8  */
  0L, /* ink 9  */
  0L, /* ink 10 */
  0L, /* ink 11 */
  0L, /* ink 12 */
  0L, /* ink 13 */
  0L, /* ink 14 */
  0L, /* ink 15 */
  0L, /* border */
};

static void amstrad_cpc_init_palette(void)
{
int ix;
XColor xcolor;

  xcolor.flags = DoRed | DoGreen | DoBlue;
  xcolor.pad = 0x00;
  for(ix = 0; ix < 32; ix++) {
    if(cfg.monitor == AMSTRAD_CPC_CTM65) {
      xcolor.red = 0x0000;
      xcolor.green = (_palette[ix][0] + _palette[ix][1] + _palette[ix][2]) / 3;
      xcolor.blue = 0x0000;
    }
    else {
      xcolor.red = _palette[ix][0];
      xcolor.green = _palette[ix][1];
      xcolor.blue = _palette[ix][2];
    }
    if(XAllocColor(_display, DefaultColormapOfScreen(_screen), &xcolor) == False) {
      fprintf(stderr, "amstrad_cpc: cannot allocate color ... %04x/%04x/%04x\n", xcolor.red, xcolor.green, xcolor.blue);
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
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (cfg.width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned long color;
unsigned long border = _ink[0x10];
int col, row = 0;

  for(cy = 0; cy < vp; cy++) {
    col = 0;
    for(cx = 0; cx < cfg.width; cx++) {
      XPutPixel(_ximage, col, row++, border);
      XPutPixel(_ximage, col++, row--, border);
    }
    row += 2;
  }
  switch(amstrad_cpc.gate_array.rom_cfg & 0x03) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
              XPutPixel(_ximage, col, row++, color);
              XPutPixel(_ximage, col++, row, color);
              XPutPixel(_ximage, col, row--, color);
              XPutPixel(_ximage, col++, row, color);
              XPutPixel(_ximage, col, row++, color);
              XPutPixel(_ximage, col++, row, color);
              XPutPixel(_ximage, col, row--, color);
              XPutPixel(_ximage, col++, row, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              XPutPixel(_ximage, col, row++, color);
              XPutPixel(_ximage, col++, row, color);
              XPutPixel(_ximage, col, row--, color);
              XPutPixel(_ximage, col++, row, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              XPutPixel(_ximage, col, row++, color);
              XPutPixel(_ximage, col++, row--, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(_ximage, col, row++, border);
            XPutPixel(_ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    col = 0;
    for(cx = 0; cx < cfg.width; cx++) {
      XPutPixel(_ximage, col, row++, border);
      XPutPixel(_ximage, col++, row--, border);
    }
    row += 2;
  }
  XPutImage(_display, _window, _gc, _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
}

static void amstrad_cpc_redraw_8(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (cfg.width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned char color;
unsigned char border = _ink[0x10];
unsigned char *dst = (unsigned char *) _ximage->data;
unsigned char *nxt = (unsigned char *) _ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.rom_cfg & 0x03) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
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
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(_display, _window, _gc, _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
}

static void amstrad_cpc_redraw_16(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (cfg.width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned short color;
unsigned short border = _ink[0x10];
unsigned short *dst = (unsigned short *) _ximage->data;
unsigned short *nxt = (unsigned short *) _ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.rom_cfg & 0x03) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
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
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(_display, _window, _gc, _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
}

static void amstrad_cpc_redraw_32(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (cfg.width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((cfg.height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned int color;
unsigned int border = _ink[0x10];
unsigned int *dst = (unsigned int *) _ximage->data;
unsigned int *nxt = (unsigned int *) _ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.rom_cfg & 0x03) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
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
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += cfg.width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += cfg.width;
    for(cx = 0; cx < cfg.width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(_display, _window, _gc, _ximage, 0, 0, 0, 0, cfg.width, cfg.height);
}

static void (*amstrad_cpc_redraw)(void) = amstrad_cpc_redraw_0;

static void amstrad_cpc_expose_cbk(Widget widget, XtPointer data, XEvent *event)
{
  XPutImage(event->xexpose.display, event->xexpose.window, _gc, _ximage, event->xexpose.x, event->xexpose.y, event->xexpose.x, event->xexpose.y, event->xexpose.width, event->xexpose.height);
}

void amstrad_cpc_init(void)
{
int ix;
FILE *file;
int cx, cy;
int clock, interrupt;

  fprintf(stderr, "amstrad_cpc: version ................. %s\n", tbl_version[cfg.version]);
  fprintf(stderr, "amstrad_cpc: monitor ................. %s\n", tbl_monitor[cfg.monitor]);
  fprintf(stderr, "amstrad_cpc: clock ................... %s\n", tbl_clock[cfg.clock]);
  fprintf(stderr, "amstrad_cpc: cassette ................ %s\n", tbl_cassette[cfg.cassette]);
  fprintf(stderr, "amstrad_cpc: printer ................. %s\n", tbl_printer[cfg.printer]);
  fprintf(stderr, "amstrad_cpc: expansion ............... %s\n", tbl_expansion[cfg.expansion]);
  fprintf(stderr, "amstrad_cpc: refresh ................. %s\n", tbl_refresh[cfg.refresh]);
  fprintf(stderr, "amstrad_cpc: manufacturer ............ %s\n", tbl_manufacturer[cfg.manufacturer]);
  fprintf(stderr, "amstrad_cpc: vsync ................... %s\n", tbl_vsync[cfg.vsync]);

  _screen = XtScreen(xarea);
  _display = XtDisplay(xarea);
  _window = XtWindow(xarea);
  _gc = XCreateGC(_display, _window, 0L, NULL);
  if((_ximage = XCreateImage(_display, DefaultVisualOfScreen(_screen), DefaultDepthOfScreen(_screen), ZPixmap, 0, NULL, cfg.width, cfg.height, 8, 0)) == NULL) {
    perror("xmcpc");
    exit(-1);
  }
  _ximage->data = (char *) XtMalloc(_ximage->bytes_per_line * _ximage->height);
  for(cy = 0; cy < _ximage->height; cy++) {
    for(cx = 0; cx < _ximage->width; cx++) {
      XPutPixel(_ximage, cx, cy, BlackPixelOfScreen(_screen));
    }
  }
  switch(_ximage->depth) {
    case 8:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 8 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_8;
      break;
    case 15:
    case 16:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 16 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_16;
      break;
    case 24:
    case 32:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 32 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_32;
      break;
    default:
      fprintf(stderr, "amstrad_cpc: screen driver ........... generic (%d bpp)\n", _ximage->depth);
      amstrad_cpc_redraw = amstrad_cpc_redraw_0;
      break;
  }
  amstrad_cpc_init_palette();
  XtVaSetValues(xarea, XtNwidth, cfg.width, XtNheight, cfg.height, NULL);
  XtAddCallback(xarea, XtNkeyPressCallback, (XtCallbackProc) amstrad_cpc_key_press_cbk, NULL);
  XtAddCallback(xarea, XtNkeyReleaseCallback, (XtCallbackProc) amstrad_cpc_key_release_cbk, NULL);
  XtAddCallback(xarea, XtNexposeCallback, (XtCallbackProc) amstrad_cpc_expose_cbk, NULL);

  fprintf(stderr, "amstrad_cpc: on-board ram ............ %d Kb\n", cfg.ramsize);
  if((amstrad_cpc.memory.ram = (byte *) malloc(cfg.ramsize * 1024)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  if((file = fopen(cfg.rom[0], "r")) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fprintf(stderr, "amstrad_cpc: system lower rom ........ %s\n", cfg.rom[0]);
  if((amstrad_cpc.memory.lower_rom = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.lower_rom, 1, 16384, file);
  fprintf(stderr, "amstrad_cpc: system upper rom ........ %s\n", cfg.rom[0]);
  if((amstrad_cpc.memory.upper_rom[0] = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.upper_rom[0], 1, 16384, file);
  fclose(file);
  for(ix = 1; ix < 8; ix++) {
    amstrad_cpc.memory.upper_rom[ix] = NULL;
    fprintf(stderr, "amstrad_cpc: expansion rom #%d ........ %s\n", ix, (cfg.rom[ix] == NULL ? "(not present)" : cfg.rom[ix]));
    if(cfg.rom[ix] != NULL) {
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
    case AMSTRAD_CPC_1_6MHZ:
      clock = 1650000;
      break;
    case AMSTRAD_CPC_2MHZ:
      clock = 2000000;
      break;
    case AMSTRAD_CPC_3_3MHZ:
      clock = 3300000;
      break;
    case AMSTRAD_CPC_4MHZ:
      clock = 4000000;
      break;
    case AMSTRAD_CPC_6_6MHZ:
      clock = 6600000;
      break;
    case AMSTRAD_CPC_8MHZ:
      clock = 8000000;
      break;
    case AMSTRAD_CPC_9_9MHZ:
      clock = 9900000;
      break;
    case AMSTRAD_CPC_16MHZ:
      clock = 16000000;
      break;
  }
  switch(cfg.refresh) {
    case AMSTRAD_CPC_50HZ:
      interrupt = 64;
      break;
    case AMSTRAD_CPC_60HZ:
      interrupt = 53;
      break;
  }
  z80.IPeriod = (clock * interrupt) / 1000000;
  z80_init();
  crtc_6845_init();
  ppi_8255_init();
  ay_3_8910_init();
  fdc_765_init();
  amstrad_cpc_reset();
}

void amstrad_cpc_reset(void)
{
int ix;

  amstrad_cpc.ticks = 0;
  amstrad_cpc.cycle = 0;
  amstrad_cpc.memory.expansion = 0x00;
  amstrad_cpc.keyboard.row = 0x00;
  for(ix = 0; ix < 16; ix++) {
    amstrad_cpc.keyboard.line[ix] = 0xFF;
  }
  amstrad_cpc.gate_array.pen = 0x00;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = 0x00;
    _ink[ix] = _col[amstrad_cpc.gate_array.ink[ix]];
  }
  amstrad_cpc.gate_array.rom_cfg = 0x00;
  amstrad_cpc.gate_array.ram_cfg = 0x00;
  amstrad_cpc.gate_array.counter = 0x00;
  amstrad_cpc_ram_select();
  z80_reset();
  crtc_6845_reset();
  ppi_8255_reset();
  ppi_8255.port_b = (cfg.cassette << 7) | (cfg.printer << 6) | (cfg.expansion << 5) | (cfg.refresh << 4) | (cfg.manufacturer << 1) | (cfg.vsync << 0);
  ay_3_8910_reset();
  fdc_765_reset();
}

void amstrad_cpc_exit(void)
{
int ix;

  XFreeGC(_display, _gc);
  XtFree(_ximage->data);
  _ximage->data = NULL;
  XDestroyImage(_ximage);
  _screen = NULL;
  _display = NULL;
  _window = None;
  _gc = None;
  _ximage = NULL;
  XtRemoveCallback(xarea, XtNkeyPressCallback, (XtCallbackProc) amstrad_cpc_key_press_cbk, NULL);
  XtRemoveCallback(xarea, XtNkeyReleaseCallback, (XtCallbackProc) amstrad_cpc_key_release_cbk, NULL);
  XtRemoveCallback(xarea, XtNexposeCallback, (XtCallbackProc) amstrad_cpc_expose_cbk, NULL);
  amstrad_cpc_redraw = amstrad_cpc_redraw_0;
  free(amstrad_cpc.memory.lower_rom);
  amstrad_cpc.memory.lower_rom = NULL;
  free(amstrad_cpc.memory.ram);
  amstrad_cpc.memory.ram = NULL;
  for(ix = 0; ix < 8; ix++) {
    if(amstrad_cpc.memory.upper_rom[ix] != NULL) {
      free(amstrad_cpc.memory.upper_rom[ix]);
      amstrad_cpc.memory.upper_rom[ix] = NULL;
    }
  }
  z80_exit();
  crtc_6845_exit();
  ppi_8255_exit();
  ay_3_8910_exit();
  fdc_765_exit();
}

static void amstrad_cpc_synchronize(int signum)
{
  if(!paused) {
    amstrad_cpc.ticks++;
  }
}

int amstrad_cpc_main(int argc, char **argv)
{
char *progname = *argv;
struct itimerval new;
struct itimerval old;
int refresh;

  while(--argc) {
    argv++;
    if(!strcmp("-cpc464", *argv)) {
      cfg.version = AMSTRAD_CPC_464;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_3_3MHZ;
      cfg.expansion = AMSTRAD_CPC_NOT_PRESENT;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 64;
      cfg.rom[0x00] = ROMSDIR "/cpc464.rom";
      cfg.rom[0x07] = NULL;
    }
    else if(!strcmp("-cpc664", *argv)) {
      cfg.version = AMSTRAD_CPC_664;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 64;
      cfg.rom[0x00] = ROMSDIR "/cpc664.rom";
      cfg.rom[0x07] = ROMSDIR "/amsdos.rom";
    }
    else if(!strcmp("-cpc6128", *argv)) {
      cfg.version = AMSTRAD_CPC_6128;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 128;
      cfg.rom[0x00] = ROMSDIR "/cpc6128.rom";
      cfg.rom[0x07] = ROMSDIR "/amsdos.rom";
    }
    else if(!strcmp("-cpc464+", *argv)) {
      cfg.version = AMSTRAD_CPC_464_PLUS;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_3_3MHZ;
      cfg.expansion = AMSTRAD_CPC_NOT_PRESENT;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 64;
      cfg.rom[0x00] = ROMSDIR "/cpc464+.rom";
      cfg.rom[0x07] = NULL;
    }
    else if(!strcmp("-cpc6128+", *argv)) {
      cfg.version = AMSTRAD_CPC_6128_PLUS;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.ramsize = 128;
      cfg.rom[0x00] = ROMSDIR "/cpc6128+.rom";
      cfg.rom[0x07] = ROMSDIR "/amsdos.rom";
    }
    else if(!strcmp("-1.6MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_1_6MHZ;
    }
    else if(!strcmp("-2MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_2MHZ;
    }
    else if(!strcmp("-3.3MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_3_3MHZ;
    }
    else if(!strcmp("-4MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_4MHZ;
    }
    else if(!strcmp("-6.6MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_6_6MHZ;
    }
    else if(!strcmp("-8MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_8MHZ;
    }
    else if(!strcmp("-9.9MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_9_9MHZ;
    }
    else if(!strcmp("-16MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_16MHZ;
    }
    else if(!strcmp("-50Hz", *argv)) {
      cfg.refresh = AMSTRAD_CPC_50HZ;
    }
    else if(!strcmp("-60Hz", *argv)) {
      cfg.refresh = AMSTRAD_CPC_60HZ;
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
    else if(!strcmp("-CTM65", *argv)) {
      cfg.monitor = AMSTRAD_CPC_CTM65;
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
      fprintf(stderr, "usage: %s [-h]\n", progname);
      fprintf(stderr, "             [-cpc464] [-cpc664] [-cpc6128]\n");
      fprintf(stderr, "             [-CTM65] [-CTM644]\n");
      fprintf(stderr, "             [-1.6MHz] [-2MHz] [-3.3MHz] [-4MHz]\n");
      fprintf(stderr, "             [-6.6MHz] [-8MHz] [-9.9MHz] [-16MHz]\n");
      fprintf(stderr, "             [-50Hz] [-60Hz]\n");
      fprintf(stderr, "             [-isp] [-triumph] [-saisho] [-solavox]\n");
      fprintf(stderr, "             [-awa] [-schneider] [-orion] [-amstrad]\n");
      fprintf(stderr, "             [-tiny] [-small] [-medium] [-big] [-huge]\n");
      return(-1);
    }
  }
  amstrad_cpc_init();
  switch(cfg.refresh) {
    case AMSTRAD_CPC_50HZ:
      refresh = 50;
      break;
    case AMSTRAD_CPC_60HZ:
      refresh = 60;
      break;
  }
  new.it_interval.tv_sec = new.it_value.tv_sec = 0;
  new.it_interval.tv_usec = new.it_value.tv_usec = 1000000 / refresh;
  signal(SIGALRM, amstrad_cpc_synchronize);
  setitimer(ITIMER_REAL, &new, &old);
  z80_run();
  setitimer(ITIMER_REAL, &old, &new);
  amstrad_cpc_exit();
  return(0);
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
    fprintf(stderr, "amstrad_cpc: not a valid snapshot file (bad signature)\n");
    fclose(file);
    return;
  } bufptr += 8;
  bufptr += 8; /* not used */
  bufptr++; /* snapshot version */
  z80.AF.B.l = *bufptr++;
  z80.AF.B.h = *bufptr++;
  z80.BC.B.l = *bufptr++;
  z80.BC.B.h = *bufptr++;
  z80.DE.B.l = *bufptr++;
  z80.DE.B.h = *bufptr++;
  z80.HL.B.l = *bufptr++;
  z80.HL.B.h = *bufptr++;
  z80.Refresh = *bufptr++;
  z80.I = *bufptr++;
  z80.IFF = (!(*bufptr++) ? z80.IFF & 0x7F : z80.IFF | 0x80); /* IFF1 */
  z80.IFF = (!(*bufptr++) ? z80.IFF & 0xFE : z80.IFF | 0x01); /* IFF2 */
  z80.IX.B.l = *bufptr++;
  z80.IX.B.h = *bufptr++;
  z80.IY.B.l = *bufptr++;
  z80.IY.B.h = *bufptr++;
  z80.SP.B.l = *bufptr++;
  z80.SP.B.h = *bufptr++;
  z80.PC.B.l = *bufptr++;
  z80.PC.B.h = *bufptr++;
  switch(*bufptr++) { /* IM */
    case 0:
      z80.IFF &= 0xF9;
      break;
    case 1:
      z80.IFF = (z80.IFF & 0xF9) | 2;
      break;
    case 2:
      z80.IFF = (z80.IFF & 0xF9) | 4;
      break;
  }
  z80.AF1.B.l = *bufptr++;
  z80.AF1.B.h = *bufptr++;
  z80.BC1.B.l = *bufptr++;
  z80.BC1.B.h = *bufptr++;
  z80.DE1.B.l = *bufptr++;
  z80.DE1.B.h = *bufptr++;
  z80.HL1.B.l = *bufptr++;
  z80.HL1.B.h = *bufptr++;
  amstrad_cpc.gate_array.pen = *bufptr++;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = *bufptr++;
    _ink[ix] = _col[amstrad_cpc.gate_array.ink[ix]];
  }
  amstrad_cpc.gate_array.rom_cfg = *bufptr++;
  amstrad_cpc.gate_array.ram_cfg = *bufptr++;
  amstrad_cpc_ram_select();
  crtc_6845.current = *bufptr++;
  for(ix = 0; ix < 18; ix++) {
    crtc_6845.registers[ix] = *bufptr++;
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
    fprintf(stderr, "amstrad_cpc: snapshot file too large (%d Kb)\n", ramsize);
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
  *bufptr++ = z80.AF.B.l;
  *bufptr++ = z80.AF.B.h;
  *bufptr++ = z80.BC.B.l;
  *bufptr++ = z80.BC.B.h;
  *bufptr++ = z80.DE.B.l;
  *bufptr++ = z80.DE.B.h;
  *bufptr++ = z80.HL.B.l;
  *bufptr++ = z80.HL.B.h;
  *bufptr++ = z80.Refresh;
  *bufptr++ = z80.I;
  *bufptr++ = (z80.IFF & 0x80 ? 0x01 : 0x00);
  *bufptr++ = (z80.IFF & 0x01 ? 0x01 : 0x00);
  *bufptr++ = z80.IX.B.l;
  *bufptr++ = z80.IX.B.h;
  *bufptr++ = z80.IY.B.l;
  *bufptr++ = z80.IY.B.h;
  *bufptr++ = z80.SP.B.l;
  *bufptr++ = z80.SP.B.h;
  *bufptr++ = z80.PC.B.l;
  *bufptr++ = z80.PC.B.h;
  switch(z80.IFF & 0x06) {
    case 0:
      *bufptr++ = 0x00;
      break;
    case 2:
      *bufptr++ = 0x01;
      break;
    case 4:
      *bufptr++ = 0x02;
      break;
    default:
      *bufptr++ = 0x00;
      break;
  }
  *bufptr++ = z80.AF1.B.l;
  *bufptr++ = z80.AF1.B.h;
  *bufptr++ = z80.BC1.B.l;
  *bufptr++ = z80.BC1.B.h;
  *bufptr++ = z80.DE1.B.l;
  *bufptr++ = z80.DE1.B.h;
  *bufptr++ = z80.HL1.B.l;
  *bufptr++ = z80.HL1.B.h;
  *bufptr++ = amstrad_cpc.gate_array.pen;
  for(ix = 0; ix < 17; ix++) {
    *bufptr++ = amstrad_cpc.gate_array.ink[ix];
  }
  *bufptr++ = amstrad_cpc.gate_array.rom_cfg;
  *bufptr++ = amstrad_cpc.gate_array.ram_cfg;
  *bufptr++ = crtc_6845.current;
  for(ix = 0; ix < 18; ix++) {
    *bufptr++ = crtc_6845.registers[ix];
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

byte z80_read(register word address)
{
  return(_read[address >> 14][address & 0x3FFF]);
}

void z80_write(register word address, register byte value)
{
  _write[address >> 14][address & 0x3FFF] = value;
}

byte z80_in(register word port)
{
  if(!(port & 0x8000)) { /* Gate-Array (bit 15 = 0; Port 7Fxx) */
    fprintf(stderr, "amstrad_cpc: (Gate-Array) ............. IN (%04X) (Illegal; write only)\n", port);
  }
  if(!(port & 0x4000)) { /* CRTC 6845 (bit 14 = 0; Port BCxx-BFxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Register select (port BCxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 select) ....... IN (%04X) (Illegal; write only)\n", port);
        break;
      case 0x0100: /* Register write (port BDxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 write) ........ IN (%04X) (Illegal; write only)\n", port);
        break;
      case 0x0200: /* Dependent of version (port BExx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845) .............. IN (%04X) (What to do ?)\n", port);
        break;
      case 0x0300: /* Register read (port BFxx) */
        return(crtc_6845.registers[crtc_6845.current]);
        break;
    }
  }
  if(!(port & 0x2000)) { /* ROM select (bit 13 = 0; Port DFxx) */
    fprintf(stderr, "amstrad_cpc: (ROM select) ............. IN (%04X)\n", port);
  }
  if(!(port & 0x1000)) { /* Printer port (bit 12 = 0; Port EFxx) */
    fprintf(stderr, "amstrad_cpc: (Printer port) ........... IN (%04X)\n", port);
  }
  if(!(port & 0x0800)) { /* PPI 8255 (bit 11 = 0; Port F4xx-F7xx) */
    switch(port & 0x0300) {
      case 0x0000: /* Port A (port F4xx) */
        return(amstrad_cpc.keyboard.line[amstrad_cpc.keyboard.row]);
        /*return(ppi_8255.port_a);*/
        break;
      case 0x0100: /* Port B (port F5xx) */
        return(ppi_8255.port_b);
        break;
      case 0x0200: /* Port C (port F6xx) */
        return(ppi_8255.port_c);
        break;
      case 0x0300: /* Control register (port F7xx) */
        fprintf(stderr, "amstrad_cpc: (PPI 8255 CTRL) .......... IN (%04X) (Illegal; write only)\n", port);
        break;
    }
  }
  if(!(port & 0x0400)) { /* Expansion (bit 10 = 0; Port F8xx-FBxx) */
    switch(port & 0x300) {
      case 0x0000: /* Serial port (port F8xx) */
        fprintf(stderr, "amstrad_cpc: (Serial port) ............ IN (%04X)\n", port);
        break;
      case 0x0100: /* (port F9xx) */
        fprintf(stderr, "amstrad_cpc: (F9xx) ................... IN (%04X)\n", port);
        break;
      case 0x0200: /* FDC motor (port FAxx) */
        return(fdc_765_get_motor());
        break;
      case 0x0300: /* FDC register (port FBxx) */
        if(!(port & 0x0001)) { /* status (port FB7E) */
          return(fdc_765_get_status());
        }
        else { /* data  (port FB7F) */
          fprintf(stderr, "amstrad_cpc: (FDC 765 - FB7F) ......... IN (%04X)\n", port);
        }
        break;
    }
  }
  return(0x00);
}

void z80_out(register word port, register byte value)
{
  if(!(port & 0x8000)) { /* Gate-Array (bit 15 = 0; Port 7Fxx) */
    switch(value & 0xC0) { /* Select function */
      case 0x00: /* Select pen */
        amstrad_cpc.gate_array.pen = (value & 0x10 ? 0x10 : value & 0x0F);
        break;
      case 0x40: /* Select color */
        amstrad_cpc.gate_array.ink[amstrad_cpc.gate_array.pen] = value & 0x1F;
        _ink[amstrad_cpc.gate_array.pen] = _col[amstrad_cpc.gate_array.ink[amstrad_cpc.gate_array.pen]];
        break;
      case 0x80: /* Interrupt control, ROM configuration and screen mode */
        amstrad_cpc.gate_array.rom_cfg = value;
        amstrad_cpc_rom_select();
        break;
      case 0xC0: /* RAM memory management */
        amstrad_cpc.gate_array.ram_cfg = value & 0x07;
        amstrad_cpc_ram_select();
        break;
    }
  }
  if(!(port & 0x4000)) { /* CRTC 6845 (bit 14 = 0; Port BCxx-BFxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Register select (port BCxx) */
        crtc_6845.current = value;
        break;
      case 0x0100: /* Register write (port BDxx) */
        crtc_6845.registers[crtc_6845.current] = value;
        break;
      case 0x0200: /* Dependent of version (port BExx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845) ............. OUT (%04X),%02X (What to do ?)\n", port, value);
        break;
      case 0x0300: /* Register read (port BFxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 read) ........ OUT (%04X),%02X\n (Illegal; read only)\n", port, value);
        break;
    }
  }
  if(!(port & 0x2000)) { /* ROM select (bit 13 = 0; Port DFxx) */
    amstrad_cpc.memory.expansion = value & 0x07;
    amstrad_cpc_rom_select();
  }
  if(!(port & 0x1000)) { /* Printer port (bit 12 = 0; Port EFxx) */
    /*printf("cpc (Printer port) ...... OUT (%04X),%02X\n", port, value);*/
  }
  if(!(port & 0x0800)) { /* PPI 8255 (bit 11 = 0; Port F4xx-F7xx) */
    switch(port & 0x0300) {
      case 0x0000: /* Port A (port F4xx) */
        ppi_8255.port_a = value;
        break;
      case 0x0100: /* Port B (port F5xx) */
        /*ppi_8255.port_b = value;*/
        break;
      case 0x0200: /* Port C (port F6xx) */
        ppi_8255.port_c = value;
        amstrad_cpc.keyboard.row = value & 0x0F;
        break;
      case 0x0300: /* Control register (port F7xx) */
        ppi_8255.control = value;
        break;
    }
  }
  if(!(port & 0x0400)) { /* Expansion (bit 10 = 0; Port F8xx-FBxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Serial port (port F8xx) */
        fprintf(stderr, "amstrad_cpc: (Serial port) ........... OUT (%04X),%02X\n", port, value);
        break;
      case 0x0100: /* (port F9xx) */
        fprintf(stderr, "amstrad_cpc: (F9xx) .................. OUT (%04X),%02X\n", port, value);
        break;
      case 0x0200: /* FDC motor (port FAxx) */
        fdc_765_set_motor(value);
        break;
      case 0x0300: /* FDC register (port FBxx) */
        if(!(port & 0x0001)) { /* status (port FB7E) */
          fprintf(stderr, "amstrad_cpc: (FDC status) ............ OUT (%04X),%02X\n", port, value);
        }
        else { /* data (port FB7F) */
          fprintf(stderr, "amstrad_cpc: (FDC data) .............. OUT (%04X),%02X\n", port, value);
        }
        break;
    }
  }
}

word z80_periodic(void)
{
XtInputMask mask;

  if(amstrad_cpc.gate_array.counter++ >= 51) { /* 300Hz irq */
    amstrad_cpc.gate_array.counter = 0;
    if(amstrad_cpc.cycle++ >= 5) { /* 50Hz irq */
      amstrad_cpc.cycle = 0;
      while(mask = (!paused ? XtAppPending(appcontext) : XtIMAll)) {
        XtAppProcessEvent(appcontext, mask);
      }
      ppi_8255.port_b |= 0x01; /* set VSYNC */
      if(amstrad_cpc.ticks-- <= 0) {
        amstrad_cpc_redraw();
        pause(); /* 50Hz real-time synchronization */
      }
    }
    else {
      ppi_8255.port_b &= 0xFE; /* reset VSYNC */
    }
    return(0x0038); /* DO IRQ */
  }
  return(0xFFFF); /* DO NOTHING */
}

