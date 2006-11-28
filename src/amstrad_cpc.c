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
#include <errno.h>
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#ifdef HAVE_XSHM
#include <X11/extensions/XShm.h>
#endif
#include <Xem/Emulator.h>
#include "amstrad_cpc.h"
#include "xcpc.h"

#define AMSTRAD_CPC_SCR_W 768
#define AMSTRAD_CPC_SCR_H 576

#define SHFT_L_MASK 0x01
#define SHFT_R_MASK 0x02
#define CTRL_L_MASK 0x04
#define CTRL_R_MASK 0x08

#ifdef HAVE_XSHM
static gboolean *cfg_no_xshm  = FALSE;
#endif
static gchar    *cfg_model    = NULL;
static gchar    *cfg_monitor  = NULL;
static gchar    *cfg_keyboard = NULL;
static gchar    *cfg_firmname = NULL;
static gchar    *cfg_sys_rom  = NULL;
static gchar    *cfg_exp_rom[256] = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

static GOptionEntry options[] = {
#ifdef HAVE_XSHM
  { "no-xshm" , 0, 0, G_OPTION_ARG_NONE    , &cfg_no_xshm     , "Don't use the X11-SHM extension"                       , NULL       },
#endif
  { "model"   , 0, 0, G_OPTION_ARG_STRING  , &cfg_model       , "cpc464|cpc664|cpc6128"                                 , "value"    },
  { "monitor" , 0, 0, G_OPTION_ARG_STRING  , &cfg_monitor     , "color|green"                                           , "value"    },
  { "keyboard", 0, 0, G_OPTION_ARG_STRING  , &cfg_keyboard    , "qwerty|azerty"                                         , "value"    },
  { "firmname", 0, 0, G_OPTION_ARG_STRING  , &cfg_firmname    , "isp|triumph|saisho|solavox|awa|schneider|orion|amstrad", "value"    },
  { "sysrom"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_sys_rom     , "32Kb system rom"                                       , "filename" },
  { "rom000"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x0], "16Kb expansion rom #00"                                , "filename" },
  { "rom001"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x1], "16Kb expansion rom #01"                                , "filename" },
  { "rom002"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x2], "16Kb expansion rom #02"                                , "filename" },
  { "rom003"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x3], "16Kb expansion rom #03"                                , "filename" },
  { "rom004"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x4], "16Kb expansion rom #04"                                , "filename" },
  { "rom005"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x5], "16Kb expansion rom #05"                                , "filename" },
  { "rom006"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x6], "16Kb expansion rom #06"                                , "filename" },
  { "rom007"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x7], "16Kb expansion rom #07"                                , "filename" },
  { "rom008"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x8], "16Kb expansion rom #08"                                , "filename" },
  { "rom009"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0x9], "16Kb expansion rom #09"                                , "filename" },
  { "rom010"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xa], "16Kb expansion rom #10"                                , "filename" },
  { "rom011"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xb], "16Kb expansion rom #11"                                , "filename" },
  { "rom012"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xc], "16Kb expansion rom #12"                                , "filename" },
  { "rom013"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xd], "16Kb expansion rom #13"                                , "filename" },
  { "rom014"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xe], "16Kb expansion rom #14"                                , "filename" },
  { "rom015"  , 0, 0, G_OPTION_ARG_FILENAME, &cfg_exp_rom[0xf], "16Kb expansion rom #15"                                , "filename" },
  { NULL } /* end-of-options */
};

static unsigned short hw_palette[32][4] = {
  { 0x8000, 0x8000, 0x8000, 0x8000 }, /* White                        */
  { 0x8000, 0x8000, 0x8000, 0x8000 }, /* White (not official)         */
  { 0x0000, 0xffff, 0x8000, 0xa4dd }, /* Sea Green                    */
  { 0xffff, 0xffff, 0x8000, 0xf168 }, /* Pastel Yellow                */
  { 0x0000, 0x0000, 0x8000, 0x0e97 }, /* Blue                         */
  { 0xffff, 0x0000, 0x8000, 0x5b22 }, /* Purple                       */
  { 0x0000, 0x8000, 0x8000, 0x59ba }, /* Cyan                         */
  { 0xffff, 0x8000, 0x8000, 0xa645 }, /* Pink                         */
  { 0xffff, 0x0000, 0x8000, 0x5b22 }, /* Purple (not official)        */
  { 0xffff, 0xffff, 0x8000, 0xf168 }, /* Pastel Yellow (not official) */
  { 0xffff, 0xffff, 0x0000, 0xe2d0 }, /* Bright Yellow                */
  { 0xffff, 0xffff, 0xffff, 0xffff }, /* Bright White                 */
  { 0xffff, 0x0000, 0x0000, 0x4c8b }, /* Bright Red                   */
  { 0xffff, 0x0000, 0xffff, 0x69ba }, /* Bright Magenta               */
  { 0xffff, 0x8000, 0x0000, 0x97ad }, /* Orange                       */
  { 0xffff, 0x8000, 0xffff, 0xb4dc }, /* Pastel Magenta               */
  { 0x0000, 0x0000, 0x8000, 0x0e97 }, /* Blue (not official)          */
  { 0x0000, 0xffff, 0x8000, 0xa4dd }, /* Sea Green (not official)     */
  { 0x0000, 0xffff, 0x0000, 0x9645 }, /* Bright Green                 */
  { 0x0000, 0xffff, 0xffff, 0xb374 }, /* Bright Cyan                  */
  { 0x0000, 0x0000, 0x0000, 0x0000 }, /* Black                        */
  { 0x0000, 0x0000, 0xffff, 0x1d2f }, /* Bright Blue                  */
  { 0x0000, 0x8000, 0x0000, 0x4b23 }, /* Green                        */
  { 0x0000, 0x8000, 0xffff, 0x6852 }, /* Sky Blue                     */
  { 0x8000, 0x0000, 0x8000, 0x34dd }, /* Magenta                      */
  { 0x8000, 0xffff, 0x8000, 0xcb22 }, /* Pastel Green                 */
  { 0x8000, 0xffff, 0x0000, 0xbc8b }, /* Lime                         */
  { 0x8000, 0xffff, 0xffff, 0xd9ba }, /* Pastel Cyan                  */
  { 0x8000, 0x0000, 0x0000, 0x2645 }, /* Red                          */
  { 0x8000, 0x0000, 0xffff, 0x4374 }, /* Mauve                        */
  { 0x8000, 0x8000, 0x0000, 0x7168 }, /* Yellow                       */
  { 0x8000, 0x8000, 0xffff, 0x8e97 }  /* Pastel Blue                  */
};

AMSTRAD_CPC amstrad_cpc;

static void amstrad_cpc_mem_select(AMSTRAD_CPC *self)
{
  GdevGArray *garray = self->garray;

  switch(garray->ram_cfg) {
    case 0x00:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x04000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x0c000;
      break;
    case 0x01:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x04000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x1c000;
      break;
    case 0x02:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x10000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x14000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x18000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x1c000;
      break;
    case 0x03:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x04000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x1c000;
      break;
    case 0x04:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x10000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x0c000;
      break;
    case 0x05:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x14000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x0c000;
      break;
    case 0x06:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x18000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x0c000;
      break;
    case 0x07:
      self->rd_bank[0] = self->wr_bank[0] = self->memory.total_ram + 0x00000;
      self->rd_bank[1] = self->wr_bank[1] = self->memory.total_ram + 0x1c000;
      self->rd_bank[2] = self->wr_bank[2] = self->memory.total_ram + 0x08000;
      self->rd_bank[3] = self->wr_bank[3] = self->memory.total_ram + 0x0c000;
      break;
    default:
      (void) fprintf(stderr, "RAM-SELECT: Bad Configuration (%02x) !!\n", garray->ram_cfg);
      (void) fflush(stderr);
      break;
  }
  if((garray->rom_cfg & 0x04) == 0) {
    if(self->memory.lower_rom != NULL) {
      self->rd_bank[0] = self->memory.lower_rom->data;
    }
  }
  if((garray->rom_cfg & 0x08) == 0) {
    if(self->memory.upper_rom != NULL) {
      self->rd_bank[3] = self->memory.upper_rom->data;
    }
    if(self->memory.expan_rom[self->memory.expansion] != NULL) {
      self->rd_bank[3] = self->memory.expan_rom[self->memory.expansion]->data;
    }
  }
}

static void amstrad_cpc_init_palette(AMSTRAD_CPC *self)
{
  int type = 0, ix;
  XColor xcolor;

  if(cfg_monitor != NULL) {
    if(strcmp("color", cfg_monitor) == 0) {
      type = 0;
    }
    if(strcmp("green", cfg_monitor) == 0) {
      type = 1;
    }
  }
  for(ix = 0; ix < 32; ix++) {
    if(type == 1) {
      xcolor.flags = DoRed | DoGreen | DoBlue;
      xcolor.red   = 0;
      xcolor.green = hw_palette[ix][3];
      xcolor.blue  = 0;
      xcolor.pad   = 0;
    }
    else {
      xcolor.flags = DoRed | DoGreen | DoBlue;
      xcolor.red   = hw_palette[ix][0];
      xcolor.green = hw_palette[ix][1];
      xcolor.blue  = hw_palette[ix][2];
      xcolor.pad   = 0;
    }
    if(XAllocColor(DisplayOfScreen(self->screen), DefaultColormapOfScreen(self->screen), &xcolor) == False) {
      (void) fprintf(stderr, "cannot allocate color ... %04x/%04x/%04x\n", xcolor.red, xcolor.green, xcolor.blue);
      (void) fflush(stderr);
    }
    self->palette[ix] = xcolor.pixel;
  }
}

static void amstrad_cpc_render00(AMSTRAD_CPC *self, XtPointer user)
{
}

static void amstrad_cpc_render08(AMSTRAD_CPC *self, XtPointer user)
{
  GdevMC6845 *mc6845 = self->mc6845;
  GdevGArray *garray = self->garray;
  unsigned int sa = ((mc6845->reg_file[12] << 8) | mc6845->reg_file[13]);
  unsigned int hd = mc6845->reg_file[1];
  unsigned int hp = ((AMSTRAD_CPC_SCR_W >> 0) - (hd << 4)) >> 1;
  unsigned int mr = mc6845->reg_file[9] + 1;
  unsigned int vt = mc6845->reg_file[4] + 1;
  unsigned int vd = mc6845->reg_file[6];
  unsigned int vp = ((AMSTRAD_CPC_SCR_H >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint8 *dst = (guint8 *) self->ximage->data, *nxt = dst;
  guint8 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint8 data;

#ifdef HAVE_XSHM
  if(self->useshm != False) {
    (void) XSync(DisplayOfScreen(self->screen), False);
  }
#endif
  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[4];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += AMSTRAD_CPC_SCR_W;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x01:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x02:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
      }
      dst = nxt; sl++;
    }
    sa += hd;
  }
  sl = &self->scanline[(vd * mr) + (0 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(self->window != None) {
#ifdef HAVE_XSHM
    if(self->useshm != False) {
      (void) XShmPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H, False);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
    else {
      (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
#else
    (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
    (void) XFlush(DisplayOfScreen(self->screen));
#endif
  }
}

static void amstrad_cpc_render16(AMSTRAD_CPC *self, XtPointer user)
{
  GdevMC6845 *mc6845 = self->mc6845;
  GdevGArray *garray = self->garray;
  unsigned int sa = ((mc6845->reg_file[12] << 8) | mc6845->reg_file[13]);
  unsigned int hd = mc6845->reg_file[1];
  unsigned int hp = ((AMSTRAD_CPC_SCR_W >> 0) - (hd << 4)) >> 1;
  unsigned int mr = mc6845->reg_file[9] + 1;
  unsigned int vt = mc6845->reg_file[4] + 1;
  unsigned int vd = mc6845->reg_file[6];
  unsigned int vp = ((AMSTRAD_CPC_SCR_H >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint16 *dst = (guint16 *) self->ximage->data, *nxt = dst;
  guint16 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint8 data;

#ifdef HAVE_XSHM
  if(self->useshm != False) {
    (void) XSync(DisplayOfScreen(self->screen), False);
  }
#endif
  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[4];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += AMSTRAD_CPC_SCR_W;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x01:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x02:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
      }
      dst = nxt; sl++;
    }
    sa += hd;
  }
  sl = &self->scanline[(vd * mr) + (0 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(self->window != None) {
#ifdef HAVE_XSHM
    if(self->useshm != False) {
      (void) XShmPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H, False);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
    else {
      (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
#else
    (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
    (void) XFlush(DisplayOfScreen(self->screen));
#endif
  }
}

static void amstrad_cpc_render32(AMSTRAD_CPC *self, XtPointer user)
{
  GdevMC6845 *mc6845 = self->mc6845;
  GdevGArray *garray = self->garray;
  unsigned int sa = ((mc6845->reg_file[12] << 8) | mc6845->reg_file[13]);
  unsigned int hd = mc6845->reg_file[1];
  unsigned int hp = ((AMSTRAD_CPC_SCR_W >> 0) - (hd << 4)) >> 1;
  unsigned int mr = mc6845->reg_file[9] + 1;
  unsigned int vt = mc6845->reg_file[4] + 1;
  unsigned int vd = mc6845->reg_file[6];
  unsigned int vp = ((AMSTRAD_CPC_SCR_H >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint32 *dst = (guint32 *) self->ximage->data, *nxt = dst;
  guint32 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint8 data;

#ifdef HAVE_XSHM
  if(self->useshm != False) {
    (void) XSync(DisplayOfScreen(self->screen), False);
  }
#endif
  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[4];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += AMSTRAD_CPC_SCR_W;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode0[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 4;
            pixel = sl->ink[data & 0x0f];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x01:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode1[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 2;
            pixel = sl->ink[data & 0x03];
            *dst++ = *nxt++ = pixel;
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
        case 0x02:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 0) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 0 */
            data = garray->mode2[self->wr_bank[addr >> 14][(addr | 1) & 0x3fff]];
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 1 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 2 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 3 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 4 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 5 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 6 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
            /* pixel 7 */
            data >>= 1;
            pixel = sl->ink[data & 0x01];
            *dst++ = *nxt++ = pixel;
          }
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          break;
      }
      dst = nxt; sl++;
    }
    sa += hd;
  }
  sl = &self->scanline[(vd * mr) + (0 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += AMSTRAD_CPC_SCR_W;
    pixel = sl->ink[16];
    for(cx = 0; cx < AMSTRAD_CPC_SCR_W; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(self->window != None) {
#ifdef HAVE_XSHM
    if(self->useshm != False) {
      (void) XShmPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H, False);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
    else {
      (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
      (void) XFlush(DisplayOfScreen(self->screen));
    }
#else
    (void) XPutImage(DisplayOfScreen(self->screen), self->window, DefaultGCOfScreen(self->screen), self->ximage, 0, 0, 0, 0, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
    (void) XFlush(DisplayOfScreen(self->screen));
#endif
  }
}

static void amstrad_cpc_qwerty_hnd(AMSTRAD_CPC *self, XEvent *xevent)
{
  KeySym keysym;
  guint8 line = 0x09;
  guint8 mask = 0x40;
  guint8 mods = 0x00;

  if((self->keyboard.mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((self->keyboard.mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, NULL, 0, &keysym, NULL);
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
          self->keyboard.mods |=  SHFT_L_MASK;
        }
        else {
          self->keyboard.mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  SHFT_R_MASK;
        }
        else {
          self->keyboard.mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  CTRL_L_MASK;
        }
        else {
          self->keyboard.mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  CTRL_R_MASK;
        }
        else {
          self->keyboard.mods &= ~CTRL_R_MASK;
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
  if((self->keyboard.mods & SHFT_L_MASK) != 0) {
    self->keyboard.bits[0x02] &= ~0x20;
  }
  else {
    self->keyboard.bits[0x02] |=  0x20;
  }
  if((self->keyboard.mods & CTRL_L_MASK) != 0) {
    self->keyboard.bits[0x02] &= ~0x80;
  }
  else {
    self->keyboard.bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      self->keyboard.bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      self->keyboard.bits[0x02] &= ~0x80;
    }
    self->keyboard.bits[line] &= ~mask;
  }
  else {
    self->keyboard.bits[line] |=  mask;
  }
}

static void amstrad_cpc_azerty_hnd(AMSTRAD_CPC *self, XEvent *xevent)
{
  KeySym keysym;
  guint8 line = 0x09;
  guint8 mask = 0x40;
  guint8 mods = 0x00;

  if((self->keyboard.mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((self->keyboard.mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, NULL, 0, &keysym, NULL);
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
          self->keyboard.mods |=  SHFT_L_MASK;
        }
        else {
          self->keyboard.mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  SHFT_R_MASK;
        }
        else {
          self->keyboard.mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  CTRL_L_MASK;
        }
        else {
          self->keyboard.mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          self->keyboard.mods |=  CTRL_R_MASK;
        }
        else {
          self->keyboard.mods &= ~CTRL_R_MASK;
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
  if((self->keyboard.mods & SHFT_L_MASK) != 0) {
    self->keyboard.bits[0x02] &= ~0x20;
  }
  else {
    self->keyboard.bits[0x02] |=  0x20;
  }
  if((self->keyboard.mods & CTRL_L_MASK) != 0) {
    self->keyboard.bits[0x02] &= ~0x80;
  }
  else {
    self->keyboard.bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      self->keyboard.bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      self->keyboard.bits[0x02] &= ~0x80;
    }
    self->keyboard.bits[line] &= ~mask;
  }
  else {
    self->keyboard.bits[line] |=  mask;
  }
}

void amstrad_cpc_reset(void)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  int ix;

  self->memory.expansion = 0x00;
  self->keyboard.mods = 0x00;
  self->keyboard.line = 0x00;
  for(ix = 0; ix < 16; ix++) {
    self->keyboard.bits[ix] = 0xff;
  }
  self->beam.x = 0;
  self->beam.y = 0;

  gdev_device_reset(GDEV_DEVICE(self->z80cpu));
  gdev_device_reset(GDEV_DEVICE(self->garray));
  gdev_device_reset(GDEV_DEVICE(self->mc6845));
  gdev_device_reset(GDEV_DEVICE(self->ay8910));
  gdev_device_reset(GDEV_DEVICE(self->upd765));
  gdev_device_reset(GDEV_DEVICE(self->i8255));
  amstrad_cpc_mem_select(self);
  (void) gettimeofday(&self->timer1, NULL);
  (void) gettimeofday(&self->timer2, NULL);
}

int amstrad_cpc_parse(int *argc, char ***argv)
{
  GOptionContext *ctxt = g_option_context_new(NULL);

  g_option_context_add_main_entries(ctxt, options, NULL);
  if(g_option_context_parse(ctxt, argc, argv, NULL) != FALSE) {
  }
  else {
  }
  g_option_context_free(ctxt);
  ctxt = (GOptionContext *) NULL;
  return(EXIT_SUCCESS);
}

void amstrad_cpc_load_snapshot(char *filename)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  FILE *file;
  guint8 buffer[256], *bufptr = buffer;
  int ramsize= 0;
  int ix;

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
  self->z80cpu->AF.B.l = *bufptr++;
  self->z80cpu->AF.B.h = *bufptr++;
  self->z80cpu->BC.B.l = *bufptr++;
  self->z80cpu->BC.B.h = *bufptr++;
  self->z80cpu->DE.B.l = *bufptr++;
  self->z80cpu->DE.B.h = *bufptr++;
  self->z80cpu->HL.B.l = *bufptr++;
  self->z80cpu->HL.B.h = *bufptr++;
  self->z80cpu->IR.B.l = *bufptr++;
  self->z80cpu->IR.B.h = *bufptr++;
  self->z80cpu->IFF = (*bufptr++ != 0 ? self->z80cpu->IFF | IFF_1 : self->z80cpu->IFF & (~IFF_2));
  self->z80cpu->IFF = (*bufptr++ != 0 ? self->z80cpu->IFF | IFF_1 : self->z80cpu->IFF & (~IFF_2));
  self->z80cpu->IX.B.l = *bufptr++;
  self->z80cpu->IX.B.h = *bufptr++;
  self->z80cpu->IY.B.l = *bufptr++;
  self->z80cpu->IY.B.h = *bufptr++;
  self->z80cpu->SP.B.l = *bufptr++;
  self->z80cpu->SP.B.h = *bufptr++;
  self->z80cpu->PC.B.l = *bufptr++;
  self->z80cpu->PC.B.h = *bufptr++;
  switch(*bufptr++) {
    case 1:
      self->z80cpu->IFF = (self->z80cpu->IFF | IFF_IM1) & ~(IFF_IM2);
      break;
    case 2:
      self->z80cpu->IFF = (self->z80cpu->IFF | IFF_IM2) & ~(IFF_IM1);
      break;
    default:
      self->z80cpu->IFF = (self->z80cpu->IFF) & ~(IFF_IM1 | IFF_IM2);
      break;
  }
  self->z80cpu->AF1.B.l = *bufptr++;
  self->z80cpu->AF1.B.h = *bufptr++;
  self->z80cpu->BC1.B.l = *bufptr++;
  self->z80cpu->BC1.B.h = *bufptr++;
  self->z80cpu->DE1.B.l = *bufptr++;
  self->z80cpu->DE1.B.h = *bufptr++;
  self->z80cpu->HL1.B.l = *bufptr++;
  self->z80cpu->HL1.B.h = *bufptr++;
  self->garray->pen = *bufptr++;
  for(ix = 0; ix < 17; ix++) {
    self->garray->ink[ix] = *bufptr++;
  }
  self->garray->rom_cfg = *bufptr++;
  self->garray->ram_cfg = *bufptr++;
  amstrad_cpc_mem_select(self);
  self->mc6845->addr_reg = *bufptr++;
  for(ix = 0; ix < 18; ix++) {
    self->mc6845->reg_file[ix] = *bufptr++;
  }
  self->memory.expansion = *bufptr++;
  self->i8255->port_a = *bufptr++;
  self->i8255->port_b = *bufptr++;
  self->i8255->port_c = *bufptr++;
  self->i8255->control = *bufptr++;
  self->ay8910->addr_reg = *bufptr++;
  for(ix = 0; ix < 16; ix++) {
    self->ay8910->reg_file[ix] = *bufptr++;
  }
  ramsize |= *bufptr++ << 0;
  ramsize |= *bufptr++ << 8;
  if(ramsize > self->ramsize) {
    fprintf(stderr, "snapshot file too large (%d Kb)\n", ramsize);
    amstrad_cpc_reset();
    fclose(file);
    return;
  }
  fread(self->memory.total_ram, 1, ramsize * 1024, file);
  fclose(file);
}

void amstrad_cpc_save_snapshot(char *filename)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  FILE *file;
  guint8 buffer[256], *bufptr = buffer;
  int ramsize = 0;
  int ix;

  if((file = fopen(filename, "w")) == NULL) {
    perror("amstrad_cpc");
    return;
  }
  memcpy(bufptr, "MV - SNA", 8); bufptr += 8;
  memset(bufptr, 0, 8); bufptr += 8; /* not used */
  *bufptr++ = 1; /* snapshot version */
  *bufptr++ = self->z80cpu->AF.B.l;
  *bufptr++ = self->z80cpu->AF.B.h;
  *bufptr++ = self->z80cpu->BC.B.l;
  *bufptr++ = self->z80cpu->BC.B.h;
  *bufptr++ = self->z80cpu->DE.B.l;
  *bufptr++ = self->z80cpu->DE.B.h;
  *bufptr++ = self->z80cpu->HL.B.l;
  *bufptr++ = self->z80cpu->HL.B.h;
  *bufptr++ = self->z80cpu->IR.B.l;
  *bufptr++ = self->z80cpu->IR.B.h;
  *bufptr++ = (self->z80cpu->IFF & IFF_1 ? 0x01 : 0x00);
  *bufptr++ = (self->z80cpu->IFF & IFF_2 ? 0x01 : 0x00);
  *bufptr++ = self->z80cpu->IX.B.l;
  *bufptr++ = self->z80cpu->IX.B.h;
  *bufptr++ = self->z80cpu->IY.B.l;
  *bufptr++ = self->z80cpu->IY.B.h;
  *bufptr++ = self->z80cpu->SP.B.l;
  *bufptr++ = self->z80cpu->SP.B.h;
  *bufptr++ = self->z80cpu->PC.B.l;
  *bufptr++ = self->z80cpu->PC.B.h;
  switch(self->z80cpu->IFF & (IFF_IM1 | IFF_IM2)) {
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
  *bufptr++ = self->z80cpu->AF1.B.l;
  *bufptr++ = self->z80cpu->AF1.B.h;
  *bufptr++ = self->z80cpu->BC1.B.l;
  *bufptr++ = self->z80cpu->BC1.B.h;
  *bufptr++ = self->z80cpu->DE1.B.l;
  *bufptr++ = self->z80cpu->DE1.B.h;
  *bufptr++ = self->z80cpu->HL1.B.l;
  *bufptr++ = self->z80cpu->HL1.B.h;
  *bufptr++ = self->garray->pen;
  for(ix = 0; ix < 17; ix++) {
    *bufptr++ = self->garray->ink[ix];
  }
  *bufptr++ = self->garray->rom_cfg;
  *bufptr++ = self->garray->ram_cfg;
  *bufptr++ = self->mc6845->addr_reg;
  for(ix = 0; ix < 18; ix++) {
    *bufptr++ = self->mc6845->reg_file[ix];
  }
  *bufptr++ = self->memory.expansion;
  *bufptr++ = self->i8255->port_a;
  *bufptr++ = self->i8255->port_b;
  *bufptr++ = self->i8255->port_c;
  *bufptr++ = self->i8255->control;
  *bufptr++ = self->ay8910->addr_reg;
  for(ix = 0; ix < 16; ix++) {
    *bufptr++ = self->ay8910->reg_file[ix];
  }
  *bufptr++ = (self->ramsize >> 0) & 0xff;
  *bufptr++ = (self->ramsize >> 8) & 0xff;
  memset(bufptr, 0, 147);
  bufptr += 147;
  fwrite(buffer, 1, 256, file);
  fwrite(self->memory.total_ram, 1, self->ramsize * 1024, file);
  fclose(file);
}

/**
 * amstrad_cpc::z80cpu::mm_rd
 *
 * @param z80cpu
 * @param addr
 *
 * @return
 */
static guint8 z80cpu_mm_rd(GdevZ80CPU *z80cpu, guint16 addr)
{
  return(amstrad_cpc.rd_bank[addr >> 14][addr & 0x3fff]);
}

/**
 * amstrad_cpc::z80cpu::mm_wr
 *
 * @param z80cpu
 * @param addr
 * @param data
 */
static void z80cpu_mm_wr(GdevZ80CPU *z80cpu, guint16 addr, guint8 data)
{
  amstrad_cpc.wr_bank[addr >> 14][addr & 0x3fff] = data;
}

/**
 * amstrad_cpc::z80cpu::io_rd
 *
 * @param z80cpu
 * @param port
 *
 * @return
 */
static guint8 z80cpu_io_rd(GdevZ80CPU *z80cpu, guint16 port)
{
  guint8 data = 0x00;

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
        data = amstrad_cpc.mc6845->reg_file[amstrad_cpc.mc6845->addr_reg];
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
        amstrad_cpc.i8255->port_a = amstrad_cpc.keyboard.bits[amstrad_cpc.keyboard.line];
        data = amstrad_cpc.i8255->port_a;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        amstrad_cpc.i8255->port_b = ((0                         & 0x01) << 7)
                                  | ((1                         & 0x01) << 6)
                                  | ((1                         & 0x01) << 5)
                                  | ((amstrad_cpc.refresh       & 0x01) << 4)
                                  | ((amstrad_cpc.firmname      & 0x07) << 1)
                                  | ((amstrad_cpc.mc6845->v_syn & 0x01) << 0);
        data = amstrad_cpc.i8255->port_b;
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        data = amstrad_cpc.i8255->port_c;
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
        (void) fprintf(stderr, "IO_RD[0x%04x]: FDC-765      [---- Illegal ----]\n", port);
        (void) fflush(stderr);
        break;
      case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
        GDEV_FDC765_GET_CLASS(amstrad_cpc.upd765->fdc)->rstat(amstrad_cpc.upd765->fdc, &data);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        GDEV_FDC765_GET_CLASS(amstrad_cpc.upd765->fdc)->rdata(amstrad_cpc.upd765->fdc, &data);
        break;
    }
  }
  return(data);
}

/**
 * amstrad_cpc::z80cpu::io_wr
 *
 * @param z80cpu
 * @param port
 * @param data
 */
static void z80cpu_io_wr(GdevZ80CPU *z80cpu, guint16 port, guint8 data)
{
  /* Gate-Array   [0-------xxxxxxxx] [0x7fxx] */
  if((port & 0x8000) == 0) {
    switch((data >> 6) & 3) {
      case 0: /* Select pen */
        amstrad_cpc.garray->pen = (data & 0x10 ? 0x10 : data & 0x0f);
        break;
      case 1: /* Select color */
        amstrad_cpc.garray->ink[amstrad_cpc.garray->pen] = data & 0x1f;
        break;
      case 2: /* Interrupt control, ROM configuration and screen mode */
        if((data & 0x10) != 0) {
          amstrad_cpc.garray->counter = 0;
          amstrad_cpc.garray->gen_irq = 0;
        }
        amstrad_cpc.garray->rom_cfg = data & 0x1f;
        amstrad_cpc_mem_select(&amstrad_cpc);
        break;
      case 3: /* RAM memory management */
        amstrad_cpc.garray->ram_cfg = data & 0x3f;
        amstrad_cpc_mem_select(&amstrad_cpc);
        break;
    }
  }
  /* CRTC-6845    [-0------xxxxxxxx] [0xbfxx] */
  if((port & 0x4000) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
        amstrad_cpc.mc6845->addr_reg = data;
        break;
      case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
        amstrad_cpc.mc6845->reg_file[amstrad_cpc.mc6845->addr_reg] = data;
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
    amstrad_cpc_mem_select(&amstrad_cpc);
  }
  /* Printer Port [---0----xxxxxxxx] [0xefxx] */
  if((port & 0x1000) == 0) {
  }
  /* PPI-8255     [----0---xxxxxxxx] [0xf7xx] */
  if((port & 0x0800) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
        amstrad_cpc.i8255->port_a = data;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        /*amstrad_cpc.i8255->port_b = data;*/
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        amstrad_cpc.i8255->port_c = data;
        amstrad_cpc.keyboard.line = data & 0x0F;
        break;
      case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
        amstrad_cpc.i8255->control = data;
        break;
    }
  }
  /* FDC-765      [-----0--0xxxxxxx] [0xfb7f] */
  if((port & 0x0480) == 0) {
    switch(((port >> 7) & 2) | ((port >> 0) & 1)) {
      case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
        gdev_upd765_set_motor(amstrad_cpc.upd765, ((data & 1) << 1) | ((data & 1) << 0));
        break;
      case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
        gdev_upd765_set_motor(amstrad_cpc.upd765, ((data & 1) << 1) | ((data & 1) << 0));
        break;
      case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
        GDEV_FDC765_GET_CLASS(amstrad_cpc.upd765->fdc)->wstat(amstrad_cpc.upd765->fdc, &data);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        GDEV_FDC765_GET_CLASS(amstrad_cpc.upd765->fdc)->wdata(amstrad_cpc.upd765->fdc, &data);
        break;
    }
  }
}

static void mc6845_hsync(GdevMC6845 *mc6845)
{
  if(mc6845->h_syn != 0) {
    if(amstrad_cpc.garray->delayed > 0) {
      if(--amstrad_cpc.garray->delayed == 0) {
        if((amstrad_cpc.garray->counter & 32) != 0) {
          amstrad_cpc.garray->gen_irq = 1;
        }
        else {
          amstrad_cpc.garray->gen_irq = 0;
        }
        amstrad_cpc.garray->counter = 0;
      }
    }
    else {
      if(++amstrad_cpc.garray->counter == 52) {
        amstrad_cpc.garray->counter = 0;
        amstrad_cpc.garray->gen_irq = 1;
      }
    }
  }
}

static void mc6845_vsync(GdevMC6845 *mc6845)
{
  if(mc6845->v_syn != 0) {
    amstrad_cpc.garray->delayed = 2;
  }
}

void amstrad_cpc_start_handler(Widget widget, XtPointer data)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  int ix;
  FILE *file;
  Arg arglist[8];
  Cardinal argcount;

  /* Model */
  if(cfg_model == NULL) {
    if(cfg_sys_rom == NULL) {
      cfg_sys_rom = g_build_filename(ROMSDIR, "cpc6128.rom", NULL);
    }
    if(cfg_exp_rom[7] == NULL) {
      cfg_exp_rom[7] = g_build_filename(ROMSDIR, "amsdos.rom", NULL);
    }
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
    self->mouse_hnd = NULL;
    self->ramsize   = 128;
    self->refresh   = 1;
    self->firmname  = 7;
  }
  else if(strcmp("cpc464", cfg_model) == 0) {
    if(cfg_sys_rom == NULL) {
      cfg_sys_rom = g_build_filename(ROMSDIR, "cpc464.rom", NULL);
    }
    if(cfg_exp_rom[7] == NULL) {
      cfg_exp_rom[7] = NULL;
    }
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
    self->mouse_hnd = NULL;
    self->ramsize   = 64;
    self->refresh   = 1;
    self->firmname  = 7;
  }
  else if(strcmp("cpc664", cfg_model) == 0) {
    if(cfg_sys_rom == NULL) {
      cfg_sys_rom = g_build_filename(ROMSDIR, "cpc664.rom", NULL);
    }
    if(cfg_exp_rom[7] == NULL) {
      cfg_exp_rom[7] = g_build_filename(ROMSDIR, "amsdos.rom", NULL);
    }
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
    self->mouse_hnd = NULL;
    self->ramsize   = 64;
    self->refresh   = 1;
    self->firmname  = 7;
  }
  else if(strcmp("cpc6128", cfg_model) == 0) {
    if(cfg_sys_rom == NULL) {
      cfg_sys_rom = g_build_filename(ROMSDIR, "cpc6128.rom", NULL);
    }
    if(cfg_exp_rom[7] == NULL) {
      cfg_exp_rom[7] = g_build_filename(ROMSDIR, "amsdos.rom", NULL);
    }
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
    self->mouse_hnd = NULL;
    self->ramsize   = 128;
    self->refresh   = 1;
    self->firmname  = 7;
  }
  else {
    if(cfg_sys_rom == NULL) {
      cfg_sys_rom = g_build_filename(ROMSDIR, "cpc6128.rom", NULL);
    }
    if(cfg_exp_rom[7] == NULL) {
      cfg_exp_rom[7] = g_build_filename(ROMSDIR, "amsdos.rom", NULL);
    }
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
    self->mouse_hnd = NULL;
    self->ramsize   = 128;
    self->refresh   = 1;
    self->firmname  = 7;
  }
  /* Keyboard handler */
  if(cfg_keyboard == NULL) {
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
  }
  else if(strcmp("qwerty", cfg_keyboard) == 0) {
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
  }
  else if(strcmp("azerty", cfg_keyboard) == 0) {
    self->keybd_hnd = amstrad_cpc_azerty_hnd;
  }
  else {
    self->keybd_hnd = amstrad_cpc_qwerty_hnd;
  }
  /* FirmName */
  if(cfg_firmname == NULL) {
    self->firmname = 7;
  }
  else if(strcmp("isp", cfg_firmname) == 0) {
    self->firmname = 0;
  }
  else if(strcmp("triumph", cfg_firmname) == 0) {
    self->firmname = 1;
  }
  else if(strcmp("saisho", cfg_firmname) == 0) {
    self->firmname = 2;
  }
  else if(strcmp("solavox", cfg_firmname) == 0) {
    self->firmname = 3;
  }
  else if(strcmp("awa", cfg_firmname) == 0) {
    self->firmname = 4;
  }
  else if(strcmp("schneider", cfg_firmname) == 0) {
    self->firmname = 5;
  }
  else if(strcmp("orion", cfg_firmname) == 0) {
    self->firmname = 6;
  }
  else if(strcmp("amstrad", cfg_firmname) == 0) {
    self->firmname = 7;
  }
  else {
    self->firmname = 7;
  }
  /* XXX */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNwidth,  AMSTRAD_CPC_SCR_W); argcount++;
  XtSetArg(arglist[argcount], XtNheight, AMSTRAD_CPC_SCR_H); argcount++;
  XtSetValues(widget, arglist, argcount);

  self->screen = XtScreen(widget);
  self->window = XtWindow(widget);
  self->ximage = NULL;
  self->useshm = False;
#ifdef HAVE_XSHM
  if(cfg_no_xshm == FALSE) {
    if(XShmQueryExtension(DisplayOfScreen(self->screen)) != False) {
      int major = 0;
      int minor = 0;
      Bool shpix = False;
      if(XShmQueryVersion(DisplayOfScreen(self->screen), &major, &minor, &shpix) != False) {
        XShmSegmentInfo *shm_info = g_new(XShmSegmentInfo, 1);
        shm_info->shmseg   = None;
        shm_info->shmid    = -1;
        shm_info->shmaddr  = (char *) -1;
        shm_info->readOnly = False;
        self->ximage = XShmCreateImage(DisplayOfScreen(self->screen), DefaultVisualOfScreen(self->screen), DefaultDepthOfScreen(self->screen), ZPixmap, NULL, shm_info, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H);
        shm_info->shmid    = shmget(IPC_PRIVATE, self->ximage->bytes_per_line * self->ximage->height, IPC_CREAT | 0600);
        if(shm_info->shmid != -1) {
          shm_info->shmaddr  = (char *) shmat(shm_info->shmid, NULL, 0);
          if(shm_info->shmaddr != (char *) -1) {
            self->ximage->data = shm_info->shmaddr;
            (void) XShmAttach(DisplayOfScreen(self->screen), shm_info);
            (void) XSync(DisplayOfScreen(self->screen), False);
            (void) shmctl(shm_info->shmid, IPC_RMID, NULL);
            self->useshm = True;
          }
        }
      }
    }
  }
#endif
  if(self->ximage == NULL) {
    self->ximage = XCreateImage(DisplayOfScreen(self->screen), DefaultVisualOfScreen(self->screen), DefaultDepthOfScreen(self->screen), ZPixmap, 0, NULL, AMSTRAD_CPC_SCR_W, AMSTRAD_CPC_SCR_H, 8, 0);
    self->ximage->data = (char *) XtMalloc(self->ximage->bytes_per_line * self->ximage->height);
    (void) memset(self->ximage->data, 0, self->ximage->bytes_per_line * self->ximage->height);
  }
  switch(self->ximage->bits_per_pixel) {
    case 8:
      self->paint_hnd = amstrad_cpc_render08;
      break;
    case 16:
      self->paint_hnd = amstrad_cpc_render16;
      break;
    case 32:
      self->paint_hnd = amstrad_cpc_render32;
      break;
    default:
      self->paint_hnd = amstrad_cpc_render00;
      break;
  }
  amstrad_cpc_init_palette(self);
  if((self->memory.total_ram = (guint8 *) malloc(self->ramsize * 1024)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  /* Load System ROM */
  self->memory.lower_rom = gdev_cpcmem_new_from_file(cfg_sys_rom, 0x0000);
  self->memory.upper_rom = gdev_cpcmem_new_from_file(cfg_sys_rom, 0x4000);
  /* Load Expansion ROMs */
  for(ix = 0; ix < 256; ix++) {
    if(cfg_exp_rom[ix] != NULL) {
      self->memory.expan_rom[ix] = gdev_cpcmem_new_from_file(cfg_exp_rom[ix], 0);
    }
    else {
      self->memory.expan_rom[ix] = NULL;
    }
  }
  switch(self->refresh) {
    default:
    case 1:
      self->cpu_period = (4000000 * 64) / 1000000;
      break;
    case 0:
      self->cpu_period = (4000000 * 53) / 1000000;
      break;
  }
  self->z80cpu = gdev_z80cpu_new();
  self->garray = gdev_garray_new();
  self->mc6845 = gdev_mc6845_new();
  self->ay8910 = gdev_ay8910_new();
  self->upd765 = gdev_upd765_new();
  self->i8255  = gdev_i8255_new();
  /* XXX */
  self->z80cpu->mm_rd = z80cpu_mm_rd;
  self->z80cpu->mm_wr = z80cpu_mm_wr;
  self->z80cpu->io_rd = z80cpu_io_rd;
  self->z80cpu->io_wr = z80cpu_io_wr;
  self->mc6845->hsync = mc6845_hsync;
  self->mc6845->vsync = mc6845_vsync;
  gdev_upd765_set_fdc(self->upd765, gdev_fdc765_new());
  gdev_upd765_set_fdd(self->upd765, gdev_fdd765_new(), 0);
  gdev_upd765_set_fdd(self->upd765, gdev_fdd765_new(), 1);
  amstrad_cpc_reset();
  (void) gettimeofday(&self->timer1, NULL);
  (void) gettimeofday(&self->timer2, NULL);
}

void amstrad_cpc_clock_handler(Widget widget, XtPointer data)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  GdevDeviceClass *z80cpu_class = GDEV_DEVICE_GET_CLASS((GdevDevice *) self->z80cpu);
  GdevDeviceClass *mc6845_class = GDEV_DEVICE_GET_CLASS((GdevDevice *) self->mc6845);
  static int num_frames = 0;
  static int drw_frames = 0;
  long delay, ix;
  int scanline = 0;

  self->beam.y = 0;
  do {
    self->scanline[self->beam.y].mode = self->garray->rom_cfg & 0x03;
    for(ix = 0; ix < 17; ix++) {
      self->scanline[self->beam.y].ink[ix] = self->palette[self->garray->ink[ix]];
    }
    for(ix = 0; ix < self->cpu_period; ix += 4) {
      (*mc6845_class->clock)((GdevDevice *) self->mc6845);
      if(self->garray->gen_irq != 0) {
        if((self->z80cpu->IFF & IFF_1) != 0) {
          gdev_z80cpu_intr(self->z80cpu, INT_RST38);
          self->garray->counter &= 31;
          self->garray->gen_irq  = 0;
        }
      }
      if((self->z80cpu->TStates += 4) > 0) {
        long TStates = self->z80cpu->TStates;
        (*z80cpu_class->clock)((GdevDevice *) self->z80cpu);
        self->z80cpu->TStates = TStates - ((TStates - self->z80cpu->TStates) + 3 & (~4));
      }
    }
    if(++self->beam.y > 311) {
      self->beam.y = 311;
    }
  } while(++scanline < 312);
  (void) gettimeofday(&self->timer2, NULL);
  delay = ((long) (self->timer2.tv_sec  -  self->timer1.tv_sec) * 1000)
        + ((long) (self->timer2.tv_usec - self->timer1.tv_usec) / 1000);
  if(delay >= 1000) {
    *(&self->timer1) = *(&self->timer2); delay = 0;
  }
  switch(self->refresh) {
    case 1:
      if((delay >= 0) && (delay <= 20)) {
        (*self->paint_hnd)(self, NULL); drw_frames++;
      }
      if((self->timer1.tv_usec += 20000) >= 1000000) {
        self->timer1.tv_usec -= 1000000; self->timer1.tv_sec++;
      }
      if(++num_frames == 50) {
        (void) printf("%2d frames -- %2d fps\r", num_frames, drw_frames);
        (void) fflush(stdout); num_frames = drw_frames = 0;
      }
      break;
    case 0:
      if((delay >= 0) && (delay <= 16)) {
        (*self->paint_hnd)(self, NULL); drw_frames++;
      }
      if((self->timer1.tv_usec += 16667) >= 1000000) {
        self->timer1.tv_usec -= 1000000; self->timer1.tv_sec++;
      }
      if(++num_frames == 50) {
        (void) printf("%2d frames -- %2d fps\r", num_frames, drw_frames);
        (void) fflush(stdout); num_frames = drw_frames = 0;
      }
      break;
  }
  delay = ((long) (self->timer1.tv_sec  -  self->timer2.tv_sec) * 1000)
        + ((long) (self->timer1.tv_usec - self->timer2.tv_usec) / 1000);
  if(delay > 0) {
    *((unsigned long *) data) = (unsigned long) (delay - 1);
  }
  else {
    *((unsigned long *) data) = (unsigned long) (1);
  }
}

void amstrad_cpc_close_handler(Widget widget, XtPointer data)
{
  AMSTRAD_CPC *self = &amstrad_cpc;
  int ix;

  if(self->ximage != NULL) {
#ifdef HAVE_XSHM
    if(self->useshm != False) {
      XShmSegmentInfo *shm_info = (XShmSegmentInfo *) self->ximage->obdata;
      (void) XShmDetach(DisplayOfScreen(self->screen), shm_info);
      (void) XSync(DisplayOfScreen(self->screen), False);
      (void) shmdt(shm_info->shmaddr);
      shm_info->shmaddr = (char *) NULL;
      self->ximage->data   = NULL;
      g_free(self->ximage->obdata);
      self->ximage->obdata = NULL;
    }
#endif
    if(self->ximage->data != NULL) {
      XtFree((char *) self->ximage->data);
      self->ximage->data = NULL;
    }
    XDestroyImage(self->ximage);
    self->ximage = NULL;
  }
  self->window = None;
  self->screen = NULL;
  if(self->memory.lower_rom != NULL) {
    g_object_unref(self->memory.lower_rom);
    self->memory.lower_rom = NULL;
  }
  if(self->memory.upper_rom != NULL) {
    g_object_unref(self->memory.upper_rom);
    self->memory.upper_rom = NULL;
  }
  if(self->memory.total_ram != NULL) {
    free(self->memory.total_ram);
    self->memory.total_ram = NULL;
  }
  for(ix = 0; ix < 256; ix++) {
    if(self->memory.expan_rom[ix] != NULL) {
      g_object_unref(self->memory.expan_rom[ix]);
      self->memory.expan_rom[ix] = NULL;
    }
  }
  g_object_unref(self->z80cpu); self->z80cpu = NULL;
  g_object_unref(self->garray); self->garray = NULL;
  g_object_unref(self->mc6845); self->mc6845 = NULL;
  g_object_unref(self->ay8910); self->ay8910 = NULL;
  g_object_unref(self->upd765); self->upd765 = NULL;
  g_object_unref(self->i8255);  self->i8255  = NULL;
}

void amstrad_cpc_keybd_handler(Widget widget, XEvent *xevent)
{
  if(amstrad_cpc.keybd_hnd != NULL) {
    (*amstrad_cpc.keybd_hnd)(&amstrad_cpc, xevent);
  }
}

void amstrad_cpc_mouse_handler(Widget widget, XEvent *xevent)
{
  if(amstrad_cpc.mouse_hnd != NULL) {
    (*amstrad_cpc.mouse_hnd)(&amstrad_cpc, xevent);
  }
}

void amstrad_cpc_paint_handler(Widget widget, XEvent *xevent)
{
  amstrad_cpc.screen = XtScreen(widget);
  amstrad_cpc.window = XtWindow(widget);
  (void) XPutImage(xevent->xexpose.display,
                   xevent->xexpose.window,
                   DefaultGCOfScreen(amstrad_cpc.screen),
                   amstrad_cpc.ximage,
                   xevent->xexpose.x,     xevent->xexpose.y,
                   xevent->xexpose.x,     xevent->xexpose.y,
                   xevent->xexpose.width, xevent->xexpose.height);
  (void) XFlush(DisplayOfScreen(amstrad_cpc.screen));
}
