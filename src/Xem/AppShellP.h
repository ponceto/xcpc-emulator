/*
 * AppShellP.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef _XemAppShellP_h
#define _XemAppShellP_h

#include <X11/ShellP.h>
#include <Xem/AppShell.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XemAppShellClassPart {
  XtPointer extension;
} XemAppShellClassPart;

typedef struct _XemAppShellClassRec {
  CoreClassPart             core_class;
  CompositeClassPart        composite_class;
  ShellClassPart            shell_class;
  WMShellClassPart          wm_shell_class;
  VendorShellClassPart      vendor_shell_class;
  TopLevelShellClassPart    top_level_shell_class;
  ApplicationShellClassPart application_shell_class;
  XemAppShellClassPart      app_shell_class;
} XemAppShellClassRec;

externalref XemAppShellClassRec xemAppShellClassRec;

typedef struct _XemAppShellPart {
  Atom WM_DELETE_WINDOW;
  Atom XdndAware;
  Atom XdndSelection;
  Atom XdndEnter;
  Atom XdndLeave;
  Atom XdndPosition;
  Atom XdndDrop;
  Atom XdndStatus;
  Atom XdndFinished;
  Atom XdndActionCopy;
  Atom XdndActionMove;
  Atom XdndActionLink;
  Atom XdndActionAsk;
  Atom XdndActionPrivate;
  Window XdndSource;
  Atom   XdndDataT1;
  Atom   XdndDataT2;
  Atom   XdndDataT3;
  Atom   XdndDataT4;
  XtCallbackList drop_uri_callback;
} XemAppShellPart;

typedef struct _XemAppShellRec {
  CorePart             core;
  CompositePart        composite;
  ShellPart            shell;
  WMShellPart          wm_shell;
  VendorShellPart      vendor_shell;
  TopLevelShellPart    top_level_shell;
  ApplicationShellPart application_shell;
  XemAppShellPart      app_shell;
} XemAppShellRec;

#ifdef __cplusplus
}
#endif

#endif
