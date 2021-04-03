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

#include <xcpc/machine/machine.h>
#include "xcpc-motif2.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _
#define _(string) (string)
#endif

typedef struct _XcpcResourcesRec
{
    String  appname;
    String  appclass;
    Boolean quiet_flag;
    Boolean trace_flag;
    Boolean debug_flag;
} XcpcResourcesRec;

typedef struct _XcpcPixmapsRec
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
} XcpcPixmapsRec;

typedef struct _XcpcFileMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget load_snapshot;
    Widget save_snapshot;
    Widget separator1;
    Widget exit;
} XcpcFileMenuRec;

typedef struct _XcpcDrv0MenuRec
{
    Widget menu;
    Widget pulldown;
    Widget drive0_insert;
    Widget drive0_remove;
} XcpcDrv0MenuRec;

typedef struct _XcpcDrv1MenuRec
{
    Widget menu;
    Widget pulldown;
    Widget drive1_insert;
    Widget drive1_remove;
} XcpcDrv1MenuRec;

typedef struct _XcpcCtrlMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget pause_emulator;
    Widget reset_emulator;
} XcpcCtrlMenuRec;

typedef struct _XcpcHelpMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget legal;
    Widget separator1;
    Widget about;
} XcpcHelpMenuRec;

typedef struct _XcpcLayoutRec
{
    Widget toplevel;
    Widget window;
    Widget emulator;
} XcpcLayoutRec;

typedef struct _XcpcMenuBarRec
{
    Widget widget;
    XcpcFileMenuRec file;
    XcpcCtrlMenuRec ctrl;
    XcpcDrv0MenuRec drv0;
    XcpcDrv1MenuRec drv1;
    XcpcHelpMenuRec help;
} XcpcMenuBarRec;

typedef struct _XcpcToolBarRec
{
    Widget widget;
    Widget load_snapshot;
    Widget save_snapshot;
    Widget pause_emulator;
    Widget reset_emulator;
} XcpcToolBarRec;

typedef struct _XcpcApplicationRec
{
    FILE*            input_stream;
    FILE*            print_stream;
    FILE*            error_stream;
    XcpcMachine*     machine;
    XtAppContext     appcontext;
    XcpcResourcesRec resources;
    XcpcPixmapsRec   pixmaps;
    XcpcLayoutRec    layout;
    XcpcMenuBarRec   menubar;
    XcpcToolBarRec   toolbar;
} XcpcApplicationRec;

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MOTIF2_PRIV_H__ */
