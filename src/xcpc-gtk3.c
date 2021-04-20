/*
 * xcpc-gtk3.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xcpc-gtk3-priv.h"

/*
 * ---------------------------------------------------------------------------
 * some useful stuff
 * ---------------------------------------------------------------------------
 */

static const gchar* const sig_destroy  = "destroy";
static const gchar* const sig_activate = "activate";

/*
 * ---------------------------------------------------------------------------
 * callbacks
 * ---------------------------------------------------------------------------
 */

gboolean destroy_callback(GtkWidget* widget, GtkWidget** widget_reference)
{
    if((widget_reference != NULL) && (*widget_reference == widget)) {
        xcpc_log_print("destroy_callback <%s>", gtk_widget_get_name(widget));
        *widget_reference = NULL;
    }
    return FALSE;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static void build_layout(GtkApplication* application, XcpcApplication* self)
{
    /* create window */ {
        self->layout.window = gtk_application_window_new(application);
        gtk_widget_set_name(GTK_WIDGET(self->layout.window), "main-window");
        gtk_window_set_title(GTK_WINDOW(self->layout.window), "Xcpc - Amstrad CPC emulator");
        gtk_window_set_default_size(GTK_WINDOW(self->layout.window), 640, 480);
        (void) g_signal_connect(self->layout.window, sig_destroy, G_CALLBACK(&destroy_callback), &self->layout.window);
    }
    /* show all */ {
        gtk_widget_show_all(self->layout.window);
    }
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* xcpc_application_new(void)
{
    XcpcApplication* self = g_new0(XcpcApplication, 1);

    /* intialize the machine */ {
        self->machine = xcpc_machine_new();
    }
    return self;
}

XcpcApplication* xcpc_application_delete(XcpcApplication* self)
{
    /* finalize */ {
        self->layout.application = (g_object_unref(self->layout.application), NULL);
    }
    /* finalize the emulator */ {
        self->machine = xcpc_machine_delete(self->machine);
    }
    /* free */ {
        self = (g_free(self), NULL);
    }
    return self;
}

XcpcApplication* xcpc_application_run(XcpcApplication* self, int* argc, char*** argv)
{
    /* parse the command-line */ {
        (void) xcpc_machine_parse(self->machine, argc, argv);
    }
    /* create application */ {
        self->layout.application = gtk_application_new("org.gtk.xcpc", G_APPLICATION_FLAGS_NONE);
        (void) g_signal_connect(self->layout.application, sig_activate, G_CALLBACK(&build_layout), self);
    }
    /* loop */ {
        (void) g_application_run(G_APPLICATION(self->layout.application), *argc, *argv);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication
 * ---------------------------------------------------------------------------
 */

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplication* self = xcpc_application_new();

    /* run */ {
        (void) xcpc_application_run(self, argc, argv);
    }
    /* delete */ {
        self = xcpc_application_delete(self);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
