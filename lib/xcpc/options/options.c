/*
 * options.c - Copyright (c) 2001-2023 - Olivier Poncet
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
#include "options-priv.h"

static const char val_not_set[]  = "{not-set}";

static const char arg_company[]  = "--company={value}";
static const char arg_machine[]  = "--machine={value}";
static const char arg_monitor[]  = "--monitor={value}";
static const char arg_refresh[]  = "--refresh={value}";
static const char arg_keyboard[] = "--keyboard={value}";
static const char arg_sysrom[]   = "--sysrom={filename}";
static const char arg_rom000[]   = "--rom000={filename}";
static const char arg_rom001[]   = "--rom001={filename}";
static const char arg_rom002[]   = "--rom002={filename}";
static const char arg_rom003[]   = "--rom003={filename}";
static const char arg_rom004[]   = "--rom004={filename}";
static const char arg_rom005[]   = "--rom005={filename}";
static const char arg_rom006[]   = "--rom006={filename}";
static const char arg_rom007[]   = "--rom007={filename}";
static const char arg_rom008[]   = "--rom008={filename}";
static const char arg_rom009[]   = "--rom009={filename}";
static const char arg_rom010[]   = "--rom010={filename}";
static const char arg_rom011[]   = "--rom011={filename}";
static const char arg_rom012[]   = "--rom012={filename}";
static const char arg_rom013[]   = "--rom013={filename}";
static const char arg_rom014[]   = "--rom014={filename}";
static const char arg_rom015[]   = "--rom015={filename}";
static const char arg_drive0[]   = "--drive0={filename}";
static const char arg_drive1[]   = "--drive1={filename}";
static const char arg_snapshot[] = "--snapshot={filename}";
static const char arg_turbo[]    = "--turbo";
static const char arg_no_turbo[] = "--no-turbo";
static const char arg_xshm[]     = "--xshm";
static const char arg_no_xshm[]  = "--no-xshm";
static const char arg_fps[]      = "--fps";
static const char arg_no_fps[]   = "--no-fps";
static const char arg_help[]     = "--help";
static const char arg_version[]  = "--version";
static const char arg_quiet[]    = "--quiet";
static const char arg_trace[]    = "--trace";
static const char arg_debug[]    = "--debug";

static const char txt_company[]  = "Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad";
static const char txt_machine[]  = "cpc464, cpc664, cpc6128";
static const char txt_monitor[]  = "color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm12";
static const char txt_refresh[]  = "50Hz, 60Hz";
static const char txt_keyboard[] = "qwerty, azerty";
static const char txt_sysrom[]   = "32Kb system rom";
static const char txt_rom000[]   = "16Kb expansion rom #00";
static const char txt_rom001[]   = "16Kb expansion rom #01";
static const char txt_rom002[]   = "16Kb expansion rom #02";
static const char txt_rom003[]   = "16Kb expansion rom #03";
static const char txt_rom004[]   = "16Kb expansion rom #04";
static const char txt_rom005[]   = "16Kb expansion rom #05";
static const char txt_rom006[]   = "16Kb expansion rom #07";
static const char txt_rom007[]   = "16Kb expansion rom #08";
static const char txt_rom008[]   = "16Kb expansion rom #09";
static const char txt_rom009[]   = "16Kb expansion rom #10";
static const char txt_rom010[]   = "16Kb expansion rom #11";
static const char txt_rom011[]   = "16Kb expansion rom #12";
static const char txt_rom012[]   = "16Kb expansion rom #13";
static const char txt_rom013[]   = "16Kb expansion rom #14";
static const char txt_rom014[]   = "16Kb expansion rom #15";
static const char txt_rom015[]   = "16Kb expansion rom #16";
static const char txt_drive0[]   = "drive0 disk image";
static const char txt_drive1[]   = "drive1 disk image";
static const char txt_snapshot[] = "initial snapshot";
static const char txt_turbo[]    = "enable the turbo mode";
static const char txt_no_turbo[] = "disable the turbo mode";
static const char txt_xshm[]     = "use the XShm extension";
static const char txt_no_xshm[]  = "don't use the XShm extension";
static const char txt_fps[]      = "print framerate";
static const char txt_no_fps[]   = "don't print framerate";
static const char txt_help[]     = "display this help and exit";
static const char txt_version[]  = "display the version and exit";
static const char txt_quiet[]    = "set the loglevel to quiet mode";
static const char txt_trace[]    = "set the loglevel to trace mode";
static const char txt_debug[]    = "set the loglevel to debug mode";

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcOptions::%s()", function);
}

static void print_usage(XcpcOptions* self)
{
    const char* format = "    %-24s    %s";

    xcpc_println("Usage: %s [toolkit-options] [program-options]", self->state.program);
    xcpc_println(""                                );
    xcpc_println("Help options:"                   );
    xcpc_println(format, arg_help    , txt_help    );
    xcpc_println(format, arg_version , txt_version );
    xcpc_println(""                                );
    xcpc_println("Emulation options:"              );
    xcpc_println(format, arg_company , txt_company );
    xcpc_println(format, arg_machine , txt_machine );
    xcpc_println(format, arg_monitor , txt_monitor );
    xcpc_println(format, arg_refresh , txt_refresh );
    xcpc_println(format, arg_keyboard, txt_keyboard);
    xcpc_println(format, arg_sysrom  , txt_sysrom  );
    xcpc_println(format, arg_rom000  , txt_rom000  );
    xcpc_println(format, arg_rom001  , txt_rom001  );
    xcpc_println(format, arg_rom002  , txt_rom002  );
    xcpc_println(format, arg_rom003  , txt_rom003  );
    xcpc_println(format, arg_rom004  , txt_rom004  );
    xcpc_println(format, arg_rom005  , txt_rom005  );
    xcpc_println(format, arg_rom006  , txt_rom006  );
    xcpc_println(format, arg_rom007  , txt_rom007  );
    xcpc_println(format, arg_rom008  , txt_rom008  );
    xcpc_println(format, arg_rom009  , txt_rom009  );
    xcpc_println(format, arg_rom010  , txt_rom010  );
    xcpc_println(format, arg_rom011  , txt_rom011  );
    xcpc_println(format, arg_rom012  , txt_rom012  );
    xcpc_println(format, arg_rom013  , txt_rom013  );
    xcpc_println(format, arg_rom014  , txt_rom014  );
    xcpc_println(format, arg_rom015  , txt_rom015  );
    xcpc_println(format, arg_drive0  , txt_drive0  );
    xcpc_println(format, arg_drive1  , txt_drive1  );
    xcpc_println(format, arg_snapshot, txt_snapshot);
    xcpc_println(""                                );
    xcpc_println("Misc. options:"                  );
    xcpc_println(format, arg_turbo   , txt_turbo   );
    xcpc_println(format, arg_no_turbo, txt_no_turbo);
    xcpc_println(format, arg_xshm    , txt_xshm    );
    xcpc_println(format, arg_no_xshm , txt_no_xshm );
    xcpc_println(format, arg_fps     , txt_fps     );
    xcpc_println(format, arg_no_fps  , txt_no_fps  );
    xcpc_println(""                                );
    xcpc_println("Debug options:"                  );
    xcpc_println(format, arg_quiet   , txt_quiet   );
    xcpc_println(format, arg_trace   , txt_trace   );
    xcpc_println(format, arg_debug   , txt_debug   );
    xcpc_println(""                                );
}

static void print_version(XcpcOptions* self)
{
    xcpc_println ( "Xcpc - Amstrad CPC emulator - v%d.%d.%d"
                 , xcpc_major_version()
                 , xcpc_minor_version()
                 , xcpc_micro_version() );
}

static char* replace_setting(char* actual, const char* string, int flag)
{
    if(actual != NULL) {
        actual = xcpc_free("string", actual);
    }
    if(string != NULL) {
        if(flag != 0) {
            const char* equals = strchr(string, '=');
            if(equals != NULL) {
                string = equals + 1;
            }
        }
        actual = xcpc_malloc("string", (strlen(string) + 1));
        actual = strcpy(actual, string);
    }
    return actual;
}

static int check_arg(const char* expected, const char* argument)
{
    const char* equals = strchr(expected, '=');
    int         result = 0;

    if(equals != NULL) {
        result = strncmp(expected, argument, ((equals - expected) + 1));
    }
    else {
        result = strcmp(expected, argument);
    }
    return result == 0;
}

XcpcOptions* xcpc_options_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcOptions);
}

XcpcOptions* xcpc_options_free(XcpcOptions* self)
{
    log_trace("free");

    return xcpc_delete(XcpcOptions, self);
}

XcpcOptions* xcpc_options_construct(XcpcOptions* self, int* argc, char*** argv)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcOptionsIface));
        (void) memset(&self->setup, 0, sizeof(XcpcOptionsSetup));
        (void) memset(&self->state, 0, sizeof(XcpcOptionsState));
    }
    /* initialize iface */ {
        (void) xcpc_options_set_iface(self, NULL);
    }
    /* initialize setup */ {
        self->setup.argc = argc;
        self->setup.argv = argv;
    }
    /* initialize state */ {
        self->state.program  = replace_setting(NULL, val_not_set, 0);
        self->state.company  = replace_setting(NULL, val_not_set, 0);
        self->state.machine  = replace_setting(NULL, val_not_set, 0);
        self->state.monitor  = replace_setting(NULL, val_not_set, 0);
        self->state.refresh  = replace_setting(NULL, val_not_set, 0);
        self->state.keyboard = replace_setting(NULL, val_not_set, 0);
        self->state.sysrom   = replace_setting(NULL, val_not_set, 0);
        self->state.rom000   = replace_setting(NULL, val_not_set, 0);
        self->state.rom001   = replace_setting(NULL, val_not_set, 0);
        self->state.rom002   = replace_setting(NULL, val_not_set, 0);
        self->state.rom003   = replace_setting(NULL, val_not_set, 0);
        self->state.rom004   = replace_setting(NULL, val_not_set, 0);
        self->state.rom005   = replace_setting(NULL, val_not_set, 0);
        self->state.rom006   = replace_setting(NULL, val_not_set, 0);
        self->state.rom007   = replace_setting(NULL, val_not_set, 0);
        self->state.rom008   = replace_setting(NULL, val_not_set, 0);
        self->state.rom009   = replace_setting(NULL, val_not_set, 0);
        self->state.rom010   = replace_setting(NULL, val_not_set, 0);
        self->state.rom011   = replace_setting(NULL, val_not_set, 0);
        self->state.rom012   = replace_setting(NULL, val_not_set, 0);
        self->state.rom013   = replace_setting(NULL, val_not_set, 0);
        self->state.rom014   = replace_setting(NULL, val_not_set, 0);
        self->state.rom015   = replace_setting(NULL, val_not_set, 0);
        self->state.drive0   = replace_setting(NULL, val_not_set, 0);
        self->state.drive1   = replace_setting(NULL, val_not_set, 0);
        self->state.snapshot = replace_setting(NULL, val_not_set, 0);
        self->state.turbo    = 0;
        self->state.xshm     = 1;
        self->state.fps      = 0;
        self->state.help     = 0;
        self->state.version  = 0;
        self->state.loglevel = xcpc_get_loglevel();
    }
    return self;
}

XcpcOptions* xcpc_options_destruct(XcpcOptions* self)
{
    log_trace("destruct");

    /* finalize state */ {
        self->state.program  = replace_setting(self->state.program , NULL, 0);
        self->state.company  = replace_setting(self->state.company , NULL, 0);
        self->state.machine  = replace_setting(self->state.machine , NULL, 0);
        self->state.monitor  = replace_setting(self->state.monitor , NULL, 0);
        self->state.refresh  = replace_setting(self->state.refresh , NULL, 0);
        self->state.keyboard = replace_setting(self->state.keyboard, NULL, 0);
        self->state.sysrom   = replace_setting(self->state.sysrom  , NULL, 0);
        self->state.rom000   = replace_setting(self->state.rom000  , NULL, 0);
        self->state.rom001   = replace_setting(self->state.rom001  , NULL, 0);
        self->state.rom002   = replace_setting(self->state.rom002  , NULL, 0);
        self->state.rom003   = replace_setting(self->state.rom003  , NULL, 0);
        self->state.rom004   = replace_setting(self->state.rom004  , NULL, 0);
        self->state.rom005   = replace_setting(self->state.rom005  , NULL, 0);
        self->state.rom006   = replace_setting(self->state.rom006  , NULL, 0);
        self->state.rom007   = replace_setting(self->state.rom007  , NULL, 0);
        self->state.rom008   = replace_setting(self->state.rom008  , NULL, 0);
        self->state.rom009   = replace_setting(self->state.rom009  , NULL, 0);
        self->state.rom010   = replace_setting(self->state.rom010  , NULL, 0);
        self->state.rom011   = replace_setting(self->state.rom011  , NULL, 0);
        self->state.rom012   = replace_setting(self->state.rom012  , NULL, 0);
        self->state.rom013   = replace_setting(self->state.rom013  , NULL, 0);
        self->state.rom014   = replace_setting(self->state.rom014  , NULL, 0);
        self->state.rom015   = replace_setting(self->state.rom015  , NULL, 0);
        self->state.drive0   = replace_setting(self->state.drive0  , NULL, 0);
        self->state.drive1   = replace_setting(self->state.drive1  , NULL, 0);
        self->state.snapshot = replace_setting(self->state.snapshot, NULL, 0);
        self->state.turbo    = 0;
        self->state.xshm     = 0;
        self->state.fps      = 0;
        self->state.help     = 0;
        self->state.version  = 0;
        self->state.loglevel = 0;
    }
    /* finalize setup */ {
        self->setup.argc = NULL;
        self->setup.argv = NULL;
    }
    return self;
}

XcpcOptions* xcpc_options_new(int* argc, char*** argv)
{
    log_trace("new");

    return xcpc_options_construct(xcpc_options_alloc(), argc, argv);
}

XcpcOptions* xcpc_options_delete(XcpcOptions* self)
{
    log_trace("delete");

    return xcpc_options_free(xcpc_options_destruct(self));
}

XcpcOptions* xcpc_options_set_iface(XcpcOptions* self, const XcpcOptionsIface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
    }
    return self;
}

XcpcOptions* xcpc_options_parse(XcpcOptions* self)
{
    log_trace("parse");

    if((self->setup.argv != NULL) && (self->setup.argv != NULL) && (*self->setup.argv != NULL)) {
        int    argi = 0;
        int    argc = *self->setup.argc;
        char** argv = *self->setup.argv;
        int    arg_count = 0;
        char** arg_table = *self->setup.argv;
        while(argi < argc) {
            char* argument = argv[argi];
            if(argi == 0) {
                self->state.program = replace_setting(self->state.program, argument, 0);
                *arg_table++ = argument; ++arg_count;
            }
            else if(check_arg(arg_company , argument) != 0) { self->state.company  = replace_setting(self->state.company  , argument, 1); }
            else if(check_arg(arg_machine , argument) != 0) { self->state.machine  = replace_setting(self->state.machine  , argument, 1); }
            else if(check_arg(arg_monitor , argument) != 0) { self->state.monitor  = replace_setting(self->state.monitor  , argument, 1); }
            else if(check_arg(arg_refresh , argument) != 0) { self->state.refresh  = replace_setting(self->state.refresh  , argument, 1); }
            else if(check_arg(arg_keyboard, argument) != 0) { self->state.keyboard = replace_setting(self->state.keyboard , argument, 1); }
            else if(check_arg(arg_sysrom  , argument) != 0) { self->state.sysrom   = replace_setting(self->state.sysrom   , argument, 1); }
            else if(check_arg(arg_rom000  , argument) != 0) { self->state.rom000   = replace_setting(self->state.rom000   , argument, 1); }
            else if(check_arg(arg_rom001  , argument) != 0) { self->state.rom001   = replace_setting(self->state.rom001   , argument, 1); }
            else if(check_arg(arg_rom002  , argument) != 0) { self->state.rom002   = replace_setting(self->state.rom002   , argument, 1); }
            else if(check_arg(arg_rom003  , argument) != 0) { self->state.rom003   = replace_setting(self->state.rom003   , argument, 1); }
            else if(check_arg(arg_rom004  , argument) != 0) { self->state.rom004   = replace_setting(self->state.rom004   , argument, 1); }
            else if(check_arg(arg_rom005  , argument) != 0) { self->state.rom005   = replace_setting(self->state.rom005   , argument, 1); }
            else if(check_arg(arg_rom006  , argument) != 0) { self->state.rom006   = replace_setting(self->state.rom006   , argument, 1); }
            else if(check_arg(arg_rom007  , argument) != 0) { self->state.rom007   = replace_setting(self->state.rom007   , argument, 1); }
            else if(check_arg(arg_rom008  , argument) != 0) { self->state.rom008   = replace_setting(self->state.rom008   , argument, 1); }
            else if(check_arg(arg_rom009  , argument) != 0) { self->state.rom009   = replace_setting(self->state.rom009   , argument, 1); }
            else if(check_arg(arg_rom010  , argument) != 0) { self->state.rom010   = replace_setting(self->state.rom010   , argument, 1); }
            else if(check_arg(arg_rom011  , argument) != 0) { self->state.rom011   = replace_setting(self->state.rom011   , argument, 1); }
            else if(check_arg(arg_rom012  , argument) != 0) { self->state.rom012   = replace_setting(self->state.rom012   , argument, 1); }
            else if(check_arg(arg_rom013  , argument) != 0) { self->state.rom013   = replace_setting(self->state.rom013   , argument, 1); }
            else if(check_arg(arg_rom014  , argument) != 0) { self->state.rom014   = replace_setting(self->state.rom014   , argument, 1); }
            else if(check_arg(arg_rom015  , argument) != 0) { self->state.rom015   = replace_setting(self->state.rom015   , argument, 1); }
            else if(check_arg(arg_drive0  , argument) != 0) { self->state.drive0   = replace_setting(self->state.drive0   , argument, 1); }
            else if(check_arg(arg_drive1  , argument) != 0) { self->state.drive1   = replace_setting(self->state.drive1   , argument, 1); }
            else if(check_arg(arg_snapshot, argument) != 0) { self->state.snapshot = replace_setting(self->state.snapshot , argument, 1); }
            else if(check_arg(arg_turbo   , argument) != 0) { self->state.turbo    = 1;                                                   }
            else if(check_arg(arg_no_turbo, argument) != 0) { self->state.turbo    = 0;                                                   }
            else if(check_arg(arg_xshm    , argument) != 0) { self->state.xshm     = 1;                                                   }
            else if(check_arg(arg_no_xshm , argument) != 0) { self->state.xshm     = 0;                                                   }
            else if(check_arg(arg_fps     , argument) != 0) { self->state.fps      = 1;                                                   }
            else if(check_arg(arg_no_fps  , argument) != 0) { self->state.fps      = 0;                                                   }
            else if(check_arg(arg_help    , argument) != 0) { self->state.help     = 1;                                                   }
            else if(check_arg(arg_version , argument) != 0) { self->state.version  = 1;                                                   }
            else if(check_arg(arg_quiet   , argument) != 0) { self->state.loglevel = XCPC_LOGLEVEL_QUIET;                                 }
            else if(check_arg(arg_trace   , argument) != 0) { self->state.loglevel = XCPC_LOGLEVEL_TRACE;                                 }
            else if(check_arg(arg_debug   , argument) != 0) { self->state.loglevel = XCPC_LOGLEVEL_DEBUG;                                 }
            else {
                *arg_table++ = argument; ++arg_count;
            }
            ++argi;
        }
        /* finalize adjusted command-line */ {
            *arg_table        = NULL;
            *self->setup.argc = arg_count;
        }
    }
    /* set loglevel */ {
        (void) xcpc_set_loglevel(self->state.loglevel);
    }
    /* debug */ {
        xcpc_log_debug("options.program  = %s", self->state.program );
        xcpc_log_debug("options.company  = %s", self->state.company );
        xcpc_log_debug("options.machine  = %s", self->state.machine );
        xcpc_log_debug("options.monitor  = %s", self->state.monitor );
        xcpc_log_debug("options.refresh  = %s", self->state.refresh );
        xcpc_log_debug("options.keyboard = %s", self->state.keyboard);
        xcpc_log_debug("options.sysrom   = %s", self->state.sysrom  );
        xcpc_log_debug("options.rom000   = %s", self->state.rom000  );
        xcpc_log_debug("options.rom001   = %s", self->state.rom001  );
        xcpc_log_debug("options.rom002   = %s", self->state.rom002  );
        xcpc_log_debug("options.rom003   = %s", self->state.rom003  );
        xcpc_log_debug("options.rom004   = %s", self->state.rom004  );
        xcpc_log_debug("options.rom005   = %s", self->state.rom005  );
        xcpc_log_debug("options.rom006   = %s", self->state.rom006  );
        xcpc_log_debug("options.rom007   = %s", self->state.rom007  );
        xcpc_log_debug("options.rom008   = %s", self->state.rom008  );
        xcpc_log_debug("options.rom009   = %s", self->state.rom009  );
        xcpc_log_debug("options.rom010   = %s", self->state.rom010  );
        xcpc_log_debug("options.rom011   = %s", self->state.rom011  );
        xcpc_log_debug("options.rom012   = %s", self->state.rom012  );
        xcpc_log_debug("options.rom013   = %s", self->state.rom013  );
        xcpc_log_debug("options.rom014   = %s", self->state.rom014  );
        xcpc_log_debug("options.rom015   = %s", self->state.rom015  );
        xcpc_log_debug("options.drive0   = %s", self->state.drive0  );
        xcpc_log_debug("options.drive1   = %s", self->state.drive1  );
        xcpc_log_debug("options.snapshot = %s", self->state.snapshot);
        xcpc_log_debug("options.turbo    = %d", self->state.turbo   );
        xcpc_log_debug("options.xshm     = %d", self->state.xshm    );
        xcpc_log_debug("options.fps      = %d", self->state.fps     );
        xcpc_log_debug("options.help     = %d", self->state.help    );
        xcpc_log_debug("options.version  = %d", self->state.version );
        xcpc_log_debug("options.loglevel = %d", self->state.loglevel);
    }
    if(self->state.help != 0) {
        print_usage(self);
        return self;
    }
    if(self->state.version != 0) {
        print_version(self);
        return self;
    }
    return self;
}

int xcpc_options_quit(XcpcOptions* self)
{
    if((self->state.help != 0) || (self->state.version != 0)) {
        return 1;
    }
    return 0;
}
