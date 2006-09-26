/*
 * xcpc_motif1.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "XArea.h"
#include "common.h"
#include "amstrad_cpc.h"
#include "xcpc.h"

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
  "*about_dialog.dialogType:       XmDIALOG_QUESTION",
  "*about_dialog.messageAlignment: XmALIGNMENT_CENTER",
  "*screen.width:                  656",
  "*screen.height:                 416",
  "*title:                         XCPC - Amstrad CPC emulator",
  "*menu_bar*file.labelString:     File",
  "*menu_bar*insert.labelString:   Insert floppy ...",
  "*menu_bar*remove.labelString:   Remove floppy",
  "*menu_bar*open.labelString:     Open a snapshot ...",
  "*menu_bar*save.labelString:     Save a snapshot ...",
  "*menu_bar*exit.labelString:     Exit",
  "*menu_bar*controls.labelString: Controls",
  "*menu_bar*play.labelString:     Play",
  "*menu_bar*pause.labelString:    Pause",
  "*menu_bar*reset.labelString:    Reset",
  "*menu_bar*help.labelString:     Help",
  "*menu_bar*about.labelString:    About ...",
  "*open_dialog.dialogTitle:       Open a snapshot ...",
  "*open_dialog.okLabelString:     Open",
  "*save_dialog.dialogTitle:       Save a snapshot ...",
  "*save_dialog.okLabelString:     Save",
  "*about_dialog.dialogTitle:      About ...",
  "*about_dialog.messageString:    XCPC - Motif1 version\\nAmstrad CPC emulator for UNIX\\nCoded by Olivier Poncet <ponceto@noos.fr>",
  "*about_dialog.okLabelString:    Close",
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
    paused = 0;
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
  String appname  = NULL;
  String appclass = NULL;
  Widget toplevel = NULL;
  Cardinal argcount = 0;
  Arg arglist[4];

  (void) XtSetLanguageProc(NULL, NULL, NULL);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNmappedWhenManaged, TRUE); argcount++;
  XtSetArg(arglist[argcount], XmNallowShellResize, TRUE); argcount++;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDO_NOTHING); argcount++;
  toplevel = XtOpenApplication(&appcontext, "Xcpc", options, XtNumber(options), &argc, argv, fallback_resources, applicationShellWidgetClass, arglist, argcount);
  XtAddCallback(toplevel, XmNdestroyCallback, (XtCallbackProc) DestroyCbk, (XtPointer) &toplevel);
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
    void create_gui(Widget toplevel);
    (void) create_gui(toplevel);
  }
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
XtAppContext appcontext = NULL;

typedef struct {
  Widget shell;
  Widget main_window;
  Widget menu_bar;
  Widget file_pulldown;
  Widget file;
  Widget insert;
  Widget remove;
  Widget sep1;
  Widget open;
  Widget save;
  Widget sep2;
  Widget exit;
  Widget controls_pulldown;
  Widget controls;
  Widget play;
  Widget pause;
  Widget reset;
  Widget help_pulldown;
  Widget help;
  Widget about;
  Widget screen;
} GUI;

void xcpc_popup_cbk(Widget widget)
{
  paused = 1;
  XtManageChild(widget);
}

void xcpc_popdown_cbk(Widget widget)
{
  while(XtIsShell(widget) == False) {
    widget = XtParent(widget);
  }
  XtDestroyWidget(widget);
  paused = 0;
}

void xcpc_cancel_cbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  xcpc_popdown_cbk(widget);
}

void xcpc_insert_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  fprintf(stderr, "xcpc_insert_cbk\n");
}

void xcpc_remove_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  fprintf(stderr, "xcpc_remove_cbk\n");
}

void xcpc_open_ok_cbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
char *filename;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);
  amstrad_cpc_load_snapshot(filename);
  XtFree(filename);
  xcpc_popdown_cbk(widget);
}

void xcpc_open_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal argcount;
Arg arglist[1];
Widget shell, dialog;

  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  shell = XmCreateDialogShell(gui->shell, "open_shell", arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  dialog = XmCreateFileSelectionBox(shell, "open_dialog", arglist, argcount);
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xcpc_open_ok_cbk, NULL);
  XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup_cbk(shell);
}

void xcpc_save_ok_cbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
char *filename;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);
  amstrad_cpc_save_snapshot(filename);
  XtFree(filename);
  xcpc_popdown_cbk(widget);
}

void xcpc_save_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal argcount;
Arg arglist[1];
Widget shell, dialog;

  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  shell = XmCreateDialogShell(gui->shell, "save_shell", arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  dialog = XmCreateFileSelectionBox(shell, "save_dialog", arglist, argcount);
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xcpc_save_ok_cbk, NULL);
  XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup_cbk(shell);
}

void xcpc_exit_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  exit(0);
}

void xcpc_play_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  paused = 0;
}

void xcpc_pause_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  paused = 1;
}

void xcpc_reset_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  amstrad_cpc_reset();
  paused = 0;
}

void xcpc_about_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal argcount;
Arg arglist[1];
Widget shell, dialog;

  argcount = 0;
  XtSetArg(arglist[argcount], XmNdeleteResponse, XmDESTROY); argcount++;
  shell = XmCreateDialogShell(gui->shell, "about_shell", arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); argcount++;
  dialog = XmCreateMessageBox(shell, "about_dialog", arglist, argcount);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup_cbk(shell);
}

void create_gui(Widget toplevel)
{
GUI *gui;
Cardinal argcount;
Arg arglist[8];

  gui = (GUI *) XtMalloc(sizeof(GUI));

  gui->shell = toplevel;
  gui->main_window = XmCreateMainWindow(gui->shell, "main_window", NULL, 0);
  XtManageChild(gui->main_window);
  gui->menu_bar = XmCreateMenuBar(gui->main_window, "menu_bar", NULL, 0);
  XtManageChild(gui->menu_bar);
  gui->file_pulldown = XmCreatePulldownMenu(gui->menu_bar, "file_pulldown", NULL, 0);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->file_pulldown); argcount++;
  gui->file = XmCreateCascadeButtonGadget(gui->menu_bar, "file", arglist, argcount);
  XtManageChild(gui->file);
  gui->insert = XmCreatePushButtonGadget(gui->file_pulldown, "insert", NULL, 0);
  XtManageChild(gui->insert);
  XtAddCallback(gui->insert, XmNactivateCallback, (XtCallbackProc) xcpc_insert_cbk, gui);
  gui->remove = XmCreatePushButtonGadget(gui->file_pulldown, "remove", NULL, 0);
  XtManageChild(gui->remove);
  XtAddCallback(gui->remove, XmNactivateCallback, (XtCallbackProc) xcpc_remove_cbk, gui);
  gui->sep1 = XmCreateSeparatorGadget(gui->file_pulldown, "sep2", NULL, 0);
  XtManageChild(gui->sep1);
  gui->open = XmCreatePushButtonGadget(gui->file_pulldown, "open", NULL, 0);
  XtManageChild(gui->open);
  XtAddCallback(gui->open, XmNactivateCallback, (XtCallbackProc) xcpc_open_cbk, gui);
  gui->save = XmCreatePushButtonGadget(gui->file_pulldown, "save", NULL, 0);
  XtManageChild(gui->save);
  XtAddCallback(gui->save, XmNactivateCallback, (XtCallbackProc) xcpc_save_cbk, gui);
  gui->sep2 = XmCreateSeparatorGadget(gui->file_pulldown, "sep2", NULL, 0);
  XtManageChild(gui->sep2);
  gui->exit = XmCreatePushButtonGadget(gui->file_pulldown, "exit", NULL, 0);
  XtManageChild(gui->exit);
  XtAddCallback(gui->exit, XmNactivateCallback, (XtCallbackProc) xcpc_exit_cbk, gui);
  gui->controls_pulldown = XmCreatePulldownMenu(gui->menu_bar, "controls_pulldown", NULL, 0);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->controls_pulldown); argcount++;
  gui->controls = XmCreateCascadeButtonGadget(gui->menu_bar, "controls", arglist, argcount);
  XtManageChild(gui->controls);
  gui->play = XmCreatePushButtonGadget(gui->controls_pulldown, "play", NULL, 0);
  XtManageChild(gui->play);
  XtAddCallback(gui->play, XmNactivateCallback, (XtCallbackProc) xcpc_play_cbk, gui);
  gui->pause = XmCreatePushButtonGadget(gui->controls_pulldown, "pause", NULL, 0);
  XtManageChild(gui->pause);
  XtAddCallback(gui->pause, XmNactivateCallback, (XtCallbackProc) xcpc_pause_cbk, gui);
  gui->reset = XmCreatePushButtonGadget(gui->controls_pulldown, "reset", NULL, 0);
  XtManageChild(gui->reset);
  XtAddCallback(gui->reset, XmNactivateCallback, (XtCallbackProc) xcpc_reset_cbk, gui);
  gui->help_pulldown = XmCreatePulldownMenu(gui->menu_bar, "help_pulldown", NULL, 0);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNsubMenuId, gui->help_pulldown); argcount++;
  gui->help = XmCreateCascadeButtonGadget(gui->menu_bar, "help", arglist, argcount);
  XtManageChild(gui->help);
  argcount = 0;
  XtSetArg(arglist[argcount], XmNmenuHelpWidget, gui->help); argcount++;
  XtSetValues(gui->menu_bar, arglist, argcount);
  gui->about = XmCreatePushButtonGadget(gui->help_pulldown, "about", NULL, 0);
  XtManageChild(gui->about);
  XtAddCallback(gui->about, XmNactivateCallback, (XtCallbackProc) xcpc_about_cbk, gui);

  argcount = 0;
  XtSetArg(arglist[argcount], XtNemuStartHandler, amstrad_cpc_start_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuClockHandler, amstrad_cpc_clock_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuCloseHandler, amstrad_cpc_close_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuKeybdHandler, amstrad_cpc_keybd_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuMouseHandler, amstrad_cpc_mouse_handler); argcount++;
  XtSetArg(arglist[argcount], XtNemuPaintHandler, amstrad_cpc_paint_handler); argcount++;
  gui->screen = XAreaCreate(gui->main_window, "screen", arglist, argcount);
  XtManageChild(gui->screen);
}
