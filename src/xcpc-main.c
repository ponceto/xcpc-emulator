/*
 * xcpc-main.c - Copyright (c) 2001-2022 - Olivier Poncet
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
#include <limits.h>
#include "xcpc-main-priv.h"

static char XAPPLRESDIR[PATH_MAX + 1] = "{not-set}";

/*
 * ---------------------------------------------------------------------------
 * setup
 * ---------------------------------------------------------------------------
 */

static int setup(void)
{
    /* XAPPLRESDIR */ {
        const char* var_name = "XAPPLRESDIR";
        const char* var_value = getenv(var_name);
        const char* var_default = XCPC_RESDIR;
        if((var_value == NULL) || (*var_value == '\0')) {
            (void) snprintf(XAPPLRESDIR, sizeof(XAPPLRESDIR), "%s=%s", var_name, var_default);
        }
        else {
            (void) snprintf(XAPPLRESDIR, sizeof(XAPPLRESDIR), "%s=%s", var_name, var_value);
        }
        (void) putenv(XAPPLRESDIR);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------------
 */

int main(int argc, char* argv[])
{
    int status = setup();

    if(status == EXIT_SUCCESS) {
        xcpc_begin();
        status = xcpc_main(&argc, &argv);
        xcpc_end();
    }
    return status;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
