/*
 * settings.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "settings-priv.h"

static const char arg_drive0[]   = "--drive0=";
static const char arg_drive1[]   = "--drive1=";
static const char arg_snapshot[] = "--snapshot=";

static int check_arg(const char* expected, const char* argument)
{
    int result = strncmp(expected, argument, strlen(expected));

    return result == 0;
}

static char* replace_setting(char* actual, const char* string, int flag)
{
    if(actual != NULL) {
        actual = xcpc_free("string", actual);
    }
    if(string != NULL) {
        const char* equals = strchr(string, '=');
        if(equals != NULL) {
            string = equals + 1;
        }
        actual = xcpc_malloc("string", (strlen(string) + 1));
        actual = strcpy(actual, string);
    }
    return actual;
}

static void xcpc_settings_trace(const char* function)
{
    xcpc_trace("XcpcSettings::%s()", function);
}

XcpcSettings* xcpc_settings_alloc(void)
{
    xcpc_settings_trace("alloc");

    return xcpc_new(XcpcSettings);
}

XcpcSettings* xcpc_settings_free(XcpcSettings* self)
{
    xcpc_settings_trace("free");

    return xcpc_delete(XcpcSettings, self);
}

XcpcSettings* xcpc_settings_construct(XcpcSettings* self)
{
    xcpc_settings_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcSettingsIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcSettingsState));
    }
    return self;
}

XcpcSettings* xcpc_settings_destruct(XcpcSettings* self)
{
    xcpc_settings_trace("destruct");

    /* destruct */ {
        self->state.program  = replace_setting(self->state.program , NULL, 0);
        self->state.drive0   = replace_setting(self->state.drive0  , NULL, 0);
        self->state.drive1   = replace_setting(self->state.drive1  , NULL, 0);
        self->state.snapshot = replace_setting(self->state.snapshot, NULL, 0);
    }
    return self;
}

XcpcSettings* xcpc_settings_new(void)
{
    xcpc_settings_trace("new");

    return xcpc_settings_construct(xcpc_settings_alloc());
}

XcpcSettings* xcpc_settings_delete(XcpcSettings* self)
{
    xcpc_settings_trace("delete");

    return xcpc_settings_free(xcpc_settings_destruct(self));
}

XcpcSettings* xcpc_settings_parse(XcpcSettings* self, int* argcp, char*** argvp)
{
    xcpc_settings_trace("parse");

    if((argvp == NULL) || (argvp == NULL) || (*argvp == NULL)) {
        int    argi = 0;
        int    argc = *argcp;
        char** argv = *argvp;
        while(argi < argc) {
            const char* argument = argv[argi];
            if(argi == 0) {
                self->state.program = replace_setting(self->state.program, argument, 0);
            }
            else if(check_arg(arg_drive0, argument) != 0) {
                self->state.drive0 = replace_setting(self->state.drive0, argument, 1);
            }
            else if(check_arg(arg_drive1, argument) != 0) {
                self->state.drive1 = replace_setting(self->state.drive1, argument, 1);
            }
            else if(check_arg(arg_snapshot, argument) != 0) {
                self->state.snapshot = replace_setting(self->state.snapshot, argument, 1);
            }
            ++argi;
        }
    }
    /* debug */ {
        xcpc_debug("%s", self->state.program );
        xcpc_debug("%s", self->state.drive0  );
        xcpc_debug("%s", self->state.drive1  );
        xcpc_debug("%s", self->state.snapshot);
    }
    return self;
}
