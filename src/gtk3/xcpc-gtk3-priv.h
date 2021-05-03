/*
 * xcpc-gtk3-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_GTK3_PRIV_H__
#define __XCPC_GTK3_PRIV_H__

#include <gtk/gtk.h>
#include <gtk3/gememulator.h>
#include <xcpc/options/options.h>
#include <xcpc/machine/machine.h>
#include "xcpc-gtk3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcFileMenuRec    XcpcFileMenuRec;
typedef struct _XcpcCtrlMenuRec    XcpcCtrlMenuRec;
typedef struct _XcpcDrv0MenuRec    XcpcDrv0MenuRec;
typedef struct _XcpcDrv1MenuRec    XcpcDrv1MenuRec;
typedef struct _XcpcHelpMenuRec    XcpcHelpMenuRec;
typedef struct _XcpcMenuBarRec     XcpcMenuBarRec;
typedef struct _XcpcToolBarRec     XcpcToolBarRec;
typedef struct _XcpcInfoBarRec     XcpcInfoBarRec;
typedef struct _XcpcWorkWndRec     XcpcWorkWndRec;
typedef struct _XcpcLayoutRec      XcpcLayoutRec;
typedef struct _XcpcApplicationRec XcpcApplicationRec;

struct _XcpcFileMenuRec
{
    GtkWidget* widget;
    GtkWidget* menu;
    GtkWidget* load_snapshot;
    GtkWidget* save_snapshot;
    GtkWidget* separator1;
    GtkWidget* exit;
};

struct _XcpcCtrlMenuRec
{
    GtkWidget* widget;
    GtkWidget* menu;
    GtkWidget* play_emulator;
    GtkWidget* pause_emulator;
    GtkWidget* reset_emulator;
};

struct _XcpcDrv0MenuRec
{
    GtkWidget* widget;
    GtkWidget* menu;
    GtkWidget* drive0_insert;
    GtkWidget* drive0_remove;
};

struct _XcpcDrv1MenuRec
{
    GtkWidget* widget;
    GtkWidget* menu;
    GtkWidget* drive1_insert;
    GtkWidget* drive1_remove;
};

struct _XcpcHelpMenuRec
{
    GtkWidget* widget;
    GtkWidget* menu;
    GtkWidget* legal;
    GtkWidget* about;
    GtkWidget* separator1;
    GtkWidget* help;
};

struct _XcpcMenuBarRec
{
    GtkWidget* widget;
    XcpcFileMenuRec file;
    XcpcCtrlMenuRec ctrl;
    XcpcDrv0MenuRec drv0;
    XcpcDrv1MenuRec drv1;
    XcpcHelpMenuRec help;
};

struct _XcpcToolBarRec
{
    GtkWidget*   widget;
    GtkToolItem* load_snapshot;
    GtkToolItem* save_snapshot;
    GtkToolItem* play_emulator;
    GtkToolItem* pause_emulator;
    GtkToolItem* reset_emulator;
};

struct _XcpcInfoBarRec
{
    GtkWidget* widget;
    GtkWidget* status;
    GtkWidget* drive0;
    GtkWidget* drive1;
    GtkWidget* system;
};

struct _XcpcWorkWndRec
{
    GtkWidget* widget;
    GtkWidget* emulator;
};

struct _XcpcLayoutRec
{
    GtkApplication* application;
    GdkPixbuf*      logo;
    GtkWidget*      window;
    GtkWidget*      vbox;
    XcpcMenuBarRec  menubar;
    XcpcToolBarRec  toolbar;
    XcpcWorkWndRec  workwnd;
    XcpcInfoBarRec  infobar;
};

struct _XcpcApplicationRec
{
    XcpcOptions*  options;
    XcpcMachine*  machine;
    XcpcLayoutRec layout;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_GTK3_PRIV_H__ */
