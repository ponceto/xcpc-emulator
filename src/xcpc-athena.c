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
    "Xcpc*title: Xcpc - Amstrad CPC emulator",
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
    widget = FindShell(widget);
    XtPopdown(widget);
    XtDestroyWidget(widget);
    XtSetSensitive(self->layout.emulator, TRUE);
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
            if(strcmp(&str[length - 4], ".sna") == 0) {
                xcpc_machine_load_snapshot(self->machine, str);
            }
            if(strcmp(&str[length - 4], ".dsk") == 0) {
                xcpc_machine_insert_drive0(self->machine, str);
                xcpc_machine_insert_drive1(self->machine, str);
            }
            XtSetSensitive(self->layout.emulator, TRUE);
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * XXX
 * ---------------------------------------------------------------------------
 */

static void OnLoadSnapshotOkCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    char *value = XawDialogGetValueString(XtParent(widget));
    if(value != NULL) {
        xcpc_machine_load_snapshot(self->machine, value);
    }
    DismissCallback(widget, self, cbs);
}

static void OnLoadSnapshotCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, btn_ok, cancel;
    Arg arglist[8];
    Cardinal argcount;

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* load-snapshot-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("load-snapshot-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* load-snapshot-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Load a snapshot ...")); argcount++;
    XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
    dialog = XtCreateWidget("load-snapshot-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* load-snapshot-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _(" Load ")); argcount++;
    btn_ok = XtCreateManagedWidget("load-snapshot-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnLoadSnapshotOkCbk, (XtPointer) self);
    XtManageChild(btn_ok);
    /* load-snapshot-cancel */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
    cancel = XtCreateManagedWidget("load-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(cancel, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(cancel);
    /* load-snapshot-popup */
    XtPopup(shell, XtGrabExclusive);
}

static void OnSaveSnapshotOkCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    char *value = XawDialogGetValueString(XtParent(widget));
    if(value != NULL) {
        xcpc_machine_save_snapshot(self->machine, value);
    }
    DismissCallback(widget, self, cbs);
}

static void OnSaveSnapshotCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, btn_ok, cancel;
    Arg arglist[8];
    Cardinal argcount;

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* save-snapshot-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("save-snapshot-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* save-snapshot-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Save a snapshot ...")); argcount++;
    XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
    dialog = XtCreateWidget("save-snapshot-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* save-snapshot-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _(" Save ")); argcount++;
    btn_ok = XtCreateManagedWidget("save-snapshot-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnSaveSnapshotOkCbk, (XtPointer) self);
    XtManageChild(btn_ok);
    /* save-snapshot-cancel */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
    cancel = XtCreateManagedWidget("save-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(cancel, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(cancel);
    /* save-snapshot-popup */
    XtPopup(shell, XtGrabExclusive);
}

static void OnDriveAInsertOkCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    char *value = XawDialogGetValueString(XtParent(widget));
    if((value != NULL) && (*value != '\0')) {
        xcpc_machine_insert_drive0(self->machine, value);
    }
    DismissCallback(widget, self, cbs);
}

static void OnDriveAInsertCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, btn_ok, cancel;
    Arg arglist[8];
    Cardinal argcount;

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* drivea-insert-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("drivea-insert-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* drivea-insert-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive A ...")); argcount++;
    XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
    dialog = XtCreateWidget("drivea-insert-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* drivea-insert-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Insert")); argcount++;
    btn_ok = XtCreateManagedWidget("drivea-insert-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnDriveAInsertOkCbk, (XtPointer) self);
    XtManageChild(btn_ok);
    /* drivea-insert-cancel */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
    cancel = XtCreateManagedWidget("drivea-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(cancel, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(cancel);
    /* drivea-insert-popup */
    XtPopup(shell, XtGrabExclusive);
}

static void OnDriveAEjectCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    xcpc_machine_remove_drive0(self->machine);
}

static void OnDriveBInsertOkCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    char *value = XawDialogGetValueString(XtParent(widget));
    if((value != NULL) && (*value != '\0')) {
        xcpc_machine_insert_drive1(self->machine, value);
    }
    DismissCallback(widget, self, cbs);
}

static void OnDriveBInsertCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, btn_ok, cancel;
    Arg arglist[8];
    Cardinal argcount;

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* driveb-insert-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("driveb-insert-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* driveb-insert-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive B ...")); argcount++;
    XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
    dialog = XtCreateWidget("driveb-insert-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* driveb-insert-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Insert")); argcount++;
    btn_ok = XtCreateManagedWidget("driveb-insert-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnDriveBInsertOkCbk, (XtPointer) self);
    XtManageChild(btn_ok);
    /* driveb-insert-cancel */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
    cancel = XtCreateManagedWidget("driveb-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(cancel, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(cancel);
    /* driveb-insert-popup */
    XtPopup(shell, XtGrabExclusive);
}

static void OnDriveBEjectCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    xcpc_machine_remove_drive1(self->machine);
}

static void OnExitEmulatorCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
}

static void OnPauseCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    if(XtIsSensitive(self->layout.emulator) != FALSE) {
        XtSetSensitive(self->layout.emulator, FALSE);
    }
    else {
        XtSetSensitive(self->layout.emulator, TRUE);
    }
}

static void OnResetCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    xcpc_machine_reset(self->machine);
    XtSetSensitive(self->layout.emulator, TRUE);
}

static void OnLegalInfoCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, button;
    Arg arglist[8];
    Cardinal argcount;
    String message = _(
        "Amstrad has kindly given it's permission for it's copyrighted\n"
        "material to be redistributed but Amstrad retains it's copyright.\n\n"
        "Some of the Amstrad CPC ROM code is copyright Locomotive Software.\n\n"
        "ROM and DISK images are protected under the copyrights of their authors,\n"
        "and cannot be distributed in this package. You can download and/or use\n"
        "ROM and DISK images at your own risk and responsibility."
    );

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* legal-info-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("legal-info-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* legal-info-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, message); argcount++;
    dialog = XtCreateWidget("legal-info-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* legal-info-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("OK")); argcount++;
    button = XtCreateManagedWidget("legal-info-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(button, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(button);
    /* legal-info-popup */
    XtPopup(shell, XtGrabExclusive);
}

static void OnAboutXcpcCbk(Widget widget, XcpcApplication* self, XtPointer cbs)
{
    Widget shell, dialog, button;
    Arg arglist[8];
    Cardinal argcount;
    String message = _(
        PACKAGE_STRING " - Amstrad CPC Emulator - Copyright (c) 2001-2021 - Olivier Poncet\n\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 2 of the License, or\n"
        "(at your option) any later version.\n\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see <http://www.gnu.org/licenses/>"
    );

    XtSetSensitive(self->layout.emulator, FALSE);
    widget = FindTopLevelShell(widget);
    /* about-xcpc-shell */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNtransient, TRUE); argcount++;
    XtSetArg(arglist[argcount], XtNtransientFor, widget); argcount++;
    shell = XtCreatePopupShell("about-xcpc-shell", xemDlgShellWidgetClass, widget, arglist, argcount);
    /* about-xcpc-dialog */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, message); argcount++;
    dialog = XtCreateWidget("about-xcpc-dialog", dialogWidgetClass, shell, arglist, argcount);
    XtManageChild(dialog);
    /* about-xcpc-btn-ok */
    argcount = 0;
    XtSetArg(arglist[argcount], XtNlabel, _("OK")); argcount++;
    button = XtCreateManagedWidget("about-xcpc-btn-ok", commandWidgetClass, dialog, arglist, argcount);
    XtAddCallback(button, XtNcallback, (XtCallbackProc) DismissCallback, (XtPointer) self);
    XtManageChild(button);
    /* about-xcpc-popup */
    XtPopup(shell, XtGrabExclusive);
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication* private methods
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* BuildLayout(XcpcApplication* self)
{
    Arg arglist[8];
    Cardinal argcount;

    /* main-wnd */ {
        argcount = 0;
        self->layout.main_wnd = XtCreateWidget("main-wnd", boxWidgetClass, self->layout.toplevel, arglist, argcount);
        XtManageChild(self->layout.main_wnd);
    }
    /* menu-bar */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal); argcount++;
        XtSetArg(arglist[argcount], XtNborderWidth, 0); argcount++;
        self->layout.menu_bar = XtCreateWidget("menu-bar", boxWidgetClass, self->layout.main_wnd, arglist, argcount);
        XtManageChild(self->layout.menu_bar);
    }
    /* file-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("File")); argcount++;
        XtSetArg(arglist[argcount], XtNmenuName, "file-pldn"); argcount++;
        self->layout.file_menu = XtCreateWidget("file-menu", menuButtonWidgetClass, self->layout.menu_bar, arglist, argcount);
        XtManageChild(self->layout.file_menu);
    }
    /* file-pldn */ {
        argcount = 0;
        self->layout.file_pldn = XtCreatePopupShell("file-pldn", simpleMenuWidgetClass, self->layout.file_menu, arglist, argcount);
    }
    /* load-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Load Snapshot")); argcount++;
        self->layout.load_snapshot = XtCreateWidget("load-snapshot", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.load_snapshot, XtNcallback, (XtCallbackProc) OnLoadSnapshotCbk, (XtPointer) self);
        XtManageChild(self->layout.load_snapshot);
    }
    /* save-snapshot */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Save Snapshot")); argcount++;
        self->layout.save_snapshot = XtCreateWidget("save-snapshot", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.save_snapshot, XtNcallback, (XtCallbackProc) OnSaveSnapshotCbk, (XtPointer) self);
        XtManageChild(self->layout.save_snapshot);
    }
    /* separator1 */ {
        argcount = 0;
        self->layout.separator1 = XtCreateWidget("separator1", smeLineObjectClass, self->layout.file_pldn, arglist, argcount);
        XtManageChild(self->layout.separator1);
    }
    /* drivea-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive A")); argcount++;
        self->layout.drivea_insert = XtCreateWidget("drivea-insert", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.drivea_insert, XtNcallback, (XtCallbackProc) OnDriveAInsertCbk, (XtPointer) self);
        XtManageChild(self->layout.drivea_insert);
    }
    /* drivea-eject */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Eject disk from drive A")); argcount++;
        self->layout.drivea_eject = XtCreateWidget("drivea-eject", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.drivea_eject, XtNcallback, (XtCallbackProc) OnDriveAEjectCbk, (XtPointer) self);
        XtManageChild(self->layout.drivea_eject);
    }
    /* separator2 */ {
        argcount = 0;
        self->layout.separator2 = XtCreateWidget("separator2", smeLineObjectClass, self->layout.file_pldn, arglist, argcount);
        XtManageChild(self->layout.separator2);
    }
    /* driveb-insert */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive B")); argcount++;
        self->layout.driveb_insert = XtCreateWidget("driveb-insert", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.driveb_insert, XtNcallback, (XtCallbackProc) OnDriveBInsertCbk, (XtPointer) self);
        XtManageChild(self->layout.driveb_insert);
    }
    /* driveb-eject */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Eject disk from drive B")); argcount++;
        self->layout.driveb_eject = XtCreateWidget("driveb-eject", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.driveb_eject, XtNcallback, (XtCallbackProc) OnDriveBEjectCbk, (XtPointer) self);
        XtManageChild(self->layout.driveb_eject);
    }
    /* separator3 */ {
        argcount = 0;
        self->layout.separator3 = XtCreateWidget("separator3", smeLineObjectClass, self->layout.file_pldn, arglist, argcount);
        XtManageChild(self->layout.separator3);
    }
    /* exit-emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Exit")); argcount++;
        self->layout.exit_emulator = XtCreateWidget("exit-emulator", smeBSBObjectClass, self->layout.file_pldn, arglist, argcount);
        XtAddCallback(self->layout.exit_emulator, XtNcallback, (XtCallbackProc) OnExitEmulatorCbk, (XtPointer) self);
        XtManageChild(self->layout.exit_emulator);
    }
    /* ctrl-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Controls")); argcount++;
        XtSetArg(arglist[argcount], XtNmenuName, "ctrl-pldn"); argcount++;
        self->layout.ctrl_menu = XtCreateWidget("ctrl-menu", menuButtonWidgetClass, self->layout.menu_bar, arglist, argcount);
        XtManageChild(self->layout.ctrl_menu);
    }
    /* ctrl-pldn */ {
        argcount = 0;
        self->layout.ctrl_pldn = XtCreatePopupShell("ctrl-pldn", simpleMenuWidgetClass, self->layout.ctrl_menu, arglist, argcount);
    }
    /* pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Play / Pause")); argcount++;
        self->layout.pause_emu = XtCreateWidget("pause-emu", smeBSBObjectClass, self->layout.ctrl_pldn, arglist, argcount);
        XtAddCallback(self->layout.pause_emu, XtNcallback, (XtCallbackProc) OnPauseCbk, (XtPointer) self);
        XtManageChild(self->layout.pause_emu);
    }
    /* reset-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Reset")); argcount++;
        self->layout.reset_emu = XtCreateWidget("reset-emu", smeBSBObjectClass, self->layout.ctrl_pldn, arglist, argcount);
        XtAddCallback(self->layout.reset_emu, XtNcallback, (XtCallbackProc) OnResetCbk, (XtPointer) self);
        XtManageChild(self->layout.reset_emu);
    }
    /* help-menu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Help")); argcount++;
        XtSetArg(arglist[argcount], XtNmenuName, "help-pldn"); argcount++;
        self->layout.help_menu = XtCreateWidget("help-menu", menuButtonWidgetClass, self->layout.menu_bar, arglist, argcount);
        XtManageChild(self->layout.help_menu);
    }
    /* help-pldn */ {
        argcount = 0;
        self->layout.help_pldn = XtCreatePopupShell("help-pldn", simpleMenuWidgetClass, self->layout.help_menu, arglist, argcount);
    }
    /* legal-info */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("Legal Info")); argcount++;
        self->layout.legal_info = XtCreateWidget("legal-info", smeBSBObjectClass, self->layout.help_pldn, arglist, argcount);
        XtAddCallback(self->layout.legal_info, XtNcallback, (XtCallbackProc) OnLegalInfoCbk, (XtPointer) self);
        XtManageChild(self->layout.legal_info);
    }
    /* separator4 */ {
        argcount = 0;
        self->layout.separator4 = XtCreateWidget("separator4", smeLineObjectClass, self->layout.help_pldn, arglist, argcount);
        XtManageChild(self->layout.separator4);
    }
    /* about-xcpc */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, _("About Xcpc")); argcount++;
        self->layout.about_xcpc = XtCreateWidget("about-xcpc", smeBSBObjectClass, self->layout.help_pldn, arglist, argcount);
        XtAddCallback(self->layout.about_xcpc, XtNcallback, (XtCallbackProc) OnAboutXcpcCbk, (XtPointer) self);
        XtManageChild(self->layout.about_xcpc);
    }
    /* emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNemuContext    , self->machine             ); argcount++;
        XtSetArg(arglist[argcount], XtNemuCreateProc , &xcpc_machine_create_proc ); argcount++;
        XtSetArg(arglist[argcount], XtNemuDestroyProc, &xcpc_machine_destroy_proc); argcount++;
        XtSetArg(arglist[argcount], XtNemuRealizeProc, &xcpc_machine_realize_proc); argcount++;
        XtSetArg(arglist[argcount], XtNemuResizeProc , &xcpc_machine_resize_proc ); argcount++;
        XtSetArg(arglist[argcount], XtNemuExposeProc , &xcpc_machine_expose_proc ); argcount++;
        XtSetArg(arglist[argcount], XtNemuTimerProc  , &xcpc_machine_timer_proc  ); argcount++;
        XtSetArg(arglist[argcount], XtNemuInputProc  , &xcpc_machine_input_proc  ); argcount++;
        self->layout.emulator = XemCreateEmulator(self->layout.main_wnd, "emulator", arglist, argcount);
        XtManageChild(self->layout.emulator);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication* public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* XcpcApplicationInit(XcpcApplication* self, int* argc, char*** argv)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcApplicationRec));
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
        XtGetApplicationNameAndClass(XtDisplay(self->layout.toplevel), &self->resources.appname, &self->resources.appclass);
    }
    /* check command-line flags */ {
        if(self->resources.quiet_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_QUIET);
        }
        if(self->resources.trace_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_TRACE);
        }
        if(self->resources.debug_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_DEBUG);
        }
    }
    /* build user interface */ {
        (void) BuildLayout(self);
#if 0
        (void) Play(self);
#endif
    }
    return self;
}

XcpcApplication* XcpcApplicationMain(XcpcApplication* self)
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

XcpcApplication* XcpcApplicationFini(XcpcApplication* self)
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

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication
 * ---------------------------------------------------------------------------
 */

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplicationRec self;

    /* let's go */ {
        (void) XcpcApplicationInit(&self, argc, argv);
        (void) XcpcApplicationMain(&self);
        (void) XcpcApplicationFini(&self);
    }
    return EXIT_SUCCESS;
}
