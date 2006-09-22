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
  "*menu_bar.borderWidth:     0",
  "*menu_bar.orientation:     horizontal",
  "*screen.width:             656",
  "*screen.height:            416",
  "*title:                    XCPC - Amstrad CPC emulator",
  "*menu_bar*file.label:      File",
  "*menu_bar*insert.label:    Insert floppy ...",
  "*menu_bar*remove.label:    Remove floppy",
  "*menu_bar*open.label:      Open a snapshot ...",
  "*menu_bar*save.label:      Save a snapshot ...",
  "*menu_bar*exit.label:      Exit",
  "*menu_bar*controls.label:  Controls",
  "*menu_bar*play.label:      Play",
  "*menu_bar*pause.label:     Pause",
  "*menu_bar*reset.label:     Reset",
  "*menu_bar*help.label:      Help",
  "*menu_bar*about.label:     About ...",
  "*open_shell.title:         Open a snapshot ...",
  "*open_dialog.label:        Enter filename:",
  "*open_dialog*ok.label:     Open",
  "*open_dialog*cancel.label: Cancel",
  "*save_shell.title:         Save a snapshot ...",
  "*save_dialog.label:        Enter filename:",
  "*save_dialog*ok.label:     Save",
  "*save_dialog*cancel.label: Cancel",
  "*about_shell.title:        About ...",
  "*about_dialog.label:       XCPC - Athena version\\nAmstrad CPC emulator for UNIX\\nCoded by Olivier Poncet <ponceto@noos.fr>",
  "*about_dialog*ok.label:    Close",
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
    (void) fprintf(stderr, "            [-CTM65] [-CTM644]\n");
    (void) fprintf(stderr, "            [-1.6MHz] [-2MHz] [-3.3MHz] [-4MHz]\n");
    (void) fprintf(stderr, "            [-6.6MHz] [-8MHz] [-9.9MHz] [-16MHz]\n");
    (void) fprintf(stderr, "            [-50Hz] [-60Hz]\n");
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
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Dialog.h>

XtAppContext appcontext;

Atom XA_WM_DELETE_WINDOW = None;

typedef struct {
  Widget shell;
  Widget main_window;
  Widget menu_bar;
  Widget file;
  Widget file_menu;
  Widget insert;
  Widget remove;
  Widget sep1;
  Widget open;
  Widget save;
  Widget sep2;
  Widget exit;
  Widget controls;
  Widget controls_menu;
  Widget play;
  Widget pause;
  Widget reset;
  Widget help;
  Widget help_menu;
  Widget about;
  Widget screen;
} GUI;

static String resources[] = {
  NULL
};

void xcpc_popup(Widget widget, XtGrabKind grab_kind)
{
Cardinal argcount;
Arg arglist[4];
Position x, y;
Dimension w1, h1, w2, h2;

  paused = 1;
  argcount = 0;
  XtSetArg(arglist[argcount], XtNx, &x); argcount++;
  XtSetArg(arglist[argcount], XtNy, &y); argcount++;
  XtSetArg(arglist[argcount], XtNwidth, &w1); argcount++;
  XtSetArg(arglist[argcount], XtNheight, &h1); argcount++;
  XtGetValues(XtParent(widget), arglist, argcount);
  XtOverrideTranslations(widget, XtParseTranslationTable("<Message>WM_PROTOCOLS: XcpcWMClose()"));
  XtRealizeWidget(widget);
  XSetWMProtocols(XtDisplay(widget), XtWindow(widget), &XA_WM_DELETE_WINDOW, 1);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNwidth, &w2); argcount++;
  XtSetArg(arglist[argcount], XtNheight, &h2); argcount++;
  XtGetValues(widget, arglist, argcount);
  argcount = 0;
  XtSetArg(arglist[argcount], XtNx, x + ((w1 - w2) / 2)); argcount++;
  XtSetArg(arglist[argcount], XtNy, y + ((h1 - h2) / 2)); argcount++;
  XtSetValues(widget, arglist, argcount);
  XtPopup(widget, grab_kind);
}

void xcpc_popdown(Widget widget)
{
  while(XtIsShell(widget) == False) {
    widget = XtParent(widget);
  }
  XtPopdown(widget);
  XtDestroyWidget(widget);
  paused = 0;
}

void xcpc_cancel_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  xcpc_popdown(widget);
}

void xcpc_insert_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  fprintf(stderr, "xcpc_insert_cbk\n");
}

void xcpc_remove_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  fprintf(stderr, "xcpc_remove_cbk\n");
}

void xcpc_open_ok_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
char *value;

  value = XawDialogGetValueString(XtParent(widget));
  amstrad_cpc_load_snapshot(value);
  /*XtFree(value);*/
  xcpc_popdown(widget);
}

void xcpc_open_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("open_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("open_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xcpc_open_ok_cbk, NULL);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup(shell, XtGrabExclusive);
}

void xcpc_save_ok_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
char *value;

  value = XawDialogGetValueString(XtParent(widget));
  amstrad_cpc_save_snapshot(value);
  /*XtFree(value);*/
  xcpc_popdown(widget);
}

void xcpc_save_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("save_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("save_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xcpc_save_ok_cbk, NULL);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup(shell, XtGrabExclusive);
}

void xcpc_exit_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  exit(0);
}

void xcpc_play_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  paused = 0;
}

void xcpc_pause_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  paused = 1;
}

void xcpc_reset_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  amstrad_cpc_reset();
  paused = 0;
}

void xcpc_about_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("about_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("about_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xcpc_cancel_cbk, NULL);
  xcpc_popup(shell, XtGrabExclusive);
}

void create_gui(Widget toplevel)
{
GUI *gui;
Cardinal argcount;
Arg arglist[8];

  gui = (GUI *) XtMalloc(sizeof(GUI));

  gui->shell = toplevel;
  XA_WM_DELETE_WINDOW = XInternAtom(XtDisplay(gui->shell), "WM_DELETE_WINDOW", False);
  gui->main_window = XtCreateManagedWidget("main_window", boxWidgetClass, gui->shell, NULL, 0);
  gui->menu_bar = XtCreateManagedWidget("menu_bar", boxWidgetClass, gui->main_window, NULL, 0);
  gui->file = XtCreateManagedWidget("file", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->file_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->file, NULL, 0);
  gui->insert = XtCreateManagedWidget("insert", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->insert, XtNcallback, (XtCallbackProc) xcpc_insert_cbk, gui);
  gui->remove = XtCreateManagedWidget("remove", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->remove, XtNcallback, (XtCallbackProc) xcpc_remove_cbk, gui);
  gui->sep1 = XtCreateManagedWidget("sep1", smeLineObjectClass, gui->file_menu, NULL, 0);
  gui->open = XtCreateManagedWidget("open", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->open, XtNcallback, (XtCallbackProc) xcpc_open_cbk, gui);
  gui->save = XtCreateManagedWidget("save", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->save, XtNcallback, (XtCallbackProc) xcpc_save_cbk, gui);
  gui->sep2 = XtCreateManagedWidget("sep2", smeLineObjectClass, gui->file_menu, NULL, 0);
  gui->exit = XtCreateManagedWidget("exit", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->exit, XtNcallback, (XtCallbackProc) xcpc_exit_cbk, gui);
  gui->controls = XtCreateManagedWidget("controls", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->controls_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->controls, NULL, 0);
  gui->play = XtCreateManagedWidget("play", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->play, XtNcallback, (XtCallbackProc) xcpc_play_cbk, gui);
  gui->pause = XtCreateManagedWidget("pause", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->pause, XtNcallback, (XtCallbackProc) xcpc_pause_cbk, gui);
  gui->reset = XtCreateManagedWidget("reset", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->reset, XtNcallback, (XtCallbackProc) xcpc_reset_cbk, gui);
  gui->help = XtCreateManagedWidget("help", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->help_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->help, NULL, 0);
  gui->about = XtCreateManagedWidget("about", smeBSBObjectClass, gui->help_menu, NULL, 0);
  XtAddCallback(gui->about, XtNcallback, (XtCallbackProc) xcpc_about_cbk, gui);

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
