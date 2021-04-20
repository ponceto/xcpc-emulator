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

static gboolean destroy_callback(GtkWidget* widget, GtkWidget** widget_reference)
{
    if((widget_reference != NULL) && (*widget_reference == widget)) {
        xcpc_log_print("destroy %s", gtk_widget_get_name(widget));
        *widget_reference = NULL;
    }
    return FALSE;
}

static void widget_signal_connect_destroy(GtkWidget* widget, GtkWidget** widget_reference)
{
    (void) g_signal_connect(widget, sig_destroy, G_CALLBACK(&destroy_callback), widget_reference);
}

static void toolitem_signal_connect_destroy(GtkToolItem* widget, GtkToolItem** widget_reference)
{
    (void) g_signal_connect(GTK_WIDGET(widget), sig_destroy, G_CALLBACK(&destroy_callback), widget_reference);
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static void build_file_menu(XcpcApplication* self)
{
    XcpcFileMenuRec* menu = &self->menubar.file;

    /* menu */ {
        menu->menu = gtk_menu_new();
        widget_signal_connect_destroy(menu->menu, &menu->menu);
    }
    /* item */ {
        menu->item = gtk_menu_item_new_with_label("File");;
        widget_signal_connect_destroy(menu->item, &menu->item);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->item), menu->menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(self->menubar.widget), menu->item);
    }
    /* load-snapshot */ {
        menu->load_snapshot = gtk_menu_item_new_with_label("Load snapshot...");;
        widget_signal_connect_destroy(menu->load_snapshot, &menu->load_snapshot);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu), menu->load_snapshot);
    }
    /* save-snapshot */ {
        menu->save_snapshot = gtk_menu_item_new_with_label("Save snapshot...");;
        widget_signal_connect_destroy(menu->save_snapshot, &menu->save_snapshot);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu), menu->save_snapshot);
    }
    /* separator1 */ {
        menu->separator1 = gtk_separator_menu_item_new();;
        widget_signal_connect_destroy(menu->separator1, &menu->separator1);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu), menu->separator1);
    }
    /* exit */ {
        menu->exit = gtk_menu_item_new_with_label("Exit");;
        widget_signal_connect_destroy(menu->exit, &menu->exit);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu), menu->exit);
    }
}

static void build_ctrl_menu(XcpcApplication* self)
{
    XcpcCtrlMenuRec* menu = &self->menubar.ctrl;

    /* menu */ {
        menu->menu = gtk_menu_new();
        widget_signal_connect_destroy(menu->menu, &menu->menu);
    }
    /* item */ {
        menu->item = gtk_menu_item_new_with_label("Controls");;
        widget_signal_connect_destroy(menu->item, &menu->item);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->item), menu->menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(self->menubar.widget), menu->item);
    }
}

static void build_drv0_menu(XcpcApplication* self)
{
    XcpcDrv0MenuRec* menu = &self->menubar.drv0;

    /* menu */ {
        menu->menu = gtk_menu_new();
        widget_signal_connect_destroy(menu->menu, &menu->menu);
    }
    /* item */ {
        menu->item = gtk_menu_item_new_with_label("Drive A");;
        widget_signal_connect_destroy(menu->item, &menu->item);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->item), menu->menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(self->menubar.widget), menu->item);
    }
}

static void build_drv1_menu(XcpcApplication* self)
{
    XcpcDrv1MenuRec* menu = &self->menubar.drv1;

    /* menu */ {
        menu->menu = gtk_menu_new();
        widget_signal_connect_destroy(menu->menu, &menu->menu);
    }
    /* item */ {
        menu->item = gtk_menu_item_new_with_label("Drive B");;
        widget_signal_connect_destroy(menu->item, &menu->item);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->item), menu->menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(self->menubar.widget), menu->item);
    }
}

static void build_help_menu(XcpcApplication* self)
{
    XcpcHelpMenuRec* menu = &self->menubar.help;

    /* menu */ {
        menu->menu = gtk_menu_new();
        widget_signal_connect_destroy(menu->menu, &menu->menu);
    }
    /* item */ {
        menu->item = gtk_menu_item_new_with_label("Help");;
        widget_signal_connect_destroy(menu->item, &menu->item);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->item), menu->menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(self->menubar.widget), menu->item);
    }
}

static void build_menubar(XcpcApplication* self)
{
    XcpcMenuBarRec* menubar = &self->menubar;

    /* create menubar */ {
        menubar->widget = gtk_menu_bar_new();
        widget_signal_connect_destroy(menubar->widget, &menubar->widget);
        gtk_box_pack_start(GTK_BOX(self->layout.vbox), menubar->widget, FALSE, TRUE, 0);
    }
    /* build menus */ {
        build_file_menu(self);
        build_ctrl_menu(self);
        build_drv0_menu(self);
        build_drv1_menu(self);
        build_help_menu(self);
    }
}

static void build_toolbar(XcpcApplication* self)
{
    XcpcToolBarRec* toolbar = &self->toolbar;

    /* create toolbar */ {
        toolbar->widget = gtk_toolbar_new();
        widget_signal_connect_destroy(toolbar->widget, &toolbar->widget);
        gtk_box_pack_start(GTK_BOX(self->layout.vbox), toolbar->widget, FALSE, TRUE, 0);
    }
    /* create load-snapshot */ {
        toolbar->load_snapshot = gtk_tool_button_new(NULL, NULL);
        toolitem_signal_connect_destroy(toolbar->load_snapshot, &toolbar->load_snapshot);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar->load_snapshot), "document-open-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar->widget), toolbar->load_snapshot, 0);
    }
    /* create save-snapshot */ {
        toolbar->save_snapshot = gtk_tool_button_new(NULL, NULL);
        toolitem_signal_connect_destroy(toolbar->save_snapshot, &toolbar->save_snapshot);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar->save_snapshot), "document-save-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar->widget), toolbar->save_snapshot, 1);
    }
    /* create play-emulator */ {
        toolbar->play_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_signal_connect_destroy(toolbar->play_emulator, &toolbar->play_emulator);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar->play_emulator), "media-playback-start-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar->widget), toolbar->play_emulator, 2);
    }
    /* create pause-emulator */ {
        toolbar->pause_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_signal_connect_destroy(toolbar->pause_emulator, &toolbar->pause_emulator);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar->pause_emulator), "media-playback-pause-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar->widget), toolbar->pause_emulator, 3);
    }
    /* create reset-emulator */ {
        toolbar->reset_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_signal_connect_destroy(toolbar->reset_emulator, &toolbar->reset_emulator);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar->reset_emulator), "media-playlist-repeat-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar->widget), toolbar->reset_emulator, 4);
    }
}

static void build_emulator(XcpcApplication* self)
{
    XcpcLayoutRec* layout = &self->layout;

    /* create emulator */ {
        layout->emulator = gtk_drawing_area_new();
        widget_signal_connect_destroy(layout->emulator, &layout->emulator);
        gtk_box_pack_start(GTK_BOX(self->layout.vbox), layout->emulator, TRUE, TRUE, 0);
    }
}

static void build_infobar(XcpcApplication* self)
{
    XcpcInfoBarRec* infobar = &self->infobar;

    /* create infobar */ {
        infobar->widget = gtk_button_new_with_label("infobar");
        widget_signal_connect_destroy(infobar->widget, &infobar->widget);
        gtk_box_pack_start(GTK_BOX(self->layout.vbox), infobar->widget, FALSE, TRUE, 0);
    }
}

static void build_layout(GtkApplication* application, XcpcApplication* self)
{
    /* create window */ {
        self->layout.window = gtk_application_window_new(application);
        widget_signal_connect_destroy(self->layout.window, &self->layout.window);
        gtk_window_set_title(GTK_WINDOW(self->layout.window), "Xcpc - Amstrad CPC emulator");
        gtk_window_set_default_size(GTK_WINDOW(self->layout.window), 640, 480);
    }
    /* create vbox */ {
        self->layout.vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        widget_signal_connect_destroy(self->layout.vbox, &self->layout.vbox);
        gtk_container_add(GTK_CONTAINER(self->layout.window), self->layout.vbox);
    }
    /* menubar */ {
        (void) build_menubar(self);
    }
    /* toolbar */ {
        (void) build_toolbar(self);
    }
    /* emulator */ {
        (void) build_emulator(self);
    }
    /* infobar */ {
        (void) build_infobar(self);
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
