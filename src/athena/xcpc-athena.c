/*
 * xcpc-athena.c - Copyright (c) 2001-2023 - Olivier Poncet
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
#include "xcpc-athena-priv.h"

#ifndef _
#define _(string) (string)
#endif

static const char help_text[] = "\
Hotkeys:\n\
\n\
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
\n\
Keyboard emulation:\n\
\n\
The left shift and control keys are forwarded to the simulation.\n\
You must use the right shift and control keys to compose characters.\n\
\n\
Joystick emulation:\n\
\n\
    - Home/End          enable/disable\n\
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
    ".bitmapFilePath: " XCPC_RESDIR "/bitmaps",
    ".pixmapFilePath: " XCPC_RESDIR "/pixmaps",
    "Xcpc*title: Xcpc - Amstrad CPC emulator",
    "Xcpc*main-window.menubar.borderWidth: 0",
    "Xcpc*main-window.menubar.?.borderWidth: 0",
    "Xcpc*main-window.toolbar.borderWidth: 0",
    "Xcpc*main-window.toolbar.?.borderWidth: 0",
    "Xcpc*main-window.infobar.borderWidth: 0",
    "Xcpc*main-window.infobar.?.borderWidth: 0",
    "Xcpc*main-window.emulator.borderWidth: 0",
    "Xcpc*main-window.infobar*info-status.borderColor: black",
    "Xcpc*main-window.infobar*info-status.background: darkgreen",
    "Xcpc*main-window.infobar*info-status.foreground: grey90",
    "Xcpc*main-window.infobar*info-drive0.borderColor: black",
    "Xcpc*main-window.infobar*info-drive0.background: darkblue",
    "Xcpc*main-window.infobar*info-drive0.foreground: yellow",
    "Xcpc*main-window.infobar*info-drive1.borderColor: black",
    "Xcpc*main-window.infobar*info-drive1.background: darkblue",
    "Xcpc*main-window.infobar*info-drive1.foreground: yellow",
    "Xcpc*main-window.infobar*info-drive1.foreground: yellow",
    "Xcpc*main-window.menubar*SmeBSB.leftMargin: 24",
    NULL
};

/*
 * ---------------------------------------------------------------------------
 * application resources
 * ---------------------------------------------------------------------------
 */

static XtResource application_resources[] = {
    /* xcpcQuietFlag */ {
        "xcpcQuietFlag", "XcpcQuietFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, quiet_flag),
        XtRImmediate, (XtPointer) False
    },
    /* xcpcTraceFlag */ {
        "xcpcTraceFlag", "XcpcTraceFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, trace_flag),
        XtRImmediate, (XtPointer) False
    },
    /* xcpcDebugFlag */ {
        "xcpcDebugFlag", "XcpcDebugFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, debug_flag),
        XtRImmediate, (XtPointer) False
    },
};

/*
 * ---------------------------------------------------------------------------
 * icons resources
 * ---------------------------------------------------------------------------
 */

static XtResource icons_resources[] = {
    /* foreground */ {
        "foreground", "Foreground", XtRPixel,
        sizeof(Pixel), XtOffsetOf(XcpcBitmapsRec, foreground),
        XtRString, (XtPointer) XtDefaultForeground
    },
    /* background */ {
        "background", "Background", XtRPixel,
        sizeof(Pixel), XtOffsetOf(XcpcBitmapsRec, background),
        XtRString, (XtPointer) XtDefaultBackground
    },
    /* xcpcNullBitmap */ {
        "xcpcNullBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, null_icon),
        XtRString, (XtPointer) "empty.xbm"
    },
    /* xcpcFileLoadBitmap */ {
        "xcpcFileLoadBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, file_load),
        XtRString, (XtPointer) "folder-open.xbm"
    },
    /* xcpcFileSaveBitmap */ {
        "xcpcFileSaveBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, file_save),
        XtRString, (XtPointer) "save.xbm"
    },
    /* xcpcFileExitBitmap */ {
        "xcpcFileExitBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, file_exit),
        XtRString, (XtPointer) "power-off.xbm"
    },
    /* xcpcCtrlPlayBitmap */ {
        "xcpcCtrlPlayBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, ctrl_play),
        XtRString, (XtPointer) "play.xbm"
    },
    /* xcpcCtrlPauseBitmap */ {
        "xcpcCtrlPauseBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, ctrl_pause),
        XtRString, (XtPointer) "pause.xbm"
    },
    /* xcpcCtrlResetBitmap */ {
        "xcpcCtrlResetBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, ctrl_reset),
        XtRString, (XtPointer) "sync.xbm"
    },
    /* xcpcDiskInsertBitmap */ {
        "xcpcDiskInsertBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, disk_insert),
        XtRString, (XtPointer) "folder-open.xbm"
    },
    /* xcpcDiskRemoveBitmap */ {
        "xcpcDiskRemoveBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, disk_remove),
        XtRString, (XtPointer) "eject.xbm"
    },
    /* xcpcHelpLegalBitmap */ {
        "xcpcHelpLegalBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, help_legal),
        XtRString, (XtPointer) "info-circle.xbm"
    },
    /* xcpcHelpAboutBitmap */ {
        "xcpcHelpAboutBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, help_about),
        XtRString, (XtPointer) "question-circle.xbm"
    },
    /* xcpcHelpHelpBitmap */ {
        "xcpcHelpHelpBitmap", "XcpcBitmap", XtRBitmap,
        sizeof(Pixmap), XtOffsetOf(XcpcBitmapsRec, help_help),
        XtRString, (XtPointer) "question.xbm"
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

static void GetBitmaps(Widget widget, XcpcBitmapsRec* bitmaps)
{
    /* clear */ {
        (void) memset(bitmaps, 0, sizeof(XcpcBitmapsRec));
    }
    /* load bitmaps */ {
        XtGetApplicationResources(widget, (XtPointer) bitmaps, icons_resources, XtNumber(icons_resources), NULL, 0);
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
        XtUnmanageChild(self->infobar.system);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.system, arglist, argcount);
        XtManageChild(self->infobar.system);
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
        XtSetArg(arglist[argcount], XtNtitle, buffer); ++argcount;
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
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.status, arglist, argcount);
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
        filename = xcpc_machine_drive0_filename(self->machine);
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
        XtUnmanageChild(self->infobar.drive0);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.drive0, arglist, argcount);
        XtManageChild(self->infobar.drive0);
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
        filename = xcpc_machine_drive1_filename(self->machine);
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
        XtUnmanageChild(self->infobar.drive1);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.drive1, arglist, argcount);
        XtManageChild(self->infobar.drive1);
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
        (void) xcpc_machine_snapshot_load(self->machine, filename);
    }
    return self;
}

static XcpcApplication* SaveSnapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        (void) xcpc_machine_snapshot_save(self->machine, filename);
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
        (void) xcpc_machine_drive0_insert(self->machine, filename);
    }
    return SetDrive0(self);
}

static XcpcApplication* RemoveDiskFromDrive0(XcpcApplication* self)
{
    (void) xcpc_machine_drive0_remove(self->machine);

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
        (void) xcpc_machine_drive1_insert(self->machine, filename);
    }
    return SetDrive1(self);
}

static XcpcApplication* RemoveDiskFromDrive1(XcpcApplication* self)
{
    (void) xcpc_machine_drive1_remove(self->machine);

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

    /* popdown shell */ {
        XtPopdown(shell);
    }
    /* destroy shell */ {
        XtDestroyWidget(shell);
    }
    /* play */ {
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
    char filename[PATH_MAX];

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

static void LoadSnapshotOkCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    char* filename = XawDialogGetValueString(XtParent(widget));

    if((filename != NULL) && (*filename != '\0')) {
        (void) LoadSnapshot(self, filename);
    }
    DismissCallback(widget, self, info);
}

static void LoadSnapshotCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-load-snapshot-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-load-snapshot-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
    }
    /* xcpc-load-snapshot-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Load snapshot ...")); ++argcount;
        XtSetArg(arglist[argcount], XtNvalue, ""); ++argcount;
        dialog = XtCreateWidget("xcpc-load-snapshot-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-load-snapshot-accept */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Accept")); ++argcount;
        accept = XtCreateManagedWidget("xcpc-load-snapshot-accept", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(accept, XtNcallback, (XtCallbackProc) &LoadSnapshotOkCallback, (XtPointer) self);
        XtManageChild(accept);
    }
    /* xcpc-load-snapshot-cancel */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); ++argcount;
        cancel = XtCreateManagedWidget("xcpc-load-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(cancel, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(cancel);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
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

static void SaveSnapshotOkCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    char* filename = XawDialogGetValueString(XtParent(widget));

    if((filename != NULL) && (*filename != '\0')) {
        (void) SaveSnapshot(self, filename);
    }
    DismissCallback(widget, self, info);
}

static void SaveSnapshotCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-save-snapshot-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-save-snapshot-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
    }
    /* xcpc-save-snapshot-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Save snapshot ...")); ++argcount;
        XtSetArg(arglist[argcount], XtNvalue, ""); ++argcount;
        dialog = XtCreateWidget("xcpc-save-snapshot-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-save-snapshot-accept */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Accept")); ++argcount;
        accept = XtCreateManagedWidget("xcpc-save-snapshot-accept", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(accept, XtNcallback, (XtCallbackProc) &SaveSnapshotOkCallback, (XtPointer) self);
        XtManageChild(accept);
    }
    /* xcpc-save-snapshot-cancel */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); ++argcount;
        cancel = XtCreateManagedWidget("xcpc-save-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(cancel, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(cancel);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
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

static void InsertDrive0OkCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    char* filename = XawDialogGetValueString(XtParent(widget));

    if((filename != NULL) && (*filename != '\0')) {
        (void) InsertDiskIntoDrive0(self, filename);
    }
    DismissCallback(widget, self, info);
}

static void InsertDrive0Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-drive0-insert-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-drive0-insert-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
    }
    /* xcpc-drive0-insert-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive A ...")); ++argcount;
        XtSetArg(arglist[argcount], XtNvalue, ""); ++argcount;
        dialog = XtCreateWidget("xcpc-drive0-insert-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-drive0-insert-accept */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Accept")); ++argcount;
        accept = XtCreateManagedWidget("xcpc-drive0-insert-accept", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(accept, XtNcallback, (XtCallbackProc) &InsertDrive0OkCallback, (XtPointer) self);
        XtManageChild(accept);
    }
    /* xcpc-drive0-insert-cancel */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); ++argcount;
        cancel = XtCreateManagedWidget("xcpc-drive0-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(cancel, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(cancel);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
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

static void InsertDrive1OkCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    char* filename = XawDialogGetValueString(XtParent(widget));

    if((filename != NULL) && (*filename != '\0')) {
        (void) InsertDiskIntoDrive1(self, filename);
    }
    DismissCallback(widget, self, info);
}

static void InsertDrive1Callback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-drive1-insert-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-drive1-insert-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
    }
    /* xcpc-drive1-insert-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive B ...")); ++argcount;
        XtSetArg(arglist[argcount], XtNvalue, ""); ++argcount;
        dialog = XtCreateWidget("xcpc-drive1-insert-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-drive1-insert-accept */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Accept")); ++argcount;
        accept = XtCreateManagedWidget("xcpc-drive1-insert-accept", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(accept, XtNcallback, (XtCallbackProc) &InsertDrive1OkCallback, (XtPointer) self);
        XtManageChild(accept);
    }
    /* xcpc-drive1-insert-cancel */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); ++argcount;
        cancel = XtCreateManagedWidget("xcpc-drive1-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(cancel, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(cancel);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
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
    String title = _("Legal informations ...");
    String message = _(((char*)(xcpc_legal_text())));
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget button = NULL;

    /* xcpc-legal-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-legal-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
        XtAddCallback(shell, XtNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
    }
    /* xcpc-legal-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, message); ++argcount;
        dialog = XtCreateWidget("xcpc-legal-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-legal-close */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Close")); ++argcount;
        button = XtCreateManagedWidget("xcpc-legal-close", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(button, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(button);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void AboutCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    String title = _("About Xcpc ...");
    String message = _(((char*)(xcpc_about_text())));
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget button = NULL;

    /* xcpc-about-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-about-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
        XtAddCallback(shell, XtNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
    }
    /* xcpc-about-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, message); ++argcount;
        dialog = XtCreateWidget("xcpc-about-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-about-close */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Close")); ++argcount;
        button = XtCreateManagedWidget("xcpc-about-close", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(button, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(button);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
    }
    /* pause */ {
        (void) Pause(self);
    }
}

static void HelpCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    String title = _("Help ...");
    String message = _(((char*)(help_text)));
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget button = NULL;

    /* xcpc-help-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetArg(arglist[argcount], XtNtransient, True); ++argcount;
        XtSetArg(arglist[argcount], XtNtransientFor, parent); ++argcount;
        shell = XtCreatePopupShell("xcpc-help-shell", xemDlgShellWidgetClass, parent, arglist, argcount);
        XtAddCallback(shell, XtNdestroyCallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
    }
    /* xcpc-help-dialog */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, message); ++argcount;
        dialog = XtCreateWidget("xcpc-help-dialog", dialogWidgetClass, shell, arglist, argcount);
        XtManageChild(dialog);
    }
    /* xcpc-help-close */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Close")); ++argcount;
        button = XtCreateManagedWidget("xcpc-help-close", commandWidgetClass, dialog, arglist, argcount);
        XtAddCallback(button, XtNcallback, (XtCallbackProc) &DismissCallback, (XtPointer) self);
        XtManageChild(button);
    }
    /* popup shell */ {
        XtPopup(shell, XtGrabExclusive);
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
            case XK_Pause:
                if(XtIsSensitive(self->layout.emulator) == False) {
                    Play(self);
                }
                else {
                    Pause(self);
                }
                break;
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
                break;
            case XK_F5:
                ResetCallback(widget, self, NULL);
                break;
            case XK_F6:
                InsertDrive0Callback(widget, self, NULL);
                break;
            case XK_F7:
                RemoveDrive0Callback(widget, self, NULL);
                break;
            case XK_F8:
                InsertDrive1Callback(widget, self, NULL);
                break;
            case XK_F9:
                RemoveDrive1Callback(widget, self, NULL);
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

    /* file-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("File")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "file-pulldown"); ++argcount;
        menu->menu = XtCreateWidget("file-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(menu->menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
    }
    /* file-pulldown */ {
        argcount = 0;
        menu->pulldown = XtCreatePopupShell("file-pulldown", simpleMenuWidgetClass, menu->menu, arglist, argcount);
        XtAddCallback(menu->pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* file-load-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Load snapshot...")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.file_load); ++argcount;
        menu->load_snapshot = XtCreateWidget("file-load-snapshot", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->load_snapshot, XtNcallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->load_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->load_snapshot);
        XtManageChild(menu->load_snapshot);
    }
    /* file-save-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Save snapshot...")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.file_save); ++argcount;
        menu->save_snapshot = XtCreateWidget("file-save-snapshot", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->save_snapshot, XtNcallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->save_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->save_snapshot);
        XtManageChild(menu->save_snapshot);
    }
    /* file-separator1 */ {
        argcount = 0;
        menu->separator1 = XtCreateWidget("file-separator1", smeLineObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->separator1, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->separator1);
        XtManageChild(menu->separator1);
    }
    /* file-exit */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Exit")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.file_exit); ++argcount;
        menu->exit = XtCreateWidget("file-exit", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->exit, XtNcallback, (XtCallbackProc) &ExitCallback, (XtPointer) self);
        XtAddCallback(menu->exit, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->exit);
        XtManageChild(menu->exit);
    }
    return self;
}

static XcpcApplication* BuildCtrlMenu(XcpcApplication* self)
{
    XcpcCtrlMenuRec* menu = &self->menubar.ctrl;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* ctrl-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Controls")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "ctrl-pulldown"); ++argcount;
        menu->menu = XtCreateWidget("ctrl-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(menu->menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
    }
    /* ctrl-pulldown */ {
        argcount = 0;
        menu->pulldown = XtCreatePopupShell("ctrl-pulldown", simpleMenuWidgetClass, menu->menu, arglist, argcount);
        XtAddCallback(menu->pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* ctrl-play-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Play")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.ctrl_play); ++argcount;
        menu->play_emulator = XtCreateWidget("ctrl-play-emu", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->play_emulator, XtNcallback, (XtCallbackProc) &PlayCallback, (XtPointer) self);
        XtAddCallback(menu->play_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->play_emulator);
        XtManageChild(menu->play_emulator);
    }
    /* ctrl-pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Pause")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.ctrl_pause); ++argcount;
        menu->pause_emulator = XtCreateWidget("ctrl-pause-emu", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->pause_emulator, XtNcallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(menu->pause_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pause_emulator);
        XtManageChild(menu->pause_emulator);
    }
    /* ctrl-reset-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Reset")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.ctrl_reset); ++argcount;
        menu->reset_emulator = XtCreateWidget("ctrl-reset-emu", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->reset_emulator, XtNcallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(menu->reset_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->reset_emulator);
        XtManageChild(menu->reset_emulator);
    }
    return self;
}

static XcpcApplication* BuildDrv0Menu(XcpcApplication* self)
{
    XcpcDrv0MenuRec* menu = &self->menubar.drv0;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv0-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive A")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "drv0-pulldown"); ++argcount;
        menu->menu = XtCreateWidget("drv0-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(menu->menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
    }
    /* drv0-pulldown */ {
        argcount = 0;
        menu->pulldown = XtCreatePopupShell("drv0-pulldown", simpleMenuWidgetClass, menu->menu, arglist, argcount);
        XtAddCallback(menu->pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* drv0-drive0-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk...")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.disk_insert); ++argcount;
        menu->drive0_insert = XtCreateWidget("drv0-drive0-insert", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->drive0_insert, XtNcallback, (XtCallbackProc) &InsertDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_insert, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_insert);
        XtManageChild(menu->drive0_insert);
    }
    /* drv0-drive0-remove */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Remove disk")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.disk_remove); ++argcount;
        menu->drive0_remove = XtCreateWidget("drv0-drive0-remove", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->drive0_remove, XtNcallback, (XtCallbackProc) &RemoveDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_remove, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_remove);
        XtManageChild(menu->drive0_remove);
    }
    return self;
}

static XcpcApplication* BuildDrv1Menu(XcpcApplication* self)
{
    XcpcDrv1MenuRec* menu = &self->menubar.drv1;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv1-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive B")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "drv1-pulldown"); ++argcount;
        menu->menu = XtCreateWidget("drv1-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(menu->menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
    }
    /* drv1-pulldown */ {
        argcount = 0;
        menu->pulldown = XtCreatePopupShell("drv1-pulldown", simpleMenuWidgetClass, menu->menu, arglist, argcount);
        XtAddCallback(menu->pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* drv1-drive1-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk...")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.disk_insert); ++argcount;
        menu->drive1_insert = XtCreateWidget("drv1-drive1-insert", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->drive1_insert, XtNcallback, (XtCallbackProc) &InsertDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_insert, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_insert);
        XtManageChild(menu->drive1_insert);
    }
    /* drv1-drive1-remove */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Remove disk")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.disk_remove); ++argcount;
        menu->drive1_remove = XtCreateWidget("drv1-drive1-remove", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->drive1_remove, XtNcallback, (XtCallbackProc) &RemoveDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_remove, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_remove);
        XtManageChild(menu->drive1_remove);
    }
    return self;
}

static XcpcApplication* BuildHelpMenu(XcpcApplication* self)
{
    XcpcHelpMenuRec* menu = &self->menubar.help;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* help-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Help")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "help-pulldown"); ++argcount;
        menu->menu = XtCreateWidget("help-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(menu->menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->menu);
        XtManageChild(menu->menu);
    }
    /* help-pulldown */ {
        argcount = 0;
        menu->pulldown = XtCreatePopupShell("help-pulldown", simpleMenuWidgetClass, menu->menu, arglist, argcount);
        XtAddCallback(menu->pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* help-help */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Help")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.help_help); ++argcount;
        menu->help = XtCreateWidget("help-help", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->help, XtNcallback, (XtCallbackProc) &HelpCallback, (XtPointer) self);
        XtAddCallback(menu->help, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->help);
        XtManageChild(menu->help);
    }
    /* help-legal */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Legal informations")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.help_legal); ++argcount;
        menu->legal = XtCreateWidget("help-legal", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->legal, XtNcallback, (XtCallbackProc) &LegalCallback, (XtPointer) self);
        XtAddCallback(menu->legal, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->legal);
        XtManageChild(menu->legal);
    }
    /* help-separator1 */ {
        argcount = 0;
        menu->separator1 = XtCreateWidget("help-separator1", smeLineObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->separator1, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->separator1);
        XtManageChild(menu->separator1);
    }
    /* help-about */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("About Xcpc")); ++argcount;
        XtSetArg(arglist[argcount], XtNleftBitmap, self->bitmaps.help_about); ++argcount;
        menu->about = XtCreateWidget("help-about", smeBSBObjectClass, menu->pulldown, arglist, argcount);
        XtAddCallback(menu->about, XtNcallback, (XtCallbackProc) &AboutCallback, (XtPointer) self);
        XtAddCallback(menu->about, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->about);
        XtManageChild(menu->about);
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
        XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable  , False             ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz  , NULL              ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert   , NULL              ); ++argcount;
        XtSetArg(arglist[argcount], XtNtop        , XtChainTop        ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom     , XtChainTop        ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft       , XtChainLeft       ); ++argcount;
        XtSetArg(arglist[argcount], XtNright      , XtChainLeft       ); ++argcount;
        menubar->widget = XtCreateWidget("menubar", boxWidgetClass, self->layout.window, arglist, argcount);
        XtAddCallback(menubar->widget, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menubar->widget);
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
        XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal  ); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable  , False               ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz  , NULL                ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert   , self->menubar.widget); ++argcount;
        XtSetArg(arglist[argcount], XtNtop        , XtChainTop          ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom     , XtChainTop          ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft       , XtChainLeft         ); ++argcount;
        XtSetArg(arglist[argcount], XtNright      , XtChainLeft         ); ++argcount;
        toolbar->widget = XtCreateWidget("toolbar", boxWidgetClass, self->layout.window, arglist, argcount);
        XtAddCallback(toolbar->widget, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->widget);
        XtManageChild(toolbar->widget);
    }
    /* tool-load-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Load")); ++argcount;
        XtSetArg(arglist[argcount], XtNbitmap, self->bitmaps.file_load); ++argcount;
        toolbar->load_snapshot = XtCreateWidget("tool-load-snapshot", commandWidgetClass, toolbar->widget, arglist, argcount);
        XtAddCallback(toolbar->load_snapshot, XtNcallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(toolbar->load_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->load_snapshot);
        XtManageChild(toolbar->load_snapshot);
    }
    /* tool-save-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Save")); ++argcount;
        XtSetArg(arglist[argcount], XtNbitmap, self->bitmaps.file_save); ++argcount;
        toolbar->save_snapshot = XtCreateWidget("tool-save-snapshot", commandWidgetClass, toolbar->widget, arglist, argcount);
        XtAddCallback(toolbar->save_snapshot, XtNcallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(toolbar->save_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->save_snapshot);
        XtManageChild(toolbar->save_snapshot);
    }
    /* tool-play-emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Play")); ++argcount;
        XtSetArg(arglist[argcount], XtNbitmap, self->bitmaps.ctrl_play); ++argcount;
        toolbar->play_emulator = XtCreateWidget("tool-play-emulator", commandWidgetClass, toolbar->widget, arglist, argcount);
        XtAddCallback(toolbar->play_emulator, XtNcallback, (XtCallbackProc) &PlayCallback, (XtPointer) self);
        XtAddCallback(toolbar->play_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->play_emulator);
        XtManageChild(toolbar->play_emulator);
    }
    /* tool-pause-emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Pause")); ++argcount;
        XtSetArg(arglist[argcount], XtNbitmap, self->bitmaps.ctrl_pause); ++argcount;
        toolbar->pause_emulator = XtCreateWidget("tool-pause-emulator", commandWidgetClass, toolbar->widget, arglist, argcount);
        XtAddCallback(toolbar->pause_emulator, XtNcallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(toolbar->pause_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->pause_emulator);
        XtManageChild(toolbar->pause_emulator);
    }
    /* tool-reset-emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Reset")); ++argcount;
        XtSetArg(arglist[argcount], XtNbitmap, self->bitmaps.ctrl_reset); ++argcount;
        toolbar->reset_emulator = XtCreateWidget("tool-reset-emulator", commandWidgetClass, toolbar->widget, arglist, argcount);
        XtAddCallback(toolbar->reset_emulator, XtNcallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(toolbar->reset_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &toolbar->reset_emulator);
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
        XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal   ); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable  , True                 ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz  , NULL                 ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert   , self->layout.emulator); ++argcount;
        XtSetArg(arglist[argcount], XtNtop        , XtChainBottom        ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom     , XtChainBottom        ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft       , XtChainLeft          ); ++argcount;
        XtSetArg(arglist[argcount], XtNright      , XtChainLeft          ); ++argcount;
        infobar->widget = XtCreateWidget("infobar", boxWidgetClass, self->layout.window, arglist, argcount);
        XtAddCallback(infobar->widget, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->widget);
        XtManageChild(infobar->widget);
    }
    /* info-status */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Status")); ++argcount;
        infobar->status = XtCreateWidget("info-status", labelWidgetClass, infobar->widget, arglist, argcount);
        XtAddCallback(infobar->status, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->status);
        XtManageChild(infobar->status);
    }
    /* info-drive0 */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive0")); ++argcount;
        infobar->drive0 = XtCreateWidget("info-drive0", labelWidgetClass, infobar->widget, arglist, argcount);
        XtAddCallback(infobar->drive0, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->drive0);
        XtManageChild(infobar->drive0);
    }
    /* info-drive1 */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive1")); ++argcount;
        infobar->drive1 = XtCreateWidget("info-drive1", labelWidgetClass, infobar->widget, arglist, argcount);
        XtAddCallback(infobar->drive1, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->drive1);
        XtManageChild(infobar->drive1);
    }
    /* info-system */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("System")); ++argcount;
        infobar->system = XtCreateWidget("info-system", labelWidgetClass, infobar->widget, arglist, argcount);
        XtAddCallback(infobar->system, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &infobar->system);
        XtManageChild(infobar->system);
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
        XtSetArg(arglist[argcount], XtNborderWidth, 0                      ); ++argcount;
        XtSetArg(arglist[argcount], XtNsensitive  , False                  ); ++argcount;
        XtSetArg(arglist[argcount], XtNbackend    , &self->machine->backend); ++argcount;
        XtSetArg(arglist[argcount], XtNjoystick0  , xcpc_get_joystick0()   ); ++argcount;
        XtSetArg(arglist[argcount], XtNjoystick1  , xcpc_get_joystick1()   ); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable  , True                   ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz  , NULL                   ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert   , self->toolbar.widget   ); ++argcount;
        XtSetArg(arglist[argcount], XtNtop        , XtChainTop             ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom     , XtChainBottom          ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft       , XtChainLeft            ); ++argcount;
        XtSetArg(arglist[argcount], XtNright      , XtChainRight           ); ++argcount;
        layout->emulator = XemCreateEmulator(self->layout.window, "emulator", arglist, argcount);
        XtAddCallback(layout->emulator, XtNhotkeyCallback , (XtCallbackProc) &HotkeyCallback , (XtPointer) self);
        XtAddCallback(layout->emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &layout->emulator);
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
        XtSetArg(arglist[argcount], XtNorientation, XtorientVertical); ++argcount;
        self->layout.window = XtCreateWidget("main-window", formWidgetClass, self->layout.toplevel, arglist, argcount);
        XtAddCallback(self->layout.window, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.window);
        XtManageChild(self->layout.window);
    }
    /* get bitmaps */ {
        GetBitmaps(self->layout.window, &self->bitmaps);
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
    return self;
}

static void DeferredStartHandler(XcpcApplication* self, XtIntervalId* intervalId)
{
    if(*intervalId == self->intervalId) {
        self->intervalId = ((XtIntervalId)(0));
    }
    /* set initial keyboard focus */ {
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* play */ {
        (void) Play(self);
    }
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* XcpcApplicationNew(int* argc, char*** argv)
{
    XcpcApplication* self = xcpc_new(XcpcApplication);

    if(self != NULL) {
        (void) memset(self, 0, sizeof(XcpcApplication));
    }
    if(self != NULL) {
        self->argc = argc;
        self->argv = argv;
    }
    return self;
}

XcpcApplication* XcpcApplicationDelete(XcpcApplication* self)
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
        self = xcpc_delete(XcpcApplication, self);
    }
    return self;
}

XcpcApplication* XcpcApplicationRun(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* intialize options */ {
        self->options = xcpc_options_new(self->argc, self->argv);
        (void) xcpc_options_parse(self->options);
        if(xcpc_options_quit(self->options) != 0) {
            return self;
        }
    }
    /* intialize machine */ {
        self->machine = xcpc_machine_new(NULL, self->options);
    }
#ifdef HAVE_PORTAUDIO
    /* initialize portaudio */ {
        const PaError pa_error = Pa_Initialize();
        if(pa_error != paNoError) {
            xcpc_log_error("unable to initialize PortAudio (%s)", Pa_GetErrorText(pa_error));
        }
    }
#endif
    /* initialize Xaw */ {
        XawInitializeWidgetSet();
        XawInitializeDefaultConverters();
    }
    /* set language proc */ {
        (void) XtSetLanguageProc(NULL, NULL, NULL);
    }
    /* create application context and toplevel shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNmappedWhenManaged, True); ++argcount;
        XtSetArg(arglist[argcount], XtNallowShellResize , True); ++argcount;
        self->layout.toplevel = XtOpenApplication(&self->appcontext, "Xcpc", options, XtNumber(options), self->argc, *self->argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
        XtAddCallback(self->layout.toplevel, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.toplevel);
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
    /* deferred start */ {
        self->intervalId = XtAppAddTimeOut(self->appcontext, 25UL, (XtTimerCallbackProc) &DeferredStartHandler, self);
    }
    /* realize toplevel shell */ {
        XtRealizeWidget(self->layout.toplevel);
    }
    /* run application loop  */ {
        XtAppMainLoop(self->appcontext);
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
    XcpcApplication* self = XcpcApplicationNew(argc, argv);

    if(self != NULL) {
        (void) XcpcApplicationRun(self);
        self = XcpcApplicationDelete(self);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
