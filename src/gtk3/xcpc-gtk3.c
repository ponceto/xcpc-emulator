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
#ifdef HAVE_PORTAUDIO
#include <portaudio.h>
#endif
#include "xcpc-gtk3-priv.h"

#ifndef _
#define _(string) translate_string(string)
#endif

static const char help_text[] = "\
<u>Hotkeys:</u>\n\
\n\
<small><tt>\
    - F1                help\n\
    - F2                load snapshot\n\
    - F3                save snapshot\n\
    - F4                {not mapped}\n\
    - F5                reset emulator\n\
    - F6                insert disk into drive A\n\
    - F7                remove disk from drive A\n\
    - F8                insert disk into drive B\n\
    - F9                remove disk from drive B\n\
    - F10               {not mapped}\n\
    - F11               {not mapped}\n\
    - F12               {not mapped}\n\
</tt></small>\
\n\
<u>Keyboard emulation:</u>\n\
\n\
The left shift and control keys are forwarded to the simulation.\n\
You must use the right shift and control keys to compose characters.\n\
\n\
<u>Joystick emulation:</u>\n\
\n\
<small><tt>\
    - Home/End          enable/disable\n\
    - Arrows            up/down/left/right\n\
    - Left Ctrl         fire1\n\
    - Left Alt          fire2\n\
</tt></small>\
\n\
<u>Drag'n Drop:</u>\n\
\n\
You can use your file manager to drag'n drop a supported file directly to the emulator.\n\
\n\
The supported file extensions are: '.dsk', 'dsk.gz', 'dsk.bz2', '.sna'\n\
";

/*
 * ---------------------------------------------------------------------------
 * some useful constants
 * ---------------------------------------------------------------------------
 */

static const gchar app_id[]                 = "org.gtk.xcpc";
static const gchar sig_open[]               = "open";
static const gchar sig_startup[]            = "startup";
static const gchar sig_shutdown[]           = "shutdown";
static const gchar sig_activate[]           = "activate";
static const gchar sig_destroy[]            = "destroy";
static const gchar sig_clicked[]            = "clicked";
static const gchar sig_drag_data_received[] = "drag-data-received";
static const gchar sig_hotkey[]             = "hotkey";
static const gchar ico_load_snapshot[]      = "document-open-symbolic";
static const gchar ico_save_snapshot[]      = "document-save-symbolic";
static const gchar ico_play_emulator[]      = "media-playback-start-symbolic";
static const gchar ico_pause_emulator[]     = "media-playback-pause-symbolic";
static const gchar ico_reset_emulator[]     = "media-playlist-repeat-symbolic";

/*
 * ---------------------------------------------------------------------------
 * some useful functions
 * ---------------------------------------------------------------------------
 */

static const char* translate_string(const char* string)
{
    xcpc_log_debug("translate '%s'", string);

    return string;
}

static void show_widget(GtkWidget* widget)
{
    if(widget != NULL) {
        gtk_widget_show(widget);
    }
}

static void hide_widget(GtkWidget* widget)
{
    if(widget != NULL) {
        gtk_widget_hide(widget);
    }
}

/*
 * ---------------------------------------------------------------------------
 * Controls
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* set_machine(XcpcApplication* self)
{
    char buffer[256];

    /* build string */ {
        (void) snprintf ( buffer, sizeof(buffer)
                        , "%s %s %s, %s @ %s, %s"
                        , xcpc_company_name_to_string(xcpc_machine_company_name(self->machine))
                        , xcpc_machine_type_to_string(xcpc_machine_machine_type(self->machine))
                        , xcpc_memory_size_to_string(xcpc_machine_memory_size(self->machine))
                        , xcpc_monitor_type_to_string(xcpc_machine_monitor_type(self->machine))
                        , xcpc_refresh_rate_to_string(xcpc_machine_refresh_rate(self->machine))
                        , xcpc_keyboard_type_to_string(xcpc_machine_keyboard_type(self->machine)) );
    }
    if(self->layout.infobar.system != NULL) {
        gtk_label_set_text(GTK_LABEL(self->layout.infobar.system), buffer);
    }
    return self;
}

static XcpcApplication* set_title(XcpcApplication* self, const char* string)
{
    char buffer[256];

    /* inititialize buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "Xcpc - Amstrad CPC emulator - %s", string);
    }
    /* set title */ {
        if(self->layout.window != NULL) {
            gtk_window_set_title(GTK_WINDOW(self->layout.window), buffer);
        }
    }
    return self;
}

static XcpcApplication* set_status(XcpcApplication* self, const char* status)
{
    const char* format   = "<span foreground='grey90' background='darkgreen'><big> </big>%s<big> </big></span>";
    char*       string   = NULL;

    /* inititialize buffer */ {
        string = g_markup_printf_escaped(format, status);
    }
    /* set label */ {
        if(self->layout.infobar.status != NULL) {
            gtk_label_set_markup(GTK_LABEL(self->layout.infobar.status), string);
        }
    }
    /* free string */ {
        string = (g_free(string), NULL);
    }
    return set_title(set_machine(self), status);
}

static XcpcApplication* set_drive0(XcpcApplication* self)
{
    const char* format   = "<span foreground='yellow' background='darkblue'><big> </big>%s %s<big> </big></span>";
    const char* filename = NULL;
    char*       string   = NULL;

    /* fetch filename */ {
        filename = xcpc_machine_filename_drive0(self->machine);
        if((filename != NULL) && (*filename != '\0')) {
            const char* slash = strrchr(filename, '/');
            if(slash != NULL) {
                filename = (slash + 1);
            }
        }
        else {
            filename = _("{empty}");
        }
    }
    /* init string */ {
        string = g_markup_printf_escaped(format, _("A:"), filename);
    }
    /* set label */ {
        if(self->layout.infobar.drive0 != NULL) {
            gtk_label_set_markup(GTK_LABEL(self->layout.infobar.drive0), string);
        }
    }
    /* free string */ {
        string = (g_free(string), NULL);
    }
    return self;
}

static XcpcApplication* set_drive1(XcpcApplication* self)
{
    const char* format   = "<span foreground='yellow' background='darkblue'><big> </big>%s %s<big> </big></span>";
    const char* filename = NULL;
    char*       string   = NULL;

    /* fetch filename */ {
        filename = xcpc_machine_filename_drive1(self->machine);
        if((filename != NULL) && (*filename != '\0')) {
            const char* slash = strrchr(filename, '/');
            if(slash != NULL) {
                filename = (slash + 1);
            }
        }
        else {
            filename = _("{empty}");
        }
    }
    /* init string */ {
        string = g_markup_printf_escaped(format, _("B:"), filename);
    }
    /* set label */ {
        if(self->layout.infobar.drive1 != NULL) {
            gtk_label_set_markup(GTK_LABEL(self->layout.infobar.drive1), string);
        }
    }
    /* free string */ {
        string = (g_free(string), NULL);
    }
    return self;
}

static XcpcApplication* exit_emulator(XcpcApplication* self)
{
    if(self->layout.window != NULL) {
        gtk_widget_destroy(self->layout.window);
    }
    return self;
}

static XcpcApplication* play_emulator(XcpcApplication* self)
{
    /* show/hide controls */ {
        hide_widget(GTK_WIDGET(self->layout.menubar.ctrl.play_emulator));
        show_widget(GTK_WIDGET(self->layout.menubar.ctrl.pause_emulator));
        hide_widget(GTK_WIDGET(self->layout.toolbar.play_emulator));
        show_widget(GTK_WIDGET(self->layout.toolbar.pause_emulator));
    }
    if(self->layout.workwnd.emulator != NULL) {
        gtk_widget_set_sensitive(self->layout.workwnd.emulator, TRUE);
        gtk_widget_grab_focus(self->layout.workwnd.emulator);
    }
    /* set status */ {
        (void) set_status(self, _("Playing"));
        (void) set_drive0(self);
        (void) set_drive1(self);
    }
    return self;
}

static XcpcApplication* pause_emulator(XcpcApplication* self)
{
    /* show/hide controls */ {
        show_widget(GTK_WIDGET(self->layout.menubar.ctrl.play_emulator));
        hide_widget(GTK_WIDGET(self->layout.menubar.ctrl.pause_emulator));
        show_widget(GTK_WIDGET(self->layout.toolbar.play_emulator));
        hide_widget(GTK_WIDGET(self->layout.toolbar.pause_emulator));
    }
    if(self->layout.workwnd.emulator != NULL) {
        gtk_widget_set_sensitive(self->layout.workwnd.emulator, FALSE);
        gtk_widget_grab_focus(self->layout.workwnd.emulator);
    }
    /* set status */ {
        (void) set_status(self, _("Paused"));
        (void) set_drive0(self);
        (void) set_drive1(self);
    }
    return self;
}

static XcpcApplication* reset_emulator(XcpcApplication* self)
{
    if(self->layout.workwnd.emulator != NULL) {
        (void) xcpc_machine_reset(self->machine);
    }
    /* set status */ {
        (void) set_status(self, _("Reset"));
        (void) set_drive0(self);
        (void) set_drive1(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * snapshots
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* load_snapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_load_snapshot(self->machine, filename);
    }
    return self;
}

static XcpcApplication* save_snapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_save_snapshot(self->machine, filename);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * drive0
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* insert_disk_into_drive0(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive0(self->machine, filename);
    }
    return set_drive0(self);
}

static XcpcApplication* remove_disk_from_drive0(XcpcApplication* self)
{
    xcpc_machine_remove_drive0(self->machine);

    return set_drive0(self);
}

/*
 * ---------------------------------------------------------------------------
 * drive1
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* insert_disk_into_drive1(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive1(self->machine, filename);
    }
    return set_drive1(self);
}

static XcpcApplication* remove_disk_from_drive1(XcpcApplication* self)
{
    xcpc_machine_remove_drive1(self->machine);

    return set_drive1(self);
}

/*
 * ---------------------------------------------------------------------------
 * drive by default
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* insert_or_remove_disk(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        (void) insert_disk_into_drive0(self, filename);
    }
    else {
        (void) remove_disk_from_drive0(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * callbacks
 * ---------------------------------------------------------------------------
 */

static gboolean widget_destroy_callback(GtkWidget* widget, GtkWidget** reference)
{
    if((reference != NULL) && (*reference == widget)) {
        xcpc_log_debug("%s::%s()", gtk_widget_get_name(widget), sig_destroy);
        *reference = NULL;
    }
    return FALSE;
}

static void widget_activate_callback(GtkWidget* widget, XcpcApplication* self)
{
    xcpc_log_debug("%s::%s()", gtk_widget_get_name(widget), sig_activate);
}

static void widget_clicked_callback(GtkWidget* widget, XcpcApplication* self)
{
    xcpc_log_debug("%s::%s()", gtk_widget_get_name(widget), sig_clicked);
}

static void file_load_snapshot_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;
    gint       status = -1;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_file_chooser_dialog_new ( _("Load snapshot ...")
                                             , GTK_WINDOW(self->layout.window)
                                             , GTK_FILE_CHOOSER_ACTION_OPEN
                                             , _("_Cancel"), GTK_RESPONSE_CANCEL
                                             , _("_Load")  , GTK_RESPONSE_ACCEPT
                                             , NULL);
    }
    /* run dialog */ {
        status = gtk_dialog_run(GTK_DIALOG(dialog));
        if(status == GTK_RESPONSE_ACCEPT) {
            gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            (void) load_snapshot(self, filename);
            filename = (g_free(filename), NULL);
        }
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void file_save_snapshot_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;
    gint       status = -1;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_file_chooser_dialog_new ( _("Save snapshot ...")
                                             , GTK_WINDOW(self->layout.window)
                                             , GTK_FILE_CHOOSER_ACTION_SAVE
                                             , _("_Cancel"), GTK_RESPONSE_CANCEL
                                             , _("_Save")  , GTK_RESPONSE_ACCEPT
                                             , NULL);
    }
    /* run dialog */ {
        status = gtk_dialog_run(GTK_DIALOG(dialog));
        if(status == GTK_RESPONSE_ACCEPT) {
            gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            (void) save_snapshot(self, filename);
            filename = (g_free(filename), NULL);
        }
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void file_exit_callback(GtkWidget* widget, XcpcApplication* self)
{
    exit_emulator(self);
}

static void ctrl_play_emulator_callback(GtkWidget* widget, XcpcApplication* self)
{
    play_emulator(self);
}

static void ctrl_pause_emulator_callback(GtkWidget* widget, XcpcApplication* self)
{
    pause_emulator(self);
}

static void ctrl_reset_emulator_callback(GtkWidget* widget, XcpcApplication* self)
{
    pause_emulator(self);
    reset_emulator(self);
    play_emulator(self);
}

static void drv0_insert_disk_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;
    gint       status = -1;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_file_chooser_dialog_new ( _("Insert disk into drive A ...")
                                             , GTK_WINDOW(self->layout.window)
                                             , GTK_FILE_CHOOSER_ACTION_OPEN
                                             , _("_Cancel"), GTK_RESPONSE_CANCEL
                                             , _("_Open")  , GTK_RESPONSE_ACCEPT
                                             , NULL);
    }
    /* run dialog */ {
        status = gtk_dialog_run(GTK_DIALOG(dialog));
        if(status == GTK_RESPONSE_ACCEPT) {
            gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            (void) insert_disk_into_drive0(self, filename);
            filename = (g_free(filename), NULL);
        }
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void drv0_remove_disk_callback(GtkWidget* widget, XcpcApplication* self)
{
    (void) remove_disk_from_drive0(self);
}

static void drv1_insert_disk_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;
    gint       status = -1;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_file_chooser_dialog_new ( _("Insert disk into drive B ...")
                                             , GTK_WINDOW(self->layout.window)
                                             , GTK_FILE_CHOOSER_ACTION_OPEN
                                             , _("_Cancel"), GTK_RESPONSE_CANCEL
                                             , _("_Open")  , GTK_RESPONSE_ACCEPT
                                             , NULL );
    }
    /* run dialog */ {
        status = gtk_dialog_run(GTK_DIALOG(dialog));
        if(status == GTK_RESPONSE_ACCEPT) {
            gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            (void) insert_disk_into_drive1(self, filename);
            filename = (g_free(filename), NULL);
        }
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void drv1_remove_disk_callback(GtkWidget* widget, XcpcApplication* self)
{
    (void) remove_disk_from_drive1(self);
}

static void help_help_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_message_dialog_new_with_markup ( GTK_WINDOW(self->layout.window)
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_OTHER
                                                    , GTK_BUTTONS_CLOSE
                                                    , NULL );
        gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), help_text);
        gtk_window_set_title(GTK_WINDOW(dialog), _("Xcpc - Amstrad CPC emulator"));
    }
    /* run dialog */ {
        (void) gtk_dialog_run(GTK_DIALOG(dialog));
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void help_legal_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_about_dialog_new();
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), self->layout.logo);
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), _("Xcpc"));
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), _("Legal informations"));
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), _("Copyright (c) 2001-2021 - Olivier Poncet"));
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), xcpc_legal_text());
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), _("https://www.xcpc-emulator.net/"));
        gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), xcpc_about_text());
    }
    /* run dialog */ {
        (void) gtk_dialog_run(GTK_DIALOG(dialog));
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void help_about_callback(GtkWidget* widget, XcpcApplication* self)
{
    GtkWidget* dialog = NULL;

    /* pause emulator */ {
        pause_emulator(self);
    }
    /* create dialog */ {
        dialog = gtk_about_dialog_new();
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), self->layout.logo);
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), _("Xcpc"));
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), PACKAGE_STRING);
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), _("Copyright (c) 2001-2021 - Olivier Poncet"));
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), _("Xcpc is an Amstrad CPC emulator for Linux, BSD and Unix"));
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), _("https://www.xcpc-emulator.net/"));
        gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), xcpc_about_text());
    }
    /* run dialog */ {
        (void) gtk_dialog_run(GTK_DIALOG(dialog));
    }
    /* destroy dialog */ {
        dialog = (gtk_widget_destroy(dialog), NULL);
    }
    /* play emulator */ {
        play_emulator(self);
    }
}

static void emulator_hotkey_callback(GtkWidget* widget, KeySym* keysym, XcpcApplication* self)
{
    if(keysym != NULL) {
        switch(*keysym) {
            case XK_Pause:
                if(gtk_widget_is_sensitive(self->layout.workwnd.emulator) == FALSE) {
                    play_emulator(self);
                }
                else {
                    pause_emulator(self);
                }
                break;
            case XK_F1:
                help_help_callback(widget, self);
                break;
            case XK_F2:
                file_load_snapshot_callback(widget, self);
                break;
            case XK_F3:
                file_save_snapshot_callback(widget, self);
                break;
            case XK_F4:
                break;
            case XK_F5:
                ctrl_reset_emulator_callback(widget, self);
                break;
            case XK_F6:
                drv0_insert_disk_callback(widget, self);
                break;
            case XK_F7:
                drv0_remove_disk_callback(widget, self);
                break;
            case XK_F8:
                drv1_insert_disk_callback(widget, self);
                break;
            case XK_F9:
                drv1_remove_disk_callback(widget, self);
                break;
            case XK_F10:
                break;
            case XK_F11:
                break;
            case XK_F12:
                break;
            default:
                break;
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * drag'n drop callbacks
 * ---------------------------------------------------------------------------
 */

static int check_extension(const char* filename, const char* extension)
{
    const int filename_length  = strlen(filename);
    const int extension_length = strlen(extension);

    if(filename_length >= extension_length) {
        if(strcasecmp(&filename[filename_length - extension_length], extension) == 0) {
            return 1;
        }
    }
    return 0;
}

static void drag_data_received ( GtkWidget*        widget
                               , GdkDragContext*   context
                               , int               x
                               , int               y
                               , GtkSelectionData* data
                               , guint             info
                               , guint             time
                               , XcpcApplication*  self )
{
    const char* prefix_str = "file://";
    const int   prefix_len = strlen(prefix_str);
    gchar**     uri_list   = gtk_selection_data_get_uris(data);
    gchar*      filename   = NULL;

    if(uri_list != NULL) {
        filename = uri_list[0];
    }
    if(filename != NULL) {
        if(strncmp(filename, prefix_str, prefix_len) == 0) {
            filename += prefix_len;
        }
        if(check_extension(filename, ".sna") != 0) {
            (void) load_snapshot(self, filename);
            (void) play_emulator(self);
        }
        else if(check_extension(filename, ".dsk") != 0) {
            (void) insert_or_remove_disk(self, filename);
            (void) play_emulator(self);
        }
        else if(check_extension(filename, ".dsk.gz") != 0) {
            (void) insert_or_remove_disk(self, filename);
            (void) play_emulator(self);
        }
        else if(check_extension(filename, ".dsk.bz2") != 0) {
            (void) insert_or_remove_disk(self, filename);
            (void) play_emulator(self);
        }
    }
    if(uri_list != NULL) {
        uri_list = (g_strfreev(uri_list), NULL);
    }
}

/*
 * ---------------------------------------------------------------------------
 * some useful wrappers
 * ---------------------------------------------------------------------------
 */

static void widget_add_destroy_callback(GtkWidget** reference, const gchar* name)
{
    /* check reference */ {
        if((reference == NULL) || (*reference == NULL)) {
            return;
        }
    }
    /* set widget name */ {
        if(name != NULL) {
            gtk_widget_set_name(GTK_WIDGET(*reference), name);
        }
    }
    /* connect signal */ {
        (void) g_signal_connect(G_OBJECT(*reference), sig_destroy, G_CALLBACK(&widget_destroy_callback), reference);
    }
}

static void toolitem_add_destroy_callback(GtkToolItem** reference, const gchar* name)
{
    /* check reference */ {
        if((reference == NULL) || (*reference == NULL)) {
            return;
        }
    }
    /* set widget name */ {
        if(name != NULL) {
            gtk_widget_set_name(GTK_WIDGET(*reference), name);
        }
    }
    /* connect signal */ {
        (void) g_signal_connect(G_OBJECT(*reference), sig_destroy, G_CALLBACK(&widget_destroy_callback), reference);
    }
}

static void widget_add_activate_callback(GtkWidget* widget, XcpcApplication* self, GCallback callback)
{
    /* check and adjust callback */ {
        if(callback == NULL) {
            callback = G_CALLBACK(&widget_activate_callback);
        }
    }
    /* connect signal */ {
        (void) g_signal_connect(G_OBJECT(widget), sig_activate, callback, self);
    }
}

static void toolitem_add_clicked_callback(GtkToolItem* widget, XcpcApplication* self, GCallback callback)
{
    /* check and adjust callback */ {
        if(callback == NULL) {
            callback = G_CALLBACK(&widget_clicked_callback);
        }
    }
    /* connect signal */ {
        (void) g_signal_connect(G_OBJECT(widget), sig_clicked, callback, self);
    }
}

static void emulator_add_hotkey_callback(GtkWidget* widget, XcpcApplication* self, GCallback callback)
{
    /* check and adjust callback */ {
        if(callback == NULL) {
            callback = G_CALLBACK(&emulator_hotkey_callback);
        }
    }
    /* connect signal */ {
        (void) g_signal_connect(G_OBJECT(widget), sig_hotkey, callback, self);
    }
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
        widget_add_activate_callback(current->load_snapshot, self, G_CALLBACK(&file_load_snapshot_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->load_snapshot));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F2, 0);
        }
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->load_snapshot);
    }
    /* file-save-snapshot */ {
        current->save_snapshot = gtk_menu_item_new_with_label(_("Save snapshot..."));
        widget_add_destroy_callback(&current->save_snapshot, "file-save-snapshot");
        widget_add_activate_callback(current->save_snapshot, self, G_CALLBACK(&file_save_snapshot_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->save_snapshot));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F3, 0);
        }
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
        widget_add_activate_callback(current->exit, self, G_CALLBACK(&file_exit_callback));
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
        widget_add_activate_callback(current->play_emulator, self, G_CALLBACK(&ctrl_play_emulator_callback));
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->play_emulator);
    }
    /* ctrl-pause-emulator */ {
        current->pause_emulator = gtk_menu_item_new_with_label(_("Pause"));
        widget_add_destroy_callback(&current->pause_emulator, "ctrl-pause");
        widget_add_activate_callback(current->pause_emulator, self, G_CALLBACK(&ctrl_pause_emulator_callback));
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->pause_emulator);
    }
    /* ctrl-reset-emulator */ {
        current->reset_emulator = gtk_menu_item_new_with_label(_("Reset"));
        widget_add_destroy_callback(&current->reset_emulator, "ctrl-reset");
        widget_add_activate_callback(current->reset_emulator, self, G_CALLBACK(&ctrl_reset_emulator_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->reset_emulator));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F5, 0);
        }
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
        widget_add_activate_callback(current->drive0_insert, self, G_CALLBACK(&drv0_insert_disk_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->drive0_insert));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F6, 0);
        }
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive0_insert);
    }
    /* drv0-drive0-remove */ {
        current->drive0_remove = gtk_menu_item_new_with_label(_("Remove disk"));
        widget_add_destroy_callback(&current->drive0_remove, "drv0-remove");
        widget_add_activate_callback(current->drive0_remove, self, G_CALLBACK(&drv0_remove_disk_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->drive0_remove));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F7, 0);
        }
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
        widget_add_activate_callback(current->drive1_insert, self, G_CALLBACK(&drv1_insert_disk_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->drive1_insert));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F8, 0);
        }
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->drive1_insert);
    }
    /* drv1-drive1-remove */ {
        current->drive1_remove = gtk_menu_item_new_with_label(_("Remove disk"));
        widget_add_destroy_callback(&current->drive1_remove, "drv1-remove");
        widget_add_activate_callback(current->drive1_remove, self, G_CALLBACK(&drv1_remove_disk_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->drive1_remove));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F9, 0);
        }
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
        widget_add_activate_callback(current->help, self, G_CALLBACK(&help_help_callback));
        /* accelerator */ {
            GtkWidget* child = gtk_bin_get_child(GTK_BIN(current->help));
            gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F1, 0);
        }
        gtk_menu_shell_append(GTK_MENU_SHELL(current->menu), current->help);
    }
    /* help-legal */ {
        current->legal = gtk_menu_item_new_with_label(_("Legal informations"));
        widget_add_destroy_callback(&current->legal, "help-legal");
        widget_add_activate_callback(current->legal, self, G_CALLBACK(&help_legal_callback));
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
        widget_add_activate_callback(current->about, self, G_CALLBACK(&help_about_callback));
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
        toolitem_add_clicked_callback(current->load_snapshot, self, G_CALLBACK(&file_load_snapshot_callback));
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->load_snapshot), ico_load_snapshot);
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->load_snapshot, -1);
    }
    /* tool-save-snapshot */ {
        current->save_snapshot = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->save_snapshot, "tool-save-snapshot");
        toolitem_add_clicked_callback(current->save_snapshot, self, G_CALLBACK(&file_save_snapshot_callback));
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->save_snapshot), ico_save_snapshot);
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->save_snapshot, -1);
    }
    /* tool-play-emulator */ {
        current->play_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->play_emulator, "tool-play-emulator");
        toolitem_add_clicked_callback(current->play_emulator, self, G_CALLBACK(&ctrl_play_emulator_callback));
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->play_emulator), ico_play_emulator);
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->play_emulator, -1);
    }
    /* tool-pause-emulator */ {
        current->pause_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->pause_emulator, "tool-pause-emulator");
        toolitem_add_clicked_callback(current->pause_emulator, self, G_CALLBACK(&ctrl_pause_emulator_callback));
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->pause_emulator), ico_pause_emulator);
        gtk_toolbar_insert(GTK_TOOLBAR(current->widget), current->pause_emulator, -1);
    }
    /* tool-reset-emulator */ {
        current->reset_emulator = gtk_tool_button_new(NULL, NULL);
        toolitem_add_destroy_callback(&current->reset_emulator, "tool-reset-emulator");
        toolitem_add_clicked_callback(current->reset_emulator, self, G_CALLBACK(&ctrl_reset_emulator_callback));
        gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(current->reset_emulator), ico_reset_emulator);
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
        static const GtkTargetEntry target_entries[] = {
            { "text/uri-list", 0, 1 },
        };
        const GemMachine machine = {
            GEM_EMULATOR_DATA(self->machine),
            GEM_EMULATOR_FUNC(&xcpc_machine_create_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_destroy_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_realize_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_resize_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_expose_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_input_func),
            GEM_EMULATOR_FUNC(&xcpc_machine_clock_func),
        };
        current->emulator = gem_emulator_new();
        widget_add_destroy_callback(&current->emulator, "emulator");
        emulator_add_hotkey_callback(current->emulator, self, NULL);
        gtk_widget_set_sensitive(current->emulator, FALSE);
        gem_emulator_set_machine(current->emulator, &machine);
        gem_emulator_set_joystick(current->emulator, 0, xcpc_get_joystick0());
        gem_emulator_set_joystick(current->emulator, 1, xcpc_get_joystick1());
        gtk_box_pack_start(GTK_BOX(current->widget), current->emulator, TRUE, TRUE, 0);
        gtk_drag_dest_set(current->emulator, GTK_DEST_DEFAULT_ALL, target_entries, 1, (GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK));
        (void) g_signal_connect(G_OBJECT(current->emulator), sig_drag_data_received, G_CALLBACK(&drag_data_received), self);
    }
}

static void build_infobar(XcpcApplication* self)
{
    XcpcLayoutRec*  parent  = &self->layout;
    XcpcInfoBarRec* current = &parent->infobar;

    /* infobar */ {
        current->widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        widget_add_destroy_callback(&current->widget, "infobar");
        gtk_box_pack_start(GTK_BOX(parent->vbox), current->widget, FALSE, TRUE, 4);
    }
    /* info-status */ {
        current->status = gtk_label_new(_("Status"));
        widget_add_destroy_callback(&current->status, "info-status");
        gtk_box_pack_start(GTK_BOX(current->widget), current->status, FALSE, TRUE, 2);
    }
    /* info-drive0 */ {
        current->drive0 = gtk_label_new(_("Drive A"));
        gtk_label_set_ellipsize(GTK_LABEL(current->drive0), PANGO_ELLIPSIZE_MIDDLE);
        widget_add_destroy_callback(&current->drive0, "info-drive0");
        gtk_box_pack_start(GTK_BOX(current->widget), current->drive0, FALSE, TRUE, 2);
    }
    /* info-drive1 */ {
        current->drive1 = gtk_label_new(_("Drive B"));
        gtk_label_set_ellipsize(GTK_LABEL(current->drive1), PANGO_ELLIPSIZE_MIDDLE);
        widget_add_destroy_callback(&current->drive1, "info-drive1");
        gtk_box_pack_start(GTK_BOX(current->widget), current->drive1, FALSE, TRUE, 2);
    }
    /* info-system */ {
        current->system = gtk_label_new(_("System"));
        widget_add_destroy_callback(&current->system, "info-system");
        gtk_label_set_ellipsize(GTK_LABEL(current->system), PANGO_ELLIPSIZE_MIDDLE);
        gtk_box_pack_start(GTK_BOX(current->widget), current->system, TRUE, TRUE, 2);
    }
}

static void build_layout(XcpcApplication* self)
{
    XcpcLayoutRec* current = &self->layout;

    /* logo */ {
        gchar* filename = g_build_filename(xcpc_get_resdir(), "bitmaps", "xcpc-icon.png", NULL);
        current->logo = gdk_pixbuf_new_from_file(filename, NULL);
        filename = (g_free(filename), NULL);
    }
    /* window */ {
        current->window = gtk_application_window_new(self->layout.application);
        widget_add_destroy_callback(&current->window, "window");
        gtk_window_set_title(GTK_WINDOW(current->window), _("Xcpc - Amstrad CPC emulator"));
        gtk_window_set_icon(GTK_WINDOW(current->window), current->logo);
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
        gtk_widget_grab_focus(self->layout.workwnd.emulator);
    }
    /* play */ {
        (void) play_emulator(self);
    }
}

static void application_open_callback(GApplication* application, gpointer files, int num_files, char* hint, XcpcApplication* self)
{
    /* log */ {
        xcpc_log_debug("%s::%s()", "application", sig_open);
    }
}

static void application_startup_callback(GApplication* application, XcpcApplication* self)
{
    /* log */ {
        xcpc_log_debug("%s::%s()", "application", sig_startup);
    }
    /* build layout */ {
        build_layout(self);
    }
}

static void application_shutdown_callback(GApplication* application, XcpcApplication* self)
{
    /* log */ {
        xcpc_log_debug("%s::%s()", "application", sig_shutdown);
    }
    /* destroy logo */ {
        if(self->layout.logo != NULL) {
            self->layout.logo = (g_object_unref(G_OBJECT(self->layout.logo)), NULL);
        }
    }
    /* destroy window */ {
        if(self->layout.window != NULL) {
            gtk_widget_destroy(self->layout.window);
        }
    }
}

static void application_activate_callback(GApplication* application, XcpcApplication* self)
{
    /* log */ {
        xcpc_log_debug("%s::%s()", "application", sig_activate);
    }
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* xcpc_application_new(int* argc, char*** argv)
{
    XcpcApplication* self = g_new0(XcpcApplication, 1);

    if(self != NULL) {
        self->argc = argc;
        self->argv = argv;
    }
    return self;
}

XcpcApplication* xcpc_application_delete(XcpcApplication* self)
{
    /* destroy application */ {
        if(self->layout.application != NULL) {
            self->layout.application = (g_object_unref(self->layout.application), NULL);
        }
    }
    /* destroy machine */ {
        if(self->machine != NULL) {
            self->machine = xcpc_machine_delete(self->machine);
        }
    }
    /* destroy options */ {
        if(self->options != NULL) {
            self->options = xcpc_options_delete(self->options);
        }
    }
    /* delete self */ {
        self = (g_free(self), NULL);
    }
    return self;
}

XcpcApplication* xcpc_application_run(XcpcApplication* self)
{
    /* intialize the options */ {
        self->options = xcpc_options_new(self->argc, self->argv);
        (void) xcpc_options_parse(self->options);
        if(xcpc_options_quit(self->options) != 0) {
            return self;
        }
    }
    /* intialize the machine */ {
        const XcpcMachineIface machine_iface = {
            self, /* user_data */
            NULL, /* reserved0 */
            NULL, /* reserved1 */
            NULL, /* reserved2 */
            NULL, /* reserved3 */
            NULL, /* reserved4 */
            NULL, /* reserved5 */
            NULL, /* reserved6 */
            NULL, /* reserved7 */
        };
        self->machine = xcpc_machine_new(&machine_iface, self->options);
    }
#ifdef HAVE_PORTAUDIO
    /* initialize portaudio */ {
        const PaError pa_error = Pa_Initialize();
        if(pa_error != paNoError) {
            xcpc_log_error("unable to initialize PortAudio (%s)", Pa_GetErrorText(pa_error));
        }
    }
#endif
    /* create application */ {
        self->layout.application = gtk_application_new(app_id, G_APPLICATION_FLAGS_NONE);
        (void) g_signal_connect(G_OBJECT(self->layout.application), sig_open, G_CALLBACK(application_open_callback), self);
        (void) g_signal_connect(G_OBJECT(self->layout.application), sig_startup, G_CALLBACK(application_startup_callback), self);
        (void) g_signal_connect(G_OBJECT(self->layout.application), sig_shutdown, G_CALLBACK(application_shutdown_callback), self);
        (void) g_signal_connect(G_OBJECT(self->layout.application), sig_activate, G_CALLBACK(application_activate_callback), self);
    }
    /* loop */ {
        (void) g_application_run(G_APPLICATION(self->layout.application), *self->argc, *self->argv);
    }
#ifdef HAVE_PORTAUDIO
    /* finalize portaudio */ {
        const PaError pa_error = Pa_Terminate();
        if(pa_error != paNoError) {
            xcpc_log_error("unable to terminate PortAudio (%s)", Pa_GetErrorText(pa_error));
        }
    }
#endif
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * xcpc_main
 * ---------------------------------------------------------------------------
 */

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplication* self = xcpc_application_new(argc, argv);

    if(self != NULL) {
        (void) xcpc_application_run(self);
        self = xcpc_application_delete(self);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
