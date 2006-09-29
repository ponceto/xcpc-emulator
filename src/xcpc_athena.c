/*
 * xcpc_athena.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "XArea.h"
#include "common.h"
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
    "xcpcAboutFlag", "XcpcAboutFlag", XtRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, about_flag),
    XtRImmediate, (XtPointer) FALSE
  },
  /* xcpcUsageFlag */ {
    "xcpcUsageFlag", "XcpcUsageFlag", XtRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, usage_flag),
    XtRImmediate, (XtPointer) FALSE
  },
  /* xcpcEdresFlag */ {
    "xcpcEdresFlag", "XcpcEdresFlag", XtRBoolean,
    sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, edres_flag),
    XtRImmediate, (XtPointer) FALSE
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
 * WMClose Callback
 *
 * @param widget specifies the Shell
 * @param xevent specifies the XEvent
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void WMCloseCbk(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  if(XtIsApplicationShell(widget) != FALSE) {
    XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
  }
  else if(XtIsShell(widget) != FALSE) {
    XtDestroyWidget(widget);
  }
}

/**
 * Destroy Callback
 *
 * @param widget specifies the Widget instance
 * @param widref specifies the Widget reference
 * @param cbdata is not used
 */
static void DestroyCbk(Widget widget, Widget *widref, XtPointer cbdata)
{
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
  XtSetArg(arglist[argcount], XtNmappedWhenManaged, TRUE); argcount++;
  XtSetArg(arglist[argcount], XtNallowShellResize, TRUE); argcount++;
  toplevel = XtOpenApplication(&appcontext, "Xcpc", options, XtNumber(options), &argc, argv, fallback_resources, applicationShellWidgetClass, arglist, argcount);
  XtAddCallback(toplevel, XtNdestroyCallback, (XtCallbackProc) DestroyCbk, (XtPointer) &toplevel);
  argcount = 0;
  XtGetApplicationResources(toplevel, (XtPointer) &xcpc_resources, application_resources, XtNumber(application_resources), arglist, argcount);
  XtGetApplicationNameAndClass(XtDisplay(toplevel), &appname, &appclass);
  if(xcpc_resources.about_flag != FALSE) {
    (void) fprintf(stdout, "%s %s\n", appname, PACKAGE_VERSION);
    (void) fflush(stdout);
    exit(EXIT_SUCCESS);
  }
  if((xcpc_resources.usage_flag != FALSE) || (amstrad_cpc_parse(argc, argv) == EXIT_FAILURE)) {
    (void) fprintf(stdout, "Usage: %s [toolkit-options] [program-options]\n\n", appname);
    (void) fprintf(stdout, "Options:\n");
    (void) fprintf(stdout, "  -version  print version and exit.\n");
    (void) fprintf(stdout, "  -help     display this help and exit.\n");
    (void) fprintf(stderr, "            [-cpc464] [-cpc664] [-cpc6128]\n");
    (void) fprintf(stderr, "            [-GT65] [-CTM644] [-50Hz] [-60Hz]\n");
    (void) fprintf(stderr, "            [-4MHz] [-8MHz] [-12MHz] [-16MHz]\n");
    (void) fprintf(stderr, "            [-isp] [-triumph] [-saisho] [-solavox]\n");
    (void) fprintf(stderr, "            [-awa] [-schneider] [-orion] [-amstrad]\n");
    (void) fprintf(stderr, "            [-tiny] [-small] [-medium] [-big] [-huge]\n");
    (void) fflush(stdout);
    exit(EXIT_SUCCESS);
  }
  if(xcpc_resources.edres_flag != FALSE) {
    XtAddEventHandler(toplevel, NoEventMask, TRUE, (XtEventHandler) _XEditResCheckMessages, (XtPointer) NULL);
  }
  /* XXX */ {
    apwindow = CreateGUI(toplevel);
  }
  XtManageChild(apwindow);
  XtRealizeWidget(toplevel);
  if(XtIsRealized(toplevel) != FALSE) {
    Atom atom = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", FALSE);
    if(atom != None) {
      static XtActionsRec actions[] = {
        { "XcpcWMClose", WMCloseCbk }
      };
      XtAppAddActions(appcontext, actions, XtNumber(actions));
      XSetWMProtocols(XtDisplay(toplevel), XtWindow(toplevel), &atom, 1);
      XtOverrideTranslations(toplevel, XtParseTranslationTable("<Message>WM_PROTOCOLS: XcpcWMClose()"));
    }
  }
  XtAppMainLoop(appcontext);
  if(toplevel != NULL) {
    XtDestroyWidget(toplevel);
  }
  XtDestroyApplicationContext(appcontext);
  return(EXIT_SUCCESS);
}

/*
 * XXX
 */
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Dialog.h>

XtTranslations translations = NULL;

/*
 * GUI instance structure
 */
typedef struct _GUI {
  Widget main_wnd;
  Widget menu_bar;
  Widget file_menu;
  Widget file_pldn;
  Widget load_snapshot;
  Widget save_snapshot;
  Widget separator1;
  Widget exit_emulator;
  Widget ctrl_menu;
  Widget ctrl_pldn;
  Widget pause;
  Widget reset;
  Widget help_menu;
  Widget help_pldn;
  Widget legal_info;
  Widget separator2;
  Widget about_xcpc;
  Widget xarea;
} GUI;

/**
 * GUI::OnCloseCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnCloseCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  XtPopdown(widget);
  XtDestroyWidget(widget);
  XtSetSensitive(gui->xarea, TRUE);
}

/**
 * GUI::OnLoadSnapshotOkCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotOkCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if(value != NULL) {
    amstrad_cpc_load_snapshot(value);
    /* XtFree((char *) value);
    value = NULL; */
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * GUI::OnLoadSnapshotCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLoadSnapshotCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  Widget shell, dialog;
  Arg arglist[8];
  Cardinal argcount;

  while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  shell = XtCreatePopupShell("load_shell", transientShellWidgetClass, widget, arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
  dialog = XtCreateManagedWidget("load_dialog", dialogWidgetClass, shell, arglist, argcount);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) OnLoadSnapshotOkCbk, (XtPointer) gui);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtPopup(shell, XtGrabExclusive);
  if(XtIsRealized(shell) != FALSE) {
    Atom atom = XInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", FALSE);
    if(atom != None) {
      XSetWMProtocols(XtDisplay(shell), XtWindow(shell), &atom, 1);
      XtOverrideTranslations(shell, translations);
    }
  }
}

/**
 * GUI::OnSaveSnapshotOkCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotOkCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  char *value = XawDialogGetValueString(XtParent(widget));
  if(value != NULL) {
    amstrad_cpc_save_snapshot(value);
    /* XtFree((char *) value);
    value = NULL; */
  }
  OnCloseCbk(widget, gui, cbs);
}

/**
 * GUI::OnSaveSnapshotCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnSaveSnapshotCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  Widget shell, dialog;
  Arg arglist[8];
  Cardinal argcount;

  while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  shell = XtCreatePopupShell("save_shell", transientShellWidgetClass, widget, arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNvalue, ""); argcount++;
  dialog = XtCreateManagedWidget("save_dialog", dialogWidgetClass, shell, arglist, argcount);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) OnSaveSnapshotOkCbk, (XtPointer) gui);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtPopup(shell, XtGrabExclusive);
  if(XtIsRealized(shell) != FALSE) {
    Atom atom = XInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", FALSE);
    if(atom != None) {
      XSetWMProtocols(XtDisplay(shell), XtWindow(shell), &atom, 1);
      XtOverrideTranslations(shell, translations);
    }
  }
}

/**
 * GUI::OnExitEmulatorCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnExitEmulatorCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
}

/**
 * GUI::OnPauseCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnPauseCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  if(XtIsSensitive(gui->xarea) != FALSE) {
    XtSetSensitive(gui->xarea, FALSE);
  }
  else {
    XtSetSensitive(gui->xarea, TRUE);
  }
}

/**
 * GUI::OnResetCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnResetCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  amstrad_cpc_reset();
  XtSetSensitive(gui->xarea, TRUE);
}

/**
 * GUI::OnLegalInfoCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnLegalInfoCbk(Widget widget, GUI *gui, XtPointer cbs)
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

  XtSetSensitive(gui->xarea, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  shell = XtCreatePopupShell("xcpc-legal-xcpc-shl", transientShellWidgetClass, widget, arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, message); argcount++;
  dialog = XtCreateWidget("xcpc-legal-xcpc-dlg", dialogWidgetClass, shell, arglist, argcount);
  XtManageChild(dialog);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("OK")); argcount++;
  button = XtCreateManagedWidget("xcpc-legal-xcpc-btn", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(button, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(button);
  XtPopup(shell, XtGrabExclusive);
  if(XtIsRealized(shell) != FALSE) {
    Atom atom = XInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", FALSE);
    if(atom != None) {
      XSetWMProtocols(XtDisplay(shell), XtWindow(shell), &atom, 1);
      XtOverrideTranslations(shell, translations);
    }
  }
}

/**
 * GUI::OnAboutXcpcCbk
 *
 * @param widget specifies the Widget
 * @param gui specifies the GUI
 * @param cbs specifies the callback info
 */
static void OnAboutXcpcCbk(Widget widget, GUI *gui, XtPointer cbs)
{
  Widget shell, dialog, button;
  Arg arglist[8];
  Cardinal argcount;
  String message = _(
    "Xcpc - Amstrad CPC Emulator - Copyright (c) 2001, 2006 Olivier Poncet\n\n"
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
  );

  XtSetSensitive(gui->xarea, FALSE);
  while((widget != NULL) && (XtIsTopLevelShell(widget) == FALSE)) {
    widget = XtParent(widget);
  }
  argcount = 0;
  shell = XtCreatePopupShell("xcpc-about-xcpc-shl", transientShellWidgetClass, widget, arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, message); argcount++;
  dialog = XtCreateWidget("xcpc-about-xcpc-dlg", dialogWidgetClass, shell, arglist, argcount);
  XtManageChild(dialog);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("OK")); argcount++;
  button = XtCreateManagedWidget("xcpc-about-xcpc-btn", commandWidgetClass, dialog, arglist, argcount);
  XtAddCallback(button, XtNcallback, (XtCallbackProc) OnCloseCbk, (XtPointer) gui);
  XtManageChild(button);
  XtPopup(shell, XtGrabExclusive);
  if(XtIsRealized(shell) != FALSE) {
    Atom atom = XInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", FALSE);
    if(atom != None) {
      XSetWMProtocols(XtDisplay(shell), XtWindow(shell), &atom, 1);
      XtOverrideTranslations(shell, translations);
    }
  }
}

/**
 * GUI::Create
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

  translations = XtParseTranslationTable("<Message>WM_PROTOCOLS: XcpcWMClose()");
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
  /* pause */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Play / Pause")); argcount++;
  gui->pause = XtCreateWidget("pause", smeBSBObjectClass, gui->ctrl_pldn, arglist, argcount);
  XtAddCallback(gui->pause, XtNcallback, (XtCallbackProc) OnPauseCbk, (XtPointer) gui);
  XtManageChild(gui->pause);
  /* reset */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("Reset")); argcount++;
  gui->reset = XtCreateWidget("reset", smeBSBObjectClass, gui->ctrl_pldn, arglist, argcount);
  XtAddCallback(gui->reset, XtNcallback, (XtCallbackProc) OnResetCbk, (XtPointer) gui);
  XtManageChild(gui->reset);
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
  /* separator2 */
  argcount = 0;
  gui->separator2 = XtCreateWidget("separator2", smeLineObjectClass, gui->help_pldn, arglist, argcount);
  XtManageChild(gui->separator2);
  /* about-xcpc */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNlabel, _("About Xcpc")); argcount++;
  gui->about_xcpc = XtCreateWidget("about-xcpc", smeBSBObjectClass, gui->help_pldn, arglist, argcount);
  XtAddCallback(gui->about_xcpc, XtNcallback, (XtCallbackProc) OnAboutXcpcCbk, (XtPointer) gui);
  XtManageChild(gui->about_xcpc);
  /* xarea */
  argcount = 0;
  XtSetArg(arglist[argcount], XtNemuStartHandler, amstrad_cpc_start_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuClockHandler, amstrad_cpc_clock_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuCloseHandler, amstrad_cpc_close_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuKeybdHandler, amstrad_cpc_keybd_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuMouseHandler, amstrad_cpc_mouse_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuPaintHandler, amstrad_cpc_paint_handler); argcount++;
  gui->xarea = XAreaCreate(gui->main_wnd, "xarea", arglist, argcount);
  XtManageChild(gui->xarea);
  /* XXX */
  return(gui->main_wnd);
}
