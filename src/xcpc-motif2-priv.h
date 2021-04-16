/*
 * xcpc-motif2-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MOTIF2_PRIV_H__
#define __XCPC_MOTIF2_PRIV_H__

#include <Xm/XmAll.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include <xcpc/machine/machine.h>
#include "xcpc-motif2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcResourcesRec   XcpcResourcesRec;
typedef struct _XcpcPixmapsRec     XcpcPixmapsRec;
typedef struct _XcpcFileMenuRec    XcpcFileMenuRec;
typedef struct _XcpcDrv0MenuRec    XcpcDrv0MenuRec;
typedef struct _XcpcDrv1MenuRec    XcpcDrv1MenuRec;
typedef struct _XcpcCtrlMenuRec    XcpcCtrlMenuRec;
typedef struct _XcpcHelpMenuRec    XcpcHelpMenuRec;
typedef struct _XcpcLayoutRec      XcpcLayoutRec;
typedef struct _XcpcMenuBarRec     XcpcMenuBarRec;
typedef struct _XcpcToolBarRec     XcpcToolBarRec;
typedef struct _XcpcInfoBarRec     XcpcInfoBarRec;
typedef struct _XcpcApplicationRec XcpcApplicationRec;

struct _XcpcResourcesRec
{
    String  app_name;
    String  app_class;
    Boolean quiet_flag;
    Boolean trace_flag;
    Boolean debug_flag;
};

struct _XcpcPixmapsRec
{
    Pixel  foreground;
    Pixel  background;
    Pixmap null_icon;
    Pixmap xcpc_icon;
    Pixmap xcpc_mask;
    Pixmap file_load;
    Pixmap file_save;
    Pixmap file_exit;
    Pixmap ctrl_play;
    Pixmap ctrl_pause;
    Pixmap ctrl_reset;
    Pixmap disk_insert;
    Pixmap disk_remove;
    Pixmap help_legal;
    Pixmap help_about;
    Pixmap help_help;
};

struct _XcpcFileMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget load_snapshot;
    Widget save_snapshot;
    Widget separator1;
    Widget exit;
};

struct _XcpcDrv0MenuRec
{
    Widget menu;
    Widget pulldown;
    Widget drive0_insert;
    Widget drive0_remove;
};

struct _XcpcDrv1MenuRec
{
    Widget menu;
    Widget pulldown;
    Widget drive1_insert;
    Widget drive1_remove;
};

struct _XcpcCtrlMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget play_emulator;
    Widget pause_emulator;
    Widget reset_emulator;
};

struct _XcpcHelpMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget legal;
    Widget about;
    Widget separator1;
    Widget help;
};

struct _XcpcLayoutRec
{
    Widget toplevel;
    Widget window;
    Widget emulator;
};

struct _XcpcMenuBarRec
{
    Widget widget;
    XcpcFileMenuRec file;
    XcpcCtrlMenuRec ctrl;
    XcpcDrv0MenuRec drv0;
    XcpcDrv1MenuRec drv1;
    XcpcHelpMenuRec help;
};

struct _XcpcToolBarRec
{
    Widget widget;
    Widget load_snapshot;
    Widget save_snapshot;
    Widget play_emulator;
    Widget pause_emulator;
    Widget reset_emulator;
};

struct _XcpcInfoBarRec
{
    Widget widget;
    Widget status;
    Widget drive0;
    Widget drive1;
    Widget system;
};

struct _XcpcApplicationRec
{
    XcpcMachine*     machine;
    XtAppContext     appcontext;
    XcpcResourcesRec resources;
    XcpcPixmapsRec   pixmaps;
    XcpcLayoutRec    layout;
    XcpcMenuBarRec   menubar;
    XcpcToolBarRec   toolbar;
    XcpcInfoBarRec   infobar;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MOTIF2_PRIV_H__ */
