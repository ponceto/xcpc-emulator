#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include "XArea.h"
#include "config.h"
#include "amstrad_cpc.h"
#include "xcpc.h"

XtAppContext appcontext;
X11 x11;

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

static String resources[] = {
/*
 *
 */
  "*about_dialog.dialogType:       XmDIALOG_QUESTION",
  "*about_dialog.messageAlignment: XmALIGNMENT_CENTER",
/*
 * strings
 */
  "*title:                         XmCPC - Amstrad CPC emulator",
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
  "*open_dialog.directory:         snap/",
  "*open_dialog.okLabelString:     Open",
  "*save_dialog.dialogTitle:       Save a snapshot ...",
  "*save_dialog.directory:         snap/",
  "*save_dialog.okLabelString:     Save",
  "*about_dialog.dialogTitle:      About ...",
  "*about_dialog.messageString:    XCPC - Motif version\\nAmstrad CPC emulator for UNIX",
  "*about_dialog.okLabelString:    Close",
/*
 *
 */
  NULL
};

void xmcpc_popup_cbk(Widget widget)
{
  paused = 1;
  XtManageChild(widget);
}

void xmcpc_popdown_cbk(Widget widget)
{
  while(XtIsShell(widget) == False) {
    widget = XtParent(widget);
  }
  XtDestroyWidget(widget);
  paused = 0;
}

void xmcpc_cancel_cbk(Widget widget, GUI *gui, XmAnyCallbackStruct *cbs)
{
  xmcpc_popdown_cbk(widget);
}

void xmcpc_insert_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  fprintf(stderr, "xmcpc_insert_cbk\n");
}

void xmcpc_remove_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  fprintf(stderr, "xmcpc_remove_cbk\n");
}

void xmcpc_open_ok_cbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
char *filename;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);
  amstrad_cpc_load_snapshot(filename);
  XtFree(filename);
  xmcpc_popdown_cbk(widget);
}

void xmcpc_open_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal wargc;
Arg wargv[1];
Widget shell, dialog;

  wargc = 0;
  XtSetArg(wargv[wargc], XmNdeleteResponse, XmDESTROY); wargc++;
  shell = XmCreateDialogShell(gui->shell, "open_shell", wargv, wargc);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); wargc++;
  dialog = XmCreateFileSelectionBox(shell, "open_dialog", wargv, wargc);
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xmcpc_open_ok_cbk, NULL);
  XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc) xmcpc_cancel_cbk, NULL);
  xmcpc_popup_cbk(shell);
}

void xmcpc_save_ok_cbk(Widget widget, GUI *gui, XmFileSelectionBoxCallbackStruct *cbs)
{
char *filename;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename);
  amstrad_cpc_save_snapshot(filename);
  XtFree(filename);
  xmcpc_popdown_cbk(widget);
}

void xmcpc_save_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal wargc;
Arg wargv[1];
Widget shell, dialog;

  wargc = 0;
  XtSetArg(wargv[wargc], XmNdeleteResponse, XmDESTROY); wargc++;
  shell = XmCreateDialogShell(gui->shell, "save_shell", wargv, wargc);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); wargc++;
  dialog = XmCreateFileSelectionBox(shell, "save_dialog", wargv, wargc);
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xmcpc_save_ok_cbk, NULL);
  XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc) xmcpc_cancel_cbk, NULL);
  xmcpc_popup_cbk(shell);
}

void xmcpc_exit_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  exit(0);
}

void xmcpc_play_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  paused = 0;
}

void xmcpc_pause_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  paused = 1;
}

void xmcpc_reset_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
  amstrad_cpc_reset();
  paused = 0;
}

void xmcpc_about_cbk(Widget widget, GUI *gui, XmPushButtonCallbackStruct *cbs)
{
Cardinal wargc;
Arg wargv[1];
Widget shell, dialog;

  wargc = 0;
  XtSetArg(wargv[wargc], XmNdeleteResponse, XmDESTROY); wargc++;
  shell = XmCreateDialogShell(gui->shell, "about_shell", wargv, wargc);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); wargc++;
  dialog = XmCreateMessageBox(shell, "about_dialog", wargv, wargc);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtManageChild(dialog);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) xmcpc_cancel_cbk, NULL);
  xmcpc_popup_cbk(shell);
}

int main(int argc, char **argv)
{
GUI *gui;
Cardinal wargc;
Arg wargv[2];
int cx, cy, ix;

  gui = (GUI *) XtMalloc(sizeof(GUI));

  gui->shell = XtAppInitialize(&appcontext, "XmCPC", NULL, 0, &argc, argv, resources, NULL, 0);
  amstrad_cpc_parse(argc, argv);
  gui->main_window = XmCreateMainWindow(gui->shell, "main_window", NULL, 0);
  XtManageChild(gui->main_window);
  gui->menu_bar = XmCreateMenuBar(gui->main_window, "menu_bar", NULL, 0);
  XtManageChild(gui->menu_bar);
  gui->file_pulldown = XmCreatePulldownMenu(gui->menu_bar, "file_pulldown", NULL, 0);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNsubMenuId, gui->file_pulldown); wargc++;
  gui->file = XmCreateCascadeButtonGadget(gui->menu_bar, "file", wargv, wargc);
  XtManageChild(gui->file);
  gui->insert = XmCreatePushButtonGadget(gui->file_pulldown, "insert", NULL, 0);
  XtManageChild(gui->insert);
  XtAddCallback(gui->insert, XmNactivateCallback, (XtCallbackProc) xmcpc_insert_cbk, gui);
  gui->remove = XmCreatePushButtonGadget(gui->file_pulldown, "remove", NULL, 0);
  XtManageChild(gui->remove);
  XtAddCallback(gui->remove, XmNactivateCallback, (XtCallbackProc) xmcpc_remove_cbk, gui);
  gui->sep1 = XmCreateSeparatorGadget(gui->file_pulldown, "sep2", NULL, 0);
  XtManageChild(gui->sep1);
  gui->open = XmCreatePushButtonGadget(gui->file_pulldown, "open", NULL, 0);
  XtManageChild(gui->open);
  XtAddCallback(gui->open, XmNactivateCallback, (XtCallbackProc) xmcpc_open_cbk, gui);
  gui->save = XmCreatePushButtonGadget(gui->file_pulldown, "save", NULL, 0);
  XtManageChild(gui->save);
  XtAddCallback(gui->save, XmNactivateCallback, (XtCallbackProc) xmcpc_save_cbk, gui);
  gui->sep2 = XmCreateSeparatorGadget(gui->file_pulldown, "sep2", NULL, 0);
  XtManageChild(gui->sep2);
  gui->exit = XmCreatePushButtonGadget(gui->file_pulldown, "exit", NULL, 0);
  XtManageChild(gui->exit);
  XtAddCallback(gui->exit, XmNactivateCallback, (XtCallbackProc) xmcpc_exit_cbk, gui);
  gui->controls_pulldown = XmCreatePulldownMenu(gui->menu_bar, "controls_pulldown", NULL, 0);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNsubMenuId, gui->controls_pulldown); wargc++;
  gui->controls = XmCreateCascadeButtonGadget(gui->menu_bar, "controls", wargv, wargc);
  XtManageChild(gui->controls);
  gui->play = XmCreatePushButtonGadget(gui->controls_pulldown, "play", NULL, 0);
  XtManageChild(gui->play);
  XtAddCallback(gui->play, XmNactivateCallback, (XtCallbackProc) xmcpc_play_cbk, gui);
  gui->pause = XmCreatePushButtonGadget(gui->controls_pulldown, "pause", NULL, 0);
  XtManageChild(gui->pause);
  XtAddCallback(gui->pause, XmNactivateCallback, (XtCallbackProc) xmcpc_pause_cbk, gui);
  gui->reset = XmCreatePushButtonGadget(gui->controls_pulldown, "reset", NULL, 0);
  XtManageChild(gui->reset);
  XtAddCallback(gui->reset, XmNactivateCallback, (XtCallbackProc) xmcpc_reset_cbk, gui);
  gui->help_pulldown = XmCreatePulldownMenu(gui->menu_bar, "help_pulldown", NULL, 0);
  wargc = 0;
  XtSetArg(wargv[wargc], XmNsubMenuId, gui->help_pulldown); wargc++;
  gui->help = XmCreateCascadeButtonGadget(gui->menu_bar, "help", wargv, wargc);
  XtManageChild(gui->help);
  gui->about = XmCreatePushButtonGadget(gui->help_pulldown, "about", NULL, 0);
  XtManageChild(gui->about);
  XtAddCallback(gui->about, XmNactivateCallback, (XtCallbackProc) xmcpc_about_cbk, gui);
  XtVaSetValues(gui->menu_bar, XmNmenuHelpWidget, gui->help, NULL);
  wargc = 0;
  XtSetArg(wargv[wargc], XtNwidth, amstrad_cpc_width); wargc++;
  XtSetArg(wargv[wargc], XtNheight, amstrad_cpc_height); wargc++;
  gui->screen = XtCreateXArea(gui->main_window, "screen", wargv, wargc);
  XtManageChild(gui->screen);
  XtAddCallback(gui->screen, XtNkeyPressCallback, (XtCallbackProc) amstrad_cpc_key_press, gui);
  XtAddCallback(gui->screen, XtNkeyReleaseCallback, (XtCallbackProc) amstrad_cpc_key_release, gui);
  XtAddCallback(gui->screen, XtNexposeCallback, (XtCallbackProc) amstrad_cpc_expose, gui);
  XtRealizeWidget(gui->shell);

  x11.screen = XtScreen(gui->screen);
  x11.display = DisplayOfScreen(x11.screen);
  x11.window = XtWindow(gui->screen);
  x11.gc = XCreateGC(x11.display, x11.window, 0L, NULL);
  if((x11.ximage = XCreateImage(x11.display, DefaultVisualOfScreen(x11.screen), DefaultDepthOfScreen(x11.screen), ZPixmap, 0, NULL, amstrad_cpc_width, amstrad_cpc_height, 8, 0)) == NULL) {
    perror("xmcpc");
    exit(-1);
  }
  x11.ximage->data = (char *) XtMalloc(x11.ximage->bytes_per_line * x11.ximage->height);
  for(cy = 0; cy < x11.ximage->height; cy++) {
    for(cx = 0; cx < x11.ximage->width; cx++) {
      XPutPixel(x11.ximage, cx, cy, BlackPixelOfScreen(x11.screen));
    }
  }

  amstrad_cpc_init();
  amstrad_cpc_run();
  amstrad_cpc_exit();

  return(0);
}
