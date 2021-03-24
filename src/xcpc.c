/*
 * xcpc.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xcpc.h"

XcpcSettings xcpc_settings = {
    /* turbo           */ FALSE,   
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

static const char turbo_description[]        = "Turbo mode";
static const char no_xshm_description[]      = "Don't use the XShm extension";
static const char show_fps_description[]     = "Show fps statistics";
static const char model_description[]        = "cpc464, cpc664, cpc6128";
static const char monitor_description[]      = "color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm12";
static const char keyboard_description[]     = "qwerty, azerty";
static const char refresh_description[]      = "50Hz, 60Hz";
static const char manufacturer_description[] = "Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad";
static const char snapshot_description[]     = "Snapshot to load at start";
static const char system_rom_description[]   = "32Kb system rom";
static const char expansion00_description[]  = "16Kb expansion rom #00";
static const char expansion01_description[]  = "16Kb expansion rom #01";
static const char expansion02_description[]  = "16Kb expansion rom #02";
static const char expansion03_description[]  = "16Kb expansion rom #03";
static const char expansion04_description[]  = "16Kb expansion rom #04";
static const char expansion05_description[]  = "16Kb expansion rom #05";
static const char expansion06_description[]  = "16Kb expansion rom #06";
static const char expansion07_description[]  = "16Kb expansion rom #07";
static const char expansion08_description[]  = "16Kb expansion rom #08";
static const char expansion09_description[]  = "16Kb expansion rom #09";
static const char expansion10_description[]  = "16Kb expansion rom #10";
static const char expansion11_description[]  = "16Kb expansion rom #11";
static const char expansion12_description[]  = "16Kb expansion rom #12";
static const char expansion13_description[]  = "16Kb expansion rom #13";
static const char expansion14_description[]  = "16Kb expansion rom #14";
static const char expansion15_description[]  = "16Kb expansion rom #15";

static GOptionEntry xcpc_entries[] = {
    { "turbo"       , 0, 0, G_OPTION_ARG_NONE    , &xcpc_settings.turbo          , turbo_description        , NULL                },
    { "no-xshm"     , 0, 0, G_OPTION_ARG_NONE    , &xcpc_settings.no_xshm        , no_xshm_description      , NULL                },
    { "show-fps"    , 0, 0, G_OPTION_ARG_NONE    , &xcpc_settings.show_fps       , show_fps_description     , NULL                },
    { "model"       , 0, 0, G_OPTION_ARG_STRING  , &xcpc_settings.computer_model , model_description        , "{computer-model}"  },
    { "monitor"     , 0, 0, G_OPTION_ARG_STRING  , &xcpc_settings.monitor_model  , monitor_description      , "{monitor-model}"   },
    { "keyboard"    , 0, 0, G_OPTION_ARG_STRING  , &xcpc_settings.keyboard_layout, keyboard_description     , "{keyboard-layout}" },
    { "refresh"     , 0, 0, G_OPTION_ARG_STRING  , &xcpc_settings.refresh_rate   , refresh_description      , "{refresh-rate}"    },
    { "manufacturer", 0, 0, G_OPTION_ARG_STRING  , &xcpc_settings.manufacturer   , manufacturer_description , "{manufacturer}"    },
    { "snapshot"    , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.snapshot       , snapshot_description     , "{filename}"        },
    { "sysrom"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.system_rom     , system_rom_description   , "{filename}"        },
    { "rom000"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x0] , expansion00_description  , "{filename}"        },
    { "rom001"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x1] , expansion01_description  , "{filename}"        },
    { "rom002"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x2] , expansion02_description  , "{filename}"        },
    { "rom003"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x3] , expansion03_description  , "{filename}"        },
    { "rom004"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x4] , expansion04_description  , "{filename}"        },
    { "rom005"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x5] , expansion05_description  , "{filename}"        },
    { "rom006"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x6] , expansion06_description  , "{filename}"        },
    { "rom007"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x7] , expansion07_description  , "{filename}"        },
    { "rom008"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x8] , expansion08_description  , "{filename}"        },
    { "rom009"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0x9] , expansion09_description  , "{filename}"        },
    { "rom010"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xa] , expansion10_description  , "{filename}"        },
    { "rom011"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xb] , expansion11_description  , "{filename}"        },
    { "rom012"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xc] , expansion12_description  , "{filename}"        },
    { "rom013"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xd] , expansion13_description  , "{filename}"        },
    { "rom014"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xe] , expansion14_description  , "{filename}"        },
    { "rom015"      , 0, 0, G_OPTION_ARG_FILENAME, &xcpc_settings.expansion[0xf] , expansion15_description  , "{filename}"        },
    { NULL }
};

int xcpc_parse(int* argc, char*** argv)
{
    GOptionContext* context = g_option_context_new("xcpc");

    /* add entries to option-context */ {
        g_option_context_add_main_entries(context, xcpc_entries, NULL);
    }
    /* parse options */ {
        if(g_option_context_parse(context, argc, argv, NULL) != FALSE) {
            /* valid argument(s) */
        }
        else {
            /* invalid argument(s) */
        }
    }
    /* parse options */ {
        context = (g_option_context_free(context), NULL);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    return xcpc(&argc, &argv);
}
