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
#include <glib/gi18n.h>
#include <Xm/XmAll.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include "amstrad-cpc.h"
#include "xcpc-motif2-priv.h"

/*
 * ---------------------------------------------------------------------------
 * options
 * ---------------------------------------------------------------------------
 */

static XrmOptionDescRec options[] = {
    { "-quiet"  , ".xcpcQuietFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-trace"  , ".xcpcTraceFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-debug"  , ".xcpcDebugFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-version", ".xcpcAboutFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-help"   , ".xcpcUsageFlag", XrmoptionNoArg, (XPointer) "true" },
};

/*
 * ---------------------------------------------------------------------------
 * fallback resources
 * ---------------------------------------------------------------------------
 */

static String fallback_resources[] = {
    "*title: Xcpc - Amstrad CPC emulator",
    "*menu-bar*pixmapTextPadding: 8",
    "*shadowThickness: 1",
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
    /* xcpcAboutFlag */ {
        "xcpcAboutFlag", "XcpcAboutFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, about_flag),
        XmRImmediate, (XtPointer) FALSE
    },
    /* xcpcUsageFlag */ {
        "xcpcUsageFlag", "XcpcUsageFlag", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, usage_flag),
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
        char*   name;
        Pixmap* pixmap;
    } list[] = {
        { "xcpc-icon.png"      , &pixmaps->xcpc_icon       },
        { "xcpc-mask.png"      , &pixmaps->xcpc_mask       },
        { "eject.png"          , &pixmaps->eject           },
        { "empty.png"          , &pixmaps->empty           },
        { "folder-open.png"    , &pixmaps->folder_open     },
        { "info-circle.png"    , &pixmaps->info_circle     },
        { "pause.png"          , &pixmaps->pause           },
        { "play.png"           , &pixmaps->play            },
        { "power-off.png"      , &pixmaps->power_off       },
        { "question-circle.png", &pixmaps->question_circle },
        { "save.png"           , &pixmaps->save            },
        { "sync.png"           , &pixmaps->sync            },
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
            *list[index].pixmap = XmGetPixmap(screen, list[index].name, foreground, background);
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

    if(self->ctrl.pause_emulator != NULL) {
        XmString string = XmStringCreateLocalized(_("Pause"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string             ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.pause); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP_AND_STRING); ++argcount;
        XtSetValues(self->ctrl.pause_emulator, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    if(self->tool.pause_emulator != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.pause); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP           ); ++argcount;
        XtSetValues(self->tool.pause_emulator, arglist, argcount);
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

    if(self->ctrl.pause_emulator != NULL) {
        XmString string = XmStringCreateLocalized(_("Play"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string             ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.play ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP_AND_STRING); ++argcount;
        XtSetValues(self->ctrl.pause_emulator, arglist, argcount);
        string = (XmStringFree(string), NULL);
    }
    if(self->tool.pause_emulator != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.play ); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType  , XmPIXMAP           ); ++argcount;
        XtSetValues(self->tool.pause_emulator, arglist, argcount);
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
        amstrad_cpc_reset(&amstrad_cpc);
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
        amstrad_cpc_load_snapshot(&amstrad_cpc, filename);
    }
    return self;
}

static XcpcApplication SaveSnapshot(XcpcApplication self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        amstrad_cpc_save_snapshot(&amstrad_cpc, filename);
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
        amstrad_cpc_insert_drive0(&amstrad_cpc, filename);
    }
    return self;
}

static XcpcApplication RemoveDiskFromDrive0(XcpcApplication self)
{
    amstrad_cpc_remove_drive0(&amstrad_cpc);

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
        amstrad_cpc_insert_drive1(&amstrad_cpc, filename);
    }
    return self;
}

static XcpcApplication RemoveDiskFromDrive1(XcpcApplication self)
{
    amstrad_cpc_remove_drive1(&amstrad_cpc);

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
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-snapshot-load-dialog", arglist, argcount);
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
        dialog = XmCreateFileSelectionDialog(FindTopLevelShell(widget), "xcpc-snapshot-save-dialog", arglist, argcount);
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
        xcpc_debug("-------- 8< --------");
        xcpc_debug("XcpcApplication:");
        /* root */ {
            xcpc_debug("    method_name             : %s", method_name              );
            xcpc_debug("    appcontext              : %p", self->appcontext         );
        }
        /* layout */ {
            xcpc_debug("    layout:"                                                );
            xcpc_debug("        toplevel            : %p", self->layout.toplevel    );
            xcpc_debug("        main_wnd            : %p", self->layout.main_wnd    );
            xcpc_debug("        menu_bar            : %p", self->layout.menu_bar    );
            xcpc_debug("        frame               : %p", self->layout.frame       );
            xcpc_debug("        emulator            : %p", self->layout.emulator    );
        }
        /* file */ {
            xcpc_debug("    layout:"                                                );
            xcpc_debug("        file.menu           : %p", self->file.menu          );
            xcpc_debug("        file.pulldown       : %p", self->file.pulldown      );
            xcpc_debug("        file.snapshot_load  : %p", self->file.snapshot_load );
            xcpc_debug("        file.snapshot_save  : %p", self->file.snapshot_save );
            xcpc_debug("        file.separator1     : %p", self->file.separator1    );
            xcpc_debug("        file.exit           : %p", self->file.exit          );
        }
        /* drv0 */ {
            xcpc_debug("    drv0:"                                                  );
            xcpc_debug("        drv0.menu           : %p", self->drv0.menu          );
            xcpc_debug("        drv0.pulldown       : %p", self->drv0.pulldown      );
            xcpc_debug("        drv0.drive0_insert  : %p", self->drv0.drive0_insert );
            xcpc_debug("        drv0.drive0_remove  : %p", self->drv0.drive0_remove );
        }
        /* drv1 */ {
            xcpc_debug("    drv1:"                                                  );
            xcpc_debug("        drv1.menu           : %p", self->drv1.menu          );
            xcpc_debug("        drv1.pulldown       : %p", self->drv1.pulldown      );
            xcpc_debug("        drv1.drive1_insert  : %p", self->drv1.drive1_insert );
            xcpc_debug("        drv1.drive1_remove  : %p", self->drv1.drive1_remove );
        }
        /* ctrl */ {
            xcpc_debug("    ctrl:"                                                  );
            xcpc_debug("        ctrl.menu           : %p", self->ctrl.menu          );
            xcpc_debug("        ctrl.pulldown       : %p", self->ctrl.pulldown      );
            xcpc_debug("        ctrl.pause_emulator : %p", self->ctrl.pause_emulator);
            xcpc_debug("        ctrl.reset_emulator : %p", self->ctrl.reset_emulator);
        }
        /* help */ {
            xcpc_debug("    help:"                                                  );
            xcpc_debug("        help.menu           : %p", self->help.menu          );
            xcpc_debug("        help.pulldown       : %p", self->help.pulldown      );
            xcpc_debug("        help.legal_info     : %p", self->help.legal_info    );
            xcpc_debug("        help.separator1     : %p", self->help.separator1    );
            xcpc_debug("        help.about_xcpc     : %p", self->help.about_xcpc    );
        }
        /* tool */ {
            xcpc_debug("    tool:"                                                  );
            xcpc_debug("        tool.container      : %p", self->tool.container     );
            xcpc_debug("        tool.snapshot_load  : %p", self->tool.snapshot_load );
            xcpc_debug("        tool.snapshot_save  : %p", self->tool.snapshot_save );
            xcpc_debug("        tool.pause_emulator : %p", self->tool.pause_emulator);
            xcpc_debug("        tool.reset_emulator : %p", self->tool.reset_emulator);
        }
        xcpc_debug("-------- 8< --------");
    }
    return self;
}

static XcpcApplication PrintAbout(XcpcApplication self)
{
    FILE* stream = self->print_stream;

    if(stream != NULL) {
        (void) fprintf(stream, "%s %s\n", self->resources.appname, PACKAGE_VERSION);
        (void) fflush(stream);
    }
    return Exit(self);
}

static XcpcApplication PrintUsage(XcpcApplication self)
{
    FILE* stream = self->print_stream;

    if(stream != NULL) {
        (void) fprintf(stream, "%s %s\n", self->resources.appname, PACKAGE_VERSION);
        (void) fprintf(stream, "Usage: %s [toolkit-options] [program-options]\n\n", self->resources.appname);
        (void) fprintf(stream, "Options:\n");
        (void) fprintf(stream, "  -version  display version and exit.\n");
        (void) fprintf(stream, "  -help     display this help and exit.\n");
        (void) fprintf(stream, "\n");
        (void) fprintf(stream, "  -quiet    set loglevel to quiet mode.\n");
        (void) fprintf(stream, "  -trace    set loglevel to trace mode.\n");
        (void) fprintf(stream, "  -error    set loglevel to error mode.\n");
        (void) fflush(stream);
    }
    return Exit(self);
}

static XcpcApplication BuildLayout(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* main-wnd */ {
        argcount = 0;
        self->layout.main_wnd = XmCreateMainWindow(self->layout.toplevel, "main-wnd", arglist, argcount);
        XtAddCallback(self->layout.main_wnd, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.main_wnd);
        XtManageChild(self->layout.main_wnd);
    }
    /* menu-bar */ {
        argcount = 0;
        self->layout.menu_bar = XmCreateMenuBar(self->layout.main_wnd, "menu-bar", arglist, argcount);
        XtAddCallback(self->layout.menu_bar, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.menu_bar);
        XtManageChild(self->layout.menu_bar);
    }
    /* tool-bar */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNshadowType  , XmSHADOW_OUT); ++argcount;
        self->layout.tool_bar = XmCreateFrame(self->layout.main_wnd, "tool-bar", arglist, argcount);
        XtAddCallback(self->layout.tool_bar, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.tool_bar);
        XtManageChild(self->layout.tool_bar);
    }
    /* frame */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNshadowType  , XmSHADOW_OUT); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginWidth , 4           ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginHeight, 4           ); ++argcount;
        self->layout.frame = XmCreateFrame(self->layout.main_wnd, "frame", arglist, argcount);
        XtAddCallback(self->layout.frame, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.frame);
        XtManageChild(self->layout.frame);
    }
    /* emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNemuContext    , &amstrad_cpc             ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuCreateProc , &amstrad_cpc_create_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuDestroyProc, &amstrad_cpc_destroy_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuRealizeProc, &amstrad_cpc_realize_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNemuResizeProc , &amstrad_cpc_resize_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuRedrawProc , &amstrad_cpc_redraw_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuTimerProc  , &amstrad_cpc_timer_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNemuInputProc  , &amstrad_cpc_input_proc  ); ++argcount;
        self->layout.emulator = XemCreateEmulator(self->layout.frame, "emulator", arglist, argcount);
        XtAddCallback(self->layout.emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.emulator);
        XtManageChild(self->layout.emulator);
    }
    /* set main-window areas */ {
        XmMainWindowSetAreas ( self->layout.main_wnd
                             , self->layout.menu_bar
                             , self->layout.tool_bar
                             , NULL
                             , NULL
                             , self->layout.frame );
    }
    /* get pixmaps */ {
        GetPixmaps(self->layout.menu_bar, &self->pixmaps);
    }
    return self;
}

static XcpcApplication BuildFileMenu(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* file-pulldown */ {
        argcount = 0;
        self->file.pulldown = XmCreatePulldownMenu(self->layout.menu_bar, "file-pulldown", arglist, argcount);
        XtAddCallback(self->file.pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.pulldown);
    }
    /* file-snapshot-load */ {
        XmString string = XmStringCreateLocalized(_("Load snapshot..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.folder_open); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->file.snapshot_load = XmCreatePushButtonGadget(self->file.pulldown, "file-snapshot-load", arglist, argcount);
        XtAddCallback(self->file.snapshot_load, XmNactivateCallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->file.snapshot_load, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.snapshot_load);
        XtManageChild(self->file.snapshot_load);
        string = (XmStringFree(string), NULL);
    }
    /* file-snapshot-save */ {
        XmString string = XmStringCreateLocalized(_("Save snapshot as..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->file.snapshot_save = XmCreatePushButtonGadget(self->file.pulldown, "file-snapshot-save", arglist, argcount);
        XtAddCallback(self->file.snapshot_save, XmNactivateCallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->file.snapshot_save, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.snapshot_save);
        XtManageChild(self->file.snapshot_save);
        string = (XmStringFree(string), NULL);
    }
    /* file-separator1 */ {
        argcount = 0;
        self->file.separator1 = XmCreateSeparatorGadget(self->file.pulldown, "file-separator1", arglist, argcount);
        XtAddCallback(self->file.separator1, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.separator1);
        XtManageChild(self->file.separator1);
    }
    /* file-exit */ {
        XmString string = XmStringCreateLocalized(_("Exit"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.power_off); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->file.exit = XmCreatePushButtonGadget(self->file.pulldown, "file-exit", arglist, argcount);
        XtAddCallback(self->file.exit, XmNactivateCallback, (XtCallbackProc) &ExitCallback, (XtPointer) self);
        XtAddCallback(self->file.exit, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.exit);
        XtManageChild(self->file.exit);
        string = (XmStringFree(string), NULL);
    }
    /* file-menu */ {
        XmString string = XmStringCreateLocalized(_("File"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, self->file.pulldown); ++argcount;
        self->file.menu = XmCreateCascadeButtonGadget(self->layout.menu_bar, "file-menu", arglist, argcount);
        XtAddCallback(self->file.menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->file.menu);
        XtManageChild(self->file.menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication BuildCtrlMenu(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* ctrl-pulldown */ {
        argcount = 0;
        self->ctrl.pulldown = XmCreatePulldownMenu(self->layout.menu_bar, "ctrl-pulldown", arglist, argcount);
        XtAddCallback(self->ctrl.pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->ctrl.pulldown);
    }
    /* ctrl-pause-emu */ {
        XmString string = XmStringCreateLocalized(_("Play / Pause"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.empty); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->ctrl.pause_emulator = XmCreatePushButtonGadget(self->ctrl.pulldown, "ctrl-pause-emu", arglist, argcount);
        XtAddCallback(self->ctrl.pause_emulator, XmNactivateCallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(self->ctrl.pause_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->ctrl.pause_emulator);
        XtManageChild(self->ctrl.pause_emulator);
        string = (XmStringFree(string), NULL);
    }
    /* ctrl-reset-emu */ {
        XmString string = XmStringCreateLocalized(_("Reset"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.sync); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->ctrl.reset_emulator = XmCreatePushButtonGadget(self->ctrl.pulldown, "ctrl-reset-emu", arglist, argcount);
        XtAddCallback(self->ctrl.reset_emulator, XmNactivateCallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(self->ctrl.reset_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->ctrl.reset_emulator);
        XtManageChild(self->ctrl.reset_emulator);
        string = (XmStringFree(string), NULL);
    }
    /* ctrl-menu */ {
        XmString string = XmStringCreateLocalized(_("Controls"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, self->ctrl.pulldown); ++argcount;
        self->ctrl.menu = XmCreateCascadeButtonGadget(self->layout.menu_bar, "ctrl-menu", arglist, argcount);
        XtAddCallback(self->ctrl.menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->ctrl.menu);
        XtManageChild(self->ctrl.menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication BuildDrv0Menu(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv0-pulldown */ {
        argcount = 0;
        self->drv0.pulldown = XmCreatePulldownMenu(self->layout.menu_bar, "drv0-pulldown", arglist, argcount);
        XtAddCallback(self->drv0.pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv0.pulldown);
    }
    /* drv0-drive0-insert */ {
        XmString string = XmStringCreateLocalized(_("Insert disk..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->drv0.drive0_insert = XmCreatePushButtonGadget(self->drv0.pulldown, "drv0-drive0-insert", arglist, argcount);
        XtAddCallback(self->drv0.drive0_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive0Callback, (XtPointer) self);
        XtAddCallback(self->drv0.drive0_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv0.drive0_insert);
        XtManageChild(self->drv0.drive0_insert);
        string = (XmStringFree(string), NULL);
    }
    /* drv0-drive0-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.eject); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->drv0.drive0_remove = XmCreatePushButtonGadget(self->drv0.pulldown, "drv0-drive0-remove", arglist, argcount);
        XtAddCallback(self->drv0.drive0_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive0Callback, (XtPointer) self);
        XtAddCallback(self->drv0.drive0_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv0.drive0_remove);
        XtManageChild(self->drv0.drive0_remove);
        string = (XmStringFree(string), NULL);
    }
    /* drv0-menu */ {
        XmString string = XmStringCreateLocalized(_("Drive A"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, self->drv0.pulldown); ++argcount;
        self->drv0.menu = XmCreateCascadeButtonGadget(self->layout.menu_bar, "drv0-menu", arglist, argcount);
        XtAddCallback(self->drv0.menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv0.menu);
        XtManageChild(self->drv0.menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication BuildDrv1Menu(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* drv1-pulldown */ {
        argcount = 0;
        self->drv1.pulldown = XmCreatePulldownMenu(self->layout.menu_bar, "drv1-pulldown", arglist, argcount);
        XtAddCallback(self->drv1.pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv1.pulldown);
    }
    /* drv1-drive1-insert */ {
        XmString string = XmStringCreateLocalized(_("Insert disk..."));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->drv1.drive1_insert = XmCreatePushButtonGadget(self->drv1.pulldown, "drv1-drive1-insert", arglist, argcount);
        XtAddCallback(self->drv1.drive1_insert, XmNactivateCallback, (XtCallbackProc) &InsertDrive1Callback, (XtPointer) self);
        XtAddCallback(self->drv1.drive1_insert, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv1.drive1_insert);
        XtManageChild(self->drv1.drive1_insert);
        string = (XmStringFree(string), NULL);
    }
    /* drv1-drive1-remove */ {
        XmString string = XmStringCreateLocalized(_("Remove disk"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.eject); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->drv1.drive1_remove = XmCreatePushButtonGadget(self->drv1.pulldown, "drv1-drive1-remove", arglist, argcount);
        XtAddCallback(self->drv1.drive1_remove, XmNactivateCallback, (XtCallbackProc) &RemoveDrive1Callback, (XtPointer) self);
        XtAddCallback(self->drv1.drive1_remove, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv1.drive1_remove);
        XtManageChild(self->drv1.drive1_remove);
        string = (XmStringFree(string), NULL);
    }
    /* drv1-menu */ {
        XmString string = XmStringCreateLocalized(_("Drive B"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, self->drv1.pulldown); ++argcount;
        self->drv1.menu = XmCreateCascadeButtonGadget(self->layout.menu_bar, "drv1-menu", arglist, argcount);
        XtAddCallback(self->drv1.menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->drv1.menu);
        XtManageChild(self->drv1.menu);
        string = (XmStringFree(string), NULL);
    }
    return self;
}

static XcpcApplication BuildHelpMenu(XcpcApplication self)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* help-pulldown */ {
        argcount = 0;
        self->help.pulldown = XmCreatePulldownMenu(self->layout.menu_bar, "help-pulldown", arglist, argcount);
        XtAddCallback(self->help.pulldown, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->help.pulldown);
    }
    /* help-legal-info */ {
        XmString string = XmStringCreateLocalized(_("Legal Info"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.info_circle); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->help.legal_info = XmCreatePushButtonGadget(self->help.pulldown, "help-legal-info", arglist, argcount);
        XtAddCallback(self->help.legal_info, XmNactivateCallback, (XtCallbackProc) &LegalCallback, (XtPointer) self);
        XtAddCallback(self->help.legal_info, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->help.legal_info);
        XtManageChild(self->help.legal_info);
        string = (XmStringFree(string), NULL);
    }
    /* help-separator1 */ {
        argcount = 0;
        self->help.separator1 = XmCreateSeparatorGadget(self->help.pulldown, "help-separator1", arglist, argcount);
        XtAddCallback(self->help.separator1, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->help.separator1);
        XtManageChild(self->help.separator1);
    }
    /* help-about-xcpc */ {
        XmString string = XmStringCreateLocalized(_("About Xcpc"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelPixmap, self->pixmaps.question_circle); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType, XmPIXMAP_AND_STRING); ++argcount;
        self->help.about_xcpc = XmCreatePushButtonGadget(self->help.pulldown, "help-about-xcpc", arglist, argcount);
        XtAddCallback(self->help.about_xcpc, XmNactivateCallback, (XtCallbackProc) &AboutCallback, (XtPointer) self);
        XtAddCallback(self->help.about_xcpc, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->help.about_xcpc);
        XtManageChild(self->help.about_xcpc);
        string = (XmStringFree(string), NULL);
    }
    /* help-menu */ {
        XmString string = XmStringCreateLocalized(_("Help"));
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelString, string); ++argcount;
        XtSetArg(arglist[argcount], XmNsubMenuId, self->help.pulldown); ++argcount;
        self->help.menu = XmCreateCascadeButtonGadget(self->layout.menu_bar, "help-menu", arglist, argcount);
        XtAddCallback(self->help.menu, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->help.menu);
        XtManageChild(self->help.menu);
        string = (XmStringFree(string), NULL);
    }
    /* set the menu-bar help-widget */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNmenuHelpWidget, self->help.menu); ++argcount;
        XtSetValues(self->layout.menu_bar, arglist, argcount);
    }
    return self;
}

static XcpcApplication BuildMenuBar(XcpcApplication self)
{
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
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* tool-bar */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNorientation, XmHORIZONTAL); ++argcount;
        XtSetArg(arglist[argcount], XmNtraversalOn, FALSE       ); ++argcount;
        self->tool.container = XmCreateRowColumn(self->layout.tool_bar, "tool-row", arglist, argcount);
        XtAddCallback(self->tool.container, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->tool.container);
        XtManageChild(self->tool.container);
    }
    /* tool-snapshot-load */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap    , self->pixmaps.folder_open); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType      , XmPIXMAP                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginWidth    , 4                        ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginHeight   , 4                        ); ++argcount;
        XtSetArg(arglist[argcount], XmNshadowThickness, 0                        ); ++argcount;
        self->tool.snapshot_load = XmCreatePushButtonGadget(self->tool.container, "tool-snapshot-load", arglist, argcount);
        XtAddCallback(self->tool.snapshot_load, XmNactivateCallback, (XtCallbackProc) &LoadSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->tool.snapshot_load, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->tool.snapshot_load);
        XtManageChild(self->tool.snapshot_load);
    }
    /* tool-snapshot-save */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap    , self->pixmaps.save); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType      , XmPIXMAP          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginWidth    , 4                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginHeight   , 4                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNshadowThickness, 0                 ); ++argcount;
        self->tool.snapshot_save = XmCreatePushButtonGadget(self->tool.container, "tool-snapshot-save", arglist, argcount);
        XtAddCallback(self->tool.snapshot_save, XmNactivateCallback, (XtCallbackProc) &SaveSnapshotCallback, (XtPointer) self);
        XtAddCallback(self->tool.snapshot_save, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->tool.snapshot_save);
        XtManageChild(self->tool.snapshot_save);
    }
    /* tool-pause-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap    , self->pixmaps.empty); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType      , XmPIXMAP           ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginWidth    , 4                  ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginHeight   , 4                  ); ++argcount;
        XtSetArg(arglist[argcount], XmNshadowThickness, 0                  ); ++argcount;
        self->tool.pause_emulator = XmCreatePushButtonGadget(self->tool.container, "tool-pause-emu", arglist, argcount);
        XtAddCallback(self->tool.pause_emulator, XmNactivateCallback, (XtCallbackProc) &PauseCallback, (XtPointer) self);
        XtAddCallback(self->tool.pause_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->tool.pause_emulator);
        XtManageChild(self->tool.pause_emulator);
    }
    /* tool-reset-emu */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XmNlabelPixmap    , self->pixmaps.sync); ++argcount;
        XtSetArg(arglist[argcount], XmNlabelType      , XmPIXMAP          ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginWidth    , 4                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNmarginHeight   , 4                 ); ++argcount;
        XtSetArg(arglist[argcount], XmNshadowThickness, 0                 ); ++argcount;
        self->tool.reset_emulator = XmCreatePushButtonGadget(self->tool.container, "tool-reset-emu", arglist, argcount);
        XtAddCallback(self->tool.reset_emulator, XmNactivateCallback, (XtCallbackProc) &ResetCallback, (XtPointer) self);
        XtAddCallback(self->tool.reset_emulator, XmNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->tool.reset_emulator);
        XtManageChild(self->tool.reset_emulator);
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
    /* initialize libxcpc */ {
        xcpc_begin();
    }
    /* check command-line flags */ {
        if(self->resources.about_flag != FALSE) {
            return PrintAbout(self);
        }
        if(self->resources.usage_flag != FALSE) {
            return PrintUsage(self);
        }
        if(amstrad_cpc_parse(argc, argv) == EXIT_FAILURE) {
            return PrintUsage(self);
        }
    }
    /* build user interface */ {
        (void) BuildLayout(self);
        (void) BuildMenuBar(self);
        (void) BuildToolBar(self);
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
    /* finalize libxcpc */ {
        xcpc_end();
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

int xcpc(int* argc, char*** argv)
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
