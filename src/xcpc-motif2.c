/*
 * xcpc-motif2.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xcpc-motif2-priv.h"

#ifndef _
#define _(string) (string)
#endif

static const char help_text[] = "\
Hotkeys:\n\
\n\
    - F1                Help\n\
    - F2                Load snapshot\n\
    - F3                Save snapshot\n\
    - F4                Insert disk into drive A\n\
    - F5                Remove disk from drive A\n\
    - F6                Insert disk into drive B\n\
    - F7                Remove disk from drive B\n\
    - F8                {not mapped}\n\
    - F9                {not mapped}\n\
    - F10               {not mapped}\n\
    - F11               {not mapped}\n\
    - F12               {not mapped}\n\
\n\
Keyboard emulation:\n\
\n\
The left shift and control keys are forwarded to the simulation.\n\
You have use to the right shift and control keys to compose characters.\n\
\n\
Joystick emulation:\n\
\n\
    - End               enable/disable\n\
    - Arrows            up/down/left/right\n\
    - Left Ctrl         fire1\n\
    - Left Alt          fire2\n\
\n\
Drag'n Drop:\n\
\n\
You can use your file manager to drag'n drop a supported file directly to the emulator.\n\
\n\
The supported file extensions are: '.dsk', 'dsk.gz', 'dsk.bz2', '.sna'\n\
\n\
";

/*
 * ---------------------------------------------------------------------------
 * options
 * ---------------------------------------------------------------------------
 */

static XrmOptionDescRec options[] = {
    { "-quiet", ".xcpcQuietFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-trace", ".xcpcTraceFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-debug", ".xcpcDebugFlag", XrmoptionNoArg, (XPointer) "true" },
};

/*
 * ---------------------------------------------------------------------------
 * fallback resources
 * ---------------------------------------------------------------------------
 */

static String fallback_resources[] = {
    "Xcpc*title: Xcpc - Amstrad CPC emulator",
    "Xcpc*shadowThickness: 1",
    "Xcpc*main-window.shadowThickness: 0",
    "Xcpc*main-window.menubar.shadowThickness: 0",
    "Xcpc*main-window.menubar*pixmapTextPadding: 8",
    "Xcpc*main-window.toolbar.shadowThickness: 0",
    "Xcpc*main-window.toolbar.marginWidth: 4",
    "Xcpc*main-window.toolbar.marginHeight: 4",
    "Xcpc*main-window.infobar.marginWidth: 4",
    "Xcpc*main-window.infobar.marginHeight: 4",
    "Xcpc*main-window.infobar.spacing: 4",
    "Xcpc*main-window.infobar*info-status.borderColor: black",
    "Xcpc*main-window.infobar*info-status.background: darkgreen",
    "Xcpc*main-window.infobar*info-status.foreground: grey90",
    "Xcpc*main-window.infobar*info-drive0.borderColor: black",
    "Xcpc*main-window.infobar*info-drive0.background: darkblue",
    "Xcpc*main-window.infobar*info-drive0.foreground: yellow",
    "Xcpc*main-window.infobar*info-drive1.borderColor: black",
    "Xcpc*main-window.infobar*info-drive1.background: darkblue",
    "Xcpc*main-window.infobar*info-drive1.foreground: yellow",
    NULL
};

/*
 * ---------------------------------------------------------------------------
 * application resources
 * ---------------------------------------------------------------------------
 */

static XtResource application_resources[] = {
    /* xcpcQuietFlag */ {
        "xcpcQuietFlag", "XcpcQuietFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, quiet_flag),
        XmRImmediate, (XtPointer) False
    },
    /* xcpcTraceFlag */ {
        "xcpcTraceFlag", "XcpcTraceFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, trace_flag),
        XmRImmediate, (XtPointer) False
    },
    /* xcpcDebugFlag */ {
        "xcpcDebugFlag", "XcpcDebugFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, debug_flag),
        XmRImmediate, (XtPointer) False
    },
};

/*
 * ---------------------------------------------------------------------------
 * Toolkit utilities
 * ---------------------------------------------------------------------------
 */

static void ShowWidget(Widget widget)
{
    if(widget != NULL) {
        XtManageChild(widget);
    }
}

static void HideWidget(Widget widget)
{
    if(widget != NULL) {
        XtUnmanageChild(widget);
    }
}

static Widget FindShell(Widget widget)
{
    while((widget != NULL) && (XtIsShell(widget) == False)) {
        widget = XtParent(widget);
    }
    return widget;
}

static Widget FindTopLevelShell(Widget widget)
{
    while((widget != NULL) && (XtIsTopLevelShell(widget) == False)) {
        widget = XtParent(widget);
    }
    return widget;
}

static void GetPixmaps(Widget widget, XcpcPixmapsRec* pixmaps)
{
    struct {
        Pixmap* pixmap;
        char*   filename;
    } list[] = {
        { &pixmaps->null_icon  , "empty.png"           },
        { &pixmaps->xcpc_icon  , "xcpc-icon.png"       },
        { &pixmaps->xcpc_mask  , "xcpc-mask.png"       },
        { &pixmaps->file_load  , "folder-open.png"     },
        { &pixmaps->file_save  , "save.png"            },
        { &pixmaps->file_exit  , "power-off.png"       },
        { &pixmaps->ctrl_play  , "play.png"            },
        { &pixmaps->ctrl_pause , "pause.png"           },
        { &pixmaps->ctrl_reset , "sync.png"            },
        { &pixmaps->disk_insert, "folder-open.png"     },
        { &pixmaps->disk_remove, "eject.png"           },
        { &pixmaps->help_legal , "info-circle.png"     },
        { &pixmaps->help_about , "question-circle.png" },
        { &pixmaps->help_help  , "question.png"        },
    };

    /* clear */ {
        (void) memset(pixmaps, 0, sizeof(XcpcPixmapsRec));
    }
    /* get colors */ {
        Arg      arglist[4];
        Cardinal argcount = 0;
        XtSetArg(arglist[argcount], XmNforeground, &pixmaps->foreground); ++argcount;
        XtSetArg(arglist[argcount], XmNbackground, &pixmaps->background); ++argcount;
        XtGetValues(widget, arglist, argcount);
    }
    /* get pixmaps */ {
        Cardinal index      = 0;
        Cardinal count      = XtNumber(list);
        Screen*  screen     = XtScreen(widget);
        Pixel    foreground = pixmaps->foreground;
        Pixel    background = pixmaps->background;
        for(index = 0; index < count; ++index) {
            *list[index].pixmap = XmGetPixmap(screen, list[index].filename, foreground, background);
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * Controls
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* SetMachine(XcpcApplication* self)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

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
    if(self->infobar.system != NULL) {
        XmString string = XmStringCreateLocalized(buffer);
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetValues(self->infobar.system, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* SetTitle(XcpcApplication* self, const char* string)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

    /* inititialize buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "Xcpc - Amstrad CPC emulator - %s", string);
    }
    if(self->layout.toplevel != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNtitle     , buffer                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNiconPixmap, self->pixmaps.xcpc_icon); ++argcount;
        XtSetArg(arglist[argcount], XmNiconMask  , self->pixmaps.xcpc_mask); ++argcount;
        XtSetValues(self->layout.toplevel, arglist, argcount);
    }
    return self;
}

static XcpcApplication* SetStatus(XcpcApplication* self, const char* string)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

    /* inititialize buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s", string);
    }
    if(self->infobar.status != NULL) {
        XmString string = XmStringCreateLocalized(buffer);
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetValues(self->infobar.status, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    return SetTitle(SetMachine(self), string);
}

static XcpcApplication* SetDrive0(XcpcApplication* self)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    const char* filename = NULL;
    char        buffer[256];

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
    /* init buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s %s", _("A:"), filename);
    }
    if(self->infobar.drive0 != NULL) {
        XmString string = XmStringCreateLocalized(buffer);
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetValues(self->infobar.drive0, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* SetDrive1(XcpcApplication* self)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    const char* filename = NULL;
    char        buffer[256];

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
    /* init buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s %s", _("B:"), filename);
    }
    if(self->infobar.drive1 != NULL) {
        XmString string = XmStringCreateLocalized(buffer);
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetValues(self->infobar.drive1, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* Exit(XcpcApplication* self)
{
    if(self->appcontext != NULL) {
        XtAppSetExitFlag(self->appcontext);
    }
    return self;
}

static XcpcApplication* Play(XcpcApplication* self)
{
    /* show/hide controls */ {
        HideWidget(self->menubar.ctrl.play_emulator);
        ShowWidget(self->menubar.ctrl.pause_emulator);
        HideWidget(self->toolbar.play_emulator);
        ShowWidget(self->toolbar.pause_emulator);
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, True);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* set status */ {
        (void) SetStatus(self, _("Playing"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

static XcpcApplication* Pause(XcpcApplication* self)
{
    /* show/hide controls */ {
        ShowWidget(self->menubar.ctrl.play_emulator);
        HideWidget(self->menubar.ctrl.pause_emulator);
        ShowWidget(self->toolbar.play_emulator);
        HideWidget(self->toolbar.pause_emulator);
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, False);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* set status */ {
        (void) SetStatus(self, _("Paused"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

static XcpcApplication* Reset(XcpcApplication* self)
{
    if(self->layout.emulator != NULL) {
        (void) xcpc_machine_reset(self->machine);
    }
    /* set status */ {
        (void) SetStatus(self, _("Reset"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Snapshot
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* LoadSnapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_load_snapshot(self->machine, filename);
    }
    return self;
}

static XcpcApplication* SaveSnapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_save_snapshot(self->machine, filename);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Drive0
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* InsertDiskIntoDrive0(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive0(self->machine, filename);
    }
    return SetDrive0(self);
}

static XcpcApplication* RemoveDiskFromDrive0(XcpcApplication* self)
{
    xcpc_machine_remove_drive0(self->machine);

    return SetDrive0(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive1
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* InsertDiskIntoDrive1(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive1(self->machine, filename);
    }
    return SetDrive1(self);
}

static XcpcApplication* RemoveDiskFromDrive1(XcpcApplication* self)
{
    xcpc_machine_remove_drive1(self->machine);

    return SetDrive1(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive by default
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* InsertOrRemoveDisk(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        (void) InsertDiskIntoDrive0(self, filename);
    }
    else {
        (void) RemoveDiskFromDrive0(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Generic callbacks
 * ---------------------------------------------------------------------------
 */

static void DestroyCallback(Widget widget, Widget* reference, XtPointer pointer)
{
    if(XtIsApplicationShell(widget) != False) {
        XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
    }
    if((widget != NULL) && (reference != NULL) && (widget == *reference)) {
        *reference = NULL;
    }
}

static void DismissCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Widget shell = FindShell(widget);

    if(info != NULL) {
        XtDestroyWidget(shell);
    }
    else {
        (void) Play(self);
    }
}

/*
 * ---------------------------------------------------------------------------
 * Drag & Drop callbacks
 * ---------------------------------------------------------------------------
 */

static int ConvertHexChar(const char* string)
{
    int  value     = 0;
    char character = *string++;

    /* check character */ {
        if(character != '%') {
            return -1;
        }
    }
    /* convert 1st digit */ {
        switch(character = *string++) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                value = ((value << 4) | (0x0 + (character - '0')));
                break;
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                value = ((value << 4) | (0xa + (character - 'a')));
                break;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                value = ((value << 4) | (0xa + (character - 'A')));
                break;
            default:
                return -1;
        }
    }
    /* convert 2nd digit */ {
        switch(character = *string++) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                value = ((value << 4) | (0x0 + (character - '0')));
                break;
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                value = ((value << 4) | (0xa + (character - 'a')));
                break;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                value = ((value << 4) | (0xa + (character - 'A')));
                break;
            default:
                return -1;
        }
    }
    return value;
}

static void DeserializeURI(char* buffer, size_t buflen, const char* uri)
{
    const char nul = '\0';
    const char cr  = '\r';
    const char lf  = '\n';
    char character = *uri;

    /* deserialize */ {
        if(character != nul) {
            do {
                if(buflen <= 1) {
                    break;
                }
                if((character == cr) || (character == lf)) {
                    break;
                }
                else if(character == '%') {
                    const int hexval = ConvertHexChar(uri);
                    if(hexval != -1) {
                        *buffer = ((char)(hexval & 0xff));
                        uri += 2;
                    }
                    else {
                        *buffer = character;
                    }
                }
                else {
                    *buffer = character;
                }
                ++buffer;
                --buflen;
            } while((character = *++uri) != nul);
        }
    }
    /* terminate buffer */ {
        *buffer = nul;
    }
}

static int CheckExtension(const char* filename, const char* extension)
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

static void DropUriCallback(Widget widget, XcpcApplication* self, const char* uri)
{
    char filename[PATH_MAX + 1];

    if((uri != NULL) && (strncmp(uri, "file://", 7) == 0)) {
        DeserializeURI(filename, sizeof(filename), &uri[7]);
        if(CheckExtension(filename, ".sna") != 0) {
            (void) LoadSnapshot(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk.gz") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk.bz2") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * File callbacks
 * ---------------------------------------------------------------------------
 */

static void ExitCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) Exit(self);
}

/*
 * ---------------------------------------------------------------------------
 * Snapshot load callbacks
 * ---------------------------------------------------------------------------
 */

static void LoadSnapshotOkCallback(Widget widget, XcpcApplication* self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != False) {
        (void) LoadSnapshot(self, filename);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XtPointer) info);
}

static void LoadSnapshotCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* snapshot load dialog */ {
        Widget dialog = NULL;
        XmString title = XmStringCreateLocalized(_("Load a snapshot..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle   , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle   , title                          ); ++argcount;
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-load-snapshot-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback     , (XtCallbackProc) &LoadSnapshotOkCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNcancelCallback , (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback  , (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtManageChild(dialog);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

/*
 * ---------------------------------------------------------------------------
 * Snapshot save callbacks
 * ---------------------------------------------------------------------------
 */

static void SaveSnapshotOkCallback(Widget widget, XcpcApplication* self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != False) {
        (void) SaveSnapshot(self, filename);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XtPointer) info);
}

static void SaveSnapshotCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* snapshot save dialog */ {
        Widget dialog = NULL;
        XmString title = XmStringCreateLocalized(_("Save a snapshot..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle   , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle   , title                          ); ++argcount;
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-save-snapshot-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback     , (XtCallbackProc) &SaveSnapshotOkCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNcancelCallback , (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback  , (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtManageChild(dialog);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

/*
 * ---------------------------------------------------------------------------
 * Drive0 callbacks
 * ---------------------------------------------------------------------------
 */

static void InsertDrive0OkCallback(Widget widget, XcpcApplication* self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != False) {
        (void) InsertDiskIntoDrive0(self, filename);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XtPointer) info);
}

static void InsertDrive0Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drive0 insert dialog */ {
        Widget dialog = NULL;
        XmString title = XmStringCreateLocalized(_("Insert disk into drive A..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle   , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle   , title                          ); ++argcount;
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-drive0-insert-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) &InsertDrive0OkCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtManageChild(dialog);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void RemoveDrive0Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) RemoveDiskFromDrive0(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive1 callbacks
 * ---------------------------------------------------------------------------
 */

static void InsertDrive1OkCallback(Widget widget, XcpcApplication* self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != False) {
        (void) InsertDiskIntoDrive1(self, filename);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XtPointer) info);
}

static void InsertDrive1Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drive1 insert dialog */ {
        Widget dialog = NULL;
        XmString title = XmStringCreateLocalized(_("Insert disk into drive B..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle   , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle   , title                          ); ++argcount;
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-drive1-insert-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) &InsertDrive1OkCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback       , (XtPointer) self);
        XtManageChild(dialog);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void RemoveDrive1Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) RemoveDiskFromDrive1(self);
}

/*
 * ---------------------------------------------------------------------------
 * Controls callbacks
 * ---------------------------------------------------------------------------
 */

static void PlayCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) Play(self);
}

static void PauseCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) Pause(self);
}

static void ResetCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) Reset(self);
    (void) Play(self);
}

/*
 * ---------------------------------------------------------------------------
 * Help callbacks
 * ---------------------------------------------------------------------------
 */

static void LegalCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    XmString title = XmStringCreateLocalized(_("Legal Info ..."));
    XmString message = XmStringCreateLocalized(_(((char*)(xcpc_legal_text()))));

    /* legal dialog */ {
        Widget dialog = NULL;
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse  , XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle     , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle     , title                          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageAlignment, XmALIGNMENT_CENTER             ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageString   , message                        ); ++argcount;
        dialog = XmCreateMessageDialog(FindTopLevelShell(widget), "xcpc-legal-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback     , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback  , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtManageChild(dialog);
    }
    /* free strings */ {
        message = (XmStringFree(message), NULL);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void AboutCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    XmString title = XmStringCreateLocalized(_("About Xcpc ..."));
    XmString message = XmStringCreateLocalized(_(((char*)(xcpc_about_text()))));

    /* about dialog */ {
        Widget dialog = NULL;
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse  , XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle     , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle     , title                          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageAlignment, XmALIGNMENT_CENTER             ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageString   , message                        ); ++argcount;
        dialog = XmCreateMessageDialog(FindTopLevelShell(widget), "xcpc-about-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback     , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback  , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtManageChild(dialog);
    }
    /* free strings */ {
        message = (XmStringFree(message), NULL);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void HelpCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    XmString title = XmStringCreateLocalized(_("Help ..."));
    XmString message = XmStringCreateLocalized(_(((char*)(help_text))));

    /* about dialog */ {
        Widget dialog = NULL;
        argcount = 0;
        XtSetArg(arglist[argcount], XmNdeleteResponse  , XmDESTROY                      ); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogStyle     , XmDIALOG_FULL_APPLICATION_MODAL); ++argcount;
        XtSetArg(arglist[argcount], XmNdialogTitle     , title                          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageAlignment, XmALIGNMENT_BEGINNING          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageString   , message                        ); ++argcount;
        dialog = XmCreateMessageDialog(FindTopLevelShell(widget), "xcpc-help-dialog", arglist, argcount);
        XtAddCallback(dialog, XmNokCallback     , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNunmapCallback  , (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtManageChild(dialog);
    }
    /* free strings */ {
        message = (XmStringFree(message), NULL);
        title = (XmStringFree(title), NULL);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

/*
 * ---------------------------------------------------------------------------
 * Hotkey callbacks
 * ---------------------------------------------------------------------------
 */

static void HotkeyCallback(Widget widget, XcpcApplication* self, KeySym* keysym)
{
    if(keysym != NULL) {
        switch(*keysym) {
            case XK_F1:
                HelpCallback(widget, self, NULL);
                break;
            case XK_F2:
                LoadSnapshotCallback(widget, self, NULL);
                break;
            case XK_F3:
                SaveSnapshotCallback(widget, self, NULL);
                break;
            case XK_F4:
                InsertDrive0Callback(widget, self, NULL);
                break;
            case XK_F5:
                RemoveDrive0Callback(widget, self, NULL);
                break;
            case XK_F6:
                InsertDrive1Callback(widget, self, NULL);
                break;
            case XK_F7:
                RemoveDrive1Callback(widget, self, NULL);
                break;
            case XK_F8:
                break;
            case XK_F9:
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
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* BuildFileMenu(XcpcApplication* self)
{
    XcpcFileMenuRec* menu = &self->menubar.file;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* file-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "file-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* file-load-snapshot */ {
        XmString string = XmStringCreateLocalized(_("Load snapshot..."));
        XmString accelerator = XmStringCreateLocalized(_("F2"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_load); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->load_snapshot = XmCreatePushButtonGadget(menu->pulldown, "file-load-snapshot", arglist, argcount);
        XtAddCallback(menu->load_snapshot, XmNactivateCallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->load_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->load_snapshot);
        XtManageChild(menu->load_snapshot);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* file-save-snapshot */ {
        XmString string = XmStringCreateLocalized(_("Save snapshot as..."));
        XmString accelerator = XmStringCreateLocalized(_("F3"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->save_snapshot = XmCreatePushButtonGadget(menu->pulldown, "file-save-snapshot", arglist, argcount);
        XtAddCallback(menu->save_snapshot, XmNactivateCallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->save_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->save_snapshot);
        XtManageChild(menu->save_snapshot);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* file-separator1 */ {
        argcount = 0;
        menu->separator1 = XmCreateSeparatorGadget(menu->pulldown, "file-separator1", arglist, argcount);
        XtAddCallback(menu->separator1, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->separator1);
        XtManageChild(menu->separator1);
    }
    /* file-exit */ {
        XmString string = XmStringCreateLocalized(_("Exit"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_exit); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->exit = XmCreatePushButtonGadget(menu->pulldown, "file-exit", arglist, argcount);
        XtAddCallback(menu->exit, XmNactivateCallback, (XtCallbackProc) &ExitCallback, (XtPointer) self);
        XtAddCallback(menu->exit, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->exit);
        XtManageChild(menu->exit);
        string = (XmStringFree(string), NULL);
    }
    /* file-menu */ {
        XmString string = XmStringCreateLocalized(_("File"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, menu->pulldown); ++argcount;
        menu->menu = XmCreateCascadeButtonGadget(self->menubar.widget, "file-menu", arglist, argcount);
        XtAddCallback(menu->menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* BuildCtrlMenu(XcpcApplication* self)
{
    XcpcCtrlMenuRec* menu = &self->menubar.ctrl;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* ctrl-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "ctrl-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* ctrl-play-emu */ {
        XmString string = XmStringCreateLocalized(_("Play"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_play); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->play_emulator = XmCreatePushButtonGadget(menu->pulldown, "ctrl-play-emu", arglist, argcount);
        XtAddCallback(menu->play_emulator, XmNactivateCallback, (XtCallbackProc) &PlayCallback, (XtPointer) self);
        XtAddCallback(menu->play_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->play_emulator);
        XtManageChild(menu->play_emulator);
        string = (XmStringFree(string), NULL);
    }
    /* ctrl-pause-emu */ {
        XmString string = XmStringCreateLocalized(_("Pause"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_pause); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->pause_emulator = XmCreatePushButtonGadget(menu->pulldown, "ctrl-pause-emu", arglist, argcount);
        XtAddCallback(menu->pause_emulator, XmNactivateCallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(menu->pause_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pause_emulator);
        XtManageChild(menu->pause_emulator);
        string = (XmStringFree(string), NULL);
    }
    /* ctrl-reset-emu */ {
        XmString string = XmStringCreateLocalized(_("Reset"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_reset); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->reset_emulator = XmCreatePushButtonGadget(menu->pulldown, "ctrl-reset-emu", arglist, argcount);
        XtAddCallback(menu->reset_emulator, XmNactivateCallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(menu->reset_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->reset_emulator);
        XtManageChild(menu->reset_emulator);
        string = (XmStringFree(string), NULL);
    }
    /* ctrl-menu */ {
        XmString string = XmStringCreateLocalized(_("Controls"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, menu->pulldown); ++argcount;
        menu->menu = XmCreateCascadeButtonGadget(self->menubar.widget, "ctrl-menu", arglist, argcount);
        XtAddCallback(menu->menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* BuildDrv0Menu(XcpcApplication* self)
{
    XcpcDrv0MenuRec* menu = &self->menubar.drv0;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv0-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "drv0-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* drv0-drive0-insert */ {
        XmString string = XmStringCreateLocalized(_("Insert disk..."));
        XmString accelerator = XmStringCreateLocalized(_("F4"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_insert); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->drive0_insert = XmCreatePushButtonGadget(menu->pulldown, "drv0-drive0-insert", arglist, argcount);
        XtAddCallback(menu->drive0_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_insert);
        XtManageChild(menu->drive0_insert);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* drv0-drive0-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        XmString accelerator = XmStringCreateLocalized(_("F5"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_remove); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->drive0_remove = XmCreatePushButtonGadget(menu->pulldown, "drv0-drive0-remove", arglist, argcount);
        XtAddCallback(menu->drive0_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_remove);
        XtManageChild(menu->drive0_remove);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* drv0-menu */ {
        XmString string = XmStringCreateLocalized(_("Drive A"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, menu->pulldown); ++argcount;
        menu->menu = XmCreateCascadeButtonGadget(self->menubar.widget, "drv0-menu", arglist, argcount);
        XtAddCallback(menu->menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* BuildDrv1Menu(XcpcApplication* self)
{
    XcpcDrv1MenuRec* menu = &self->menubar.drv1;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv1-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "drv1-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* drv1-drive1-insert */ {
        XmString string = XmStringCreateLocalized(_("Insert disk..."));
        XmString accelerator = XmStringCreateLocalized(_("F6"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_insert); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->drive1_insert = XmCreatePushButtonGadget(menu->pulldown, "drv1-drive1-insert", arglist, argcount);
        XtAddCallback(menu->drive1_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_insert);
        XtManageChild(menu->drive1_insert);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* drv1-drive1-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        XmString accelerator = XmStringCreateLocalized(_("F7"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_remove); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->drive1_remove = XmCreatePushButtonGadget(menu->pulldown, "drv1-drive1-remove", arglist, argcount);
        XtAddCallback(menu->drive1_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_remove);
        XtManageChild(menu->drive1_remove);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* drv1-menu */ {
        XmString string = XmStringCreateLocalized(_("Drive B"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, menu->pulldown); ++argcount;
        menu->menu = XmCreateCascadeButtonGadget(self->menubar.widget, "drv1-menu", arglist, argcount);
        XtAddCallback(menu->menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication* BuildHelpMenu(XcpcApplication* self)
{
    XcpcHelpMenuRec* menu = &self->menubar.help;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* help-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "help-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* help-help */ {
        XmString string = XmStringCreateLocalized(_("Help"));
        XmString accelerator = XmStringCreateLocalized(_("F1"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.help_help); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        XtSetArg(arglist[argcount], XmNacceleratorText, accelerator); ++argcount;
        menu->help = XmCreatePushButtonGadget(menu->pulldown, "help-help", arglist, argcount);
        XtAddCallback(menu->help, XmNactivateCallback, (XtCallbackProc) &HelpCallback, (XtPointer) self);
        XtAddCallback(menu->help, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->help);
        XtManageChild(menu->help);
        accelerator = (XmStringFree(accelerator), NULL);
        string = (XmStringFree(string), NULL);
    }
    /* help-legal */ {
        XmString string = XmStringCreateLocalized(_("Legal Info"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.help_legal); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->legal = XmCreatePushButtonGadget(menu->pulldown, "help-legal", arglist, argcount);
        XtAddCallback(menu->legal, XmNactivateCallback, (XtCallbackProc) &LegalCallback, (XtPointer) self);
        XtAddCallback(menu->legal, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->legal);
        XtManageChild(menu->legal);
        string = (XmStringFree(string), NULL);
    }
    /* help-separator1 */ {
        argcount = 0;
        menu->separator1 = XmCreateSeparatorGadget(menu->pulldown, "help-separator1", arglist, argcount);
        XtAddCallback(menu->separator1, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->separator1);
        XtManageChild(menu->separator1);
    }
    /* help-about */ {
        XmString string = XmStringCreateLocalized(_("About Xcpc"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.help_about); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->about = XmCreatePushButtonGadget(menu->pulldown, "help-about", arglist, argcount);
        XtAddCallback(menu->about, XmNactivateCallback, (XtCallbackProc) &AboutCallback, (XtPointer) self);
        XtAddCallback(menu->about, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->about);
        XtManageChild(menu->about);
        string = (XmStringFree(string), NULL);
    }
    /* help-menu */ {
        XmString string = XmStringCreateLocalized(_("Help"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, menu->pulldown); ++argcount;
        menu->menu = XmCreateCascadeButtonGadget(self->menubar.widget, "help-menu", arglist, argcount);
        XtAddCallback(menu->menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
        string = (XmStringFree(string), NULL);
    }
    /* set the menubar help-widget */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNmenuHelpWidget, menu->menu); ++argcount;
        XtSetValues(self->menubar.widget, arglist, argcount);
    }
    return self;
}

static XcpcApplication* BuildMenuBar(XcpcApplication* self)
{
    XcpcMenuBarRec* menubar = &self->menubar;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* menubar */ {
        argcount = 0;
        menubar->widget = XmCreateMenuBar(self->layout.window, "menubar", arglist, argcount);
        XtAddCallback(menubar->widget, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menubar->widget);
        XtManageChild(menubar->widget);
    }
    /* build all menus */ {
        (void) BuildFileMenu(self);
        (void) BuildCtrlMenu(self);
        (void) BuildDrv0Menu(self);
        (void) BuildDrv1Menu(self);
        (void) BuildHelpMenu(self);
    }
    return self;
}

static XcpcApplication* BuildToolBar(XcpcApplication* self)
{
    XcpcToolBarRec* toolbar = &self->toolbar;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* toolbar */ {
        argcount = 0;
        toolbar->widget = XmCreateMenuBar(self->layout.window, "toolbar", arglist, argcount);
        XtAddCallback(toolbar->widget, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->widget);
        XtManageChild(toolbar->widget);
    }
    /* tool-load-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP               ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_load); ++argcount;
        toolbar->load_snapshot = XmCreateCascadeButtonGadget(toolbar->widget, "tool-load-snapshot", arglist, argcount);
        XtAddCallback(toolbar->load_snapshot, XmNactivateCallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(toolbar->load_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->load_snapshot);
        XtManageChild(toolbar->load_snapshot);
    }
    /* tool-save-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP               ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_save); ++argcount;
        toolbar->save_snapshot = XmCreateCascadeButtonGadget(toolbar->widget, "tool-save-snapshot", arglist, argcount);
        XtAddCallback(toolbar->save_snapshot, XmNactivateCallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(toolbar->save_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->save_snapshot);
        XtManageChild(toolbar->save_snapshot);
    }
    /* tool-play-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP               ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_play); ++argcount;
        toolbar->play_emulator = XmCreateCascadeButtonGadget(toolbar->widget, "tool-play-emu", arglist, argcount);
        XtAddCallback(toolbar->play_emulator, XmNactivateCallback, (XtCallbackProc) &PlayCallback, (XtPointer) self);
        XtAddCallback(toolbar->play_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->play_emulator);
        XtManageChild(toolbar->play_emulator);
    }
    /* tool-pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP                ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_pause); ++argcount;
        toolbar->pause_emulator = XmCreateCascadeButtonGadget(toolbar->widget, "tool-pause-emu", arglist, argcount);
        XtAddCallback(toolbar->pause_emulator, XmNactivateCallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(toolbar->pause_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->pause_emulator);
        XtManageChild(toolbar->pause_emulator);
    }
    /* tool-reset-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP                ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_reset); ++argcount;
        toolbar->reset_emulator = XmCreateCascadeButtonGadget(toolbar->widget, "tool-reset-emu", arglist, argcount);
        XtAddCallback(toolbar->reset_emulator, XmNactivateCallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(toolbar->reset_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->reset_emulator);
        XtManageChild(toolbar->reset_emulator);
    }
    return self;
}

static XcpcApplication* BuildInfoBar(XcpcApplication* self)
{
    XcpcInfoBarRec* infobar = &self->infobar;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* infobar */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNorientation, XmHORIZONTAL); ++argcount;
        infobar->widget = XmCreateRowColumn(self->layout.window, "infobar", arglist, argcount);
        XtAddCallback(infobar->widget, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->widget);
        XtManageChild(infobar->widget);
    }
    /* info-status */ {
        argcount = 0;
        infobar->status = XmCreateLabel(infobar->widget, "info-status", arglist, argcount);
        XtAddCallback(infobar->status, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->status);
        XtManageChild(infobar->status);
    }
    /* info-drive0 */ {
        argcount = 0;
        infobar->drive0 = XmCreateLabel(infobar->widget, "info-drive0", arglist, argcount);
        XtAddCallback(infobar->drive0, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->drive0);
        XtManageChild(infobar->drive0);
    }
    /* info-drive1 */ {
        argcount = 0;
        infobar->drive1 = XmCreateLabel(infobar->widget, "info-drive1", arglist, argcount);
        XtAddCallback(infobar->drive1, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->drive1);
        XtManageChild(infobar->drive1);
    }
    /* info-system */ {
        argcount = 0;
        infobar->system = XmCreateLabel(infobar->widget, "info-system", arglist, argcount);
        XtAddCallback(infobar->system, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->system);
        XtManageChild(infobar->system);
    }
    /* initialize */ {
        (void) SetStatus(self, "-");
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

static XcpcApplication* BuildEmulator(XcpcApplication* self)
{
    XcpcLayoutRec* layout = &self->layout;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNborderWidth   , 0                         ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuContext    , self->machine             ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuCreateProc , &xcpc_machine_create_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuDestroyProc, &xcpc_machine_destroy_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuRealizeProc, &xcpc_machine_realize_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuResizeProc , &xcpc_machine_resize_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuExposeProc , &xcpc_machine_expose_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuTimerProc  , &xcpc_machine_timer_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuInputProc  , &xcpc_machine_input_proc  ); ++argcount;
        layout->emulator = XemCreateEmulator(self->layout.window, "emulator", arglist, argcount);
        XtAddCallback(layout->emulator, XtNhotkeyCallback , (XtCallbackProc) &HotkeyCallback , (XtPointer) self);
        XtAddCallback(layout->emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &layout->emulator);
        XtManageChild(layout->emulator);
    }
    return self;
}

static XcpcApplication* BuildLayout(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* main-window */ {
        argcount = 0;
        self->layout.window = XmCreateMainWindow(self->layout.toplevel, "main-window", arglist, argcount);
        XtAddCallback(self->layout.window, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.window);
        XtManageChild(self->layout.window);
    }
    /* get pixmaps */ {
        GetPixmaps(self->layout.window, &self->pixmaps);
    }
    /* menubar */ {
        (void) BuildMenuBar(self);
    }
    /* toolbar */ {
        (void) BuildToolBar(self);
    }
    /* emulator */ {
        (void) BuildEmulator(self);
    }
    /* infobar */ {
        (void) BuildInfoBar(self);
    }
    /* set main-window areas */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNmenuBar      , self->menubar.widget ); ++argcount;
        XtSetArg(arglist[argcount], XmNcommandWindow, self->toolbar.widget ); ++argcount;
        XtSetArg(arglist[argcount], XmNworkWindow   , self->layout.emulator); ++argcount;
        XtSetArg(arglist[argcount], XmNmessageWindow, self->infobar.widget ); ++argcount;
        XtSetValues(self->layout.window, arglist, argcount);
    }
    /* get pixmaps */ {
        GetPixmaps(self->menubar.widget, &self->pixmaps);
    }
    return self;
}

static XcpcApplication* Construct(XcpcApplication* self, int* argc, char*** argv)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcApplication));
    }
    /* intialize the machine */ {
        self->machine = xcpc_machine_new();
    }
    /* parse the command-line */ {
        (void) xcpc_machine_parse(self->machine, argc, argv);
    }
    /* set language proc */ {
        (void) XtSetLanguageProc(NULL, NULL, NULL);
    }
    /* create application context and toplevel shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNmappedWhenManaged, True        ); ++argcount;
        XtSetArg(arglist[argcount], XmNallowShellResize , True        ); ++argcount;
        XtSetArg(arglist[argcount], XmNdeleteResponse   , XmDO_NOTHING); ++argcount;
        self->layout.toplevel = XtOpenApplication(&self->appcontext, "Xcpc", options, XtNumber(options), argc, *argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
        XtAddCallback(self->layout.toplevel, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.toplevel);
        XtAddCallback(self->layout.toplevel, XtNdropURICallback, (XtCallbackProc) &DropUriCallback, (XtPointer) self);
    }
    /* get application resources */ {
        argcount = 0;
        XtGetApplicationResources(self->layout.toplevel, (XtPointer) &self->resources, application_resources, XtNumber(application_resources), arglist, argcount);
    }
    /* get application name and class */ {
        XtGetApplicationNameAndClass(XtDisplay(self->layout.toplevel), &self->resources.app_name, &self->resources.app_class);
    }
    /* build user interface */ {
        (void) BuildLayout(self);
    }
    /* play */ {
        (void) Play(self);
    }
    return self;
}

static XcpcApplication* Destruct(XcpcApplication* self)
{
    /* destroy toplevel shell */ {
        if(self->layout.toplevel != NULL) {
            self->layout.toplevel = (XtDestroyWidget(self->layout.toplevel), NULL);
        }
    }
    /* destroy application context */ {
        if(self->appcontext != NULL) {
            self->appcontext = (XtDestroyApplicationContext(self->appcontext), NULL);
        }
    }
    /* finalize the emulator */ {
        self->machine = xcpc_machine_delete(self->machine);
    }
    return self;
}

static XcpcApplication* MainLoop(XcpcApplication* self)
{
    if(XtAppGetExitFlag(self->appcontext) == False) {
        /* realize toplevel shell */ {
            if((self->layout.toplevel != NULL)) {
                XtRealizeWidget(self->layout.toplevel);
            }
        }
        /* set initial keyboard focus */ {
            XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
        }
        /* run application loop  */ {
            if((self->appcontext != NULL)) {
                XtAppMainLoop(self->appcontext);
            }
        }
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* XcpcApplicationNew(int* argc, char*** argv)
{
    return Construct(xcpc_new(XcpcApplication), argc, argv);
}

XcpcApplication* XcpcApplicationDelete(XcpcApplication* self)
{
    return xcpc_delete(XcpcApplication, Destruct(self));
}

XcpcApplication* XcpcApplicationLoop(XcpcApplication* self)
{
    return MainLoop(self);
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication
 * ---------------------------------------------------------------------------
 */

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplication* self = XcpcApplicationNew(argc, argv);

    /* main */ {
        (void) XcpcApplicationLoop(self);
    }
    /* delete */ {
        self = XcpcApplicationDelete(self);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
