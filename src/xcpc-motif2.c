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
#include <Xm/XmAll.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include "xcpc-motif2-priv.h"

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
    "Xcpc*main-window.shadowThickness: 0",
    "Xcpc*main-window.menubar.shadowThickness: 0",
    "Xcpc*main-window.menubar*pixmapTextPadding: 8",
    "Xcpc*main-window.toolbar.shadowThickness: 0",
    "Xcpc*main-window.toolbar.marginWidth: 4",
    "Xcpc*main-window.toolbar.marginHeight: 4",
    "Xcpc*shadowThickness: 1",
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
        XmRImmediate, (XtPointer) FALSE
    },
    /* xcpcTraceFlag */ {
        "xcpcTraceFlag", "XcpcTraceFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, trace_flag),
        XmRImmediate, (XtPointer) FALSE
    },
    /* xcpcDebugFlag */ {
        "xcpcDebugFlag", "XcpcDebugFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, debug_flag),
        XmRImmediate, (XtPointer) FALSE
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

static XcpcApplication SetTitle(XcpcApplication self, const char* title)
{
    Arg      arglist[4];
    Cardinal argcount = 0;

    if(self->layout.toplevel != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNtitle     , title                  ); ++argcount;
        XtSetArg(arglist[argcount], XmNiconPixmap, self->pixmaps.xcpc_icon); ++argcount;
        XtSetArg(arglist[argcount], XmNiconMask  , self->pixmaps.xcpc_mask); ++argcount;
        XtSetValues(self->layout.toplevel, arglist, argcount);
    }
    return self;
}

static XcpcApplication Exit(XcpcApplication self)
{
    if(self->appcontext != NULL) {
        XtAppSetExitFlag(self->appcontext);
    }
    return self;
}

static XcpcApplication Play(XcpcApplication self)
{
    Arg      arglist[4];
    Cardinal argcount = 0;

    if(self->menubar.ctrl.pause_emulator != NULL) {
        XmString string = XmStringCreateLocalized(_("Pause"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP_AND_STRING     ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_pause); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelString, string                  ); ++argcount;
        XtSetValues(self->menubar.ctrl.pause_emulator, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    if(self->toolbar.pause_emulator != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP                ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_pause); ++argcount;
        XtSetValues(self->toolbar.pause_emulator, arglist, argcount);
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, TRUE);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    return SetTitle(self, _("Xcpc - Amstrad CPC emulator - Playing"));
}

static XcpcApplication Pause(XcpcApplication self)
{
    Arg      arglist[4];
    Cardinal argcount = 0;

    if(self->menubar.ctrl.pause_emulator != NULL) {
        XmString string = XmStringCreateLocalized(_("Play"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP_AND_STRING    ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_play); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelString, string                 ); ++argcount;
        XtSetValues(self->menubar.ctrl.pause_emulator, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    if(self->toolbar.pause_emulator != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP               ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.ctrl_play); ++argcount;
        XtSetValues(self->toolbar.pause_emulator, arglist, argcount);
    }
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, FALSE);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    return SetTitle(self, _("Xcpc - Amstrad CPC emulator - Paused"));
}

static XcpcApplication Reset(XcpcApplication self)
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

static XcpcApplication LoadSnapshot(XcpcApplication self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_load_snapshot(self->machine, filename);
    }
    return self;
}

static XcpcApplication SaveSnapshot(XcpcApplication self, const char* filename)
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

static XcpcApplication InsertDiskIntoDrive0(XcpcApplication self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive0(self->machine, filename);
    }
    return self;
}

static XcpcApplication RemoveDiskFromDrive0(XcpcApplication self)
{
    xcpc_machine_remove_drive0(self->machine);

    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Drive1
 * ---------------------------------------------------------------------------
 */

static XcpcApplication InsertDiskIntoDrive1(XcpcApplication self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive1(self->machine, filename);
    }
    return self;
}

static XcpcApplication RemoveDiskFromDrive1(XcpcApplication self)
{
    xcpc_machine_remove_drive1(self->machine);

    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Drive by default
 * ---------------------------------------------------------------------------
 */

static XcpcApplication InsertOrRemoveDisk(XcpcApplication self, const char* filename)
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

static void DismissCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    if(info != NULL) {
        XtDestroyWidget(FindShell(widget));
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

static void DropUriCallback(Widget widget, XcpcApplication self, char* uri)
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
                (void) LoadSnapshot(self, str);
                (void) Play(self);
            }
            if(strcmp(&str[length - 4], ".dsk") == 0) {
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

static void LoadSnapshotOkCallback(Widget widget, XcpcApplication self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
        (void) LoadSnapshot(self, filename);
        (void) Play(self);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XmAnyCallbackStruct *) info);
}

static void LoadSnapshotCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
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
    (void) Pause(self);
}

/*
 * ---------------------------------------------------------------------------
 * Snapshot save callbacks
 * ---------------------------------------------------------------------------
 */

static void SaveSnapshotOkCallback(Widget widget, XcpcApplication self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
        (void) SaveSnapshot(self, filename);
        (void) Play(self);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XmAnyCallbackStruct *) info);
}

static void SaveSnapshotCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
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
    (void) Pause(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive0 callbacks
 * ---------------------------------------------------------------------------
 */

static void InsertDrive0OkCallback(Widget widget, XcpcApplication self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
        (void) InsertDiskIntoDrive0(self, filename);
        (void) Play(self);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XmAnyCallbackStruct *) info);
}

static void InsertDrive0Callback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
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
    (void) Pause(self);
}

static void RemoveDrive0Callback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    (void) RemoveDiskFromDrive0(self);
    (void) Play(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive1 callbacks
 * ---------------------------------------------------------------------------
 */

static void InsertDrive1OkCallback(Widget widget, XcpcApplication self, XmFileSelectionBoxCallbackStruct* info)
{
    char* filename = NULL;

    if(XmStringGetLtoR(info->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
        (void) InsertDiskIntoDrive1(self, filename);
        (void) Play(self);
    }
    if(filename != NULL) {
        filename = (XtFree(filename), NULL);
    }
    DismissCallback(widget, self, (XmAnyCallbackStruct *) info);
}

static void InsertDrive1Callback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
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
    (void) Pause(self);
}

static void RemoveDrive1Callback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    (void) RemoveDiskFromDrive1(self);
    (void) Play(self);
}

/*
 * ---------------------------------------------------------------------------
 * Miscellaneous callbacks
 * ---------------------------------------------------------------------------
 */

static void ExitCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    (void) Exit(self);
}

/*
 * ---------------------------------------------------------------------------
 * Controls callbacks
 * ---------------------------------------------------------------------------
 */

static void PauseCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    if(XtIsSensitive(self->layout.emulator) != FALSE) {
        (void) Pause(self);
    }
    else {
        (void) Play(self);
    }
}

static void ResetCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    (void) Reset(self);
    (void) Play(self);
}

/*
 * ---------------------------------------------------------------------------
 * Legal callbacks
 * ---------------------------------------------------------------------------
 */

static void LegalCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    XmString title = XmStringCreateLocalized(_("Legal Info ..."));
    XmString message = XmStringCreateLocalized(_(
        "Amstrad has kindly given it's permission for it's copyrighted\n"
        "material to be redistributed but Amstrad retains it's copyright.\n\n"
        "Some of the Amstrad CPC ROM code is copyright Locomotive Software.\n\n"
        "ROM and DISK images are protected under the copyrights of their authors,\n"
        "and cannot be distributed in this package. You can download and/or use\n"
        "ROM and DISK images at your own risk and responsibility."
    ));

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
    (void) Pause(self);
}

/*
 * ---------------------------------------------------------------------------
 * About callbacks
 * ---------------------------------------------------------------------------
 */

static void AboutCallback(Widget widget, XcpcApplication self, XmAnyCallbackStruct* info)
{
    Arg      arglist[16];
    Cardinal argcount = 0;
    XmString title = XmStringCreateLocalized(_("About Xcpc ..."));
    XmString message = XmStringCreateLocalized(_(
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
    ));

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
    (void) Pause(self);
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static XcpcApplication DebugInstance(XcpcApplication self, const char* method_name)
{
    if(self->resources.debug_flag != FALSE) {
        xcpc_log_debug("-------- 8< --------");
        xcpc_log_debug("XcpcApplication:");
        /* root */ {
            xcpc_log_debug("    method_name             : %s", method_name                      );
            xcpc_log_debug("    appcontext              : %p", self->appcontext                 );
        }
        /* layout */ {
            xcpc_log_debug("    layout:"                                                        );
            xcpc_log_debug("        toplevel            : %p", self->layout.toplevel            );
            xcpc_log_debug("        window              : %p", self->layout.window              );
            xcpc_log_debug("        emulator            : %p", self->layout.emulator            );
        }
        /* file */ {
            xcpc_log_debug("    menubar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->menubar.widget             );
            xcpc_log_debug("        file.menu           : %p", self->menubar.file.menu          );
            xcpc_log_debug("        file.pulldown       : %p", self->menubar.file.pulldown      );
            xcpc_log_debug("        file.load_snapshot  : %p", self->menubar.file.load_snapshot );
            xcpc_log_debug("        file.save_snapshot  : %p", self->menubar.file.save_snapshot );
            xcpc_log_debug("        file.separator1     : %p", self->menubar.file.separator1    );
            xcpc_log_debug("        file.exit           : %p", self->menubar.file.exit          );
        }
        /* ctrl */ {
            xcpc_log_debug("    menubar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->menubar.widget             );
            xcpc_log_debug("        ctrl.menu           : %p", self->menubar.ctrl.menu          );
            xcpc_log_debug("        ctrl.pulldown       : %p", self->menubar.ctrl.pulldown      );
            xcpc_log_debug("        ctrl.pause_emulator : %p", self->menubar.ctrl.pause_emulator);
            xcpc_log_debug("        ctrl.reset_emulator : %p", self->menubar.ctrl.reset_emulator);
        }
        /* drv0 */ {
            xcpc_log_debug("    menubar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->menubar.widget             );
            xcpc_log_debug("        drv0.menu           : %p", self->menubar.drv0.menu          );
            xcpc_log_debug("        drv0.pulldown       : %p", self->menubar.drv0.pulldown      );
            xcpc_log_debug("        drv0.drive0_insert  : %p", self->menubar.drv0.drive0_insert );
            xcpc_log_debug("        drv0.drive0_remove  : %p", self->menubar.drv0.drive0_remove );
        }
        /* drv1 */ {
            xcpc_log_debug("    menubar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->menubar.widget             );
            xcpc_log_debug("        drv1.menu           : %p", self->menubar.drv1.menu          );
            xcpc_log_debug("        drv1.pulldown       : %p", self->menubar.drv1.pulldown      );
            xcpc_log_debug("        drv1.drive1_insert  : %p", self->menubar.drv1.drive1_insert );
            xcpc_log_debug("        drv1.drive1_remove  : %p", self->menubar.drv1.drive1_remove );
        }
        /* help */ {
            xcpc_log_debug("    menubar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->menubar.widget             );
            xcpc_log_debug("        help.menu           : %p", self->menubar.help.menu          );
            xcpc_log_debug("        help.pulldown       : %p", self->menubar.help.pulldown      );
            xcpc_log_debug("        help.legal          : %p", self->menubar.help.legal         );
            xcpc_log_debug("        help.separator1     : %p", self->menubar.help.separator1    );
            xcpc_log_debug("        help.about          : %p", self->menubar.help.about         );
        }
        /* tool */ {
            xcpc_log_debug("    toolbar:"                                                       );
            xcpc_log_debug("        widget              : %p", self->toolbar.widget             );
            xcpc_log_debug("        tool.load_snapshot  : %p", self->toolbar.load_snapshot      );
            xcpc_log_debug("        tool.save_snapshot  : %p", self->toolbar.save_snapshot      );
            xcpc_log_debug("        tool.pause_emulator : %p", self->toolbar.pause_emulator     );
            xcpc_log_debug("        tool.reset_emulator : %p", self->toolbar.reset_emulator     );
        }
        xcpc_log_debug("-------- 8< --------");
    }
    return self;
}

static XcpcApplication BuildFileMenu(XcpcApplication self)
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
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_load); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->load_snapshot = XmCreatePushButtonGadget(menu->pulldown, "file-load-snapshot", arglist, argcount);
        XtAddCallback(menu->load_snapshot, XmNactivateCallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->load_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->load_snapshot);
        XtManageChild(menu->load_snapshot);
        string = (XmStringFree(string), NULL);
    }
    /* file-save-snapshot */ {
        XmString string = XmStringCreateLocalized(_("Save snapshot as..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.file_save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->save_snapshot = XmCreatePushButtonGadget(menu->pulldown, "file-save-snapshot", arglist, argcount);
        XtAddCallback(menu->save_snapshot, XmNactivateCallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(menu->save_snapshot, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->save_snapshot);
        XtManageChild(menu->save_snapshot);
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

static XcpcApplication BuildCtrlMenu(XcpcApplication self)
{
    XcpcCtrlMenuRec* menu = &self->menubar.ctrl;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* ctrl-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "ctrl-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
    }
    /* ctrl-pause-emu */ {
        XmString string = XmStringCreateLocalized(_("Play / Pause"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.null_icon); ++argcount;
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

static XcpcApplication BuildDrv0Menu(XcpcApplication self)
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
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_insert); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->drive0_insert = XmCreatePushButtonGadget(menu->pulldown, "drv0-drive0-insert", arglist, argcount);
        XtAddCallback(menu->drive0_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_insert);
        XtManageChild(menu->drive0_insert);
        string = (XmStringFree(string), NULL);
    }
    /* drv0-drive0-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_remove); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->drive0_remove = XmCreatePushButtonGadget(menu->pulldown, "drv0-drive0-remove", arglist, argcount);
        XtAddCallback(menu->drive0_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive0Callback, (XtPointer) self);
        XtAddCallback(menu->drive0_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive0_remove);
        XtManageChild(menu->drive0_remove);
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

static XcpcApplication BuildDrv1Menu(XcpcApplication self)
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
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_insert); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->drive1_insert = XmCreatePushButtonGadget(menu->pulldown, "drv1-drive1-insert", arglist, argcount);
        XtAddCallback(menu->drive1_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_insert);
        XtManageChild(menu->drive1_insert);
        string = (XmStringFree(string), NULL);
    }
    /* drv1-drive1-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.disk_remove); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        menu->drive1_remove = XmCreatePushButtonGadget(menu->pulldown, "drv1-drive1-remove", arglist, argcount);
        XtAddCallback(menu->drive1_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive1Callback, (XtPointer) self);
        XtAddCallback(menu->drive1_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->drive1_remove);
        XtManageChild(menu->drive1_remove);
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

static XcpcApplication BuildHelpMenu(XcpcApplication self)
{
    XcpcHelpMenuRec* menu = &self->menubar.help;
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* help-pulldown */ {
        argcount = 0;
        menu->pulldown = XmCreatePulldownMenu(self->menubar.widget, "help-pulldown", arglist, argcount);
        XtAddCallback(menu->pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &menu->pulldown);
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

static XcpcApplication BuildMenuBar(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* menubar */ {
        argcount = 0;
        self->menubar.widget = XmCreateMenuBar(self->layout.window, "menubar", arglist, argcount);
        XtAddCallback(self->menubar.widget, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->menubar.widget);
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

static XcpcApplication BuildToolBar(XcpcApplication self)
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
    /* tool-pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP               ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.null_icon); ++argcount;
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

static XcpcApplication BuildLayout(XcpcApplication self)
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
        self->layout.emulator = XemCreateEmulator(self->layout.window, "emulator", arglist, argcount);
        XtAddCallback(self->layout.emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.emulator);
        XtManageChild(self->layout.emulator);
    }
    /* set main-window areas */ {
        XmMainWindowSetAreas ( self->layout.window
                             , self->menubar.widget
                             , self->toolbar.widget
                             , NULL
                             , NULL
                             , self->layout.emulator );
    }
    /* get pixmaps */ {
        GetPixmaps(self->menubar.widget, &self->pixmaps);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication XcpcApplicationInit(XcpcApplication self, int* argc, char*** argv)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcApplicationRec));
    }
    /* setup streams */ {
        self->input_stream = stdin;
        self->print_stream = stdout;
        self->error_stream = stderr;
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
        XtSetArg(arglist[argcount], XmNmappedWhenManaged, TRUE        ); ++argcount;
        XtSetArg(arglist[argcount], XmNallowShellResize , TRUE        ); ++argcount;
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
        (void) Play(self);
    }
    return DebugInstance(self, "XcpcApplicationInit()");
}

XcpcApplication XcpcApplicationMain(XcpcApplication self)
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
    return DebugInstance(self, "XcpcApplicationMain()");
}

XcpcApplication XcpcApplicationFini(XcpcApplication self)
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
    return DebugInstance(self, "XcpcApplicationFini()");
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication
 * ---------------------------------------------------------------------------
 */

static void setup(void)
{
    static char XAPPLRESDIR[PATH_MAX + 1] = { '\0' };

    /* XAPPLRESDIR */ {
        const char* env_var = "XAPPLRESDIR";
        const char* env_val = getenv(env_var);
        const char* env_dfl = XCPC_RESDIR;
        if((env_val == NULL) || (*env_val == '\0')) {
            (void) snprintf(XAPPLRESDIR, sizeof(XAPPLRESDIR), "%s=%s", env_var, env_dfl);
            (void) putenv(XAPPLRESDIR);
        }
    }
}

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplicationRec self;

    /* setup */ {
        setup();
    }
    /* let's go */ {
        (void) XcpcApplicationInit(&self, argc, argv);
        (void) XcpcApplicationMain(&self);
        (void) XcpcApplicationFini(&self);
    }
    return EXIT_SUCCESS;
}
