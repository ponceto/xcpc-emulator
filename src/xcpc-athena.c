/*
 * xcpc-athena.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xcpc-athena-priv.h"

#ifndef _
#define _(string) (string)
#endif

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
    ".pixmapFilePath: " XCPC_RESDIR "/pixmaps",
    "Xcpc*title: Xcpc - Amstrad CPC emulator",
    "Xcpc*menubar.borderWidth: 0",
    "Xcpc*menubar.?.borderWidth: 0",
    "Xcpc*emulator.borderWidth: 0",
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
        XtRImmediate, (XtPointer) FALSE
    },
    /* xcpcTraceFlag */ {
        "xcpcTraceFlag", "XcpcTraceFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, trace_flag),
        XtRImmediate, (XtPointer) FALSE
    },
    /* xcpcDebugFlag */ {
        "xcpcDebugFlag", "XcpcDebugFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, debug_flag),
        XtRImmediate, (XtPointer) FALSE
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
    while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
        widget = XtParent(widget);
    }
    return widget;
}

static Widget FindTopLevelShell(Widget widget)
{
    while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
        widget = XtParent(widget);
    }
    return widget;
}

/*
 * ---------------------------------------------------------------------------
 * Controls
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* SetTitle(XcpcApplication* self, const char* title)
{
    Arg      arglist[4];
    Cardinal argcount = 0;

    if(self->layout.toplevel != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetValues(self->layout.toplevel, arglist, argcount);
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
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, TRUE);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    return SetTitle(self, _("Xcpc - Amstrad CPC emulator - Playing"));
}

static XcpcApplication* Pause(XcpcApplication* self)
{
    /* show/hide controls */ {
        ShowWidget(self->menubar.ctrl.play_emulator);
        HideWidget(self->menubar.ctrl.pause_emulator);
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, FALSE);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    return SetTitle(self, _("Xcpc - Amstrad CPC emulator - Paused"));
}

static XcpcApplication* Reset(XcpcApplication* self)
{
    if(self->layout.emulator != NULL) {
        (void) xcpc_machine_reset(self->machine);
    }
    return SetTitle(self, _("Xcpc - Amstrad CPC emulator - Reset"));
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
    return self;
}

static XcpcApplication* RemoveDiskFromDrive0(XcpcApplication* self)
{
    xcpc_machine_remove_drive0(self->machine);

    return self;
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
    return self;
}

static XcpcApplication* RemoveDiskFromDrive1(XcpcApplication* self)
{
    xcpc_machine_remove_drive1(self->machine);

    return self;
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
    if(XtIsApplicationShell(widget) != FALSE) {
        XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
    }
    if((widget != NULL) && (reference != NULL) && (widget == *reference)) {
        *reference = NULL;
    }
}

static void DismissCallback(Widget widget, XcpcApplication* self, XtPointer* info)
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

static void DropUriCallback(Widget widget, XcpcApplication* self, char* uri)
{
    int length = 0;

    if((uri != NULL) && (strncmp(uri, "file://", 7) == 0)) {
        char* str = &uri[7];
        char* eol = strstr(str, "\r\n");
        if(eol != NULL) {
            *eol = '\0';
        }
        if((length = strlen(str)) >= 4) {
            if(strcasecmp(&str[length - 4], ".sna") == 0) {
                (void) LoadSnapshot(self, str);
                (void) Play(self);
            }
            if(strcasecmp(&str[length - 4], ".dsk") == 0) {
                (void) InsertOrRemoveDisk(self, str);
                (void) Play(self);
            }
        }
    }
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
    Arg arglist[16];
    Cardinal argcount;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-load-snapshot-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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
    Arg arglist[16];
    Cardinal argcount;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-save-snapshot-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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
    Arg arglist[16];
    Cardinal argcount;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-drive0-insert-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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
    Arg arglist[16];
    Cardinal argcount;
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget accept = NULL;
    Widget cancel = NULL;

    /* xcpc-drive1-insert-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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
 * File callbacks
 * ---------------------------------------------------------------------------
 */

static void ExitCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Exit(self);
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
 * Legal callbacks
 * ---------------------------------------------------------------------------
 */

static void LegalCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg arglist[16];
    Cardinal argcount;
    String title = _("Legal Info ...");
    String message = _(((char*)(xcpc_legal_text())));
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget button = NULL;

    /* xcpc-legal-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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

/*
 * ---------------------------------------------------------------------------
 * About callbacks
 * ---------------------------------------------------------------------------
 */

static void AboutCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Arg arglist[16];
    Cardinal argcount;
    String title = _("About Xcpc ...");
    String message = _(((char*)(xcpc_about_text())));
    Widget parent = FindTopLevelShell(widget);
    Widget shell  = NULL;
    Widget dialog = NULL;
    Widget button = NULL;

    /* xcpc-about-shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, title); ++argcount;
        XtSetArg(arglist[argcount], XtNtransient, TRUE); ++argcount;
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

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* BuildFileMenu(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* file-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("File")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "file-pulldown"); ++argcount;
        self->menubar.file.menu = XtCreateWidget("file-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(self->menubar.file.menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.menu);
        XtManageChild(self->menubar.file.menu);
    }
    /* file-pulldown */ {
        argcount = 0;
        self->menubar.file.pulldown = XtCreatePopupShell("file-pulldown", simpleMenuWidgetClass, self->menubar.file.menu, arglist, argcount);
        XtAddCallback(self->menubar.file.pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.pulldown);
    }
    /* file-load-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Load snapshot...")); ++argcount;
        self->menubar.file.load_snapshot = XtCreateWidget("file-load-snapshot", smeBSBObjectClass, self->menubar.file.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.file.load_snapshot, XtNcallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->menubar.file.load_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.load_snapshot);
        XtManageChild(self->menubar.file.load_snapshot);
    }
    /* file-save-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Save snapshot...")); ++argcount;
        self->menubar.file.save_snapshot = XtCreateWidget("file-save-snapshot", smeBSBObjectClass, self->menubar.file.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.file.save_snapshot, XtNcallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->menubar.file.save_snapshot, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.save_snapshot);
        XtManageChild(self->menubar.file.save_snapshot);
    }
    /* file-separator1 */ {
        argcount = 0;
        self->menubar.file.separator1 = XtCreateWidget("file-separator1", smeLineObjectClass, self->menubar.file.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.file.separator1, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.separator1);
        XtManageChild(self->menubar.file.separator1);
    }
    /* file-exit */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Exit")); ++argcount;
        self->menubar.file.exit = XtCreateWidget("file-exit", smeBSBObjectClass, self->menubar.file.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.file.exit, XtNcallback, (XtCallbackProc) &ExitCallback, (XtPointer) self);
        XtAddCallback(self->menubar.file.exit, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.file.exit);
        XtManageChild(self->menubar.file.exit);
    }
    return self;
}

static XcpcApplication* BuildCtrlMenu(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* ctrl-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Controls")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "ctrl-pulldown"); ++argcount;
        self->menubar.ctrl.menu = XtCreateWidget("ctrl-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(self->menubar.ctrl.menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.ctrl.menu);
        XtManageChild(self->menubar.ctrl.menu);
    }
    /* ctrl-pulldown */ {
        argcount = 0;
        self->menubar.ctrl.pulldown = XtCreatePopupShell("ctrl-pulldown", simpleMenuWidgetClass, self->menubar.ctrl.menu, arglist, argcount);
        XtAddCallback(self->menubar.ctrl.pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.ctrl.pulldown);
    }
    /* ctrl-play-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Play")); ++argcount;
        self->menubar.ctrl.play_emulator = XtCreateWidget("ctrl-play-emu", smeBSBObjectClass, self->menubar.ctrl.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.ctrl.play_emulator, XtNcallback, (XtCallbackProc) &PlayCallback, (XtPointer) self);
        XtAddCallback(self->menubar.ctrl.play_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.ctrl.play_emulator);
        XtManageChild(self->menubar.ctrl.play_emulator);
    }
    /* ctrl-pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Pause")); ++argcount;
        self->menubar.ctrl.pause_emulator = XtCreateWidget("ctrl-pause-emu", smeBSBObjectClass, self->menubar.ctrl.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.ctrl.pause_emulator, XtNcallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(self->menubar.ctrl.pause_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.ctrl.pause_emulator);
        XtManageChild(self->menubar.ctrl.pause_emulator);
    }
    /* ctrl-reset-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Reset")); ++argcount;
        self->menubar.ctrl.reset_emulator = XtCreateWidget("ctrl-reset-emu", smeBSBObjectClass, self->menubar.ctrl.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.ctrl.reset_emulator, XtNcallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(self->menubar.ctrl.reset_emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.ctrl.reset_emulator);
        XtManageChild(self->menubar.ctrl.reset_emulator);
    }
    return self;
}

static XcpcApplication* BuildDrv0Menu(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv0-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive A")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "drv0-pulldown"); ++argcount;
        self->menubar.drv0.menu = XtCreateWidget("drv0-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(self->menubar.drv0.menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv0.menu);
        XtManageChild(self->menubar.drv0.menu);
    }
    /* drv0-pulldown */ {
        argcount = 0;
        self->menubar.drv0.pulldown = XtCreatePopupShell("drv0-pulldown", simpleMenuWidgetClass, self->menubar.drv0.menu, arglist, argcount);
        XtAddCallback(self->menubar.drv0.pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv0.pulldown);
    }
    /* drv0-drive0-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk...")); ++argcount;
        self->menubar.drv0.drive0_insert = XtCreateWidget("drv0-drive0-insert", smeBSBObjectClass, self->menubar.drv0.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.drv0.drive0_insert, XtNcallback, (XtCallbackProc) &InsertDrive0Callback, (XtPointer) self);
        XtAddCallback(self->menubar.drv0.drive0_insert, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv0.drive0_insert);
        XtManageChild(self->menubar.drv0.drive0_insert);
    }
    /* drv0-drive0-remove */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Remove disk")); ++argcount;
        self->menubar.drv0.drive0_remove = XtCreateWidget("drv0-drive0-remove", smeBSBObjectClass, self->menubar.drv0.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.drv0.drive0_remove, XtNcallback, (XtCallbackProc) &RemoveDrive0Callback, (XtPointer) self);
        XtAddCallback(self->menubar.drv0.drive0_remove, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv0.drive0_remove);
        XtManageChild(self->menubar.drv0.drive0_remove);
    }
    return self;
}

static XcpcApplication* BuildDrv1Menu(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv1-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Drive B")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "drv1-pulldown"); ++argcount;
        self->menubar.drv1.menu = XtCreateWidget("drv1-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(self->menubar.drv1.menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv1.menu);
        XtManageChild(self->menubar.drv1.menu);
    }
    /* drv1-pulldown */ {
        argcount = 0;
        self->menubar.drv1.pulldown = XtCreatePopupShell("drv1-pulldown", simpleMenuWidgetClass, self->menubar.drv1.menu, arglist, argcount);
        XtAddCallback(self->menubar.drv1.pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv1.pulldown);
    }
    /* drv1-drive1-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk...")); ++argcount;
        self->menubar.drv1.drive1_insert = XtCreateWidget("drv1-drive1-insert", smeBSBObjectClass, self->menubar.drv1.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.drv1.drive1_insert, XtNcallback, (XtCallbackProc) &InsertDrive1Callback, (XtPointer) self);
        XtAddCallback(self->menubar.drv1.drive1_insert, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv1.drive1_insert);
        XtManageChild(self->menubar.drv1.drive1_insert);
    }
    /* drv1-drive1-remove */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Remove disk")); ++argcount;
        self->menubar.drv1.drive1_remove = XtCreateWidget("drv1-drive1-remove", smeBSBObjectClass, self->menubar.drv1.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.drv1.drive1_remove, XtNcallback, (XtCallbackProc) &RemoveDrive1Callback, (XtPointer) self);
        XtAddCallback(self->menubar.drv1.drive1_remove, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.drv1.drive1_remove);
        XtManageChild(self->menubar.drv1.drive1_remove);
    }
    return self;
}

static XcpcApplication* BuildHelpMenu(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* help-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Help")); ++argcount;
        XtSetArg(arglist[argcount], XtNmenuName, "help-pulldown"); ++argcount;
        self->menubar.help.menu = XtCreateWidget("help-menu", menuButtonWidgetClass, self->menubar.widget, arglist, argcount);
        XtAddCallback(self->menubar.help.menu, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.help.menu);
        XtManageChild(self->menubar.help.menu);
    }
    /* help-pulldown */ {
        argcount = 0;
        self->menubar.help.pulldown = XtCreatePopupShell("help-pulldown", simpleMenuWidgetClass, self->menubar.help.menu, arglist, argcount);
        XtAddCallback(self->menubar.help.pulldown, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.help.pulldown);
    }
    /* help-legal */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Legal Info")); ++argcount;
        self->menubar.help.legal = XtCreateWidget("help-legal", smeBSBObjectClass, self->menubar.help.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.help.legal, XtNcallback, (XtCallbackProc) &LegalCallback, (XtPointer) self);
        XtAddCallback(self->menubar.help.legal, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.help.legal);
        XtManageChild(self->menubar.help.legal);
    }
    /* help-separator1 */ {
        argcount = 0;
        self->menubar.help.separator1 = XtCreateWidget("help-separator1", smeLineObjectClass, self->menubar.help.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.help.separator1, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.help.separator1);
        XtManageChild(self->menubar.help.separator1);
    }
    /* help-about */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("About Xcpc")); ++argcount;
        self->menubar.help.about = XtCreateWidget("help-about", smeBSBObjectClass, self->menubar.help.pulldown, arglist, argcount);
        XtAddCallback(self->menubar.help.about, XtNcallback, (XtCallbackProc) &AboutCallback, (XtPointer) self);
        XtAddCallback(self->menubar.help.about, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.help.about);
        XtManageChild(self->menubar.help.about);
    }
    return self;
}

static XcpcApplication* BuildMenuBar(XcpcApplication* self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* menubar */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable  , True              ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz  , NULL              ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert   , NULL              ); ++argcount;
        XtSetArg(arglist[argcount], XtNtop        , XtChainTop        ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom     , XtChainTop        ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft       , XtChainLeft       ); ++argcount;
        XtSetArg(arglist[argcount], XtNright      , XtChainLeft       ); ++argcount;
        self->menubar.widget = XtCreateWidget("menubar", boxWidgetClass, self->layout.window, arglist, argcount);
        XtAddCallback(self->menubar.widget, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.widget);
        XtManageChild(self->menubar.widget);
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
    /* menubar */ {
        (void) BuildMenuBar(self);
    }
    /* emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNemuContext    , self->machine             ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuCreateProc , &xcpc_machine_create_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuDestroyProc, &xcpc_machine_destroy_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuRealizeProc, &xcpc_machine_realize_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuResizeProc , &xcpc_machine_resize_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuExposeProc , &xcpc_machine_expose_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuTimerProc  , &xcpc_machine_timer_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuInputProc  , &xcpc_machine_input_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNresizable     , True                      ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromHoriz     , NULL                      ); ++argcount;
        XtSetArg(arglist[argcount], XtNfromVert      , self->menubar.widget      ); ++argcount;
        XtSetArg(arglist[argcount], XtNtop           , XtChainTop                ); ++argcount;
        XtSetArg(arglist[argcount], XtNbottom        , XtChainBottom             ); ++argcount;
        XtSetArg(arglist[argcount], XtNleft          , XtChainLeft               ); ++argcount;
        XtSetArg(arglist[argcount], XtNright         , XtChainRight              ); ++argcount;
        self->layout.emulator = XemCreateEmulator(self->layout.window, "emulator", arglist, argcount);
        XtAddCallback(self->layout.emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.emulator);
        XtManageChild(self->layout.emulator);
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
    /* initialize Xaw */ {
        XawInitializeWidgetSet();
        XawInitializeDefaultConverters();
    }
    /* set language proc */ {
        (void) XtSetLanguageProc(NULL, NULL, NULL);
    }
    /* create application context and toplevel shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNmappedWhenManaged, TRUE); ++argcount;
        XtSetArg(arglist[argcount], XtNallowShellResize , TRUE); ++argcount;
        self->layout.toplevel = XtOpenApplication(&self->appcontext, "Xcpc", options, XtNumber(options), argc, *argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
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
    if(XtAppGetExitFlag(self->appcontext) == FALSE) {
        /* realize toplevel shell */ {
            if((self->layout.toplevel != NULL)) {
                XtRealizeWidget(self->layout.toplevel);
            }
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
