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

#ifndef _
#define _(string) translate_string(string)
#endif

/*
 * ---------------------------------------------------------------------------
 * some useful constants
 * ---------------------------------------------------------------------------
 */

static const gchar application_id[] = "org.gtk.xcpc";
static const gchar sig_activate[]   = "activate";
static const gchar sig_destroy[]    = "destroy";
static const gchar sig_clicked[]    = "clicked";

/*
 * ---------------------------------------------------------------------------
 * callbacks
 * ---------------------------------------------------------------------------
 */

static gboolean destroy_callback(GtkWidget* widget, GtkWidget** reference)
{
    if((reference != NULL) && (*reference == widget)) {
        xcpc_log_debug("%s::%s()", gtk_widget_get_name(GTK_WIDGET(widget)), sig_destroy);
        *reference = NULL;
    }
    return FALSE;
}

static void activate_callback(GtkWidget* widget, XcpcApplication* self)
{
    xcpc_log_debug("%s::%s()", gtk_widget_get_name(GTK_WIDGET(widget)), sig_activate);
}

static void clicked_callback(GtkWidget* widget, XcpcApplication* self)
{
    xcpc_log_debug("%s::%s()", gtk_widget_get_name(GTK_WIDGET(widget)), sig_clicked);
}

/*
 * ---------------------------------------------------------------------------
 * some useful wrappers
 * ---------------------------------------------------------------------------
 */

static const char* translate_string(const char* string)
{
    xcpc_log_debug("translate '%s'", string);

    return string;
}

static void widget_add_destroy_callback(GtkWidget** reference, const gchar* name)
{
    if((reference != NULL) && (*reference != NULL)) {
        if(name != NULL) {
            gtk_widget_set_name(GTK_WIDGET(*reference), name);
        }
        (void) g_signal_connect(G_OBJECT(*reference), sig_destroy, G_CALLBACK(&destroy_callback), reference);
        xcpc_log_debug("%s::registered()", gtk_widget_get_name(GTK_WIDGET(*reference)));
    }
}

static void widget_add_activate_callback(GtkWidget* widget, XcpcApplication* self)
{
    (void) g_signal_connect(G_OBJECT(widget), sig_activate, G_CALLBACK(&activate_callback), self);
}

static void toolitem_add_destroy_callback(GtkToolItem** reference, const gchar* name)
{
    if((reference != NULL) && (*reference != NULL)) {
        if(name != NULL) {
            gtk_widget_set_name(GTK_WIDGET(*reference), name);
        }
        (void) g_signal_connect(G_OBJECT(*reference), sig_destroy, G_CALLBACK(&destroy_callback), reference);
        xcpc_log_debug("%s::registered()", gtk_widget_get_name(GTK_WIDGET(*reference)));
    }
}

static void toolitem_add_clicked_callback(GtkToolItem* widget, XcpcApplication* self)
{
    (void) g_signal_connect(G_OBJECT(widget), sig_clicked, G_CALLBACK(&clicked_callback), self);
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static void build_file_menu(XcpcApplication* self)
{
    XcpcLayoutRec*   layout  = &self->layout;
    XcpcMenuBarRec*  parent  = &layout->menubar;
    XcpcFileMenuRec* current = &parent->file;

    /* file-item */ {
        current->widget = gtk_menu_item_new_with_label(_("File"));
        widget_add_destroy_callback(&current->widget, "file-item");
        gtk_menu_shell_append(GTK_MENU_SHELL(parent->widget), current->widget);
    }
    /* file-menu */ {
        current->menu = gtk_menu_new();
        widget_add_destroy_callback(&current->menu, "file-menu");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(current->widget), current->menu);
    }
    /* file-load-snapshot */ {
        current->load_snapshot = gtk_menu_item_new_with_label(_("Load snapshot..."));
        widget_add_destroy_callback(&current->load_snapshot, "file-load-snapshot");
        widget_add_activate_callback(current->load_snapshot, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->load_snapshot);
    }
    /* file-save-snapshot */ {
        current->save_snapshot = gtk_menu_item_new_with_label(_("Save snapshot..."));
        widget_add_destroy_callback(&current->save_snapshot, "file-save-snapshot");
        widget_add_activate_callback(current->save_snapshot, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->save_snapshot);
    }
    /* file-separator1 */ {
        current->separator1 = gtk_separator_menu_item_new();
        widget_add_destroy_callback(&current->separator1, "file-separator1");
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->separator1);
    }
    /* file-exit */ {
        current->exit = gtk_menu_item_new_with_label(_("Exit"));
        widget_add_destroy_callback(&current->exit, "file-exit");
        widget_add_activate_callback(current->exit, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->exit);
    }
}

static void build_ctrl_menu(XcpcApplication* self)
{
    XcpcLayoutRec*   layout  = &self->layout;
    XcpcMenuBarRec*  parent  = &layout->menubar;
    XcpcCtrlMenuRec* current = &parent->ctrl;

    /* ctrl-item */ {
        current->widget = gtk_menu_item_new_with_label(_("Controls"));
        widget_add_destroy_callback(&current->widget, "ctrl-item");
        gtk_menu_shell_append(GTK_MENU_SHELL(parent->widget), current->widget);
    }
    /* ctrl-menu */ {
        current->menu = gtk_menu_new();
        widget_add_destroy_callback(&current->menu, "ctrl-menu");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(current->widget), current->menu);
    }
    /* ctrl-play-emulator */ {
        current->play_emulator = gtk_menu_item_new_with_label(_("Play"));
        widget_add_destroy_callback(&current->play_emulator, "ctrl-play");
        widget_add_activate_callback(current->play_emulator, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->play_emulator);
    }
    /* ctrl-pause-emulator */ {
        current->pause_emulator = gtk_menu_item_new_with_label(_("Pause"));
        widget_add_destroy_callback(&current->pause_emulator, "ctrl-pause");
        widget_add_activate_callback(current->pause_emulator, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->pause_emulator);
    }
    /* ctrl-reset-emulator */ {
        current->reset_emulator = gtk_menu_item_new_with_label(_("Reset"));
        widget_add_destroy_callback(&current->reset_emulator, "ctrl-reset");
        widget_add_activate_callback(current->reset_emulator, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->reset_emulator);
    }
}

static void build_drv0_menu(XcpcApplication* self)
{
    XcpcLayoutRec*   layout  = &self->layout;
    XcpcMenuBarRec*  parent  = &layout->menubar;
    XcpcDrv0MenuRec* current = &parent->drv0;

    /* drv0-item */ {
        current->widget = gtk_menu_item_new_with_label(_("Drive A"));
        widget_add_destroy_callback(&current->widget, "drv0-item");
        gtk_menu_shell_append(GTK_MENU_SHELL(parent->widget), current->widget);
    }
    /* drv0-menu */ {
        current->menu = gtk_menu_new();
        widget_add_destroy_callback(&current->menu, "drv0-menu");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(current->widget), current->menu);
    }
    /* drv0-drive0-insert */ {
        current->drive0_insert = gtk_menu_item_new_with_label(_("Insert disk..."));
        widget_add_destroy_callback(&current->drive0_insert, "drv0-insert");
        widget_add_activate_callback(current->drive0_insert, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive0_insert);
    }
    /* drv0-drive0-remove */ {
        current->drive0_remove = gtk_menu_item_new_with_label(_("Remove disk"));
        widget_add_destroy_callback(&current->drive0_remove, "drv0-remove");
        widget_add_activate_callback(current->drive0_remove, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive0_remove);
    }
}

static void build_drv1_menu(XcpcApplication* self)
{
    XcpcLayoutRec*   layout  = &self->layout;
    XcpcMenuBarRec*  parent  = &layout->menubar;
    XcpcDrv1MenuRec* current = &parent->drv1;

    /* drv1-item */ {
        current->widget = gtk_menu_item_new_with_label(_("Drive B"));
        widget_add_destroy_callback(&current->widget, "drv1-item");
        gtk_menu_shell_append(GTK_MENU_SHELL(parent->widget), current->widget);
    }
    /* drv1-menu */ {
        current->menu = gtk_menu_new();
        widget_add_destroy_callback(&current->menu, "drv1-menu");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(current->widget), current->menu);
    }
    /* drv1-drive1-insert */ {
        current->drive1_insert = gtk_menu_item_new_with_label(_("Insert disk..."));
        widget_add_destroy_callback(&current->drive1_insert, "drv1-insert");
        widget_add_activate_callback(current->drive1_insert, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive1_insert);
    }
    /* drv1-drive1-remove */ {
        current->drive1_remove = gtk_menu_item_new_with_label(_("Remove disk"));
        widget_add_destroy_callback(&current->drive1_remove, "drv1-remove");
        widget_add_activate_callback(current->drive1_remove, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive1_remove);
    }
}

static void build_help_menu(XcpcApplication* self)
{
    XcpcLayoutRec*   layout  = &self->layout;
    XcpcMenuBarRec*  parent  = &layout->menubar;
    XcpcHelpMenuRec* current = &parent->help;

    /* help-item */ {
        current->widget = gtk_menu_item_new_with_label(_("Help"));
        widget_add_destroy_callback(&current->widget, "help-item");
        gtk_menu_shell_append(GTK_MENU_SHELL(parent->widget), current->widget);
    }
    /* help-menu */ {
        current->menu = gtk_menu_new();
        widget_add_destroy_callback(&current->menu, "help-menu");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(current->widget), current->menu);
    }
    /* help-help */ {
        current->help = gtk_menu_item_new_with_label(_("Help"));
        widget_add_destroy_callback(&current->help, "help-help");
        widget_add_activate_callback(current->help, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->help);
    }
    /* help-legal */ {
        current->legal = gtk_menu_item_new_with_label(_("Legal info"));
        widget_add_destroy_callback(&current->legal, "help-legal");
        widget_add_activate_callback(current->legal, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->legal);
    }
    /* help-separator1 */ {
        current->separator1 = gtk_separator_menu_item_new();
        widget_add_destroy_callback(&current->separator1, "help-separator1");
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->separator1);
    }
    /* help-about */ {
        current->about = gtk_menu_item_new_with_label(_("About Xcpc"));
        widget_add_destroy_callback(&current->about, "help-about");
        widget_add_activate_callback(current->about, self);
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->about);
    }
}

static void build_menubar(XcpcApplication* self)
{
    XcpcLayoutRec*  parent  = &self->layout;
    XcpcMenuBarRec* current = &parent->menubar;

    /* menubar */ {
        current->widget = gtk_menu_bar_new();
        widget_add_destroy_callback(&current->widget, "menubar");
        gtk_box_pack_start(GTK_BOX(parent->vbox), current->widget, FALSE, TRUE, 0);
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
    XcpcLayoutRec*  parent  = &self->layout;
    XcpcToolBarRec* current = &parent->toolbar;

    /* toolbar */ {
        current->widget = gtk_toolbar_new();
        widget_add_destroy_callback(&current->widget, "toolbar");
        gtk_box_pack_start(GTK_BOX(parent->vbox), current->widget, FALSE, TRUE, 0);
    }
    /* tool-load-snapshot */ {
        current->load_snapshot = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->load_snapshot, "tool-load-snapshot");
        toolitem_add_clicked_callback(current->load_snapshot, self);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->load_snapshot), "document-open-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->load_snapshot, -1);
    }
    /* tool-save-snapshot */ {
        current->save_snapshot = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->save_snapshot, "tool-save-snapshot");
        toolitem_add_clicked_callback(current->save_snapshot, self);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->save_snapshot), "document-save-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->save_snapshot, -1);
    }
    /* tool-play-emulator */ {
        current->play_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->play_emulator, "tool-play-emulator");
        toolitem_add_clicked_callback(current->play_emulator, self);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->play_emulator), "media-playback-start-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->play_emulator, -1);
    }
    /* tool-pause-emulator */ {
        current->pause_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->pause_emulator, "tool-pause-emulator");
        toolitem_add_clicked_callback(current->pause_emulator, self);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->pause_emulator), "media-playback-pause-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->pause_emulator, -1);
    }
    /* tool-reset-emulator */ {
        current->reset_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->reset_emulator, "tool-reset-emulator");
        toolitem_add_clicked_callback(current->reset_emulator, self);
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->reset_emulator), "media-playlist-repeat-symbolic");
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->reset_emulator, -1);
    }
}

static void build_workwnd(XcpcApplication* self)
{
    XcpcLayoutRec*  parent  = &self->layout;
    XcpcWorkWndRec* current = &parent->workwnd;

    /* hbox */ {
        current->widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        widget_add_destroy_callback(&current->widget, "hbox");
        gtk_box_pack_start(GTK_BOX(parent->vbox), current->widget, TRUE, TRUE, 0);
    }
    /* emulator */ {
        current->emulator = gtk_drawing_area_new();
        widget_add_destroy_callback(&current->emulator, "emulator");
        gtk_box_pack_start(GTK_BOX(current->widget), current->emulator, TRUE, TRUE, 0);
    }
}

static void build_infobar(XcpcApplication* self)
{
    XcpcLayoutRec*  parent  = &self->layout;
    XcpcInfoBarRec* current = &parent->infobar;

    /* infobar */ {
        current->widget = gtk_button_new_with_label("infobar");
        widget_add_destroy_callback(&current->widget, "infobar");
        gtk_box_pack_start(GTK_BOX(parent->vbox), current->widget, FALSE, TRUE, 0);
    }
}

static void build_layout(GtkApplication* application, XcpcApplication* self)
{
    XcpcLayoutRec* current = &self->layout;

    /* window */ {
        current->window = gtk_application_window_new(application);
        widget_add_destroy_callback(&current->window, "window");
        gtk_window_set_title(GTK_WINDOW(current->window), _("Xcpc - Amstrad CPC emulator"));
        gtk_window_set_default_size(GTK_WINDOW(current->window), 640, 480);
    }
    /* vbox */ {
        current->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        widget_add_destroy_callback(&current->vbox, "vbox");
        gtk_container_add(GTK_CONTAINER(current->window), current->vbox);
    }
    /* menubar */ {
        (void) build_menubar(self);
    }
    /* toolbar */ {
        (void) build_toolbar(self);
    }
    /* workwnd */ {
        (void) build_workwnd(self);
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
        self->layout.application = gtk_application_new(application_id, G_APPLICATION_FLAGS_NONE);
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
