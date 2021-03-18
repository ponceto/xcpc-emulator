/*
 * amstrad-cpc.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include <unistd.h>
#include <errno.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xem/Emulator.h>
#include "amstrad-cpc.h"

static AMSTRAD_CPC_SETTINGS settings = {
  /* no_xshm         */ FALSE,   
  /* show_fps        */ FALSE,   
  /* computer_model  */ NULL,    
  /* monitor_model   */ NULL,    
  /* keyboard_layout */ NULL,    
  /* refresh_rate    */ NULL,    
  /* manufacturer    */ NULL,    
  /* snapshot        */ NULL,    
  /* system_rom      */ NULL,    
  /* expansion       */ {
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
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  } 
};

static const gchar model_description[]        = "cpc464, cpc664, cpc6128";
static const gchar monitor_description[]      = "color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm14";
static const gchar keyboard_description[]     = "qwerty, azerty";
static const gchar refresh_description[]      = "50Hz, 60Hz";
static const gchar manufacturer_description[] = "Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad";

static GOptionEntry options[] = {
  { "no-xshm"     , 0, 0, G_OPTION_ARG_NONE    , &settings.no_xshm        , "Don't use the XShm extension", NULL                },
  { "show-fps"    , 0, 0, G_OPTION_ARG_NONE    , &settings.show_fps       , "Show fps statistics"         , NULL                },
  { "model"       , 0, 0, G_OPTION_ARG_STRING  , &settings.computer_model , model_description             , "{computer-model}"  },
  { "monitor"     , 0, 0, G_OPTION_ARG_STRING  , &settings.monitor_model  , monitor_description           , "{monitor-model}"   },
  { "keyboard"    , 0, 0, G_OPTION_ARG_STRING  , &settings.keyboard_layout, keyboard_description          , "{keyboard-layout}" },
  { "refresh"     , 0, 0, G_OPTION_ARG_STRING  , &settings.refresh_rate   , refresh_description           , "{refresh-rate}"    },
  { "manufacturer", 0, 0, G_OPTION_ARG_STRING  , &settings.manufacturer   , manufacturer_description      , "{manufacturer}"    },
  { "snapshot"    , 0, 0, G_OPTION_ARG_FILENAME, &settings.snapshot       , "Snapshot to load at start"   , "{filename}"        },
  { "sysrom"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.system_rom     , "32Kb system rom"             , "{filename}"        },
  { "rom000"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x0] , "16Kb expansion rom #00"      , "{filename}"        },
  { "rom001"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x1] , "16Kb expansion rom #01"      , "{filename}"        },
  { "rom002"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x2] , "16Kb expansion rom #02"      , "{filename}"        },
  { "rom003"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x3] , "16Kb expansion rom #03"      , "{filename}"        },
  { "rom004"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x4] , "16Kb expansion rom #04"      , "{filename}"        },
  { "rom005"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x5] , "16Kb expansion rom #05"      , "{filename}"        },
  { "rom006"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x6] , "16Kb expansion rom #06"      , "{filename}"        },
  { "rom007"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x7] , "16Kb expansion rom #07"      , "{filename}"        },
  { "rom008"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x8] , "16Kb expansion rom #08"      , "{filename}"        },
  { "rom009"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0x9] , "16Kb expansion rom #09"      , "{filename}"        },
  { "rom010"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xa] , "16Kb expansion rom #10"      , "{filename}"        },
  { "rom011"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xb] , "16Kb expansion rom #11"      , "{filename}"        },
  { "rom012"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xc] , "16Kb expansion rom #12"      , "{filename}"        },
  { "rom013"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xd] , "16Kb expansion rom #13"      , "{filename}"        },
  { "rom014"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xe] , "16Kb expansion rom #14"      , "{filename}"        },
  { "rom015"      , 0, 0, G_OPTION_ARG_FILENAME, &settings.expansion[0xf] , "16Kb expansion rom #15"      , "{filename}"        },
  { NULL } /* end-of-options */
};

static guint8 font_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0x36, 0x36, 0x36, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x7f, 0x36, 0x7f, 0x36, 0x36, 0x00,
  0x18, 0x7c, 0x1a, 0x3c, 0x58, 0x3e, 0x18, 0x00, 0x00, 0x63, 0x33, 0x18,
  0x0c, 0x66, 0x63, 0x00, 0x1c, 0x36, 0x1c, 0x6e, 0x3b, 0x33, 0x6e, 0x00,
  0x18, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c,
  0x0c, 0x18, 0x30, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00,
  0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e,
  0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x0c,
  0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00,
  0x3e, 0x63, 0x73, 0x6b, 0x67, 0x63, 0x3e, 0x00, 0x18, 0x1c, 0x18, 0x18,
  0x18, 0x18, 0x7e, 0x00, 0x3c, 0x66, 0x60, 0x3c, 0x06, 0x66, 0x7e, 0x00,
  0x3c, 0x66, 0x60, 0x38, 0x60, 0x66, 0x3c, 0x00, 0x38, 0x3c, 0x36, 0x33,
  0x7f, 0x30, 0x78, 0x00, 0x7e, 0x46, 0x06, 0x3e, 0x60, 0x66, 0x3c, 0x00,
  0x3c, 0x66, 0x06, 0x3e, 0x66, 0x66, 0x3c, 0x00, 0x7e, 0x66, 0x60, 0x30,
  0x18, 0x18, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00,
  0x3c, 0x66, 0x66, 0x7c, 0x60, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x18,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x0c,
  0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x00, 0x00, 0x00, 0x7e, 0x00,
  0x00, 0x7e, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x18, 0x0c, 0x06, 0x00,
  0x3c, 0x66, 0x66, 0x30, 0x18, 0x00, 0x18, 0x00, 0x3e, 0x63, 0x7b, 0x7b,
  0x7b, 0x03, 0x3e, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x00,
  0x3f, 0x66, 0x66, 0x3e, 0x66, 0x66, 0x3f, 0x00, 0x3c, 0x66, 0x03, 0x03,
  0x03, 0x66, 0x3c, 0x00, 0x1f, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1f, 0x00,
  0x7f, 0x46, 0x16, 0x1e, 0x16, 0x46, 0x7f, 0x00, 0x7f, 0x46, 0x16, 0x1e,
  0x16, 0x06, 0x0f, 0x00, 0x3c, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7c, 0x00,
  0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x7e, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x7e, 0x00, 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1e, 0x00,
  0x67, 0x66, 0x36, 0x1e, 0x36, 0x66, 0x67, 0x00, 0x0f, 0x06, 0x06, 0x06,
  0x46, 0x66, 0x7f, 0x00, 0x63, 0x77, 0x7f, 0x7f, 0x6b, 0x63, 0x63, 0x00,
  0x63, 0x67, 0x6f, 0x7b, 0x73, 0x63, 0x63, 0x00, 0x1c, 0x36, 0x63, 0x63,
  0x63, 0x36, 0x1c, 0x00, 0x3f, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x0f, 0x00,
  0x1c, 0x36, 0x63, 0x63, 0x5b, 0x33, 0x6e, 0x00, 0x3f, 0x66, 0x66, 0x3e,
  0x36, 0x66, 0x67, 0x00, 0x3c, 0x66, 0x06, 0x3c, 0x60, 0x66, 0x3c, 0x00,
  0x7e, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x66, 0x66, 0x66, 0x66,
  0x66, 0x66, 0x3c, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00,
  0x63, 0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x00, 0x63, 0x36, 0x1c, 0x1c,
  0x36, 0x63, 0x63, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x00,
  0x7f, 0x63, 0x31, 0x18, 0x4c, 0x66, 0x7f, 0x00, 0x3c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x3c, 0x00, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x40, 0x00,
  0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x18, 0x3c, 0x7e, 0x18,
  0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
  0x0c, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x30,
  0x3e, 0x33, 0x6e, 0x00, 0x07, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x3b, 0x00,
  0x00, 0x00, 0x3c, 0x66, 0x06, 0x66, 0x3c, 0x00, 0x38, 0x30, 0x3e, 0x33,
  0x33, 0x33, 0x6e, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x3c, 0x00,
  0x38, 0x6c, 0x0c, 0x1e, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x7c, 0x66,
  0x66, 0x7c, 0x60, 0x3e, 0x07, 0x06, 0x36, 0x6e, 0x66, 0x66, 0x67, 0x00,
  0x18, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x60, 0x00, 0x70, 0x60,
  0x60, 0x66, 0x66, 0x3c, 0x07, 0x06, 0x66, 0x36, 0x1e, 0x36, 0x67, 0x00,
  0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x36, 0x7f,
  0x6b, 0x6b, 0x63, 0x00, 0x00, 0x00, 0x3b, 0x66, 0x66, 0x66, 0x66, 0x00,
  0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x3b, 0x66,
  0x66, 0x3e, 0x06, 0x0f, 0x00, 0x00, 0x6e, 0x33, 0x33, 0x3e, 0x30, 0x78,
  0x00, 0x00, 0x3b, 0x6e, 0x06, 0x06, 0x0f, 0x00, 0x00, 0x00, 0x3c, 0x06,
  0x3c, 0x60, 0x3e, 0x00, 0x0c, 0x0c, 0x3e, 0x0c, 0x0c, 0x6c, 0x38, 0x00,
  0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x66, 0x66,
  0x66, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x63, 0x6b, 0x6b, 0x7f, 0x36, 0x00,
  0x00, 0x00, 0x63, 0x36, 0x1c, 0x36, 0x63, 0x00, 0x00, 0x00, 0x66, 0x66,
  0x66, 0x7c, 0x60, 0x3e, 0x00, 0x00, 0x7e, 0x32, 0x18, 0x4c, 0x7e, 0x00,
  0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00, 0x18, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x00, 0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00,
  0x6e, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

AMSTRAD_CPC_EMULATOR amstrad_cpc = {
  &settings,
};

static void cpc_mem_select(AMSTRAD_CPC_EMULATOR *self)
{
  if(self->ramsize >= 128) {
    switch(self->memory.ram.config) {
      case 0x00:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
        break;
      case 0x01:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
        break;
      case 0x02:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[4]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[5]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[6]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
        break;
      case 0x03:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[3]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
        break;
      case 0x04:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[4]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
        break;
      case 0x05:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[5]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
        break;
      case 0x06:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[6]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
        break;
      case 0x07:
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[7]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
        break;
      default:
        g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "RAM-SELECT: Bad Configuration (%02x) !!", self->memory.ram.config);
        break;
    }
  }
  else {
    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
  }
  if((self->vga_core->state.rmr & 0x04) == 0) {
    if(self->rom_bank[0] != NULL) {
      self->memory.rd.bank[0] = self->rom_bank[0]->state.data;
    }
  }
  if((self->vga_core->state.rmr & 0x08) == 0) {
    if(self->rom_bank[1] != NULL) {
      self->memory.rd.bank[3] = self->rom_bank[1]->state.data;
    }
    if(self->exp_bank[self->memory.rom.config] != NULL) {
      self->memory.rd.bank[3] = self->exp_bank[self->memory.rom.config]->state.data;
    }
  }
}

static guint8 cpu_mreq_m1(GdevZ80CPU *z80cpu, guint16 addr)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;

  return self->memory.rd.bank[addr >> 14][addr & 0x3fff];
}

static guint8 cpu_mreq_rd(GdevZ80CPU *z80cpu, guint16 addr)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;

  return self->memory.rd.bank[addr >> 14][addr & 0x3fff];
}

static void cpu_mreq_wr(GdevZ80CPU *z80cpu, guint16 addr, guint8 data)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;

  self->memory.wr.bank[addr >> 14][addr & 0x3fff] = data;
}

static guint8 cpu_iorq_m1(GdevZ80CPU *z80cpu, guint16 port)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;

  self->vga_core->state.counter &= 0x1f;

  return 0x00;
}

static guint8 cpu_iorq_rd(GdevZ80CPU *z80cpu, guint16 port)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;
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
        data = xcpc_vdc_6845_rd(self->vdc_6845, 0xff);
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
        self->ppi_8255->state.port_a = self->keyboard->state.keys[self->keyboard->state.line];
        data = self->ppi_8255->state.port_a;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        self->ppi_8255->state.port_b = ((0                                             & 0x01) << 7)
                                     | ((1                                             & 0x01) << 6)
                                     | ((1                                             & 0x01) << 5)
                                     | ((self->refresh_rate                            & 0x01) << 4)
                                     | ((self->manufacturer                            & 0x07) << 1)
                                     | ((self->vdc_6845->state.ctrs.named.vsync_signal & 0x01) << 0);
        data = self->ppi_8255->state.port_b;
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        data = self->ppi_8255->state.port_c;
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
        xcpc_fdc_765a_rstat(self->fdc_765a, &data);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        xcpc_fdc_765a_rdata(self->fdc_765a, &data);
        break;
    }
  }
  return data;
}

static void cpu_iorq_wr(GdevZ80CPU *z80cpu, guint16 port, guint8 data)
{
  AMSTRAD_CPC_EMULATOR *self = &amstrad_cpc;

  /* Gate-Array   [0-------xxxxxxxx] [0x7fxx] */
  if((port & 0x8000) == 0) {
    switch((data >> 6) & 3) {
      case 0: /* Select pen */
        self->vga_core->state.pen = (data & 0x10 ? 0x10 : data & 0x0f);
        break;
      case 1: /* Select color */
        self->vga_core->state.ink[self->vga_core->state.pen] = data & 0x1f;
        break;
      case 2: /* Interrupt control, ROM configuration and screen mode */
        if((data & 0x10) != 0) {
          self->vga_core->state.counter = 0;
        }
        self->vga_core->state.rmr = data & 0x1f;
        cpc_mem_select(self);
        break;
      case 3: /* RAM memory management */
        self->memory.ram.config = data & 0x3f;
        cpc_mem_select(self);
        break;
    }
  }
  /* CRTC-6845    [-0------xxxxxxxx] [0xbfxx] */
  if((port & 0x4000) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
        xcpc_vdc_6845_rs(self->vdc_6845, data);
        break;
      case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
        xcpc_vdc_6845_wr(self->vdc_6845, data);
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
    self->memory.rom.config = data;
    cpc_mem_select(self);
  }
  /* Printer Port [---0----xxxxxxxx] [0xefxx] */
  if((port & 0x1000) == 0) {
  }
  /* PPI-8255     [----0---xxxxxxxx] [0xf7xx] */
  if((port & 0x0800) == 0) {
    switch((port >> 8) & 3) {
      case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
        self->ppi_8255->state.port_a = data;
        break;
      case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
        /*self->ppi_8255->state.port_b = data;*/
        break;
      case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
        self->ppi_8255->state.port_c = data;
        self->keyboard->state.line = data & 0x0F;
        break;
      case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
        self->ppi_8255->state.ctrl_p = data;
        break;
    }
  }
  /* FDC-765      [-----0--0xxxxxxx] [0xfb7f] */
  if((port & 0x0480) == 0) {
    switch(((port >> 7) & 2) | ((port >> 0) & 1)) {
      case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
        xcpc_fdc_765a_motor(self->fdc_765a, ((data & 1) << 1) | ((data & 1) << 0));
        break;
      case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
        xcpc_fdc_765a_motor(self->fdc_765a, ((data & 1) << 1) | ((data & 1) << 0));
        break;
      case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
        xcpc_fdc_765a_wstat(self->fdc_765a, &data);
        break;
      case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
        xcpc_fdc_765a_wdata(self->fdc_765a, &data);
        break;
    }
  }
}

static void vdc_hsync(XcpcVdc6845 *vdc_6845, int hsync, void* user_data)
{
  AMSTRAD_CPC_EMULATOR *self = ((AMSTRAD_CPC_EMULATOR*)(user_data));
  XcpcVgaCore *vga_core = self->vga_core;

  if(hsync == 0) { /* falling edge */
    if(++vga_core->state.counter == 52) {
      gdev_z80cpu_assert_int(self->z80cpu);
      vga_core->state.counter = 0;
    }
    if(vga_core->state.delayed > 0) {
      if(--vga_core->state.delayed == 0) {
        if(vga_core->state.counter >= 32) {
          gdev_z80cpu_assert_int(self->z80cpu);
        }
        vga_core->state.counter = 0;
      }
    }
    /* XXX */ {
      XcpcBlitter *blitter = self->blitter;
      struct _scanline *sl = &self->scanline[(self->cur_scanline + 1) % 312];
      int ix = 0;
      sl->mode = vga_core->state.rmr & 0x03;
      do {
        sl->ink[ix] = blitter->state.palette[vga_core->state.ink[ix]].pixel;
      } while(++ix < 17);
    }
  }
}

static void vdc_vsync(XcpcVdc6845 *vdc_6845, int vsync, void* user_data)
{
  AMSTRAD_CPC_EMULATOR *self = ((AMSTRAD_CPC_EMULATOR*)(user_data));
  XcpcVgaCore *vga_core = self->vga_core;

  if(vsync != 0) { /* rising edge */
    vga_core->state.delayed = 2;
  }
}

static void compute_stats(AMSTRAD_CPC_EMULATOR *self)
{
  struct timeval prev_time  = self->timer.profiler;
  struct timeval curr_time  = self->timer.profiler;
  unsigned long  elapsed_us = 0;

  /* get the current time */ {
    if(gettimeofday(&curr_time, NULL) != 0) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "gettimeofday() has failed");
    }
  }
  /* compute the elapsed time in us */ {
    const long long t1 = (((long long) prev_time.tv_sec) * 1000000LL) + ((long long) prev_time.tv_usec);
    const long long t2 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
    if(t2 >= t1) {
      elapsed_us = ((unsigned long)(t2 - t1));
    }
    else {
      elapsed_us = 0UL;
    }
  }
  /* compute and build the statistics */ {
    if(elapsed_us != 0) {
      const double stats_frames  = (double) (self->frame.drawn * 1000000UL);
      const double stats_elapsed = (double) elapsed_us;
      const double stats_fps     = (stats_frames / stats_elapsed);
      (void) snprintf(self->stats, sizeof(self->stats), "refresh = %2d Hz, framerate = %.2f fps", self->frame.rate, stats_fps);
    }
    else {
      (void) snprintf(self->stats, sizeof(self->stats), "refresh = %2d Hz", self->frame.rate);
    }
  }
  /* set the new reference */ {
    self->timer.profiler = curr_time;
    self->frame.count    = 0;
    self->frame.drawn    = 0;
  }
}

static void amstrad_cpc_paint_default(AMSTRAD_CPC_EMULATOR *self)
{
}

static void amstrad_cpc_paint_08bpp(AMSTRAD_CPC_EMULATOR *self)
{
  XcpcBlitter *blitter = self->blitter;
  XcpcVdc6845 *vdc_6845 = self->vdc_6845;
  XcpcVgaCore *vga_core = self->vga_core;
  unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
  unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
  unsigned int hp = ((XCPC_BLITTER_WIDTH >> 0) - (hd << 4)) >> 1;
  unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
  unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
  unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
  unsigned int vp = ((XCPC_BLITTER_HEIGHT >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint8 *dst = (guint8 *) blitter->state.image->data, *nxt = dst;
  guint8 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint16 bank;
  guint16 disp;
  guint8 data;

  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[6];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += XCPC_BLITTER_WIDTH;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode0[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode0[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode1[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode1[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode2[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode2[data];
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
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(settings.show_fps != FALSE) {
    char *str = self->stats; int len = 0;
    guint8 fg = blitter->state.palette[vga_core->state.ink[0x01]].pixel;
    guint8 bg = blitter->state.palette[vga_core->state.ink[0x10]].pixel;
    guint8 *pt0 = (guint8 *) ((guint8 *) blitter->state.image->data + ((blitter->state.image->height - 9) * blitter->state.image->bytes_per_line));
    while(*str != 0) {
      guint8 *pt1 = pt0;
      for(cy = 0; cy < 8; cy++) {
        guint8 *pt2 = pt1;
        data = font_bits[((*str & 0x7f) << 3) + cy];
        for(cx = 0; cx < 8; cx++) {
          *pt2++ = (data & 0x01 ? fg : bg); data >>= 1;
        }
        pt1 = (guint8 *) (((guint8 *) pt1) + blitter->state.image->bytes_per_line);
      }
      pt0 += 8; str++; len++;
    }
  }
  (void) xcpc_blitter_put_image(self->blitter);
}

static void amstrad_cpc_paint_16bpp(AMSTRAD_CPC_EMULATOR *self)
{
  XcpcBlitter *blitter = self->blitter;
  XcpcVdc6845 *vdc_6845 = self->vdc_6845;
  XcpcVgaCore *vga_core = self->vga_core;
  unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
  unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
  unsigned int hp = ((XCPC_BLITTER_WIDTH >> 0) - (hd << 4)) >> 1;
  unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
  unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
  unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
  unsigned int vp = ((XCPC_BLITTER_HEIGHT >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint16 *dst = (guint16 *) blitter->state.image->data, *nxt = dst;
  guint16 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint16 bank;
  guint16 disp;
  guint8 data;

  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[6];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += XCPC_BLITTER_WIDTH;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode0[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode0[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode1[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode1[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode2[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode2[data];
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
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(settings.show_fps != FALSE) {
    char *str = self->stats; int len = 0;
    guint16 fg = blitter->state.palette[vga_core->state.ink[0x01]].pixel;
    guint16 bg = blitter->state.palette[vga_core->state.ink[0x10]].pixel;
    guint16 *pt0 = (guint16 *) ((guint8 *) blitter->state.image->data + ((blitter->state.image->height - 9) * blitter->state.image->bytes_per_line));
    while(*str != 0) {
      guint16 *pt1 = pt0;
      for(cy = 0; cy < 8; cy++) {
        guint16 *pt2 = pt1;
        data = font_bits[((*str & 0x7f) << 3) + cy];
        for(cx = 0; cx < 8; cx++) {
          *pt2++ = (data & 0x01 ? fg : bg); data >>= 1;
        }
        pt1 = (guint16 *) (((guint8 *) pt1) + blitter->state.image->bytes_per_line);
      }
      pt0 += 8; str++; len++;
    }
  }
  (void) xcpc_blitter_put_image(self->blitter);
}

static void amstrad_cpc_paint_32bpp(AMSTRAD_CPC_EMULATOR *self)
{
  XcpcBlitter *blitter = self->blitter;
  XcpcVdc6845 *vdc_6845 = self->vdc_6845;
  XcpcVgaCore *vga_core = self->vga_core;
  unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
  unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
  unsigned int hp = ((XCPC_BLITTER_WIDTH >> 0) - (hd << 4)) >> 1;
  unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
  unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
  unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
  unsigned int vp = ((XCPC_BLITTER_HEIGHT >> 1) - (vd * mr)) >> 1;
  struct _scanline *sl = NULL;
  guint32 *dst = (guint32 *) blitter->state.image->data, *nxt = dst;
  guint32 pixel;
  unsigned int cx, cy, ra;
  guint16 addr;
  guint16 bank;
  guint16 disp;
  guint8 data;

  sl = &self->scanline[(vt * mr) - (1 * vp)];
  for(cy = 0; cy < vp; cy++) {
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  sl = &self->scanline[6];
  for(cy = 0; cy < vd; cy++) {
    for(ra = 0; ra < mr; ra++) {
      nxt += XCPC_BLITTER_WIDTH;
      switch(sl->mode) {
        case 0x00:
          pixel = sl->ink[16];
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = pixel;
          }
          for(cx = 0; cx < hd; cx++) {
            addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode0[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode0[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode1[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode1[data];
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
            bank = (addr >> 14);
            disp = (addr & 0x3fff);
            /* pixel 0 */
            data = self->ram_bank[bank]->state.data[disp | 0];
            data = vga_core->state.mode2[data];
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
            data = self->ram_bank[bank]->state.data[disp | 1];
            data = vga_core->state.mode2[data];
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
    nxt += XCPC_BLITTER_WIDTH;
    pixel = sl->ink[16];
    for(cx = 0; cx < XCPC_BLITTER_WIDTH; cx++) {
      *dst++ = *nxt++ = pixel;
    }
    dst = nxt; sl++;
  }
  if(settings.show_fps != FALSE) {
    char *str = self->stats; int len = 0;
    guint32 fg = blitter->state.palette[vga_core->state.ink[0x01]].pixel;
    guint32 bg = blitter->state.palette[vga_core->state.ink[0x10]].pixel;
    guint32 *pt0 = (guint32 *) ((guint8 *) blitter->state.image->data + ((blitter->state.image->height - 9) * blitter->state.image->bytes_per_line));
    while(*str != 0) {
      guint32 *pt1 = pt0;
      for(cy = 0; cy < 8; cy++) {
        guint32 *pt2 = pt1;
        data = font_bits[((*str & 0x7f) << 3) + cy];
        for(cx = 0; cx < 8; cx++) {
          *pt2++ = (data & 0x01 ? fg : bg); data >>= 1;
        }
        pt1 = (guint32 *) (((guint8 *) pt1) + blitter->state.image->bytes_per_line);
      }
      pt0 += 8; str++; len++;
    }
  }
  (void) xcpc_blitter_put_image(self->blitter);
}

static void amstrad_cpc_keybd_default(AMSTRAD_CPC_EMULATOR *self, XEvent *event)
{
}

static void amstrad_cpc_keybd_qwerty(AMSTRAD_CPC_EMULATOR *self, XEvent *event)
{
  (void) xcpc_keyboard_qwerty(self->keyboard, &event->xkey);
}

static void amstrad_cpc_keybd_azerty(AMSTRAD_CPC_EMULATOR *self, XEvent *event)
{
  (void) xcpc_keyboard_azerty(self->keyboard, &event->xkey);
}

static void amstrad_cpc_mouse_default(AMSTRAD_CPC_EMULATOR *self, XEvent *event)
{
}

int amstrad_cpc_parse(int *argc, char ***argv)
{
  GOptionContext *context = g_option_context_new(NULL);

  g_option_context_add_main_entries(context, options, NULL);
  if(g_option_context_parse(context, argc, argv, NULL) != FALSE) {
    /* TODO */
  }
  else {
    /* TODO */
  }
  g_option_context_free(context);
  context = (GOptionContext *) NULL;
  return EXIT_SUCCESS;
}

void amstrad_cpc_start(AMSTRAD_CPC_EMULATOR *self)
{
  /* initialize libxcpc */ {
      xcpc_log_begin();
  }
  /* init machine */ {
    self->computer_model = xcpc_computer_model(self->settings->computer_model, XCPC_COMPUTER_MODEL_6128);
    switch(self->computer_model) {
      case XCPC_COMPUTER_MODEL_464: {
          if(self->settings->system_rom == NULL) {
            self->settings->system_rom = g_build_filename(XCPC_RESDIR, "roms", "cpc464.rom", NULL);
          }
          if(self->settings->expansion[7] == NULL) {
            self->settings->expansion[7] = NULL;
          }
          self->paint.proc      = &amstrad_cpc_paint_default;
          self->keybd.proc      = &amstrad_cpc_keybd_default;
          self->mouse.proc      = &amstrad_cpc_mouse_default;
          self->ramsize         = 64;
          self->monitor_model   = xcpc_monitor_model(self->settings->monitor_model, XCPC_MONITOR_MODEL_CTM644);
          self->keyboard_layout = xcpc_keyboard_layout(self->settings->keyboard_layout, XCPC_KEYBOARD_LAYOUT_QWERTY);
          self->refresh_rate    = xcpc_refresh_rate(self->settings->refresh_rate, XCPC_REFRESH_RATE_50HZ);
          self->manufacturer    = xcpc_manufacturer(self->settings->manufacturer, XCPC_MANUFACTURER_AMSTRAD);
        }
        break;
      case XCPC_COMPUTER_MODEL_664: {
          if(self->settings->system_rom == NULL) {
            self->settings->system_rom = g_build_filename(XCPC_RESDIR, "roms", "cpc664.rom", NULL);
          }
          if(self->settings->expansion[7] == NULL) {
            self->settings->expansion[7] = g_build_filename(XCPC_RESDIR, "roms", "amsdos.rom", NULL);
          }
          self->paint.proc      = &amstrad_cpc_paint_default;
          self->keybd.proc      = &amstrad_cpc_keybd_default;
          self->mouse.proc      = &amstrad_cpc_mouse_default;
          self->ramsize         = 64;
          self->monitor_model   = xcpc_monitor_model(self->settings->monitor_model, XCPC_MONITOR_MODEL_CTM644);
          self->keyboard_layout = xcpc_keyboard_layout(self->settings->keyboard_layout, XCPC_KEYBOARD_LAYOUT_QWERTY);
          self->refresh_rate    = xcpc_refresh_rate(self->settings->refresh_rate, XCPC_REFRESH_RATE_50HZ);
          self->manufacturer    = xcpc_manufacturer(self->settings->manufacturer, XCPC_MANUFACTURER_AMSTRAD);
        }
        break;
      case XCPC_COMPUTER_MODEL_6128: {
          if(self->settings->system_rom == NULL) {
            self->settings->system_rom = g_build_filename(XCPC_RESDIR, "roms", "cpc6128.rom", NULL);
          }
          if(self->settings->expansion[7] == NULL) {
            self->settings->expansion[7] = g_build_filename(XCPC_RESDIR, "roms", "amsdos.rom", NULL);
          }
          self->paint.proc      = &amstrad_cpc_paint_default;
          self->keybd.proc      = &amstrad_cpc_keybd_default;
          self->mouse.proc      = &amstrad_cpc_mouse_default;
          self->ramsize         = 128;
          self->monitor_model   = xcpc_monitor_model(self->settings->monitor_model, XCPC_MONITOR_MODEL_CTM640);
          self->keyboard_layout = xcpc_keyboard_layout(self->settings->keyboard_layout, XCPC_KEYBOARD_LAYOUT_QWERTY);
          self->refresh_rate    = xcpc_refresh_rate(self->settings->refresh_rate, XCPC_REFRESH_RATE_50HZ);
          self->manufacturer    = xcpc_manufacturer(self->settings->manufacturer, XCPC_MANUFACTURER_AMSTRAD);
        }
        break;
      default:
        g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "unknown computer model");
        break;
    }
  }
  /* create devices */ {
    self->z80cpu = gdev_z80cpu_new();
  }
  /* initialize devices handlers */ {
    self->z80cpu->mreq_m1 = cpu_mreq_m1;
    self->z80cpu->mreq_rd = cpu_mreq_rd;
    self->z80cpu->mreq_wr = cpu_mreq_wr;
    self->z80cpu->iorq_m1 = cpu_iorq_m1;
    self->z80cpu->iorq_rd = cpu_iorq_rd;
    self->z80cpu->iorq_wr = cpu_iorq_wr;
  }
  /* create blitter */ {
    self->blitter = xcpc_blitter_new();
  }
  /* create monitor */ {
    self->monitor = xcpc_monitor_new();
  }
  /* create keyboard */ {
    self->keyboard = xcpc_keyboard_new();
  }
  /* create joystick */ {
    self->joystick = xcpc_joystick_new();
  }
  /* create vga_core */ {
    self->vga_core = xcpc_vga_core_new();
  }
  /* create vdc_6845 */ {
    self->vdc_6845 = xcpc_vdc_6845_new();
    self->vdc_6845->iface.user_data = self;
    self->vdc_6845->iface.hsync_callback = &vdc_hsync;
    self->vdc_6845->iface.vsync_callback = &vdc_vsync;
  }
  /* create ppi_8255 */ {
    self->ppi_8255 = xcpc_ppi_8255_new();
  }
  /* create psg_8910 */ {
    self->psg_8910 = xcpc_psg_8910_new();
  }
  /* create fdc_765a */ {
    self->fdc_765a = xcpc_fdc_765a_new();
    (void) xcpc_fdc_765a_attach(self->fdc_765a, 0);
    (void) xcpc_fdc_765a_attach(self->fdc_765a, 1);
  }
  /* create ram banks */ {
    size_t requested = self->ramsize * 1024UL;
    size_t allocated = 0UL;
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->ram_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(allocated < requested) {
        self->ram_bank[bank_index] = xcpc_ram_bank_new();
        allocated += sizeof(self->ram_bank[bank_index]->state.data);
      }
      else {
        self->ram_bank[bank_index] = NULL;
        allocated += 0UL;
      }
    }
  }
  /* create lower rom bank */ {
    XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
    XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, self->settings->system_rom, 0x0000);
    if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "lower-rom: loading error (%s)", self->settings->system_rom);
    }
    self->rom_bank[0] = rom_bank;
  }
  /* create upper rom bank */ {
    XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
    XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, self->settings->system_rom, 0x4000);
    if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "upper-rom: loading error (%s)", self->settings->system_rom);
    }
    self->rom_bank[1] = rom_bank;
  }
  /* create expansion roms banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->exp_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->settings->expansion[bank_index] != NULL) {
        XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
        XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, self->settings->expansion[bank_index], 0x0000);
        if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
          g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "expansion-rom: loading error (%s)", self->settings->expansion[bank_index]);
        }
        self->exp_bank[bank_index] = rom_bank;
      }
      else {
        self->exp_bank[bank_index] = NULL;
      }
    }
  }
  /* initialize memory pager */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = 4;
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      self->memory.rd.bank[bank_index] = NULL;
      self->memory.wr.bank[bank_index] = NULL;
    }
    self->memory.ram.config = 0x00;
    self->memory.rom.config = 0x00;
  }
  /* initialize timers */ {
    if(gettimeofday(&self->timer.deadline, NULL) != 0) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "gettimeofday() has failed");
    }
    if(gettimeofday(&self->timer.profiler, NULL) != 0) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "gettimeofday() has failed");
    }
  }
  /* initialize frame */ {
    self->frame.rate  = 0;
    self->frame.time  = 0;
    self->frame.count = 0;
    self->frame.drawn = 0;
  }
  /* compute frame rate/time and cpu period */ {
    switch(self->refresh_rate) {
      case XCPC_REFRESH_RATE_50HZ:
        self->frame.rate = 50;
        self->frame.time = 20000;
        self->cpu_period = (int) (4000000.0 / (50.0 * 312.5));
        break;
      case XCPC_REFRESH_RATE_60HZ:
        self->frame.rate = 60;
        self->frame.time = 16667;
        self->cpu_period = (int) (4000000.0 / (60.0 * 262.5));
        break;
      default:
        g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "unsupported refresh rate %d", self->refresh_rate);
        break;
    }
  }
  /* reset instance */ {
    amstrad_cpc_reset(self);
  }
  /* Load initial snapshot */
  if(self->settings->snapshot != NULL) {
    amstrad_cpc_load_snapshot(self, self->settings->snapshot);
  }
}

void amstrad_cpc_close(AMSTRAD_CPC_EMULATOR *self)
{
  /* cleanup handlers */ {
    self->mouse.proc = &amstrad_cpc_mouse_default;
    self->keybd.proc = &amstrad_cpc_keybd_default;
    self->paint.proc = &amstrad_cpc_paint_default;
  }
  /* cleanup memory pager */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = 4;
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      self->memory.rd.bank[bank_index] = NULL;
      self->memory.wr.bank[bank_index] = NULL;
    }
    self->memory.ram.config = 0x00;
    self->memory.rom.config = 0x00;
  }
  /* destroy expansion roms banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->exp_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->exp_bank[bank_index] != NULL) {
        self->exp_bank[bank_index] = xcpc_rom_bank_delete(self->exp_bank[bank_index]);
      }
    }
  }
  /* destroy lower/upper rom banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->rom_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->rom_bank[bank_index] != NULL) {
        self->rom_bank[bank_index] = xcpc_rom_bank_delete(self->rom_bank[bank_index]);
      }
    }
  }
  /* destroy ram banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->ram_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->ram_bank[bank_index] != NULL) {
        self->ram_bank[bank_index] = xcpc_ram_bank_delete(self->ram_bank[bank_index]);
      }
    }
  }
  /* destroy fdc_765a */ {
    self->fdc_765a = xcpc_fdc_765a_delete(self->fdc_765a);
  }
  /* destroy psg_8910 */ {
    self->psg_8910 = xcpc_psg_8910_delete(self->psg_8910);
  }
  /* destroy ppi_8255 */ {
    self->ppi_8255 = xcpc_ppi_8255_delete(self->ppi_8255);
  }
  /* destroy vdc_6845 */ {
    self->vdc_6845 = xcpc_vdc_6845_delete(self->vdc_6845);
  }
  /* destroy vga_core */ {
    self->vga_core = xcpc_vga_core_delete(self->vga_core);
  }
  /* destroy joystick */ {
    self->joystick = xcpc_joystick_delete(self->joystick);
  }
  /* destroy keyboard */ {
    self->keyboard = xcpc_keyboard_delete(self->keyboard);
  }
  /* destroy monitor */ {
    self->monitor = xcpc_monitor_delete(self->monitor);
  }
  /* destroy blitter */ {
    self->blitter = xcpc_blitter_delete(self->blitter);
  }
  /* destroy devices */ {
    self->z80cpu = (g_object_unref(self->z80cpu), NULL);
  }
  /* finalize libxcpc */ {
      xcpc_log_end();
  }
}

void amstrad_cpc_reset(AMSTRAD_CPC_EMULATOR *self)
{
  /* reset devices */ {
    gdev_device_reset(GDEV_DEVICE(self->z80cpu));
  }
  /* reset blitter */ {
    (void) xcpc_blitter_reset(self->blitter);
  }
  /* reset monitor */ {
    (void) xcpc_monitor_reset(self->monitor);
  }
  /* reset keyboard */ {
    (void) xcpc_keyboard_reset(self->keyboard);
  }
  /* reset joystick */ {
    (void) xcpc_joystick_reset(self->joystick);
  }
  /* reset vga_core */ {
    (void) xcpc_vga_core_reset(self->vga_core);
  }
  /* reset vdc_6845 */ {
    (void) xcpc_vdc_6845_reset(self->vdc_6845);
  }
  /* reset ppi_8255 */ {
    (void) xcpc_ppi_8255_reset(self->ppi_8255);
  }
  /* reset psg_8910 */ {
    (void) xcpc_psg_8910_reset(self->psg_8910);
  }
  /* reset fdc_765a */ {
    (void) xcpc_fdc_765a_reset(self->fdc_765a);
  }
  /* reset ram banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->ram_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->ram_bank[bank_index] != NULL) {
        (void) xcpc_ram_bank_reset(self->ram_bank[bank_index]);
      }
    }
  }
  /* reset lower/upper rom banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->rom_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->rom_bank[bank_index] != NULL) {
        (void) xcpc_rom_bank_reset(self->rom_bank[bank_index]);
      }
    }
  }
  /* reset expansion roms banks */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = countof(self->exp_bank);
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      if(self->exp_bank[bank_index] != NULL) {
        (void) xcpc_rom_bank_reset(self->exp_bank[bank_index]);
      }
    }
  }
  /* reset memory pager */ {
    unsigned int bank_index = 0;
    unsigned int bank_count = 4;
    for(bank_index = 0; bank_index < bank_count; ++bank_index) {
      self->memory.rd.bank[bank_index] = NULL;
      self->memory.wr.bank[bank_index] = NULL;
    }
    self->memory.ram.config = 0x00;
    self->memory.rom.config = 0x00;
    cpc_mem_select(self);
  }
  /* timer */ {
    if(gettimeofday(&self->timer.deadline, NULL) != 0) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "gettimeofday() has failed");
    }
    if(gettimeofday(&self->timer.profiler, NULL) != 0) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "gettimeofday() has failed");
    }
  }
  /* frame */ {
    self->frame.rate  |= 0; /* no reset */
    self->frame.time  |= 0; /* no reset */
    self->frame.count &= 0; /* do reset */
    self->frame.drawn &= 0; /* do reset */
  }
  self->stats[0]  = 0;
}

void amstrad_cpc_load_snapshot(AMSTRAD_CPC_EMULATOR* self, const char *filename)
{
  XcpcSnapshot*      snapshot = xcpc_snapshot_new();
  XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;

  /* load snapshot */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      status = xcpc_snapshot_load(snapshot, filename);
    }
  }
  /* cpu */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->z80cpu->reg.AF.b.l = snapshot->header.cpu_p_af_l;
      self->z80cpu->reg.AF.b.h = snapshot->header.cpu_p_af_h;
      self->z80cpu->reg.BC.b.l = snapshot->header.cpu_p_bc_l;
      self->z80cpu->reg.BC.b.h = snapshot->header.cpu_p_bc_h;
      self->z80cpu->reg.DE.b.l = snapshot->header.cpu_p_de_l;
      self->z80cpu->reg.DE.b.h = snapshot->header.cpu_p_de_h;
      self->z80cpu->reg.HL.b.l = snapshot->header.cpu_p_hl_l;
      self->z80cpu->reg.HL.b.h = snapshot->header.cpu_p_hl_h;
      self->z80cpu->reg.IR.b.l = snapshot->header.cpu_p_ir_l;
      self->z80cpu->reg.IR.b.h = snapshot->header.cpu_p_ir_h;
      if(snapshot->header.cpu_p_iff1 != 0) {
        self->z80cpu->reg.IF.w.l |= _IFF1;
      }
      else {
        self->z80cpu->reg.IF.w.l &= ~_IFF1;
      }
      if(snapshot->header.cpu_p_iff2 != 0) {
        self->z80cpu->reg.IF.w.l |= _IFF2;
      }
      else {
        self->z80cpu->reg.IF.w.l &= ~_IFF2;
      }
      self->z80cpu->reg.IX.b.l = snapshot->header.cpu_p_ix_l;
      self->z80cpu->reg.IX.b.h = snapshot->header.cpu_p_ix_h;
      self->z80cpu->reg.IY.b.l = snapshot->header.cpu_p_iy_l;
      self->z80cpu->reg.IY.b.h = snapshot->header.cpu_p_iy_h;
      self->z80cpu->reg.SP.b.l = snapshot->header.cpu_p_sp_l;
      self->z80cpu->reg.SP.b.h = snapshot->header.cpu_p_sp_h;
      self->z80cpu->reg.PC.b.l = snapshot->header.cpu_p_pc_l;
      self->z80cpu->reg.PC.b.h = snapshot->header.cpu_p_pc_h;
      switch(snapshot->header.cpu_p_im_l) {
        case 1:
          self->z80cpu->reg.IF.w.l |=  _IM1;
          self->z80cpu->reg.IF.w.l &= ~_IM2;
          break;
        case 2:
          self->z80cpu->reg.IF.w.l &= ~_IM1;
          self->z80cpu->reg.IF.w.l |=  _IM2;
          break;
        default:
          self->z80cpu->reg.IF.w.l &= ~_IM1;
          self->z80cpu->reg.IF.w.l &= ~_IM2;
          break;
      }
      self->z80cpu->reg.AF.b.y = snapshot->header.cpu_a_af_l;
      self->z80cpu->reg.AF.b.x = snapshot->header.cpu_a_af_h;
      self->z80cpu->reg.BC.b.y = snapshot->header.cpu_a_bc_l;
      self->z80cpu->reg.BC.b.x = snapshot->header.cpu_a_bc_h;
      self->z80cpu->reg.DE.b.y = snapshot->header.cpu_a_de_l;
      self->z80cpu->reg.DE.b.x = snapshot->header.cpu_a_de_h;
      self->z80cpu->reg.HL.b.y = snapshot->header.cpu_a_hl_l;
      self->z80cpu->reg.HL.b.x = snapshot->header.cpu_a_hl_h;
    }
  }
  /* vga */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->vga_core->state.pen       = snapshot->header.vga_ink_ix;
      self->vga_core->state.ink[0x00] = snapshot->header.vga_ink_00;
      self->vga_core->state.ink[0x01] = snapshot->header.vga_ink_01;
      self->vga_core->state.ink[0x02] = snapshot->header.vga_ink_02;
      self->vga_core->state.ink[0x03] = snapshot->header.vga_ink_03;
      self->vga_core->state.ink[0x04] = snapshot->header.vga_ink_04;
      self->vga_core->state.ink[0x05] = snapshot->header.vga_ink_05;
      self->vga_core->state.ink[0x06] = snapshot->header.vga_ink_06;
      self->vga_core->state.ink[0x07] = snapshot->header.vga_ink_07;
      self->vga_core->state.ink[0x08] = snapshot->header.vga_ink_08;
      self->vga_core->state.ink[0x09] = snapshot->header.vga_ink_09;
      self->vga_core->state.ink[0x0a] = snapshot->header.vga_ink_10;
      self->vga_core->state.ink[0x0b] = snapshot->header.vga_ink_11;
      self->vga_core->state.ink[0x0c] = snapshot->header.vga_ink_12;
      self->vga_core->state.ink[0x0d] = snapshot->header.vga_ink_13;
      self->vga_core->state.ink[0x0e] = snapshot->header.vga_ink_14;
      self->vga_core->state.ink[0x0f] = snapshot->header.vga_ink_15;
      self->vga_core->state.ink[0x10] = snapshot->header.vga_ink_16;
      self->vga_core->state.rmr       = snapshot->header.vga_config;
    }
  }
  /* ram select */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->memory.ram.config = snapshot->header.ram_select;
    }
  }
  /* vdc */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->vdc_6845->state.regs.array.addr       = snapshot->header.vdc_reg_ix;
      self->vdc_6845->state.regs.array.data[0x00] = snapshot->header.vdc_reg_00;
      self->vdc_6845->state.regs.array.data[0x01] = snapshot->header.vdc_reg_01;
      self->vdc_6845->state.regs.array.data[0x02] = snapshot->header.vdc_reg_02;
      self->vdc_6845->state.regs.array.data[0x03] = snapshot->header.vdc_reg_03;
      self->vdc_6845->state.regs.array.data[0x04] = snapshot->header.vdc_reg_04;
      self->vdc_6845->state.regs.array.data[0x05] = snapshot->header.vdc_reg_05;
      self->vdc_6845->state.regs.array.data[0x06] = snapshot->header.vdc_reg_06;
      self->vdc_6845->state.regs.array.data[0x07] = snapshot->header.vdc_reg_07;
      self->vdc_6845->state.regs.array.data[0x08] = snapshot->header.vdc_reg_08;
      self->vdc_6845->state.regs.array.data[0x09] = snapshot->header.vdc_reg_09;
      self->vdc_6845->state.regs.array.data[0x0a] = snapshot->header.vdc_reg_10;
      self->vdc_6845->state.regs.array.data[0x0b] = snapshot->header.vdc_reg_11;
      self->vdc_6845->state.regs.array.data[0x0c] = snapshot->header.vdc_reg_12;
      self->vdc_6845->state.regs.array.data[0x0d] = snapshot->header.vdc_reg_13;
      self->vdc_6845->state.regs.array.data[0x0e] = snapshot->header.vdc_reg_14;
      self->vdc_6845->state.regs.array.data[0x0f] = snapshot->header.vdc_reg_15;
      self->vdc_6845->state.regs.array.data[0x10] = snapshot->header.vdc_reg_16;
      self->vdc_6845->state.regs.array.data[0x11] = snapshot->header.vdc_reg_17;
    }
  }
  /* rom select */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->memory.rom.config = snapshot->header.rom_select;
    }
  }
  /* ppi */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->ppi_8255->state.port_a = snapshot->header.ppi_port_a;
      self->ppi_8255->state.port_b = snapshot->header.ppi_port_b;
      self->ppi_8255->state.port_c = snapshot->header.ppi_port_c;
      self->ppi_8255->state.ctrl_p = snapshot->header.ppi_ctrl_p;
    }
  }
  /* psg */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      self->psg_8910->state.regs.array.addr       = snapshot->header.psg_reg_ix;
      self->psg_8910->state.regs.array.data[0x00] = snapshot->header.psg_reg_00;
      self->psg_8910->state.regs.array.data[0x01] = snapshot->header.psg_reg_01;
      self->psg_8910->state.regs.array.data[0x02] = snapshot->header.psg_reg_02;
      self->psg_8910->state.regs.array.data[0x03] = snapshot->header.psg_reg_03;
      self->psg_8910->state.regs.array.data[0x04] = snapshot->header.psg_reg_04;
      self->psg_8910->state.regs.array.data[0x05] = snapshot->header.psg_reg_05;
      self->psg_8910->state.regs.array.data[0x06] = snapshot->header.psg_reg_06;
      self->psg_8910->state.regs.array.data[0x07] = snapshot->header.psg_reg_07;
      self->psg_8910->state.regs.array.data[0x08] = snapshot->header.psg_reg_08;
      self->psg_8910->state.regs.array.data[0x09] = snapshot->header.psg_reg_09;
      self->psg_8910->state.regs.array.data[0x0a] = snapshot->header.psg_reg_10;
      self->psg_8910->state.regs.array.data[0x0b] = snapshot->header.psg_reg_11;
      self->psg_8910->state.regs.array.data[0x0c] = snapshot->header.psg_reg_12;
      self->psg_8910->state.regs.array.data[0x0d] = snapshot->header.psg_reg_13;
      self->psg_8910->state.regs.array.data[0x0e] = snapshot->header.psg_reg_14;
      self->psg_8910->state.regs.array.data[0x0f] = snapshot->header.psg_reg_15;
    }
  }
  /* ram */ {
    size_t ram_size = 0UL;
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      ram_size |= (((size_t)(snapshot->header.ram_size_h)) << 18);
      ram_size |= (((size_t)(snapshot->header.ram_size_l)) << 10);
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      if(ram_size == 0UL) {
        status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
      }
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      size_t         snap_size  = self->ramsize * 1024UL;
      unsigned int   bank_index = 0;
      unsigned int   bank_count = countof(snapshot->memory);
      for(bank_index = 0; bank_index < bank_count; ++bank_index) {
        unsigned char* snap_data = self->ram_bank[bank_index]->state.data;
        unsigned char* bank_data = snapshot->memory[bank_index].data;
        size_t         bank_size = sizeof(snapshot->memory[bank_index].data);
        if(snap_size >= bank_size) {
          (void) memcpy(snap_data, bank_data, bank_size);
          snap_size -= bank_size;
        }
        else {
          break;
        }
      }
      if(snap_size != 0) {
        status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
      }
    }
  }
  /* cleanup */ {
    snapshot = xcpc_snapshot_delete(snapshot);
  }
  /* check for error */ {
    if(status != XCPC_SNAPSHOT_STATUS_SUCCESS) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "load snapshot : %s", xcpc_snapshot_strerror(status));
    }
  }
  /* perform memory mapping or reset */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      cpc_mem_select(self);
    }
    else {
      amstrad_cpc_reset(self);
    }
  }
}

void amstrad_cpc_save_snapshot(AMSTRAD_CPC_EMULATOR* self, const char *filename)
{
  XcpcSnapshot*      snapshot = xcpc_snapshot_new();
  XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;

  /* cpu */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.cpu_p_af_l = self->z80cpu->reg.AF.b.l;
      snapshot->header.cpu_p_af_h = self->z80cpu->reg.AF.b.h;
      snapshot->header.cpu_p_bc_l = self->z80cpu->reg.BC.b.l;
      snapshot->header.cpu_p_bc_h = self->z80cpu->reg.BC.b.h;
      snapshot->header.cpu_p_de_l = self->z80cpu->reg.DE.b.l;
      snapshot->header.cpu_p_de_h = self->z80cpu->reg.DE.b.h;
      snapshot->header.cpu_p_hl_l = self->z80cpu->reg.HL.b.l;
      snapshot->header.cpu_p_hl_h = self->z80cpu->reg.HL.b.h;
      snapshot->header.cpu_p_ir_l = self->z80cpu->reg.IR.b.l;
      snapshot->header.cpu_p_ir_h = self->z80cpu->reg.IR.b.h;
      if(self->z80cpu->reg.IF.w.l & _IFF1) {
        snapshot->header.cpu_p_iff1 = 1;
      }
      else {
        snapshot->header.cpu_p_iff1 = 0;
      }
      if(self->z80cpu->reg.IF.w.l & _IFF2) {
        snapshot->header.cpu_p_iff2 = 1;
      }
      else {
        snapshot->header.cpu_p_iff2 = 0;
      }
      snapshot->header.cpu_p_ix_l = self->z80cpu->reg.IX.b.l;
      snapshot->header.cpu_p_ix_h = self->z80cpu->reg.IX.b.h;
      snapshot->header.cpu_p_iy_l = self->z80cpu->reg.IY.b.l;
      snapshot->header.cpu_p_iy_h = self->z80cpu->reg.IY.b.h;
      snapshot->header.cpu_p_sp_l = self->z80cpu->reg.SP.b.l;
      snapshot->header.cpu_p_sp_h = self->z80cpu->reg.SP.b.h;
      snapshot->header.cpu_p_pc_l = self->z80cpu->reg.PC.b.l;
      snapshot->header.cpu_p_pc_h = self->z80cpu->reg.PC.b.h;
      switch(self->z80cpu->reg.IF.w.l & (_IM1 | _IM2)) {
        case _IM1:
          snapshot->header.cpu_p_im_l = 1;
          break;
        case _IM2:
          snapshot->header.cpu_p_im_l = 2;
          break;
        default:
          snapshot->header.cpu_p_im_l = 0;
          break;
      }
      snapshot->header.cpu_a_af_l = self->z80cpu->reg.AF.b.y;
      snapshot->header.cpu_a_af_h = self->z80cpu->reg.AF.b.x;
      snapshot->header.cpu_a_bc_l = self->z80cpu->reg.BC.b.y;
      snapshot->header.cpu_a_bc_h = self->z80cpu->reg.BC.b.x;
      snapshot->header.cpu_a_de_l = self->z80cpu->reg.DE.b.y;
      snapshot->header.cpu_a_de_h = self->z80cpu->reg.DE.b.x;
      snapshot->header.cpu_a_hl_l = self->z80cpu->reg.HL.b.y;
      snapshot->header.cpu_a_hl_h = self->z80cpu->reg.HL.b.x;
    }
  }
  /* vga */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.vga_ink_ix = self->vga_core->state.pen;
      snapshot->header.vga_ink_00 = self->vga_core->state.ink[0x00];
      snapshot->header.vga_ink_01 = self->vga_core->state.ink[0x01];
      snapshot->header.vga_ink_02 = self->vga_core->state.ink[0x02];
      snapshot->header.vga_ink_03 = self->vga_core->state.ink[0x03];
      snapshot->header.vga_ink_04 = self->vga_core->state.ink[0x04];
      snapshot->header.vga_ink_05 = self->vga_core->state.ink[0x05];
      snapshot->header.vga_ink_06 = self->vga_core->state.ink[0x06];
      snapshot->header.vga_ink_07 = self->vga_core->state.ink[0x07];
      snapshot->header.vga_ink_08 = self->vga_core->state.ink[0x08];
      snapshot->header.vga_ink_09 = self->vga_core->state.ink[0x09];
      snapshot->header.vga_ink_10 = self->vga_core->state.ink[0x0a];
      snapshot->header.vga_ink_11 = self->vga_core->state.ink[0x0b];
      snapshot->header.vga_ink_12 = self->vga_core->state.ink[0x0c];
      snapshot->header.vga_ink_13 = self->vga_core->state.ink[0x0d];
      snapshot->header.vga_ink_14 = self->vga_core->state.ink[0x0e];
      snapshot->header.vga_ink_15 = self->vga_core->state.ink[0x0f];
      snapshot->header.vga_ink_16 = self->vga_core->state.ink[0x10];
      snapshot->header.vga_config = self->vga_core->state.rmr;
    }
  }
  /* ram select */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.ram_select = self->memory.ram.config;
    }
  }
  /* vdc */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.vdc_reg_ix = self->vdc_6845->state.regs.array.addr;
      snapshot->header.vdc_reg_00 = self->vdc_6845->state.regs.array.data[0x00];
      snapshot->header.vdc_reg_01 = self->vdc_6845->state.regs.array.data[0x01];
      snapshot->header.vdc_reg_02 = self->vdc_6845->state.regs.array.data[0x02];
      snapshot->header.vdc_reg_03 = self->vdc_6845->state.regs.array.data[0x03];
      snapshot->header.vdc_reg_04 = self->vdc_6845->state.regs.array.data[0x04];
      snapshot->header.vdc_reg_05 = self->vdc_6845->state.regs.array.data[0x05];
      snapshot->header.vdc_reg_06 = self->vdc_6845->state.regs.array.data[0x06];
      snapshot->header.vdc_reg_07 = self->vdc_6845->state.regs.array.data[0x07];
      snapshot->header.vdc_reg_08 = self->vdc_6845->state.regs.array.data[0x08];
      snapshot->header.vdc_reg_09 = self->vdc_6845->state.regs.array.data[0x09];
      snapshot->header.vdc_reg_10 = self->vdc_6845->state.regs.array.data[0x0a];
      snapshot->header.vdc_reg_11 = self->vdc_6845->state.regs.array.data[0x0b];
      snapshot->header.vdc_reg_12 = self->vdc_6845->state.regs.array.data[0x0c];
      snapshot->header.vdc_reg_13 = self->vdc_6845->state.regs.array.data[0x0d];
      snapshot->header.vdc_reg_14 = self->vdc_6845->state.regs.array.data[0x0e];
      snapshot->header.vdc_reg_15 = self->vdc_6845->state.regs.array.data[0x0f];
      snapshot->header.vdc_reg_16 = self->vdc_6845->state.regs.array.data[0x10];
      snapshot->header.vdc_reg_17 = self->vdc_6845->state.regs.array.data[0x11];
    }
  }
  /* rom select */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.rom_select = self->memory.rom.config;
    }
  }
  /* ppi */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.ppi_port_a = self->ppi_8255->state.port_a;
      snapshot->header.ppi_port_b = self->ppi_8255->state.port_b;
      snapshot->header.ppi_port_c = self->ppi_8255->state.port_c;
      snapshot->header.ppi_ctrl_p = self->ppi_8255->state.ctrl_p;
    }
  }
  /* psg */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.psg_reg_ix = self->psg_8910->state.regs.array.addr;
      snapshot->header.psg_reg_00 = self->psg_8910->state.regs.array.data[0x00];
      snapshot->header.psg_reg_01 = self->psg_8910->state.regs.array.data[0x01];
      snapshot->header.psg_reg_02 = self->psg_8910->state.regs.array.data[0x02];
      snapshot->header.psg_reg_03 = self->psg_8910->state.regs.array.data[0x03];
      snapshot->header.psg_reg_04 = self->psg_8910->state.regs.array.data[0x04];
      snapshot->header.psg_reg_05 = self->psg_8910->state.regs.array.data[0x05];
      snapshot->header.psg_reg_06 = self->psg_8910->state.regs.array.data[0x06];
      snapshot->header.psg_reg_07 = self->psg_8910->state.regs.array.data[0x07];
      snapshot->header.psg_reg_08 = self->psg_8910->state.regs.array.data[0x08];
      snapshot->header.psg_reg_09 = self->psg_8910->state.regs.array.data[0x09];
      snapshot->header.psg_reg_10 = self->psg_8910->state.regs.array.data[0x0a];
      snapshot->header.psg_reg_11 = self->psg_8910->state.regs.array.data[0x0b];
      snapshot->header.psg_reg_12 = self->psg_8910->state.regs.array.data[0x0c];
      snapshot->header.psg_reg_13 = self->psg_8910->state.regs.array.data[0x0d];
      snapshot->header.psg_reg_14 = self->psg_8910->state.regs.array.data[0x0e];
      snapshot->header.psg_reg_15 = self->psg_8910->state.regs.array.data[0x0f];
    }
  }
  /* ram */ {
    size_t ram_size = self->ramsize * 1024UL;
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      snapshot->header.ram_size_l = ((unsigned char)((ram_size >> 10) & 0xff));
      snapshot->header.ram_size_h = ((unsigned char)((ram_size >> 18) & 0xff));
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      if(ram_size == 0UL) {
        status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
      }
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      size_t         snap_size  = self->ramsize * 1024UL;
      unsigned int   bank_index = 0;
      unsigned int   bank_count = countof(snapshot->memory);
      for(bank_index = 0; bank_index < bank_count; ++bank_index) {
        unsigned char* snap_data = self->ram_bank[bank_index]->state.data;
        unsigned char* bank_data = snapshot->memory[bank_index].data;
        size_t         bank_size = sizeof(snapshot->memory[bank_index].data);
        if(snap_size >= bank_size) {
          (void) memcpy(bank_data, snap_data, bank_size);
          snap_size -= bank_size;
        }
        else {
          break;
        }
      }
      if(snap_size != 0) {
        status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
      }
    }
  }
  /* save snapshot */ {
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
      status = xcpc_snapshot_save(snapshot, filename);
    }
  }
  /* cleanup */ {
    snapshot = xcpc_snapshot_delete(snapshot);
  }
  /* check for error */ {
    if(status != XCPC_SNAPSHOT_STATUS_SUCCESS) {
      g_log(XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "save snapshot : %s", xcpc_snapshot_strerror(status));
    }
  }
}

void amstrad_cpc_insert_drive0(AMSTRAD_CPC_EMULATOR* self, const char *filename)
{
  xcpc_fdc_765a_insert(self->fdc_765a, filename, 0);
}

void amstrad_cpc_insert_drive1(AMSTRAD_CPC_EMULATOR* self, const char *filename)
{
  xcpc_fdc_765a_insert(self->fdc_765a, filename, 1);
}

unsigned long amstrad_cpc_create_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
  if(self != NULL) {
    amstrad_cpc_start(self);
  }
  return 0UL;
}

unsigned long amstrad_cpc_destroy_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
  if(self != NULL) {
    amstrad_cpc_close(self);
  }
  return 0UL;
}

unsigned long amstrad_cpc_realize_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
  if(self != NULL) {
    /* realize */ {
        (void) xcpc_blitter_realize ( self->blitter
                                    , self->monitor_model
                                    , XtDisplay(widget)
                                    , XtWindow(widget)
                                    , (self->settings->no_xshm == FALSE ? True : False) );
    }
    /* init paint handler */ {
      switch(self->blitter->state.image->bits_per_pixel) {
        case 8:
          self->paint.proc = &amstrad_cpc_paint_08bpp;
          break;
        case 16:
          self->paint.proc = &amstrad_cpc_paint_16bpp;
          break;
        case 32:
          self->paint.proc = &amstrad_cpc_paint_32bpp;
          break;
        default:
          self->paint.proc = &amstrad_cpc_paint_default;
          break;
      }
    }
    /* init keybd handler */ {
      switch(self->keyboard_layout) {
        case XCPC_KEYBOARD_LAYOUT_QWERTY:
          self->keybd.proc = &amstrad_cpc_keybd_qwerty;
          break;
        case XCPC_KEYBOARD_LAYOUT_AZERTY:
          self->keybd.proc = &amstrad_cpc_keybd_azerty;
          break;
        default:
          self->keybd.proc = &amstrad_cpc_keybd_default;
          break;
      }
    }
  }
  return 0UL;
}

unsigned long amstrad_cpc_resize_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent *event)
{
  if(self != NULL) {
    (void) xcpc_blitter_resize(self->blitter, event);
  }
  return 0UL;
}

unsigned long amstrad_cpc_redraw_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent *event)
{
  if(self != NULL) {
    (void) xcpc_blitter_expose(self->blitter, event);
  }
  return 0UL;
}

unsigned long amstrad_cpc_timer_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
  GdevZ80CPU *z80cpu            = self->z80cpu;
  GdevDeviceClass *z80cpu_class = GDEV_DEVICE_GET_CLASS(z80cpu);
  unsigned long elapsed = 0;
  unsigned long timeout = 0;

  /* process each scanline */ {
    self->cur_scanline = 0;
    do {
      int cpu_tick;
      for(cpu_tick = 0; cpu_tick < self->cpu_period; cpu_tick += 4) {
        xcpc_vdc_6845_clock(self->vdc_6845);
        if((z80cpu->ccounter += 4) > 0) {
          gint ccounter = z80cpu->ccounter;
          (*z80cpu_class->clock)(GDEV_DEVICE(z80cpu));
          z80cpu->ccounter = ccounter - (((ccounter - z80cpu->ccounter) + 3) & (~3));
        }
      }
    } while(++self->cur_scanline < 312);
  }
  /* compute the elapsed time in us */ {
    struct timeval prev_time = self->timer.deadline;
    struct timeval curr_time;
    if(gettimeofday(&curr_time, NULL) == 0) {
      const long long t1 = (((long long) prev_time.tv_sec) * 1000000LL) + ((long long) prev_time.tv_usec);
      const long long t2 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
      if(t2 >= t1) {
        elapsed = ((unsigned long)(t2 - t1));
      }
      else {
        elapsed = 0UL;
      }
      if(elapsed >= 1000000UL) {
        self->timer.deadline = curr_time;
        elapsed              = 0UL;
      }
    }
  }
  /* draw the frame and compute stats if needed */ {
    if(elapsed <= self->frame.time) {
      (*self->paint.proc)(self);
      ++self->frame.drawn;
    }
    if(++self->frame.count == self->frame.rate) {
      compute_stats(self);
    }
  }
  /* compute the next frame absolute time */ {
    if((self->timer.deadline.tv_usec += self->frame.time) >= 1000000) {
      self->timer.deadline.tv_usec -= 1000000;
      self->timer.deadline.tv_sec  += 1;
    }
  }
  /* compute the deadline timeout in us */ {
    struct timeval next_time = self->timer.deadline;
    struct timeval curr_time;
    if(gettimeofday(&curr_time, NULL) == 0) {
      const long long t1 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
      const long long t2 = (((long long) next_time.tv_sec) * 1000000LL) + ((long long) next_time.tv_usec);
      if(t2 >= t1) {
        timeout = ((unsigned long)(t2 - t1));
      }
      else {
        timeout = 0UL;
      }
    }
  }
  /* schedule the next frame in ms */ {
    if((timeout /= 1000UL) == 0UL) {
      timeout = 1UL;
    }
  }
  return timeout;
}

unsigned long amstrad_cpc_input_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent *event)
{
  switch(event->type) {
    case KeyPress:
      (*self->keybd.proc)(self, event);
      break;
    case KeyRelease:
      (*self->keybd.proc)(self, event);
      break;
    case ButtonPress:
      (*self->mouse.proc)(self, event);
      break;
    case ButtonRelease:
      (*self->mouse.proc)(self, event);
      break;
    case MotionNotify:
      (*self->mouse.proc)(self, event);
      break;
    default:
      break;
  }
  return 0UL;
}
