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
#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Dialog.h>
#include "XArea.h"
#include "common.h"
#include "amstrad_cpc.h"
#include "xcpc.h"

XtAppContext appcontext;
Widget xarea;

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
/*
 *
 */
  "*menu_bar.borderWidth:     0",
  "*menu_bar.orientation:     horizontal",
  "*screen.width:             656",
  "*screen.height:            416",
/*
 * strings
 */
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
/*
 *
 */
  NULL
};

void xawcpc_close(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  if((event->type == ClientMessage) && (event->xclient.data.l[0] == XA_WM_DELETE_WINDOW)) {
    XtDestroyWidget(widget);
    paused = 0;
  }
}

void xawcpc_quit(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  if((event->type == ClientMessage) && (event->xclient.data.l[0] == XA_WM_DELETE_WINDOW)) {
    exit(0);
  }
}

static XtActionsRec actions[] = {
  { "xawcpc_close", xawcpc_close },
  { "xawcpc_quit",  xawcpc_quit  },
};

void xawcpc_popup(Widget widget, XtGrabKind grab_kind)
{
Cardinal wargc;
Arg wargv[4];
Position x, y;
Dimension w1, h1, w2, h2;

  paused = 1;
  wargc = 0;
  XtSetArg(wargv[wargc], XtNx, &x); wargc++;
  XtSetArg(wargv[wargc], XtNy, &y); wargc++;
  XtSetArg(wargv[wargc], XtNwidth, &w1); wargc++;
  XtSetArg(wargv[wargc], XtNheight, &h1); wargc++;
  XtGetValues(XtParent(widget), wargv, wargc);
  XtOverrideTranslations(widget, XtParseTranslationTable("<Message>WM_PROTOCOLS: xawcpc_close()"));
  XtRealizeWidget(widget);
  XSetWMProtocols(XtDisplay(widget), XtWindow(widget), &XA_WM_DELETE_WINDOW, 1);
  wargc = 0;
  XtSetArg(wargv[wargc], XtNwidth, &w2); wargc++;
  XtSetArg(wargv[wargc], XtNheight, &h2); wargc++;
  XtGetValues(widget, wargv, wargc);
  wargc = 0;
  XtSetArg(wargv[wargc], XtNx, x + ((w1 - w2) / 2)); wargc++;
  XtSetArg(wargv[wargc], XtNy, y + ((h1 - h2) / 2)); wargc++;
  XtSetValues(widget, wargv, wargc);
  XtPopup(widget, grab_kind);
}

void xawcpc_popdown(Widget widget)
{
  while(XtIsShell(widget) == False) {
    widget = XtParent(widget);
  }
  XtPopdown(widget);
  XtDestroyWidget(widget);
  paused = 0;
}

void xawcpc_cancel_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  xawcpc_popdown(widget);
}

void xawcpc_insert_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  fprintf(stderr, "xawcpc_insert_cbk\n");
}

void xawcpc_remove_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  fprintf(stderr, "xawcpc_remove_cbk\n");
}

void xawcpc_open_ok_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
char *value;

  value = XawDialogGetValueString(XtParent(widget));
  amstrad_cpc_load_snapshot(value);
  /*XtFree(value);*/
  xawcpc_popdown(widget);
}

void xawcpc_open_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("open_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("open_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xawcpc_open_ok_cbk, NULL);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) xawcpc_cancel_cbk, NULL);
  xawcpc_popup(shell, XtGrabExclusive);
}

void xawcpc_save_ok_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
char *value;

  value = XawDialogGetValueString(XtParent(widget));
  amstrad_cpc_save_snapshot(value);
  /*XtFree(value);*/
  xawcpc_popdown(widget);
}

void xawcpc_save_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("save_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("save_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xawcpc_save_ok_cbk, NULL);
  XawDialogAddButton(dialog, "cancel", (XtCallbackProc) xawcpc_cancel_cbk, NULL);
  xawcpc_popup(shell, XtGrabExclusive);
}

void xawcpc_exit_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  exit(0);
}

void xawcpc_play_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  paused = 0;
}

void xawcpc_pause_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  paused = 1;
}

void xawcpc_reset_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
  amstrad_cpc_reset();
  paused = 0;
}

void xawcpc_about_cbk(Widget widget, GUI *gui, XtPointer *cbs)
{
Widget shell, dialog;

  shell = XtCreatePopupShell("about_shell", transientShellWidgetClass, gui->shell, NULL, 0);
  dialog = XtCreateManagedWidget("about_dialog", dialogWidgetClass, shell, NULL, 0);
  XawDialogAddButton(dialog, "ok", (XtCallbackProc) xawcpc_cancel_cbk, NULL);
  xawcpc_popup(shell, XtGrabExclusive);
}

int main(int argc, char **argv)
{
GUI *gui;
Cardinal wargc;
Arg wargv[1];
int status;

  gui = (GUI *) XtMalloc(sizeof(GUI));

  wargc = 0;
  XtSetArg(wargv[wargc], XtNallowShellResize, True); wargc++;
  gui->shell = XtAppInitialize(&appcontext, "XawCPC", NULL, 0, &argc, argv, resources, wargv, wargc);
  XA_WM_DELETE_WINDOW = XInternAtom(XtDisplay(gui->shell), "WM_DELETE_WINDOW", False);
  XtAppAddActions(appcontext, actions, XtNumber(actions));
  gui->main_window = XtCreateManagedWidget("main_window", boxWidgetClass, gui->shell, NULL, 0);
  gui->menu_bar = XtCreateManagedWidget("menu_bar", boxWidgetClass, gui->main_window, NULL, 0);
  gui->file = XtCreateManagedWidget("file", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->file_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->file, NULL, 0);
  gui->insert = XtCreateManagedWidget("insert", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->insert, XtNcallback, (XtCallbackProc) xawcpc_insert_cbk, gui);
  gui->remove = XtCreateManagedWidget("remove", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->remove, XtNcallback, (XtCallbackProc) xawcpc_remove_cbk, gui);
  gui->sep1 = XtCreateManagedWidget("sep1", smeLineObjectClass, gui->file_menu, NULL, 0);
  gui->open = XtCreateManagedWidget("open", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->open, XtNcallback, (XtCallbackProc) xawcpc_open_cbk, gui);
  gui->save = XtCreateManagedWidget("save", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->save, XtNcallback, (XtCallbackProc) xawcpc_save_cbk, gui);
  gui->sep2 = XtCreateManagedWidget("sep2", smeLineObjectClass, gui->file_menu, NULL, 0);
  gui->exit = XtCreateManagedWidget("exit", smeBSBObjectClass, gui->file_menu, NULL, 0);
  XtAddCallback(gui->exit, XtNcallback, (XtCallbackProc) xawcpc_exit_cbk, gui);
  gui->controls = XtCreateManagedWidget("controls", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->controls_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->controls, NULL, 0);
  gui->play = XtCreateManagedWidget("play", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->play, XtNcallback, (XtCallbackProc) xawcpc_play_cbk, gui);
  gui->pause = XtCreateManagedWidget("pause", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->pause, XtNcallback, (XtCallbackProc) xawcpc_pause_cbk, gui);
  gui->reset = XtCreateManagedWidget("reset", smeBSBObjectClass, gui->controls_menu, NULL, 0);
  XtAddCallback(gui->reset, XtNcallback, (XtCallbackProc) xawcpc_reset_cbk, gui);
  gui->help = XtCreateManagedWidget("help", menuButtonWidgetClass, gui->menu_bar, NULL, 0);
  gui->help_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, gui->help, NULL, 0);
  gui->about = XtCreateManagedWidget("about", smeBSBObjectClass, gui->help_menu, NULL, 0);
  XtAddCallback(gui->about, XtNcallback, (XtCallbackProc) xawcpc_about_cbk, gui);
  xarea = gui->screen = XtCreateManagedWidget("screen", xAreaWidgetClass, gui->main_window, NULL, 0);
  XtOverrideTranslations(gui->shell, XtParseTranslationTable("<Message>WM_PROTOCOLS: xawcpc_quit()"));
  XtRealizeWidget(gui->shell);
  XSetWMProtocols(XtDisplay(gui->shell), XtWindow(gui->shell), &XA_WM_DELETE_WINDOW, 1);

  status = amstrad_cpc_main(argc, argv);

  return(status);
}
