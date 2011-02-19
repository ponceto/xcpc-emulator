/*
 * xcpc_motif1.c - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011 Olivier Poncet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Xm/XmAll.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include "amstrad_cpc.h"
#include "xcpc.h"

static Widget CreateGUI(Widget toplevel);

/*
 * command line options
 */
static XrmOptionDescRec options[] = {
  { "-version", ".xcpcAboutFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-help",    ".xcpcUsageFlag", XrmoptionNoArg, (XPointer) "true" },
  { "-editres", ".xcpcEdresFlag", XrmoptionNoArg, (XPointer) "true" }
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
  /* xcpcEdresFlag */ {
    "xcpcEdresFlag", "XcpcEdresFlag", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, edres_flag),
    XmRImmediate, (XtPointer) FALSE
  }
};

/*
 * Xcpc resources
 */
static XcpcResourcesRec xcpc_resources = {
  FALSE, /* about_flag */
  FALSE, /* usage_flag */
  FALSE  /* edres_flag */
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
 * main
 *
 * @param argc specifies the argument count
 * @param argv specifies the argument list
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  XtAppContext appcontext;
  String appname  = NULL;
  String appclass = NULL;
  Widget toplevel = NULL;
  Widget apwindow = NULL;
  Cardinal argcount = 0;
  Arg arglist[4];

  (void) XtSetLanguageProc(NULL, NULL, NULL);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNmappedWhenManaged, TRUE); argcount++;
  XtSetArg(arglist[argcount], XmNallowShellResize, TRUE); argcount++;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDO_NOTHING); argcount++;
  toplevel = XtOpenApplication(&appcontext, "Xcpc", options, XtNumber(options), &argc, argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
  XtAddCallback(toplevel, XmNdestroyCallback, (XtCallbackProc) DestroyCbk, (XtPointer) &toplevel);
  argcount = 0;
  XtGetApplicationResources(toplevel, (XtPointer) &xcpc_resources, application_resources, XtNumber(application_resources), arglist, argcount);
  XtGetApplicationNameAndClass(XtDisplay(toplevel), &appname, &appclass);
  if(xcpc_resources.about_flag != FALSE) {
    (void) fprintf(stdout, "%s %s\n", appname, PACKAGE_VERSION);
    (void) fflush(stdout);
    exit(EXIT_SUCCESS);
  }
  if((xcpc_resources.usage_flag != FALSE) || (amstrad_cpc_parse(&argc, &argv) == EXIT_FAILURE)) {
    (void) fprintf(stdout, "Usage: %s [toolkit-options] [program-options]\n\n", appname);
    (void) fprintf(stdout, "Options:\n");
    (void) fprintf(stdout, "  -version  print version and exit.\n");
    (void) fprintf(stdout, "  -help     display this help and exit.\n");
    (void) fflush(stdout);
    exit(EXIT_SUCCESS);
  }
  if(xcpc_resources.edres_flag != FALSE) {
    XtAddEventHandler(toplevel, NoEventMask, TRUE, (XtEventHandler) _XEditResCheckMessages, (XtPointer) NULL);
  }
  /* XXX */ {
    g_type_init(); apwindow = CreateGUI(toplevel);
  }
  XtManageChild(apwindow);
  XtRealizeWidget(toplevel);
  XtAppMainLoop(appcontext);
  if(toplevel != NULL) {
    XtDestroyWidget(toplevel);
  }
  XtDestroyApplicationContext(appcontext);
  return(EXIT_SUCCESS);
}

/*
 * GUI instance structure
 */
typedef struct _GUI {
  Widget main_wnd;
  Widget menu_bar;
  Widget file_pldn;
  Widget file_menu;
  Widget load_snapshot;
  Widget save_snapshot;
  Widget separator1;
  Widget drivea_insert;
  Widget drivea_eject;
  Widget separator2;
  Widget driveb_insert;
  Widget driveb_eject;
  Widget separator3;
  Widget exit_emulator;
  Widget ctrl_pldn;
  Widget ctrl_menu;
  Widget pause_emu;
  Widget reset_emu;
  Widget help_pldn;
  Widget help_menu;
  Widget legal_info;
  Widget separator4;
  Widget about_xcpc;
  Widget frame;
  Widget emulator;
} GUI;

/**
 * GUI::OnCloseCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnCloseCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  if(cbs != NULL) {
    while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
      widget = XtParent(widget);
    }
    if(widget != NULL) {
      XtDestroyWidget(widget);
    }
  }
  else {
    XtSetSensitive(gui->emulator, TRUE);
  }
}

/**
 * GUI::OnLoadSnapshotOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotOkCbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
  char *filename = NULL;

  if(XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
    if(filename != NULL) {
      amstrad_cpc_load_snapshot(filename);
      XtFree((char *) filename);
      filename = NULL;
    }
  }
  OnCloseCbk(widget, gui, (XmAnyCallbackStruct *) cbs);
}

/**
 * GUI::OnLoadSnapshotCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("Load a snapshot ..."));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  dialog = XmCreateFileSelectionDialog(widget, "xcpc-load-snapshot-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnLoadSnapshotOkCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtManageChild(dialog);
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnSaveSnapshotOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotOkCbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
  char *filename = NULL;

  if(XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
    if(filename != NULL) {
      amstrad_cpc_save_snapshot(filename);
      XtFree((char *) filename);
      filename = NULL;
    }
  }
  OnCloseCbk(widget, gui, (XmAnyCallbackStruct *) cbs);
}

/**
 * GUI::OnSaveSnapshotCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("Save a snapshot ..."));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  dialog = XmCreateFileSelectionDialog(widget, "xcpc-save-snapshot-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnSaveSnapshotOkCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtManageChild(dialog);
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnDriveAInsertOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveAInsertOkCbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
  char *filename = NULL;

  if(XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
    if(filename != NULL) {
      gdev_fdd765_insert(amstrad_cpc.upd765->fdd[0], filename);
      XtFree((char *) filename);
      filename = NULL;
    }
  }
  OnCloseCbk(widget, gui, (XmAnyCallbackStruct *) cbs);
}

/**
 * GUI::OnDriveAInsertCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveAInsertCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("Insert disk into drive A ..."));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  dialog = XmCreateFileSelectionDialog(widget, "xcpc-insert-drivea-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnDriveAInsertOkCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtManageChild(dialog);
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnDriveAEjectCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveAEjectCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  gdev_fdd765_insert(amstrad_cpc.upd765->fdd[0], NULL);
}

/**
 * GUI::OnDriveBInsertOkCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveBInsertOkCbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
  char *filename = NULL;

  if(XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename) != FALSE) {
    if(filename != NULL) {
      gdev_fdd765_insert(amstrad_cpc.upd765->fdd[1], filename);
      XtFree((char *) filename);
      filename = NULL;
    }
  }
  OnCloseCbk(widget, gui, (XmAnyCallbackStruct *) cbs);
}

/**
 * GUI::OnDriveBInsertCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveBInsertCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("Insert disk into drive B ..."));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  dialog = XmCreateFileSelectionDialog(widget, "xcpc-insert-driveb-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnDriveBInsertOkCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNcancelCallback,  (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk,          (XtPointer) gui);
  XtManageChild(dialog);
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnDriveBEjectCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnDriveBEjectCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  gdev_fdd765_insert(amstrad_cpc.upd765->fdd[1], NULL);
}

/**
 * GUI::OnExitEmulatorCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnExitEmulatorCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
}

/**
 * GUI::OnPauseCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnPauseCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  if(XtIsSensitive(gui->emulator) != FALSE) {
    XtSetSensitive(gui->emulator, FALSE);
  }
  else {
    XtSetSensitive(gui->emulator, TRUE);
  }
}

/**
 * GUI::OnResetCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnResetCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  amstrad_cpc_reset();
  XtSetSensitive(gui->emulator, TRUE);
}

/**
 * GUI::OnLegalInfoCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLegalInfoCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("Legal Info ..."));
  XmString message = XmStringCreateLocalized(_(
    "Amstrad has kindly given it's permission for it's copyrighted\n"
    "material to be redistributed but Amstrad retains it's copyright.\n\n"
    "Some of the Amstrad CPC ROM code is copyright Locomotive Software.\n\n"
    "ROM and DISK images are protected under the copyrights of their authors,\n"
    "and cannot be distributed in this package. You can download and/or use\n"
    "ROM and DISK images at your own risk and responsibility."
  ));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  XtSetArg(arglist[argcount], XmNmessageAlignment, XmALIGNMENT_CENTER); argcount++;
  XtSetArg(arglist[argcount], XmNmessageString, message); argcount++;
  dialog = XmCreateMessageDialog(widget, "xcpc-legal-info-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtManageChild(dialog);
  XmStringFree(message);
  message = NULL;
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnAboutXcpcCbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnAboutXcpcCbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  Widget dialog;
  Arg arglist[8];
  Cardinal argcount;
  XmString title = XmStringCreateLocalized(_("About Xcpc ..."));
  XmString message = XmStringCreateLocalized(_(
    PACKAGE_STRING " - Amstrad CPC Emulator - Copyright (c) 2001, 2006, 2007 Olivier Poncet\n\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or\n"
    "(at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program; if not, write to the Free Software\n"
    "Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA"
  ));

  XtSetSensitive(gui->emulator, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  XtSetArg(arglist[argcount], XmNdialogTitle, title); argcount++;
  XtSetArg(arglist[argcount], XmNmessageAlignment, XmALIGNMENT_CENTER); argcount++;
  XtSetArg(arglist[argcount], XmNmessageString, message); argcount++;
  dialog = XmCreateMessageDialog(widget, "xcpc-about-xcpc-dlg", arglist, argcount);
  XtAddCallback(dialog, XmNokCallback,      (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNunmapCallback,   (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtAddCallback(dialog, XmNdestroyCallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtManageChild(dialog);
  XmStringFree(message);
  message = NULL;
  XmStringFree(title);
  title = NULL;
}

/**
 * GUI::OnDropURICbk()
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param uri specifies the callback info
 */
static void OnDropURICbk(Widget widget, GUI *gui, char *uri)
{
  int length = 0;

  if((uri != NULL) && (strncmp(uri, "file://", 7) == 0)) {
    char *str = &uri[7], *ptr = strstr(str, "\r\n");
    if(ptr != NULL) {
      *ptr = '\0';
    }
    if((length = strlen(str)) >= 4) {
      if(strcmp(&str[length - 4], ".sna") == 0) {
        amstrad_cpc_load_snapshot(str);
      }
      if(strcmp(&str[length - 4], ".dsk") == 0) {
        gdev_fdd765_insert(amstrad_cpc.upd765->fdd[0], str);
        gdev_fdd765_insert(amstrad_cpc.upd765->fdd[1], str);
      }
      XtSetSensitive(gui->emulator, TRUE);
      XtSetSensitive(gui->emulator, TRUE);
    }
  }
}

/**
 * GUI::Create()
 *
 * @param toplevel specifies the TopLevel Shell
 *
 * @return the main-window instance
 */
static Widget CreateGUI(Widget toplevel)
{
  GUI *gui = (GUI *) XtMalloc(sizeof(GUI));
  Arg arglist[8];
  Cardinal argcount;
  XmString string;

  /* main-wnd */
  argcount = 0;
  gui->main_wnd = XmCreateMainWindow(toplevel, "main-wnd", arglist, argcount);
  /* menu-bar */
  argcount = 0;
  gui->menu_bar = XmCreateMenuBar(gui->main_wnd, "menu-bar", arglist, argcount);
  XtManageChild(gui->menu_bar);
  /* file-pldn */
  argcount = 0;
  gui->file_pldn = XmCreatePulldownMenu(gui->menu_bar, "file-pldn", arglist, argcount);
  /* file-menu */
  string = XmStringCreateLocalized(_("File"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->file_pldn); argcount++;
  gui->file_menu = XmCreateCascadeButtonGadget(gui->menu_bar, "file-menu", arglist, argcount);
  XtManageChild(gui->file_menu);
  XmStringFree(string);
  string = NULL;
  /* load-snapshot */
  string = XmStringCreateLocalized(_("Load Snapshot"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->load_snapshot = XmCreatePushButtonGadget(gui->file_pldn, "load-snapshot", arglist, argcount);
  XtAddCallback(gui->load_snapshot, XmNactivateCallback, (XtCallbackProc) OnLoadSnapshotCbk, (XtPointer) gui);
  XtManageChild(gui->load_snapshot);
  XmStringFree(string);
  string = NULL;
  /* save-snapshot */
  string = XmStringCreateLocalized(_("Save Snapshot"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->save_snapshot = XmCreatePushButtonGadget(gui->file_pldn, "save-snapshot", arglist, argcount);
  XtAddCallback(gui->save_snapshot, XmNactivateCallback, (XtCallbackProc) OnSaveSnapshotCbk, (XtPointer) gui);
  XtManageChild(gui->save_snapshot);
  XmStringFree(string);
  string = NULL;
  /* separator1 */
  argcount = 0;
  gui->separator1 = XmCreateSeparatorGadget(gui->file_pldn, "separator1", arglist, argcount);
  XtManageChild(gui->separator1);
  /* drivea-insert */
  string = XmStringCreateLocalized(_("Insert disk into drive A"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->drivea_insert = XmCreatePushButtonGadget(gui->file_pldn, "drivea-insert", arglist, argcount);
  XtAddCallback(gui->drivea_insert, XmNactivateCallback, (XtCallbackProc) OnDriveAInsertCbk, (XtPointer) gui);
  XtManageChild(gui->drivea_insert);
  XmStringFree(string);
  string = NULL;
  /* drivea-eject */
  string = XmStringCreateLocalized(_("Eject disk from drive A"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->drivea_eject = XmCreatePushButtonGadget(gui->file_pldn, "drivea-eject", arglist, argcount);
  XtAddCallback(gui->drivea_eject, XmNactivateCallback, (XtCallbackProc) OnDriveAEjectCbk, (XtPointer) gui);
  XtManageChild(gui->drivea_eject);
  XmStringFree(string);
  string = NULL;
  /* separator2 */
  argcount = 0;
  gui->separator2 = XmCreateSeparatorGadget(gui->file_pldn, "separator2", arglist, argcount);
  XtManageChild(gui->separator2);
  /* driveb-insert */
  string = XmStringCreateLocalized(_("Insert disk into drive B"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->driveb_insert = XmCreatePushButtonGadget(gui->file_pldn, "driveb-insert", arglist, argcount);
  XtAddCallback(gui->driveb_insert, XmNactivateCallback, (XtCallbackProc) OnDriveBInsertCbk, (XtPointer) gui);
  XtManageChild(gui->driveb_insert);
  XmStringFree(string);
  string = NULL;
  /* driveb-eject */
  string = XmStringCreateLocalized(_("Eject disk from drive B"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->driveb_eject = XmCreatePushButtonGadget(gui->file_pldn, "driveb-eject", arglist, argcount);
  XtAddCallback(gui->driveb_eject, XmNactivateCallback, (XtCallbackProc) OnDriveBEjectCbk, (XtPointer) gui);
  XtManageChild(gui->driveb_eject);
  XmStringFree(string);
  string = NULL;
  /* separator3 */
  argcount = 0;
  gui->separator3 = XmCreateSeparatorGadget(gui->file_pldn, "separator3", arglist, argcount);
  XtManageChild(gui->separator3);
  /* exit-emulator */
  string = XmStringCreateLocalized(_("Exit"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->exit_emulator = XmCreatePushButtonGadget(gui->file_pldn, "exit-emulator", arglist, argcount);
  XtAddCallback(gui->exit_emulator, XmNactivateCallback, (XtCallbackProc) OnExitEmulatorCbk, (XtPointer) gui);
  XtManageChild(gui->exit_emulator);
  XmStringFree(string);
  string = NULL;
  /* ctrl-pldn */
  argcount = 0;
  gui->ctrl_pldn = XmCreatePulldownMenu(gui->menu_bar, "ctrl-pldn", arglist, argcount);
  /* ctrl-menu */
  string = XmStringCreateLocalized(_("Controls"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->ctrl_pldn); argcount++;
  gui->ctrl_menu = XmCreateCascadeButtonGadget(gui->menu_bar, "ctrl-menu", arglist, argcount);
  XtManageChild(gui->ctrl_menu);
  XmStringFree(string);
  string = NULL;
  /* pause-emu */
  string = XmStringCreateLocalized(_("Play / Pause"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->pause_emu = XmCreatePushButtonGadget(gui->ctrl_pldn, "pause-emu", arglist, argcount);
  XtAddCallback(gui->pause_emu, XmNactivateCallback, (XtCallbackProc) OnPauseCbk, (XtPointer) gui);
  XtManageChild(gui->pause_emu);
  XmStringFree(string);
  string = NULL;
  /* reset-emu */
  string = XmStringCreateLocalized(_("Reset"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->reset_emu = XmCreatePushButtonGadget(gui->ctrl_pldn, "reset-emu", arglist, argcount);
  XtAddCallback(gui->reset_emu, XmNactivateCallback, (XtCallbackProc) OnResetCbk, (XtPointer) gui);
  XtManageChild(gui->reset_emu);
  XmStringFree(string);
  string = NULL;
  /* help-pldn */
  argcount = 0;
  gui->help_pldn = XmCreatePulldownMenu(gui->menu_bar, "help-pldn", arglist, argcount);
  /* help-menu */
  string = XmStringCreateLocalized(_("Help"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->help_pldn); argcount++;
  gui->help_menu = XmCreateCascadeButtonGadget(gui->menu_bar, "help-menu", arglist, argcount);
  XtManageChild(gui->help_menu);
  XmStringFree(string);
  string = NULL;
  /* legal-info */
  string = XmStringCreateLocalized(_("Legal Info"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->legal_info = XmCreatePushButtonGadget(gui->help_pldn, "legal-info", arglist, argcount);
  XtAddCallback(gui->legal_info, XmNactivateCallback, (XtCallbackProc) OnLegalInfoCbk, (XtPointer) gui);
  XtManageChild(gui->legal_info);
  XmStringFree(string);
  string = NULL;
  /* separator4 */
  argcount = 0;
  gui->separator4 = XmCreateSeparatorGadget(gui->help_pldn, "separator4", arglist, argcount);
  XtManageChild(gui->separator4);
  /* about-xcpc */
  string = XmStringCreateLocalized(_("About Xcpc"));
  argcount = 0;
  XtSetArg(arglist[argcount], XmNlabelString, string); argcount++;
  gui->about_xcpc = XmCreatePushButtonGadget(gui->help_pldn, "about-xcpc", arglist, argcount);
  XtAddCallback(gui->about_xcpc, XmNactivateCallback, (XtCallbackProc) OnAboutXcpcCbk, (XtPointer) gui);
  XtManageChild(gui->about_xcpc);
  XmStringFree(string);
  string = NULL;
  /* frame */
  argcount = 0;
  XtSetArg(arglist[argcount], XmNshadowType, XmSHADOW_OUT); argcount++;
  XtSetArg(arglist[argcount], XmNmarginWidth, 4); argcount++;
  XtSetArg(arglist[argcount], XmNmarginHeight, 4); argcount++;
  gui->frame = XmCreateFrame(gui->main_wnd, "frame", arglist, argcount);
  XtManageChild(gui->frame);
  /* emulator */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNemuStartHandler, amstrad_cpc_start_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuClockHandler, amstrad_cpc_clock_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuCloseHandler, amstrad_cpc_close_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuInputHandler, amstrad_cpc_input_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuPaintHandler, amstrad_cpc_paint_handler); argcount++;
  gui->emulator = XemCreateEmulator(gui->frame, "emulator", arglist, argcount);
  XtManageChild(gui->emulator);
  /* XXX */
  argcount = 0;
  XtSetArg(arglist[argcount], XmNmenuHelpWidget, gui->help_menu); argcount++;
  XtSetValues(gui->menu_bar, arglist, argcount);
  XtAddCallback(toplevel, XtNdropURICallback, (XtCallbackProc) OnDropURICbk, (XtPointer) gui);
  return(gui->main_wnd);
}
