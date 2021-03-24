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
#include <glib/gi18n.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Dialog.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include "amstrad-cpc.h"
#include "xcpc-athena-priv.h"

/*
 * command line options
 */
static XrmOptionDescRec options[] = {
  { "-quiet"  , ".xcpcQuietFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-trace"  , ".xcpcTraceFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-debug"  , ".xcpcDebugFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-version", ".xcpcAboutFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-help"   , ".xcpcUsageFlag", XrmoptionNoArg, (XPointer) "true" },
};

/*
 * fallback resources
 */
static String fallback_resources[] = {
  "*title: Xcpc - Amstrad CPC emulator",
  NULL
};

/*
 * application resources
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
  /* xcpcAboutFlag */ {
    "xcpcAboutFlag", "XcpcAboutFlag", XtRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, about_flag),
    XtRImmediate, (XtPointer) FALSE
  },
  /* xcpcUsageFlag */ {
    "xcpcUsageFlag", "XcpcUsageFlag", XtRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, usage_flag),
    XtRImmediate, (XtPointer) FALSE
  },
};

/*
 * Xcpc resources
 */
static XcpcResourcesRec xcpc_resources = {
  FALSE, /* quiet_flag */
  FALSE, /* trace_flag */
  FALSE, /* debug_flag */
  FALSE, /* about_flag */
  FALSE, /* usage_flag */
};

/**
 * Destroy Callback
 *
 * @param widget specifies the Widget instance
 * @param widref specifies the Widget reference
 * @param cbdata is not used
 */
static void DestroyCbk(Widget widget, Widget *widref, XtPointer cbdata)
{
  if(XtIsApplicationShell(widget) != FALSE) {
    XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
  }
  if((widget != NULL) && (widref != NULL) && (widget == *widref)) {
    *widref = NULL;
  }
}

/**
 * XcpcApplication::OnCloseCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnCloseCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  XtPopdown(widget);
  XtDestroyWidget(widget);
  XtSetSensitive(gui->emulator, TRUE);
}

/**
 * XcpcApplication::OnLoadSnapshotOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotOkCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if(value != NULL) {
    amstrad_cpc_load_snapshot(&amstrad_cpc, value);
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * XcpcApplication::OnLoadSnapshotCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  Widget shell, dialog, btn_ok, cancel;
  Arg arglist[8];
  Cardinal argcount;

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnLoadSnapshotOkCbk, (XtPointer) gui);
  XtManageChild(btn_ok);
  /* load-snapshot-cancel */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
  cancel = XtCreateManagedWidget("load-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(cancel, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(cancel);
  /* load-snapshot-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnSaveSnapshotOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotOkCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if(value != NULL) {
    amstrad_cpc_save_snapshot(&amstrad_cpc, value);
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * XcpcApplication::OnSaveSnapshotCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  Widget shell, dialog, btn_ok, cancel;
  Arg arglist[8];
  Cardinal argcount;

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnSaveSnapshotOkCbk, (XtPointer) gui);
  XtManageChild(btn_ok);
  /* save-snapshot-cancel */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
  cancel = XtCreateManagedWidget("save-snapshot-cancel", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(cancel, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(cancel);
  /* save-snapshot-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnDriveAInsertOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveAInsertOkCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if((value != NULL) && (*value != '\0')) {
    amstrad_cpc_insert_drive0(&amstrad_cpc, value);
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * XcpcApplication::OnDriveAInsertCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveAInsertCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  Widget shell, dialog, btn_ok, cancel;
  Arg arglist[8];
  Cardinal argcount;

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnDriveAInsertOkCbk, (XtPointer) gui);
  XtManageChild(btn_ok);
  /* drivea-insert-cancel */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
  cancel = XtCreateManagedWidget("drivea-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(cancel, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(cancel);
  /* drivea-insert-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnDriveAEjectCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveAEjectCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  amstrad_cpc_remove_drive0(&amstrad_cpc);
}

/**
 * XcpcApplication::OnDriveBInsertOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveBInsertOkCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if((value != NULL) && (*value != '\0')) {
    amstrad_cpc_insert_drive1(&amstrad_cpc, value);
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * XcpcApplication::OnDriveBInsertCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveBInsertCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  Widget shell, dialog, btn_ok, cancel;
  Arg arglist[8];
  Cardinal argcount;

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(btn_ok, XtNcallback, (XtCallbackProc) OnDriveBInsertOkCbk, (XtPointer) gui);
  XtManageChild(btn_ok);
  /* driveb-insert-cancel */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Cancel")); argcount++;
  cancel = XtCreateManagedWidget("driveb-insert-cancel", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(cancel, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(cancel);
  /* driveb-insert-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnDriveBEjectCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnDriveBEjectCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  amstrad_cpc_remove_drive1(&amstrad_cpc);
}

/**
 * XcpcApplication::OnExitEmulatorCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnExitEmulatorCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
}

/**
 * XcpcApplication::OnPauseCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnPauseCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  if(XtIsSensitive(gui->emulator) != FALSE) {
    XtSetSensitive(gui->emulator, FALSE);
  }
  else {
    XtSetSensitive(gui->emulator, TRUE);
  }
}

/**
 * XcpcApplication::OnResetCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnResetCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
{
  amstrad_cpc_reset(&amstrad_cpc);
  XtSetSensitive(gui->emulator, TRUE);
}

/**
 * XcpcApplication::OnLegalInfoCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnLegalInfoCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
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

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(button, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(button);
  /* legal-info-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnAboutXcpcCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param cbs specifies the callback info
 */
static void OnAboutXcpcCbk(Widget widget, XcpcApplication gui, XtPointer cbs)
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

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
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
  XtAddCallback(button, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(button);
  /* about-xcpc-popup */
  XtPopup(shell, XtGrabExclusive);
}

/**
 * XcpcApplication::OnDropURICbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the XcpcApplication
 * @param uri specifies the callback info
 */
static void OnDropURICbk(Widget widget, XcpcApplication gui, char *uri)
{
  int length = 0;

  if((uri != NULL) && (strncmp(uri, "file://", 7) == 0)) {
    char *str = &uri[7], *ptr = strstr(str, "\r\n");
    if(ptr != NULL) {
      *ptr = '\0';
    }
    if((length = strlen(str)) >= 4) {
      if(strcmp(&str[length - 4], ".sna") == 0) {
        amstrad_cpc_load_snapshot(&amstrad_cpc, str);
      }
      if(strcmp(&str[length - 4], ".dsk") == 0) {
        amstrad_cpc_insert_drive0(&amstrad_cpc, str);
        amstrad_cpc_insert_drive1(&amstrad_cpc, str);
      }
      XtSetSensitive(gui->emulator, TRUE);
      XtSetSensitive(gui->emulator, TRUE);
    }
  }
}

/**
 * XcpcApplication::Create()
 *
 * @param toplevel specifies the TopLevel Shell
 *
 * @return the main-window instance
 */
Widget XcpcCreateApplication(Widget toplevel)
{
  XcpcApplication gui = (XcpcApplication) XtMalloc(sizeof(XcpcApplicationRec));
  Arg arglist[8];
  Cardinal argcount;

  /* main-wnd */
  argcount = 0;
  gui->main_wnd = XtCreateWidget("main-wnd", boxWidgetClass, toplevel, arglist, argcount);
  /* menu-bar */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNorientation, XtorientHorizontal); argcount++;
  XtSetArg(arglist[argcount], XtNborderWidth, 0); argcount++;
  gui->menu_bar = XtCreateWidget("menu-bar", boxWidgetClass, gui->main_wnd, arglist, argcount);
  XtManageChild(gui->menu_bar);
  /* file-menu */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("File")); argcount++;
  XtSetArg(arglist[argcount], XtNmenuName, "file-pldn"); argcount++;
  gui->file_menu = XtCreateWidget("file-menu", menuButtonWidgetClass, gui->menu_bar, arglist, argcount);
  XtManageChild(gui->file_menu);
  /* file-pldn */
  argcount = 0;
  gui->file_pldn = XtCreatePopupShell("file-pldn", simpleMenuWidgetClass, gui->file_menu, arglist, argcount);
  /* load-snapshot */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Load Snapshot")); argcount++;
  gui->load_snapshot = XtCreateWidget("load-snapshot", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->load_snapshot, XtNcallback, (XtCallbackProc) OnLoadSnapshotCbk, (XtPointer) gui);
  XtManageChild(gui->load_snapshot);
  /* save-snapshot */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Save Snapshot")); argcount++;
  gui->save_snapshot = XtCreateWidget("save-snapshot", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->save_snapshot, XtNcallback, (XtCallbackProc) OnSaveSnapshotCbk, (XtPointer) gui);
  XtManageChild(gui->save_snapshot);
  /* separator1 */
  argcount = 0;
  gui->separator1 = XtCreateWidget("separator1", smeLineObjectClass, gui->file_pldn, arglist, argcount);
  XtManageChild(gui->separator1);
  /* drivea-insert */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive A")); argcount++;
  gui->drivea_insert = XtCreateWidget("drivea-insert", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->drivea_insert, XtNcallback, (XtCallbackProc) OnDriveAInsertCbk, (XtPointer) gui);
  XtManageChild(gui->drivea_insert);
  /* drivea-eject */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Eject disk from drive A")); argcount++;
  gui->drivea_eject = XtCreateWidget("drivea-eject", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->drivea_eject, XtNcallback, (XtCallbackProc) OnDriveAEjectCbk, (XtPointer) gui);
  XtManageChild(gui->drivea_eject);
  /* separator2 */
  argcount = 0;
  gui->separator2 = XtCreateWidget("separator2", smeLineObjectClass, gui->file_pldn, arglist, argcount);
  XtManageChild(gui->separator2);
  /* driveb-insert */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Insert disk into drive B")); argcount++;
  gui->driveb_insert = XtCreateWidget("driveb-insert", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->driveb_insert, XtNcallback, (XtCallbackProc) OnDriveBInsertCbk, (XtPointer) gui);
  XtManageChild(gui->driveb_insert);
  /* driveb-eject */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Eject disk from drive B")); argcount++;
  gui->driveb_eject = XtCreateWidget("driveb-eject", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->driveb_eject, XtNcallback, (XtCallbackProc) OnDriveBEjectCbk, (XtPointer) gui);
  XtManageChild(gui->driveb_eject);
  /* separator3 */
  argcount = 0;
  gui->separator3 = XtCreateWidget("separator3", smeLineObjectClass, gui->file_pldn, arglist, argcount);
  XtManageChild(gui->separator3);
  /* exit-emulator */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Exit")); argcount++;
  gui->exit_emulator = XtCreateWidget("exit-emulator", smeBSBObjectClass, gui->file_pldn, arglist, argcount);
  XtAddCallback(gui->exit_emulator, XtNcallback, (XtCallbackProc) OnExitEmulatorCbk, (XtPointer) gui);
  XtManageChild(gui->exit_emulator);
  /* ctrl-menu */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Controls")); argcount++;
  XtSetArg(arglist[argcount], XtNmenuName, "ctrl-pldn"); argcount++;
  gui->ctrl_menu = XtCreateWidget("ctrl-menu", menuButtonWidgetClass, gui->menu_bar, arglist, argcount);
  XtManageChild(gui->ctrl_menu);
  /* ctrl-pldn */
  argcount = 0;
  gui->ctrl_pldn = XtCreatePopupShell("ctrl-pldn", simpleMenuWidgetClass, gui->ctrl_menu, arglist, argcount);
  /* pause-emu */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Play / Pause")); argcount++;
  gui->pause_emu = XtCreateWidget("pause-emu", smeBSBObjectClass, gui->ctrl_pldn, arglist, argcount);
  XtAddCallback(gui->pause_emu, XtNcallback, (XtCallbackProc) OnPauseCbk, (XtPointer) gui);
  XtManageChild(gui->pause_emu);
  /* reset-emu */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Reset")); argcount++;
  gui->reset_emu = XtCreateWidget("reset-emu", smeBSBObjectClass, gui->ctrl_pldn, arglist, argcount);
  XtAddCallback(gui->reset_emu, XtNcallback, (XtCallbackProc) OnResetCbk, (XtPointer) gui);
  XtManageChild(gui->reset_emu);
  /* help-menu */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Help")); argcount++;
  XtSetArg(arglist[argcount], XtNmenuName, "help-pldn"); argcount++;
  gui->help_menu = XtCreateWidget("help-menu", menuButtonWidgetClass, gui->menu_bar, arglist, argcount);
  XtManageChild(gui->help_menu);
  /* help-pldn */
  argcount = 0;
  gui->help_pldn = XtCreatePopupShell("help-pldn", simpleMenuWidgetClass, gui->help_menu, arglist, argcount);
  /* legal-info */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Legal Info")); argcount++;
  gui->legal_info = XtCreateWidget("legal-info", smeBSBObjectClass, gui->help_pldn, arglist, argcount);
  XtAddCallback(gui->legal_info, XtNcallback, (XtCallbackProc) OnLegalInfoCbk, (XtPointer) gui);
  XtManageChild(gui->legal_info);
  /* separator4 */
  argcount = 0;
  gui->separator4 = XtCreateWidget("separator4", smeLineObjectClass, gui->help_pldn, arglist, argcount);
  XtManageChild(gui->separator4);
  /* about-xcpc */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("About Xcpc")); argcount++;
  gui->about_xcpc = XtCreateWidget("about-xcpc", smeBSBObjectClass, gui->help_pldn, arglist, argcount);
  XtAddCallback(gui->about_xcpc, XtNcallback, (XtCallbackProc) OnAboutXcpcCbk, (XtPointer) gui);
  XtManageChild(gui->about_xcpc);
  /* emulator */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNemuContext    , &amstrad_cpc             ); argcount++;
  XtSetArg(arglist[argcount], XtNemuCreateProc , &amstrad_cpc_create_proc ); argcount++;
  XtSetArg(arglist[argcount], XtNemuDestroyProc, &amstrad_cpc_destroy_proc); argcount++;
  XtSetArg(arglist[argcount], XtNemuRealizeProc, &amstrad_cpc_realize_proc); argcount++;
  XtSetArg(arglist[argcount], XtNemuResizeProc , &amstrad_cpc_resize_proc ); argcount++;
  XtSetArg(arglist[argcount], XtNemuRedrawProc , &amstrad_cpc_redraw_proc ); argcount++;
  XtSetArg(arglist[argcount], XtNemuTimerProc  , &amstrad_cpc_timer_proc  ); argcount++;
  XtSetArg(arglist[argcount], XtNemuInputProc  , &amstrad_cpc_input_proc  ); argcount++;
  gui->emulator = XemCreateEmulator(gui->main_wnd, "emulator", arglist, argcount);
  XtManageChild(gui->emulator);
  /* XXX */
  XtAddCallback(toplevel, XtNdropURICallback, (XtCallbackProc) OnDropURICbk, (XtPointer) gui);
  return(gui->main_wnd);
}

/**
 * xcpc main function
 *
 * @param argc specifies the argument count
 * @param argv specifies the argument list
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int xcpc(int* argc, char*** argv)
{
    XtAppContext appcontext  = NULL;
    String       appname     = NULL;
    String       appclass    = NULL;
    Widget       toplevel    = NULL;
    Widget       application = NULL;
    Arg          arglist[4];
    Cardinal     argcount;

    /* set language proc */ {
        (void) XtSetLanguageProc(NULL, NULL, NULL);
    }
    /* create application context and toplevel shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNmappedWhenManaged, TRUE); argcount++;
        XtSetArg(arglist[argcount], XtNallowShellResize, TRUE); argcount++;
        toplevel = XtOpenApplication(&appcontext, "Xcpc", options, XtNumber(options), argc, *argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
        XtAddCallback(toplevel, XtNdestroyCallback, (XtCallbackProc) DestroyCbk, (XtPointer) &toplevel);
    }
    /* get application resources */ {
        argcount = 0;
        XtGetApplicationResources(toplevel, (XtPointer) &xcpc_resources, application_resources, XtNumber(application_resources), arglist, argcount);
    }
    /* get application name and class */ {
        XtGetApplicationNameAndClass(XtDisplay(toplevel), &appname, &appclass);
    }
    /* check command-line flags */ {
        if(xcpc_resources.quiet_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_QUIET);
        }
        if(xcpc_resources.trace_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_TRACE);
        }
        if(xcpc_resources.debug_flag != FALSE) {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_DEBUG);
        }
    }
    /* initialize libxcpc */ {
        xcpc_begin();
    }
    /* check command-line flags */ {
        if(xcpc_resources.about_flag != FALSE) {
            (void) fprintf(stdout, "%s %s\n", appname, PACKAGE_VERSION);
            (void) fflush(stdout);
            exit(EXIT_SUCCESS);
        }
        if((xcpc_resources.usage_flag != FALSE) || (amstrad_cpc_parse(argc, argv) == EXIT_FAILURE)) {
            (void) fprintf(stdout, "Usage: %s [toolkit-options] [program-options]\n\n", appname);
            (void) fprintf(stdout, "Options:\n");
            (void) fprintf(stdout, "  -version  display version and exit.\n");
            (void) fprintf(stdout, "  -help     display this help and exit.\n");
            (void) fprintf(stdout, "\n");
            (void) fprintf(stdout, "  -quiet    set loglevel to quiet mode.\n");
            (void) fprintf(stdout, "  -trace    set loglevel to trace mode.\n");
            (void) fprintf(stdout, "  -error    set loglevel to error mode.\n");
            (void) fflush(stdout);
            exit(EXIT_SUCCESS);
        }
    }
    /* create application and run */ {
        application = XcpcCreateApplication(toplevel);
        XtManageChild(application);
        XtRealizeWidget(toplevel);
        XtAppMainLoop(appcontext);
    }
    /* destroy toplevel shell */ {
        if(toplevel != NULL) {
            XtDestroyWidget(toplevel);
        }
    }
    /* destroy application context */ {
        XtDestroyApplicationContext(appcontext);
    }
    /* finalize libxcpc */ {
        xcpc_end();
    }
    return(EXIT_SUCCESS);
}
