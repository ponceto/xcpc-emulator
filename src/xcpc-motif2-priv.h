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

#include "xcpc-motif2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcResourcesRec
{
    String  appname;
    String  appclass;
    Boolean about_flag;
    Boolean usage_flag;
    Boolean debug_flag;
} XcpcResourcesRec;

typedef struct _XcpcPixmapsRec
{
    Pixel  foreground;
    Pixel  background;
    Pixmap xcpc_icon;
    Pixmap xcpc_mask;
    Pixmap eject;
    Pixmap empty;
    Pixmap folder_open;
    Pixmap info_circle;
    Pixmap pause;
    Pixmap play;
    Pixmap power_off;
    Pixmap question_circle;
    Pixmap save;
    Pixmap sync;
} XcpcPixmapsRec;

typedef struct _XcpcLayoutRec
{
    Widget toplevel;
    Widget main_window;
    Widget menu_bar;
    Widget frame;
    Widget emulator;
} XcpcLayoutRec;

typedef struct _XcpcFileMenuRec
{
    Widget menu;
    Widget pulldown;
    Widget snapshot_load;
    Widget snapshot_save;
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
    Widget legal_info;
    Widget separator1;
    Widget about_xcpc;
} XcpcHelpMenuRec;

typedef struct _XcpcApplicationRec
{
    FILE*            input_stream;
    FILE*            print_stream;
    FILE*            error_stream;
    XtAppContext     appcontext;
    XcpcResourcesRec resources;
    XcpcPixmapsRec   pixmaps;
    XcpcLayoutRec    layout;
    XcpcFileMenuRec  file;
    XcpcCtrlMenuRec  ctrl;
    XcpcDrv0MenuRec  drv0;
    XcpcDrv1MenuRec  drv1;
    XcpcHelpMenuRec  help;
} XcpcApplicationRec;

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MOTIF2_PRIV_H__ */
